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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	inline ResourceLoaderTypeId DdsTextureResourceLoader::getResourceLoaderTypeId() const
	{
		return TYPE_ID;
	}

	inline void DdsTextureResourceLoader::onProcessing()
	{
		// Nothing here
	}

	inline bool DdsTextureResourceLoader::isFullyLoaded()
	{
		// Fully loaded
		return true;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	inline DdsTextureResourceLoader::DdsTextureResourceLoader(IResourceManager& resourceManager, IRendererRuntime& rendererRuntime) :
		ITextureResourceLoader(resourceManager),
		mRendererRuntime(rendererRuntime),
		mTexture(nullptr),
		mWidth(0),
		mHeight(0),
		mDepth(0),
		mTextureFormat(0),
		mDataContainsMipmaps(false),
		mNumberOfImageDataBytes(0),
		mNumberOfUsedImageDataBytes(0),
		mImageData(nullptr)
	{
		// Nothing here
	}

	inline DdsTextureResourceLoader::~DdsTextureResourceLoader()
	{
		delete [] mImageData;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
