/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#include <osl/interlck.h>
#include	<rtl/alloc.h>
#include	<codemaker/dependency.hxx>

using namespace rtl;

TypeDependency::TypeDependency()
{
	m_pImpl = new TypeDependencyImpl();
	acquire();
}	

TypeDependency::~TypeDependency()
{
	release();
}	

void TypeDependency::acquire()
{ 
	osl_incrementInterlockedCount(&m_pImpl->m_refCount);
}

void TypeDependency::release()
{ 
	if (0 == osl_decrementInterlockedCount(&m_pImpl->m_refCount))
	{
		delete m_pImpl;
	}
}

sal_Bool TypeDependency::insert(const OString& type, const OString& depend, sal_uInt16 use)
{
	sal_Bool ret =  sal_False;
	
	if (type.getLength() > 0 && depend.getLength() > 0)
	{
		if (m_pImpl->m_dependencies.count(type) > 0)
		{
			TypeUsing typeUsing(depend, use);
			TypeUsingSet::iterator iter;
			if ((iter = m_pImpl->m_dependencies[type].find(typeUsing)) != m_pImpl->m_dependencies[type].end())
			{
  				(((TypeUsing *) &(*iter))->m_use) = (*iter).m_use | use;
			} else
			{
				m_pImpl->m_dependencies[type].insert(typeUsing);
			}
		} else
		{
			TypeUsing typeUsing(depend, use);
			TypeUsingSet tmpSet;
			tmpSet.insert(typeUsing);
			m_pImpl->m_dependencies[type]=tmpSet;
		}
	}
	
	return ret;	
}	

TypeUsingSet TypeDependency::getDependencies(const OString& type)
{
	if (type.getLength() > 0)
	{
		if (m_pImpl->m_dependencies.count(type) > 0)
		{
			return m_pImpl->m_dependencies[type];
		}
	}
		
	return TypeUsingSet();
}	

sal_Bool TypeDependency::hasDependencies(const OString& type)
{
	if (type.getLength() > 0)
	{
		if (m_pImpl->m_dependencies.count(type) > 0)
		{
			return sal_True;
		}
	}
		
	return sal_False;	
}	

void TypeDependency::setGenerated(const OString& type, sal_uInt16 genFlag)
{
//	m_pImpl->m_generatedTypes.insert(type);
	if (m_pImpl->m_generatedTypes.count(type) > 0)
		m_pImpl->m_generatedTypes[type]= m_pImpl->m_generatedTypes[type] | genFlag;
	else
		m_pImpl->m_generatedTypes[type]=genFlag;
}	

sal_Bool TypeDependency::isGenerated(const OString& type, sal_uInt16 genFlag)
{
/*
	if (m_pImpl->m_generatedTypes.count(type) > 0)
		return sal_True;

	return sal_False;
*/
	if (m_pImpl->m_generatedTypes.count(type) > 0 &&
		m_pImpl->m_generatedTypes[type] & genFlag)
	{
		return sal_True;
	}

	return sal_False;
}	

static sal_Bool checkFieldDependencies(TypeManager& typeMgr, TypeDependency& dependencies,
									   TypeReader& reader, const OString& type)
{
	sal_uInt32 count = reader.getFieldCount();

	if (count == 0 || reader.getTypeClass() == RT_TYPE_ENUM)
		return sal_True;
		
	OString fieldType;
	for (sal_uInt16 i=0; i < count; i++)
	{
		fieldType = reader.getFieldType(i);
		
		if (fieldType.getLength() > 0)
		{
			dependencies.insert(type, fieldType, TYPEUSE_MEMBER);
			checkTypeDependencies(typeMgr, dependencies, fieldType);
		}
	}
	
	return sal_True;		
}	

