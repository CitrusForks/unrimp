/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "RendererRuntime/Resource/ShaderBlueprint/Loader/ShaderBlueprintResourceLoader.h"
#include "RendererRuntime/Resource/ShaderBlueprint/Loader/ShaderBlueprintFileFormat.h"
#include "RendererRuntime/Resource/ShaderBlueprint/ShaderBlueprintResource.h"
#include "RendererRuntime/Resource/ShaderPiece/ShaderPieceResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <fstream>
#include <unordered_set>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const ResourceLoaderTypeId ShaderBlueprintResourceLoader::TYPE_ID("shader_blueprint");


	//[-------------------------------------------------------]
	//[ Public virtual RendererRuntime::IResourceLoader methods ]
	//[-------------------------------------------------------]
	ResourceLoaderTypeId ShaderBlueprintResourceLoader::getResourceLoaderTypeId() const
	{
		return TYPE_ID;
	}

	void ShaderBlueprintResourceLoader::onDeserialization()
	{
		// TODO(co) Error handling
		try
		{
			std::ifstream inputFileStream(mAsset.assetFilename, std::ios::binary);

			// Read in the shader blueprint header
			v1ShaderBlueprint::Header shaderBlueprintHeader;
			inputFileStream.read(reinterpret_cast<char*>(&shaderBlueprintHeader), sizeof(v1ShaderBlueprint::Header));

			// Allocate more temporary memory, if required
			if (mMaximumNumberOfIncludeShaderPieceAssetIds < shaderBlueprintHeader.numberOfIncludeShaderPieceAssetIds)
			{
				mMaximumNumberOfIncludeShaderPieceAssetIds = shaderBlueprintHeader.numberOfIncludeShaderPieceAssetIds;
				delete [] mIncludeShaderPieceAssetIds;
				mIncludeShaderPieceAssetIds = new AssetId[mMaximumNumberOfIncludeShaderPieceAssetIds];
			}
			if (mMaximumNumberOfShaderSourceCodeBytes < shaderBlueprintHeader.numberOfShaderSourceCodeBytes)
			{
				mMaximumNumberOfShaderSourceCodeBytes = shaderBlueprintHeader.numberOfShaderSourceCodeBytes;
				delete [] mShaderSourceCode;
				mShaderSourceCode = new char[mMaximumNumberOfShaderSourceCodeBytes];
			}

			// Read the asset IDs of the shader pieces to include
			inputFileStream.read(reinterpret_cast<char*>(mIncludeShaderPieceAssetIds), sizeof(AssetId) * shaderBlueprintHeader.numberOfIncludeShaderPieceAssetIds);
			mShaderBlueprintResource->mIncludeShaderPieceResources.resize(shaderBlueprintHeader.numberOfIncludeShaderPieceAssetIds);

			// Read the shader blueprint ASCII source code
			inputFileStream.read(mShaderSourceCode, shaderBlueprintHeader.numberOfShaderSourceCodeBytes);
			mShaderBlueprintResource->mShaderSourceCode.assign(mShaderSourceCode, mShaderSourceCode + shaderBlueprintHeader.numberOfShaderSourceCodeBytes);
		}
		catch (const std::exception& e)
		{
			RENDERERRUNTIME_OUTPUT_ERROR_PRINTF("Renderer runtime failed to load shader blueprint asset %d: %s", mAsset.assetId, e.what());
		}
	}

	void ShaderBlueprintResourceLoader::onProcessing()
	{
		// Nothing here
	}

	void ShaderBlueprintResourceLoader::onRendererBackendDispatch()
	{
		{ // Read the shader piece resources to include
			ShaderPieceResourceManager& shaderPieceResourceManager = mRendererRuntime.getShaderPieceResourceManager();
			ShaderBlueprintResource::IncludeShaderPieceResources& includeShaderPieceResources = mShaderBlueprintResource->mIncludeShaderPieceResources;
			const size_t numberOfShaderPieceResources = includeShaderPieceResources.size();
			const AssetId* includeShaderPieceAssetIds = mIncludeShaderPieceAssetIds;
			for (size_t i = 0; i < numberOfShaderPieceResources; ++i, ++includeShaderPieceAssetIds)
			{
				includeShaderPieceResources[i] = shaderPieceResourceManager.loadShaderPieceResourceByAssetId(*includeShaderPieceAssetIds);
			}
		}

		{ // TODO(co) Cleanup: Get all influenced material blueprint resources
			typedef std::unordered_set<MaterialBlueprintResource*> MaterialBlueprintResources;
			MaterialBlueprintResources materialBlueprintResources;
			for (auto materialBlueprintResource : mRendererRuntime.getMaterialBlueprintResourceManager().mResources)
			{
				if (materialBlueprintResource->mVertexShaderBlueprint == mShaderBlueprintResource || materialBlueprintResource->mFragmentShaderBlueprint == mShaderBlueprintResource)
				{
					materialBlueprintResources.insert(materialBlueprintResource);
				}
			}
			for (MaterialBlueprintResource* materialBlueprintResource : materialBlueprintResources)
			{
				materialBlueprintResource->getPipelineStateCacheManager().clearCache();
				materialBlueprintResource->getPipelineStateCacheManager().getProgramCacheManager().clearCache();
			}
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
