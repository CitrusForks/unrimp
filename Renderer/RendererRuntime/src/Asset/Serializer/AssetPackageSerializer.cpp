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
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Asset/Serializer/AssetPackageSerializer.h"
#include "RendererRuntime/Asset/AssetPackage.h"
#include "RendererRuntime/Core/File/MemoryFile.h"


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
namespace
{
	namespace detail
	{


		//[-------------------------------------------------------]
		//[ Global definitions                                    ]
		//[-------------------------------------------------------]
		static const uint32_t FORMAT_TYPE	 = RendererRuntime::StringId("AssetPackage");
		static const uint32_t FORMAT_VERSION = 2;

		struct AssetPackageHeader
		{
			uint32_t numberOfAssets;
		};


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void AssetPackageSerializer::loadAssetPackage(AssetPackage& assetPackage, IFile& file)
	{
		// Tell the memory mapped file about the LZ4 compressed data and decompress it at once
		MemoryFile memoryFile;
		memoryFile.loadLz4CompressedDataFromFile(::detail::FORMAT_TYPE, ::detail::FORMAT_VERSION, file);
		memoryFile.decompress();

		// Read in the asset package header
		::detail::AssetPackageHeader assetPackageHeader;
		memoryFile.read(&assetPackageHeader, sizeof(::detail::AssetPackageHeader));

		// Read in the asset package content in one single burst
		AssetPackage::SortedAssetVector& sortedAssetVector = assetPackage.getWritableSortedAssetVector();
		sortedAssetVector.resize(assetPackageHeader.numberOfAssets);
		memoryFile.read(sortedAssetVector.data(), sizeof(Asset) * assetPackageHeader.numberOfAssets);
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
