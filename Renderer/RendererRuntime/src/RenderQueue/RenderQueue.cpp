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
#include "RendererRuntime/RenderQueue/RenderQueue.h"
#include "RendererRuntime/RenderQueue/RenderableManager.h"
#include "RendererRuntime/Resource/CompositorWorkspace/CompositorContextData.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Resource/Material/MaterialTechnique.h"
#include "RendererRuntime/Resource/Material/MaterialResource.h"
#include "RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/PassBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/LightBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/InstanceBufferManager.h"
#include "RendererRuntime/Resource/MaterialBlueprint/BufferManager/IndirectBufferManager.h"
#include "RendererRuntime/Core/Math/Transform.h"
#include "RendererRuntime/IRendererRuntime.h"

#include <algorithm>


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
		const int DepthBits = 15;


		//[-------------------------------------------------------]
		//[ Global functions                                      ]
		//[-------------------------------------------------------]
		// Flip the float to deal with negative & positive numbers
		// - See "Rough sorting by depth" - http://aras-p.info/blog/2014/01/16/rough-sorting-by-depth/
		inline uint32_t floatFlip(uint32_t f)
		{
			const uint32_t mask = -int(f >> 31) | 0x80000000;
			return (f ^ mask);
		}

		// Taking highest 10 bits for rough sort of floats.
		// - 0.01 maps to 752; 0.1 to 759; 1.0 to 766; 10.0 to 772;
		// - 100.0 to 779 etc. Negative numbers go similarly in 0..511 range.
		// - See "Rough sorting by depth" - http://aras-p.info/blog/2014/01/16/rough-sorting-by-depth/
		inline uint32_t depthToBits(float depth)
		{
			union { float f; uint32_t i; } f2i;
			f2i.f = depth;
			f2i.i = floatFlip(f2i.i);			// Flip bits to be sortable
			return (f2i.i >> (32 - DepthBits));	// Take highest n-bits
		}

		inline void setShaderPropertiesPropertyValue(RendererRuntime::MaterialPropertyId materialPropertyId, const RendererRuntime::MaterialPropertyValue& materialPropertyValue, RendererRuntime::ShaderProperties& shaderProperties)
		{
			switch (materialPropertyValue.getValueType())
			{
				case RendererRuntime::MaterialPropertyValue::ValueType::BOOLEAN:
					shaderProperties.setPropertyValue(materialPropertyId, materialPropertyValue.getBooleanValue());
					break;

				case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER:
					shaderProperties.setPropertyValue(materialPropertyId, materialPropertyValue.getIntegerValue());
					break;

				case RendererRuntime::MaterialPropertyValue::ValueType::UNKNOWN:
				case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_2:
				case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_3:
				case RendererRuntime::MaterialPropertyValue::ValueType::INTEGER_4:
				case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT:
				case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_2:
				case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_3:
				case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_4:
				case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_3_3:
				case RendererRuntime::MaterialPropertyValue::ValueType::FLOAT_4_4:
				case RendererRuntime::MaterialPropertyValue::ValueType::FILL_MODE:
				case RendererRuntime::MaterialPropertyValue::ValueType::CULL_MODE:
				case RendererRuntime::MaterialPropertyValue::ValueType::CONSERVATIVE_RASTERIZATION_MODE:
				case RendererRuntime::MaterialPropertyValue::ValueType::DEPTH_WRITE_MASK:
				case RendererRuntime::MaterialPropertyValue::ValueType::STENCIL_OP:
				case RendererRuntime::MaterialPropertyValue::ValueType::COMPARISON_FUNC:
				case RendererRuntime::MaterialPropertyValue::ValueType::BLEND:
				case RendererRuntime::MaterialPropertyValue::ValueType::BLEND_OP:
				case RendererRuntime::MaterialPropertyValue::ValueType::FILTER_MODE:
				case RendererRuntime::MaterialPropertyValue::ValueType::TEXTURE_ADDRESS_MODE:
				case RendererRuntime::MaterialPropertyValue::ValueType::TEXTURE_ASSET_ID:
				case RendererRuntime::MaterialPropertyValue::ValueType::GLOBAL_MATERIAL_PROPERTY_ID:
				default:
					assert(false);	// TODO(co) Error handling
					break;
			}
		}


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
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	RenderQueue::RenderQueue(IndirectBufferManager& indirectBufferManager, uint8_t minimumRenderQueueIndex, uint8_t maximumRenderQueueIndex, bool transparentPass, bool doSort) :
		mRendererRuntime(indirectBufferManager.getRendererRuntime()),
		mIndirectBufferManager(indirectBufferManager),
		mNumberOfNullDrawCalls(0),
		mNumberOfDrawIndexedInstancedCalls(0),
		mNumberOfDrawInstancedCalls(0),
		mMinimumRenderQueueIndex(minimumRenderQueueIndex),
		mMaximumRenderQueueIndex(maximumRenderQueueIndex),
		mTransparentPass(transparentPass),
		mDoSort(doSort)
	{
		assert(mMaximumRenderQueueIndex >= mMinimumRenderQueueIndex);
		mQueues.resize(static_cast<size_t>(mMaximumRenderQueueIndex - mMinimumRenderQueueIndex + 1));
	}

	void RenderQueue::clear()
	{
		if (getNumberOfDrawCalls() > 0)
		{
			for (Queue& queue : mQueues)
			{
				queue.queuedRenderables.clear();
				queue.sorted = false;
			}
			mNumberOfNullDrawCalls = mNumberOfDrawIndexedInstancedCalls = mNumberOfDrawInstancedCalls = 0;
		}
	}

	void RenderQueue::addRenderablesFromRenderableManager(const RenderableManager& renderableManager, bool castShadows)
	{
		// Sanity check
		assert(renderableManager.isVisible());

		// Quantize the cached distance to camera
		const uint32_t quantizedDepth = ::detail::depthToBits(renderableManager.getCachedDistanceToCamera());

		// Register the renderables inside our renderables queue
		for (const Renderable& renderable : renderableManager.getRenderables())
		{
			if (!castShadows || renderable.getCastShadows())
			{
				// It's valid if one or more renderables inside a renderable manager don't fall into the range processed by this render queue
				// -> At least one renderable should fall into the range processed by this render queue or the render queue is used wrong
				const uint8_t renderQueueIndex = renderable.getRenderQueueIndex();
				if (renderQueueIndex >= mMinimumRenderQueueIndex && renderQueueIndex <= mMaximumRenderQueueIndex)
				{
					// Get the precalculated static part of the sorting key
					// -> Sort renderables back-to-front (for transparency) or front-to-back (for occlusion efficiency)
					// TODO(co) Depending on "mTransparentPass" the sorting key is used
					uint64_t sortingKey = renderable.getSortingKey();

					// The quantized depth is a dynamic part which is set now
					sortingKey = quantizedDepth;	// TODO(co) Just bits influenced

					// Register the renderable inside our renderables queue
					Queue& queue = mQueues[static_cast<size_t>(renderQueueIndex - mMinimumRenderQueueIndex)];
					assert(!queue.sorted);	// Ensure render queue is still in filling state and not already in rendering state
					queue.queuedRenderables.emplace_back(renderable, sortingKey);
					if (0 != renderable.getNumberOfIndices())
					{
						if (renderable.getDrawIndexed())
						{
							++mNumberOfDrawIndexedInstancedCalls;
						}
						else
						{
							++mNumberOfDrawInstancedCalls;
						}
					}
					else
					{
						++mNumberOfNullDrawCalls;
					}
				}
			}
		}
	}

	void RenderQueue::fillCommandBuffer(const Renderer::IRenderTarget& renderTarget, MaterialTechniqueId materialTechniqueId, const CompositorContextData& compositorContextData, Renderer::CommandBuffer& commandBuffer)
	{
		// Sanity check
		assert(getNumberOfDrawCalls() > 0 && "Don't call the fill command buffer method if there's no work to be done");

		// Begin debug event
		COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer)

		// TODO(co) This is just a dummy implementation. For example automatic instancing has to be incorporated as well as more efficient buffer management.
		const MaterialResourceManager& materialResourceManager = mRendererRuntime.getMaterialResourceManager();
		const MaterialBlueprintResourceManager& materialBlueprintResourceManager = mRendererRuntime.getMaterialBlueprintResourceManager();
		const MaterialProperties& globalMaterialProperties = materialBlueprintResourceManager.getGlobalMaterialProperties();
		InstanceBufferManager& instanceBufferManager = materialBlueprintResourceManager.getInstanceBufferManager();
		LightBufferManager& lightBufferManager = materialBlueprintResourceManager.getLightBufferManager();

		// Track currently bound renderer resources and states to void generating redundant commands
		Renderer::IVertexArray* currentVertexArray = nullptr;
		Renderer::IPipelineState* currentPipelineState = nullptr;

		// We try to minimize state changes across multiple render queue fill command buffer calls, but while doing so we still need to take into account
		// that pass data like world space to clip space transform might have been changed and needs to be updated inside the pass uniform buffer
		bool enforcePassBufferManagerFillBuffer = true;

		// Get indirect buffer
		Renderer::IIndirectBuffer* indirectBuffer = nullptr;
		uint32_t indirectBufferOffset = 0;
		uint8_t* indirectBufferData = nullptr;
		if (mNumberOfDrawIndexedInstancedCalls > 0 || mNumberOfDrawInstancedCalls > 0 )
		{
			IndirectBufferManager::IndirectBuffer* managedIndirectBuffer = mIndirectBufferManager.getIndirectBuffer(sizeof(Renderer::DrawIndexedInstancedArguments) * mNumberOfDrawIndexedInstancedCalls + sizeof(Renderer::DrawInstancedArguments) * mNumberOfDrawInstancedCalls);
			assert(nullptr != managedIndirectBuffer);
			indirectBuffer		 = managedIndirectBuffer->indirectBuffer;
			indirectBufferOffset = managedIndirectBuffer->indirectBufferOffset;
			indirectBufferData   = managedIndirectBuffer->mappedData;
		}

		// Process all render queues
		// -> When adding renderables from renderable manager we could build up a minimum/maximum used render queue index to sometimes reduce
		//    the number of iterations. On the other hand, there are usually much more renderables added as iterations in here so this possible
		//    optimization might be a fact a performance degeneration while at the same time increasing the code complexity. So, not implemented by intent.
		for (Queue& queue : mQueues)
		{
			QueuedRenderables& queuedRenderables = queue.queuedRenderables;
			if (!queuedRenderables.empty())
			{
				// Sort queued renderables
				if (!queue.sorted && mDoSort)
				{
					// TODO(co) Exploit temporal coherence across frames then use insertion sorts as explained by L. Spiro in
					// http://www.gamedev.net/topic/661114-temporal-coherence-and-render-queue-sorting/?view=findpost&p=5181408
					// Keep a list of sorted indices from the previous frame (one per camera).
					// If we have the sorted list "5, 1, 4, 3, 2, 0":
					// * If it grew from last frame, append: 5, 1, 4, 3, 2, 0, 6, 7 and use insertion sort.
					// * If it's the same, leave it as is, and use insertion sort just in case.
					// * If it's shorter, reset the indices 0, 1, 2, 3, 4; probably use quicksort or other generic sort
					// TODO(co) Use radix sort? ( https://www.quora.com/What-is-the-most-efficient-way-to-sort-a-million-32-bit-integers )
					std::sort(queuedRenderables.begin(), queuedRenderables.end());
					queue.sorted = true;
				}

				// Inject queued renderables into the renderer
				for (const QueuedRenderable& queuedRenderable : queuedRenderables)
				{
					assert(nullptr != queuedRenderable.renderable);
					const Renderable& renderable = *queuedRenderable.renderable;
					Renderer::IVertexArrayPtr vertexArrayPtr = renderable.getVertexArrayPtr();
					if (nullptr != vertexArrayPtr)
					{
						// Material resource
						const MaterialResource* materialResource = materialResourceManager.tryGetById(renderable.getMaterialResourceId());
						if (nullptr != materialResource)
						{
							MaterialTechnique* materialTechnique = materialResource->getMaterialTechniqueById(materialTechniqueId);
							if (nullptr != materialTechnique)
							{
								MaterialBlueprintResource* materialBlueprintResource = materialBlueprintResourceManager.tryGetById(materialTechnique->getMaterialBlueprintResourceId());
								if (nullptr != materialBlueprintResource && IResource::LoadingState::LOADED == materialBlueprintResource->getLoadingState())
								{
									// TODO(co) Gather shader properties (later on we cache as much as possible of this work inside the renderable)
									mScratchShaderProperties.clear();
									for (int i = 0; i < NUMBER_OF_SHADER_TYPES; ++i)
									{
										mScratchDynamicShaderPieces[i].clear();
									}
									{ // Gather shader properties from static material properties generating shader combinations
										const MaterialProperties::SortedPropertyVector& sortedMaterialPropertyVector = materialResource->getSortedPropertyVector();
										const size_t numberOfMaterialProperties = sortedMaterialPropertyVector.size();
										for (size_t i = 0; i < numberOfMaterialProperties; ++i)
										{
											const MaterialProperty& materialProperty = sortedMaterialPropertyVector[i];
											if (materialProperty.getUsage() == MaterialProperty::Usage::SHADER_COMBINATION)
											{
												switch (materialProperty.getValueType())
												{
													case MaterialPropertyValue::ValueType::BOOLEAN:
														mScratchShaderProperties.setPropertyValue(materialProperty.getMaterialPropertyId(), materialProperty.getBooleanValue());
														break;

													case MaterialPropertyValue::ValueType::INTEGER:
														mScratchShaderProperties.setPropertyValue(materialProperty.getMaterialPropertyId(), materialProperty.getIntegerValue());
														break;

													case MaterialPropertyValue::ValueType::GLOBAL_MATERIAL_PROPERTY_ID:
													{
														const MaterialProperty* globalMaterialProperty = globalMaterialProperties.getPropertyById(materialProperty.getGlobalMaterialPropertyId());
														if (nullptr != globalMaterialProperty)
														{
															::detail::setShaderPropertiesPropertyValue(materialProperty.getMaterialPropertyId(), *globalMaterialProperty, mScratchShaderProperties);
														}
														else
														{
															// Try global material property reference fallback
															globalMaterialProperty = materialBlueprintResource->getMaterialProperties().getPropertyById(materialProperty.getGlobalMaterialPropertyId());
															if (nullptr != globalMaterialProperty)
															{
																::detail::setShaderPropertiesPropertyValue(materialProperty.getMaterialPropertyId(), *globalMaterialProperty, mScratchShaderProperties);
															}
															else
															{
																// Error, can't resolve reference
																assert(false);	// TODO(co) Error handling
															}
														}
														break;
													}

													case MaterialPropertyValue::ValueType::UNKNOWN:
													case MaterialPropertyValue::ValueType::INTEGER_2:
													case MaterialPropertyValue::ValueType::INTEGER_3:
													case MaterialPropertyValue::ValueType::INTEGER_4:
													case MaterialPropertyValue::ValueType::FLOAT:
													case MaterialPropertyValue::ValueType::FLOAT_2:
													case MaterialPropertyValue::ValueType::FLOAT_3:
													case MaterialPropertyValue::ValueType::FLOAT_4:
													case MaterialPropertyValue::ValueType::FLOAT_3_3:
													case MaterialPropertyValue::ValueType::FLOAT_4_4:
													case MaterialPropertyValue::ValueType::FILL_MODE:
													case MaterialPropertyValue::ValueType::CULL_MODE:
													case MaterialPropertyValue::ValueType::CONSERVATIVE_RASTERIZATION_MODE:
													case MaterialPropertyValue::ValueType::DEPTH_WRITE_MASK:
													case MaterialPropertyValue::ValueType::STENCIL_OP:
													case MaterialPropertyValue::ValueType::COMPARISON_FUNC:
													case MaterialPropertyValue::ValueType::BLEND:
													case MaterialPropertyValue::ValueType::BLEND_OP:
													case MaterialPropertyValue::ValueType::FILTER_MODE:
													case MaterialPropertyValue::ValueType::TEXTURE_ADDRESS_MODE:
													case MaterialPropertyValue::ValueType::TEXTURE_ASSET_ID:
													default:
														assert(false);	// TODO(co) Error handling
														break;
												}
											}
										}
									}

									// Automatic "UseGpuSkinning"-property setting
									if (isInitialized(renderable.getSkeletonResourceId()))
									{
										static const StringId USE_GPU_SKINNING("UseGpuSkinning");
										if (nullptr != materialBlueprintResource->getMaterialProperties().getPropertyById(USE_GPU_SKINNING))
										{
											mScratchShaderProperties.setPropertyValue(USE_GPU_SKINNING, 1);
										}
									}

									materialBlueprintResource->optimizeShaderProperties(mScratchShaderProperties);

									Renderer::IPipelineStatePtr pipelineStatePtr = materialBlueprintResource->getPipelineStateCacheManager().getPipelineStateCacheByCombination(renderable.getPrimitiveTopology(), materialTechnique->getSerializedPipelineStateHash(), mScratchShaderProperties, mScratchDynamicShaderPieces, false);
									if (nullptr != pipelineStatePtr)
									{
										// Expensive state change: Handle material blueprint resource switches
										// -> Render queue should be sorted by material blueprint resource first to reduce those expensive state changes
										bool bindMaterialBlueprint = false;
										PassBufferManager* passBufferManager = nullptr;
										const MaterialBlueprintResource::UniformBuffer* instanceUniformBuffer = materialBlueprintResource->getInstanceUniformBuffer();
										if (compositorContextData.mCurrentlyBoundMaterialBlueprintResource != materialBlueprintResource)
										{
											compositorContextData.mCurrentlyBoundMaterialBlueprintResource = materialBlueprintResource;
											bindMaterialBlueprint = true;
										}
										if (bindMaterialBlueprint || enforcePassBufferManagerFillBuffer)
										{
											// Fill the pass buffer manager
											passBufferManager = materialBlueprintResource->getPassBufferManager();
											if (nullptr != passBufferManager)
											{
												passBufferManager->fillBuffer(renderTarget, compositorContextData, *materialResource);
												enforcePassBufferManagerFillBuffer = false;
											}
										}
										if (bindMaterialBlueprint)
										{
											// Bind the material blueprint resource and instance and light buffer manager to the used renderer
											materialBlueprintResource->fillCommandBuffer(commandBuffer);
											if (nullptr != instanceUniformBuffer)
											{
												instanceBufferManager.startupBufferFilling(*materialBlueprintResource, commandBuffer);
											}
											lightBufferManager.fillCommandBuffer(*materialBlueprintResource, commandBuffer);
										}
										else if (nullptr != passBufferManager)
										{
											// Bind pass buffer manager since we filled the buffer
											passBufferManager->fillCommandBuffer(commandBuffer);
										}

										// Cheap state change: Bind the material technique to the used renderer
										if (materialTechnique->fillCommandBuffer(mRendererRuntime, commandBuffer))
										{
											// Assigned material pool changed
											// TODO(co) Break instancing
											NOP;
										}
										// TODO(co) Detect texture hash change: Break instancing

										// Setup input assembly (IA): Set the used vertex array
										if (currentVertexArray != vertexArrayPtr)
										{
											currentVertexArray = vertexArrayPtr;
											Renderer::Command::SetVertexArray::create(commandBuffer, currentVertexArray);
										}

										// Set the used pipeline state object (PSO)
										if (currentPipelineState != pipelineStatePtr)
										{
											currentPipelineState = pipelineStatePtr;
											Renderer::Command::SetPipelineState::create(commandBuffer, currentPipelineState);
										}

										// Fill the instance buffer manager
										const uint32_t startInstanceLocation = (nullptr != instanceUniformBuffer) ? instanceBufferManager.fillBuffer(*materialBlueprintResource, materialBlueprintResource->getPassBufferManager(), *instanceUniformBuffer, renderable, *materialTechnique, commandBuffer) : 0;

										// Render the specified geometric primitive, based on indexing into an array of vertices
										// -> Please note that it's valid that there are no indices, for example "RendererRuntime::CompositorInstancePassDebugGui" is using the render queue only to set the material resource blueprint
										if (0 != renderable.getNumberOfIndices())
										{
											// Sanity checks
											assert(nullptr != indirectBuffer);
											assert(nullptr != indirectBufferData);

											// Draw
											if (renderable.getDrawIndexed())
											{
												// Fill indirect buffer
												Renderer::DrawIndexedInstancedArguments* drawIndexedInstancedArguments = reinterpret_cast<Renderer::DrawIndexedInstancedArguments*>(indirectBufferData + indirectBufferOffset);
												drawIndexedInstancedArguments->indexCountPerInstance = renderable.getNumberOfIndices();
												drawIndexedInstancedArguments->instanceCount		 = 1;
												drawIndexedInstancedArguments->startIndexLocation	 = renderable.getStartIndexLocation();
												drawIndexedInstancedArguments->baseVertexLocation	 = 0;
												drawIndexedInstancedArguments->startInstanceLocation = startInstanceLocation;

												// Draw
												Renderer::Command::DrawIndexed::create(commandBuffer, *indirectBuffer, indirectBufferOffset);

												// Advance indirect buffer offset
												indirectBufferOffset += sizeof(Renderer::DrawIndexedInstancedArguments);
											}
											else
											{
												// Fill indirect buffer
												Renderer::DrawInstancedArguments* drawInstancedArguments = reinterpret_cast<Renderer::DrawInstancedArguments*>(indirectBufferData + indirectBufferOffset);
												drawInstancedArguments->vertexCountPerInstance = renderable.getNumberOfIndices();
												drawInstancedArguments->instanceCount		   = 1;
												drawInstancedArguments->startVertexLocation	   = renderable.getStartIndexLocation();
												drawInstancedArguments->startInstanceLocation  = startInstanceLocation;

												// Draw
												Renderer::Command::Draw::create(commandBuffer, *indirectBuffer, indirectBufferOffset);

												// Advance indirect buffer offset
												indirectBufferOffset += sizeof(Renderer::DrawInstancedArguments);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		// End debug event
		COMMAND_END_DEBUG_EVENT(commandBuffer)
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
