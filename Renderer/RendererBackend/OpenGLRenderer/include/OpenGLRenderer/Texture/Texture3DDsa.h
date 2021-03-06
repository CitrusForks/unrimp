/*********************************************************\
 * Copyright (c) 2012-2017 The Unrimp Team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "OpenGLRenderer/Texture/Texture3D.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL 3D texture class, effective direct state access (DSA)
	*/
	class Texture3DDsa : public Texture3D
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] width
		*    Texture width, must be >0
		*  @param[in] height
		*    Texture height, must be >0
		*  @param[in] depth
		*    Texture depth, must be >0
		*  @param[in] textureFormat
		*    Texture format
		*  @param[in] data
		*    Texture data, can be a null pointer
		*  @param[in] flags
		*    Texture flags, see "Renderer::TextureFlag::Enum"
		*/
		Texture3DDsa(OpenGLRenderer& openGLRenderer, uint32_t width, uint32_t height, uint32_t depth, Renderer::TextureFormat::Enum textureFormat, const void* data, uint32_t flags);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Texture3DDsa();


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ITexture3D methods           ]
	//[-------------------------------------------------------]
	public:
		virtual void copyDataFrom(uint32_t numberOfBytes, const void* data) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit Texture3DDsa(const Texture3DDsa& source) = delete;
		Texture3DDsa& operator =(const Texture3DDsa& source) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
