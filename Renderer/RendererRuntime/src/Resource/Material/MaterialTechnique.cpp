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
#include "RendererRuntime/Resource/Material/MaterialTechnique.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/MaterialBufferManager.h"
#include "RendererRuntime/Resource/Texture/TextureResourceManager.h"
#include "RendererRuntime/Resource/Texture/TextureResource.h"
#include "RendererRuntime/Core/Math/Math.h"
#include "RendererRuntime/IRendererRuntime.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	MaterialTechnique::MaterialTechnique(MaterialTechniqueId materialTechniqueId, MaterialResource& materialResource, MaterialBlueprintResourceId materialBlueprintResourceId) :
		MaterialBufferSlot(materialResource),
		mMaterialTechniqueId(materialTechniqueId),
		mMaterialBlueprintResourceId(materialBlueprintResourceId),
		mSerializedPipelineStateHash(getUninitialized<uint32_t>())
	{
		MaterialBufferManager* materialBufferManager = getMaterialBufferManager();
		if (nullptr != materialBufferManager)
		{
			materialBufferManager->requestSlot(*this);
		}

		// Calculate FNV1a hash of "Renderer::SerializedPipelineState"
		calculateSerializedPipelineStateHash();
	}

	MaterialTechnique::~MaterialTechnique()
	{
		MaterialBufferManager* materialBufferManager = getMaterialBufferManager();
		if (nullptr != materialBufferManager)
		{
			materialBufferManager->releaseSlot(*this);
		}
	}

	const MaterialTechnique::Textures& MaterialTechnique::getTextures(const IRendererRuntime& rendererRuntime) const
	{
		// Need for gathering the textures now?
		if (mTextures.empty())
		{
			const MaterialBlueprintResource* materialBlueprintResource = rendererRuntime.getMaterialBlueprintResourceManager().tryGetById(mMaterialBlueprintResourceId);
			if (nullptr != materialBlueprintResource)
			{
				const MaterialResource& materialResource = getMaterialResource();
				TextureResourceManager& textureResourceManager = rendererRuntime.getTextureResourceManager();
				const MaterialBlueprintResource::Textures& textures = materialBlueprintResource->getTextures();
				const size_t numberOfTextures = textures.size();
				for (size_t i = 0; i < numberOfTextures; ++i)
				{
					const MaterialBlueprintResource::Texture& blueprintTexture = textures[i];

					// Start with the material blueprint textures
					Texture texture;
					texture.rootParameterIndex = blueprintTexture.rootParameterIndex;
					texture.materialProperty   = blueprintTexture.materialProperty;
					texture.textureResourceId  = blueprintTexture.textureResourceId;

					// Apply material specific modifications
					const MaterialPropertyId materialPropertyId = texture.materialProperty.getMaterialPropertyId();
					if (isInitialized(materialPropertyId))
					{
						// Figure out the material property value
						const MaterialProperty* materialProperty = materialResource.getPropertyById(materialPropertyId);
						if (nullptr != materialProperty)
						{
							// TODO(co) Error handling: Usage mismatch etc.
							texture.materialProperty = *materialProperty;
							textureResourceManager.loadTextureResourceByAssetId(texture.materialProperty.getTextureAssetIdValue(), blueprintTexture.fallbackTextureAssetId, texture.textureResourceId, nullptr, blueprintTexture.rgbHardwareGammaCorrection);
						}
					}

					// Insert texture
					mTextures.push_back(texture);
				}
			}
		}
		return mTextures;
	}

	bool MaterialTechnique::fillCommandBuffer(const IRendererRuntime& rendererRuntime, Renderer::CommandBuffer& commandBuffer)
	{
		bool assignedMaterialPoolChange = false;

		// Sanity check
		assert(isInitialized(mMaterialBlueprintResourceId));

		{ // Bind the material buffer manager
			MaterialBufferManager* materialBufferManager = getMaterialBufferManager();
			if (nullptr != materialBufferManager)
			{
				assignedMaterialPoolChange = materialBufferManager->fillCommandBuffer(*this, commandBuffer);
			}
		}

		{ // Graphics root descriptor table: Set textures
			const Textures& textures = getTextures(rendererRuntime);
			const size_t numberOfTextures = textures.size();
			const TextureResourceManager& textureResourceManager = rendererRuntime.getTextureResourceManager();
			for (size_t i = 0; i < numberOfTextures; ++i)
			{
				const Texture& texture = textures[i];

				// Due to background texture loading, some textures might not be ready, yet
				// -> But even in this situation there should be a decent fallback texture in place
				const TextureResource* textureResource = textureResourceManager.tryGetById(texture.textureResourceId);
				if (nullptr == textureResource)
				{
					// Maybe it's a dynamically created texture like a shadow map created by "RendererRuntime::CompositorInstancePassShadowMap"
					// which might not have been ready yet when the material was originally loaded
					textureResource = textureResourceManager.getTextureResourceByAssetId(texture.materialProperty.getTextureAssetIdValue());
					if (nullptr != textureResource)
					{
						mTextures[i].textureResourceId = textureResource->getId();
					}
				}
				if (nullptr != textureResource)
				{
					Renderer::ITexturePtr texturePtr = textureResource->getTexture();
					if (nullptr != texturePtr)
					{
						Renderer::Command::SetGraphicsRootDescriptorTable::create(commandBuffer, texture.rootParameterIndex, texturePtr);
					}
					else
					{
						// Error! There should always be e.g. a fallback texture if background loading is still in progress.
						assert(false);
					}
				}
				else
				{
					// Error! Referencing none existing texture resources should never ever happen.
					assert(false);
				}
			}
		}

		// Done
		return assignedMaterialPoolChange;
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	MaterialBufferManager* MaterialTechnique::getMaterialBufferManager() const
	{
		// It's valid if a material blueprint resource doesn't contain a material uniform buffer (usually the case for compositor material blueprint resources)
		const MaterialBlueprintResource* materialBlueprintResource = getMaterialResourceManager().getRendererRuntime().getMaterialBlueprintResourceManager().tryGetById(mMaterialBlueprintResourceId);
		return (nullptr != materialBlueprintResource) ? materialBlueprintResource->getMaterialBufferManager() : nullptr;
	}

	void MaterialTechnique::calculateSerializedPipelineStateHash()
	{
		const MaterialBlueprintResource* materialBlueprintResource = getMaterialResourceManager().getRendererRuntime().getMaterialBlueprintResourceManager().tryGetById(mMaterialBlueprintResourceId);
		if (nullptr != materialBlueprintResource)
		{
			// Start with the pipeline state of the material blueprint resource
			Renderer::SerializedPipelineState serializedPipelineState = materialBlueprintResource->getPipelineState();

			// Apply material properties
			// -> Renderer toolkit counterpart is "RendererToolkit::JsonMaterialBlueprintHelper::readPipelineStateObject()"
			const MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector = getMaterialResource().getSortedPropertyVector();
			const size_t numberOfMaterialProperties = sortedMaterialPropertyVector.size();
			for (size_t i = 0; i < numberOfMaterialProperties; ++i)
			{
				const MaterialProperty& materialProperty = sortedMaterialPropertyVector[i];
				switch (materialProperty.getUsage())
				{
					case MaterialProperty::Usage::UNKNOWN:
					case MaterialProperty::Usage::STATIC:
					case MaterialProperty::Usage::SHADER_UNIFORM:
					case MaterialProperty::Usage::SHADER_COMBINATION:
						// Nothing here
						break;

					case MaterialProperty::Usage::RASTERIZER_STATE:
						// TODO(co) Implement all rasterizer state properties
						if (materialProperty.getValueType() == MaterialPropertyValue::ValueType::CULL_MODE)
						{
							serializedPipelineState.rasterizerState.cullMode = materialProperty.getCullModeValue();
						}
						break;

					case MaterialProperty::Usage::DEPTH_STENCIL_STATE:
						// TODO(co) Implement all depth stencil state properties
						break;

					case MaterialProperty::Usage::BLEND_STATE:
						// TODO(co) Implement all blend state properties
						break;

					case MaterialProperty::Usage::SAMPLER_STATE:
					case MaterialProperty::Usage::TEXTURE_REFERENCE:
					case MaterialProperty::Usage::GLOBAL_REFERENCE:
					case MaterialProperty::Usage::UNKNOWN_REFERENCE:
					case MaterialProperty::Usage::PASS_REFERENCE:
					case MaterialProperty::Usage::MATERIAL_REFERENCE:
					case MaterialProperty::Usage::INSTANCE_REFERENCE:
					case MaterialProperty::Usage::GLOBAL_REFERENCE_FALLBACK:
						// Nothing here
						break;
				}
			}

			// Calculate the FNV1a hash of "Renderer::SerializedPipelineState"
			mSerializedPipelineStateHash = Math::calculateFNV1a(reinterpret_cast<const uint8_t*>(&serializedPipelineState), sizeof(Renderer::SerializedPipelineState));

			// Register the FNV1a hash of "Renderer::SerializedPipelineState" inside the material blueprint resource manager so it's sufficient to pass around the tiny hash instead the over 400 bytes full serialized pipeline state
			getMaterialResourceManager().getRendererRuntime().getMaterialBlueprintResourceManager().addSerializedPipelineState(mSerializedPipelineStateHash, serializedPipelineState);
		}
		else
		{
			setUninitialized(mSerializedPipelineStateHash);
		}
	}

	void MaterialTechnique::scheduleForShaderUniformUpdate()
	{
		MaterialBufferManager* materialBufferManager = getMaterialBufferManager();
		if (nullptr != materialBufferManager)
		{
			materialBufferManager->scheduleForUpdate(*this);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