static sal_Bool checkMethodDependencies(TypeManager& typeMgr, TypeDependency& dependencies,
									    TypeReader& reader, const OString& type)
{
	sal_uInt32 count = reader.getMethodCount();

	if (count == 0)
		return sal_True;
		
	OString returnType, paramType, excType;
	sal_uInt32 paramCount = 0;
	sal_uInt32 excCount = 0;	
	RTParamMode paramMode = RT_PARAM_INVALID;
	for (sal_uInt16 i=0; i < count; i++)
	{
		returnType = reader.getMethodReturnType(i);
		
		dependencies.insert(type, returnType, TYPEUSE_RETURN);
		checkTypeDependencies(typeMgr, dependencies, returnType);

		paramCount = reader.getMethodParamCount(i);		
		excCount = reader.getMethodExcCount(i);

		sal_uInt16 j;
		for (j=0; j < paramCount; j++)
		{
			paramType = reader.getMethodParamType(i, j);
			paramMode = reader.getMethodParamMode(i, j);
			
			switch (paramMode)
			{
				case RT_PARAM_IN:
					dependencies.insert(type, paramType, TYPEUSE_INPARAM);
					break;
				case RT_PARAM_OUT:
					dependencies.insert(type, paramType, TYPEUSE_OUTPARAM);
					break;
				case RT_PARAM_INOUT:
					dependencies.insert(type, paramType, TYPEUSE_INOUTPARAM);
					break;
                default:
                    break;
			}

			checkTypeDependencies(typeMgr, dependencies, paramType);
		}
		
		for (j=0; j < excCount; j++)
		{
			excType = reader.getMethodExcType(i, j);
			dependencies.insert(type, excType, TYPEUSE_EXCEPTION);
			checkTypeDependencies(typeMgr, dependencies, excType);
		}
		
	}
	
	return sal_True;		
}	

static sal_Bool checkReferenceDependencies(TypeManager& typeMgr, TypeDependency& dependencies,
										   TypeReader& reader, const OString& type)
{
	sal_uInt32 count = reader.getReferenceCount();

	if (count == 0)
		return sal_True;
		
	OString referenceName;
	for (sal_uInt16 i=0; i < count; i++)
	{
		referenceName = reader.getReferenceName(i);
		
		dependencies.insert(type, referenceName, TYPEUSE_NORMAL);
		checkTypeDependencies(typeMgr, dependencies, referenceName);
	}
	
	return sal_True;	
}	

sal_Bool checkTypeDependencies(TypeManager& typeMgr, TypeDependency& dependencies, const OString& type, sal_Bool bDepend)
{
	if (!typeMgr.isValidType(type))
		return sal_False;

	if (dependencies.hasDependencies(type))
		return sal_True;		

	TypeReader reader = typeMgr.getTypeReader(type);
	
	if ( !reader.isValid() )
	{
		if (type.equals("/"))
			return sal_True;
		else
			return sal_False;
	}

    if ( bDepend && reader.getTypeClass() == RT_TYPE_MODULE)
    {
    	checkFieldDependencies(typeMgr, dependencies, reader, type);
        return sal_True;
    }
    
    for (sal_uInt16 i = 0; i < reader.getSuperTypeCount(); ++i) {
        OString superType(reader.getSuperTypeName(i));
		dependencies.insert(type, superType, TYPEUSE_SUPER);
		checkTypeDependencies(typeMgr, dependencies, superType);
	}

	if (reader.getTypeClass() == RT_TYPE_INTERFACE)
	{
		dependencies.insert(type, "com/sun/star/uno/RuntimeException", TYPEUSE_EXCEPTION);
		dependencies.insert(type, "com/sun/star/uno/TypeClass", TYPEUSE_NORMAL);
		checkTypeDependencies(typeMgr, dependencies, "com/sun/star/uno/RuntimeException", bDepend);
	}	
	
	checkFieldDependencies(typeMgr, dependencies, reader, type);
	checkMethodDependencies(typeMgr, dependencies, reader, type);
	checkReferenceDependencies(typeMgr, dependencies, reader, type);

    // make the scope modules as dependencies
    sal_Int32 nPos = type.lastIndexOf( '/' );

	if ( nPos >= 0 )
	{
		OString aScope( type.copy( 0, nPos ) );
		OStringBuffer tmpBuf(aScope.getLength());

		nPos = 0;
		do
		{
			tmpBuf.append(aScope.getToken(0, '/', nPos));
			dependencies.insert(type, tmpBuf.getStr(), TYPEUSE_SCOPE);
			tmpBuf.append('/');
		} while( nPos != -1 );
    }
    
	return sal_True;
}	
	

