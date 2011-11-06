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



#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_WRONGSPELLPRIMITIVE2D_HXX
#define INCLUDED_DRAWINGLAYER_PRIMITIVE2D_WRONGSPELLPRIMITIVE2D_HXX

#include <drawinglayer/drawinglayerdllapi.h>
#include <drawinglayer/primitive2d/baseprimitive2d.hxx>
#include <basegfx/color/bcolor.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>

//////////////////////////////////////////////////////////////////////////////
// WrongSpellPrimitive2D class

namespace drawinglayer
{
	namespace primitive2d
	{
        /** WrongSpellPrimitive2D class

            This is a helper primitive to hold evtl. WrongSpell visualisations
            in the sequence of primitives. The primitive holds this information
            separated form the TextPortions to where it belongs to tot expand the
            TextSimplePortionPrimitive2D more as needed.

            A renderer who does not want to visualize this (if contained at all)
            can detect and ignore this primitive. If it's decomposition is used,
            it will be visualized as red wavelines.

            The geometric definition defines a line on the X-Axis (no Y-coordinates)
            which will when transformed by Transformation, create the coordinate data.
         */
		class DRAWINGLAYER_DLLPUBLIC WrongSpellPrimitive2D : public BufferedDecompositionPrimitive2D
		{
		private:
            /// geometry definition
			basegfx::B2DHomMatrix							maTransformation;
            double                                          mfStart;
            double                                          mfStop;

            /// color (usually red)
			basegfx::BColor									maColor;

        protected:
			/// create local decomposition
			virtual Primitive2DSequence create2DDecomposition(const geometry::ViewInformation2D& rViewInformation) const;

		public:
            /// constructor
			WrongSpellPrimitive2D(
			    const basegfx::B2DHomMatrix& rTransformation,
                double fStart,
                double fStop,
			    const basegfx::BColor& rColor);

			/// data read access
            const basegfx::B2DHomMatrix& getTransformation() const { return maTransformation; }
            double getStart() const { return mfStart; }
            double getStop() const { return mfStop; }
            const basegfx::BColor& getColor() const { return maColor; }

			/// compare operator
			virtual bool operator==(const BasePrimitive2D& rPrimitive) const;

			/// provide unique ID
			DeclPrimitrive2DIDBlock()
		};
	} // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_DRAWINGLAYER_PRIMITIVE2D_WRONGSPELLPRIMITIVE2D_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
