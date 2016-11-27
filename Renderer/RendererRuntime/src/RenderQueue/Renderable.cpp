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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/RenderQueue/Renderable.h"
#include "RendererRuntime/Resource/Material/MaterialResourceManager.h"
#include "RendererRuntime/Core/SwizzleVectorElementRemove.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	Renderable::Renderable(const Renderer::IVertexArrayPtr& vertexArrayPtr, Renderer::PrimitiveTopology primitiveTopology, uint32_t startIndexLocation, uint32_t numberOfIndices, const MaterialResourceManager& materialResourceManager, MaterialResourceId materialResourceId) :
		// Derived data
		mSortingKey(getUninitialized<uint64_t>()),
		// Data
		mVertexArrayPtr(vertexArrayPtr),
		mPrimitiveTopology(primitiveTopology),
		mStartIndexLocation(startIndexLocation),
		mNumberOfIndices(numberOfIndices),
		mMaterialResourceId(getUninitialized<MaterialResourceId>()),
		// Cached material data
		mRenderQueueIndex(0),
		mCastShadows(false),
		// Internal data
		mMaterialResourceManager(nullptr),
		mMaterialResourceAttachmentIndex(getUninitialized<int>())
	{
		if (isInitialized(materialResourceId))
		{
			setMaterialResourceId(materialResourceManager, materialResourceId);
		}
		else
		{
			calculateSortingKey();
		}
	}

	void Renderable::setMaterialResourceId(const MaterialResourceManager& materialResourceManager, MaterialResourceId materialResourceId)
	{
		// State change?
		if (mMaterialResourceId != materialResourceId)
		{
			// Detach the renderable from the previous material resource, first
			unsetMaterialResourceIdInternal();

			// Renderables can only be attached to really existing material resources
			MaterialResource* materialResource = materialResourceManager.getMaterialResources().tryGetElementById(materialResourceId);
			if (nullptr != materialResource)
			{
				// Sanity checks
				assert(isUninitialized(mMaterialResourceAttachmentIndex));
				assert(nullptr == mMaterialResourceManager);

				// Attach the renderable from the material resource
				mMaterialResourceId = materialResourceId;
				mMaterialResourceManager = &materialResourceManager;
				mMaterialResourceAttachmentIndex = static_cast<int>(materialResource->mAttachedRenderables.size());
				materialResource->mAttachedRenderables.push_back(this);
			}
			else
			{
				// Error!
				assert(false);
			}

			// Calculate sorting key
			calculateSortingKey();
		}
		else
		{
			// Sanity check
			assert((isInitialized(mMaterialResourceId) && &materialResourceManager == mMaterialResourceManager) || (isUninitialized(mMaterialResourceId) && nullptr == mMaterialResourceManager));
		}
	}


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	void Renderable::calculateSortingKey()
	{
		// TODO(co) Implement me, "mVertexArrayPtr" and "mMaterialResourceId" have an influence

		// The quantized depth is a dynamic part which can't be set inside the cached sorting key (see "RendererRuntime::RenderQueue::addRenderablesFromRenderableManager()")
		mSortingKey = getUninitialized<uint64_t>();
	}

	void Renderable::unsetMaterialResourceIdInternal()
	{
		if (isInitialized(mMaterialResourceId))
		{
			// Sanity checks
			assert(nullptr != mMaterialResourceManager);
			assert(isInitialized(mMaterialResourceAttachmentIndex));

			// Get the material resource we're going to detach from
			MaterialResource& materialResource = mMaterialResourceManager->getMaterialResources().getElementById(mMaterialResourceId);

			// Sanity checks
			assert(mMaterialResourceAttachmentIndex < static_cast<int>(materialResource.mAttachedRenderables.size()));
			assert(this == *(materialResource.mAttachedRenderables.begin() + mMaterialResourceAttachmentIndex));
			assert(materialResource.getId() == mMaterialResourceId);

			// Detach the renderable from the material resource
			MaterialResource::AttachedRenderables::iterator iterator = materialResource.mAttachedRenderables.begin() + mMaterialResourceAttachmentIndex;
			iterator = ::detail::swizzleVectorElementRemove(materialResource.mAttachedRenderables, iterator);
			if (iterator != materialResource.mAttachedRenderables.end())
			{
				// The node that was at the end got swapped and has now a different index
				(*iterator)->mMaterialResourceAttachmentIndex = iterator - materialResource.mAttachedRenderables.begin();
			}
			setUninitialized(mMaterialResourceId);
			mMaterialResourceManager = nullptr;
			setUninitialized(mMaterialResourceAttachmentIndex);
		}
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
