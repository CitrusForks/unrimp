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
#include "RendererRuntime/Core/GetUninitialized.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline AssetId SkyboxSceneItem::getMaterialAssetId() const
	{
		return mMaterialAssetId;
	}

	inline MaterialTechniqueId SkyboxSceneItem::getMaterialTechniqueId() const
	{
		return mMaterialTechniqueId;
	}

	inline AssetId SkyboxSceneItem::getMaterialBlueprintAssetId() const
	{
		return mMaterialBlueprintAssetId;
	}

	inline const MaterialProperties& SkyboxSceneItem::getMaterialProperties() const
	{
		return mMaterialProperties;
	}

	inline MaterialResourceId SkyboxSceneItem::getMaterialResourceId() const
	{
		return mMaterialResourceId;
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	inline SceneItemTypeId SkyboxSceneItem::getSceneItemTypeId() const
	{
		return TYPE_ID;
	}

	inline void SkyboxSceneItem::onDetachedFromSceneNode(SceneNode& sceneNode)
	{
		mRenderableManager.setTransform(nullptr);

		// Call the base implementation
		ISceneItem::onDetachedFromSceneNode(sceneNode);
	}

	inline void SkyboxSceneItem::setVisible(bool visible)
	{
		mRenderableManager.setVisible(visible);
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline SkyboxSceneItem::SkyboxSceneItem(SceneResource& sceneResource) :
		ISceneItem(sceneResource),
		mMaterialResourceId(getUninitialized<MaterialResourceId>())
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
