/*********************************************************\
 * Copyright (c) 2012-2017 Christian Ofenberg
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
#include "RendererRuntime/Resource/Texture/Loader/ITextureResourceLoader.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class ITexture;
}
namespace RendererRuntime
{
	class IRendererRuntime;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class CrnTextureResourceLoader : protected ITextureResourceLoader
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class TextureResourceManager;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		static const ResourceLoaderTypeId TYPE_ID;


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	public:
		inline virtual ResourceLoaderTypeId getResourceLoaderTypeId() const override;
		virtual void onDeserialization(IFile& file) override;
		virtual void onProcessing() override;
		virtual bool onDispatch() override;
		inline virtual bool isFullyLoaded() override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		inline CrnTextureResourceLoader(IResourceManager& resourceManager, IRendererRuntime& rendererRuntime);
		inline virtual ~CrnTextureResourceLoader();
		CrnTextureResourceLoader(const CrnTextureResourceLoader&) = delete;
		CrnTextureResourceLoader& operator=(const CrnTextureResourceLoader&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&	mRendererRuntime;	///< Renderer runtime instance, do not destroy the instance
		Renderer::ITexture* mTexture;			///< In case the used renderer backend supports native multi-threading we also create the renderer resource asynchronous, but the final resource pointer reassignment must still happen synchronous
		// Temporary data
		uint32_t mWidth;
		uint32_t mHeight;
		uint8_t  mTextureFormat;	// "Renderer::TextureFormat", don't want to include the header in here
		// Temporary file data
		uint32_t mNumberOfFileDataBytes;
		uint32_t mNumberOfUsedFileDataBytes;
		uint8_t* mFileData;
		// Temporary image data
		uint32_t mNumberOfImageDataBytes;
		uint32_t mNumberOfUsedImageDataBytes;
		uint8_t* mImageData;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Texture/Loader/CrnTextureResourceLoader.inl"
