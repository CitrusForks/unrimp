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
#include "RendererRuntime/RenderQueue/RenderableManager.h"
#include "RendererRuntime/Resource/IResourceListener.h"
#include "RendererRuntime/Resource/Scene/Item/ISceneItem.h"
#include "RendererRuntime/Resource/Material/MaterialProperties.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId AssetId;				///< Asset identifier, internally just a POD "uint32_t", string ID scheme is "<project name>/<asset type>/<asset category>/<asset name>"
	typedef StringId MaterialTechniqueId;	///< Material technique identifier, internally just a POD "uint32_t", result of hashing the material technique name
	typedef uint32_t MaterialResourceId;	///< POD material resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Skybox scene item
	*/
	class SkyboxSceneItem : public ISceneItem, public IResourceListener
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class SceneFactory;	// Needs to be able to create scene item instances


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		RENDERERRUNTIME_API_EXPORT static const SceneItemTypeId TYPE_ID;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline AssetId getMaterialAssetId() const;
		inline MaterialTechniqueId getMaterialTechniqueId() const;
		inline AssetId getMaterialBlueprintAssetId() const;
		inline const MaterialProperties& getMaterialProperties() const;
		inline MaterialResourceId getMaterialResourceId() const;


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	public:
		inline virtual SceneItemTypeId getSceneItemTypeId() const override;
		virtual void deserialize(uint32_t numberOfBytes, const uint8_t* data) override;
		virtual void onAttachedToSceneNode(SceneNode& sceneNode) override;
		inline virtual void onDetachedFromSceneNode(SceneNode& sceneNode) override;
		inline virtual void setVisible(bool visible) override;
		virtual const RenderableManager* getRenderableManager() const override;


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::IResourceListener methods ]
	//[-------------------------------------------------------]
	protected:
		virtual void onLoadingStateChange(const IResource& resource) override;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline explicit SkyboxSceneItem(SceneResource& sceneResource);
		virtual ~SkyboxSceneItem();
		explicit SkyboxSceneItem(const SkyboxSceneItem&) = delete;
		SkyboxSceneItem& operator=(const SkyboxSceneItem&) = delete;
		void initialize();
		void createMaterialResource(MaterialResourceId parentMaterialResourceId);


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		AssetId				mMaterialAssetId;			///< If material blueprint asset ID is set, material asset ID must be uninitialized
		MaterialTechniqueId	mMaterialTechniqueId;		///< Must always be valid
		AssetId				mMaterialBlueprintAssetId;	///< If material asset ID is set, material blueprint asset ID must be uninitialized
		MaterialProperties	mMaterialProperties;
		MaterialResourceId  mMaterialResourceId;
		RenderableManager	mRenderableManager;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Scene/Item/Sky/SkyboxSceneItem.inl"
