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



#ifndef DBAUI_TEXTCONNECTIONHELPER_HXX
#define DBAUI_TEXTCONNECTIONHELPER_HXX

#ifndef DBAUI_CONNECTIONPAGESETUP_HXX
#include "ConnectionPageSetup.hxx"
#endif
#ifndef _DBAUI_ADMINPAGES_HXX_
#include "adminpages.hxx"
#endif
#ifndef CHARSETLISTBOX_HXX
#include "charsetlistbox.hxx"
#endif
#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif
#ifndef _DBAUI_CURLEDIT_HXX_
#include "curledit.hxx"
#endif
#ifndef SVTOOLS_INC_ROADMAPWIZARD_HXX
#include <svtools/roadmapwizard.hxx>
#endif
#ifndef _SV_FIELD_HXX
#include <vcl/field.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _SV_LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif


//.........................................................................
namespace dbaui

{
//.........................................................................

    #define TC_EXTENSION    ((short)0x01)   // a section specifying the extension of the files to connect to
    #define TC_SEPARATORS   ((short)0x02)   // a section specifying the various separators
    #define TC_HEADER       ((short)0x04)   // a section containing the "Text contains header" check box only
    #define TC_CHARSET      ((short)0x08)   // not yet implemented

	//========================================================================
	//= OTextConnectionPage
	//========================================================================
	class OTextConnectionHelper : public Control
	{
        OTextConnectionHelper();

		Link		m_aModifiedHandler;		/// to be called if something on the page has been modified

    public:
		OTextConnectionHelper( Window* pParent, const short _nAvailableSections );
        virtual ~OTextConnectionHelper();

    private:
        FixedText   m_aFTExtensionHeader;
        RadioButton m_aRBAccessTextFiles;
        RadioButton m_aRBAccessCSVFiles;
        RadioButton m_aRBAccessOtherFiles;
        Edit        m_aETOwnExtension;
        FixedText   m_aFTExtensionExample;
		FixedLine	m_aLineFormat;
		FixedText	m_aFieldSeparatorLabel;
		ComboBox	m_aFieldSeparator;
		FixedText	m_aTextSeparatorLabel;
		ComboBox	m_aTextSeparator;
		FixedText	m_aDecimalSeparatorLabel;
		ComboBox	m_aDecimalSeparator;
		FixedText	m_aThousandsSeparatorLabel;
		ComboBox	m_aThousandsSeparator;
  		CheckBox	m_aRowHeader;
        FixedLine   m_aCharSetHeader;
        FixedText   m_aCharSetLabel;
        CharSetListBox  m_aCharSet;
		String		m_aFieldSeparatorList;
		String		m_aTextSeparatorList;
		String		m_aTextNone;
        String      m_aOldExtension;
        Link		m_aGetExtensionHandler;	/// to be called if a new type is selected

        short       m_nAvailableSections;

    protected:
		void callModifiedHdl() const { if (m_aModifiedHandler.IsSet()) m_aModifiedHandler.Call((void*)this); }
        Link getControlModifiedLink() { return LINK(this, OTextConnectionHelper, OnControlModified); }
        DECL_LINK(OnSetExtensionHdl,RadioButton*);
        DECL_LINK(OnControlModified,Control*);
		DECL_LINK(OnEditModified,Edit*);

	private:
		String		GetSeparator( const ComboBox& rBox, const String& rList );
		void		SetSeparator( ComboBox& rBox, const String& rList, const String& rVal );
        void        SetExtension(const String& _rVal);

		
    public:
		void        implInitControls(const SfxItemSet& _rSet, sal_Bool _bValid);
		void        fillControls(::std::vector< ISaveValueWrapper* >& _rControlList);
		void        fillWindows(::std::vector< ISaveValueWrapper* >& _rControlList);
        void        SetClickHandler(const Link& _rHandler) { m_aGetExtensionHandler = _rHandler; }
        String      GetExtension();
        sal_Bool FillItemSet( SfxItemSet& rSet, const sal_Bool bChangedSomething );
        sal_Bool prepareLeave();
	};

//.........................................................................
}	// namespace dbaui
//.........................................................................

#endif // DBAUI_DBWIZ2_HXX

