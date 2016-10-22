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
#include "PrecompiledHeader.h"
#include "Runtime/FirstScene/FirstScene.h"

#include <RendererRuntime/Vr/IVrManager.h>
#include <RendererRuntime/Core/Math/Transform.h>
#include <RendererRuntime/DebugGui/DebugGuiManager.h>
#include <RendererRuntime/Resource/Scene/ISceneResource.h>
#include <RendererRuntime/Resource/Scene/SceneResourceManager.h>
#include <RendererRuntime/Resource/Scene/Node/ISceneNode.h>
#include <RendererRuntime/Resource/Scene/Item/MeshSceneItem.h>
#include <RendererRuntime/Resource/Scene/Item/CameraSceneItem.h>
#include <RendererRuntime/Resource/CompositorWorkspace/CompositorWorkspaceInstance.h>
#include <RendererRuntime/Resource/MaterialBlueprint/MaterialBlueprintResourceManager.h>
#include <RendererRuntime/Resource/Material/MaterialResourceManager.h>

#include <imgui/imgui.h>


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
		static const RendererRuntime::AssetId MaterialAssetId("Example/Material/Character/Imrod");


//[-------------------------------------------------------]
//[ Anonymous detail namespace                            ]
//[-------------------------------------------------------]
	} // detail
}


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstScene::FirstScene(const char *rendererName) :
	IApplicationRendererRuntime(rendererName),
	mCompositorWorkspaceInstance(nullptr),
	mSceneResource(nullptr),
	mMaterialResourceId(RendererRuntime::getUninitialized<RendererRuntime::MaterialResourceId>()),
	mCloneMaterialResourceId(RendererRuntime::getUninitialized<RendererRuntime::MaterialResourceId>()),
	mCameraSceneItem(nullptr),
	mSceneNode(nullptr),
	mGlobalTimer(0.0f),
	mRotationSpeed(1.0f),
	mSunLightColor{1.0f, 1.0f, 1.0f},
	mWetness(1.0f),
	mPerformLighting(true),
	mUseDiffuseMap(true),
	mUseEmissiveMap(true),
	mUseNormalMap(true),
	mUseSpecularMap(true)
{
	// Nothing here
}

FirstScene::~FirstScene()
{
	// The resources are released within "onDeinitialization()"
	// Nothing here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstScene::onInitialization()
{
	// Call the base implementation
	IApplicationRendererRuntime::onInitialization();

	// Get and check the renderer runtime instance
	RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
	if (nullptr != rendererRuntime)
	{
		// Create the compositor workspace instance
		mCompositorWorkspaceInstance = new RendererRuntime::CompositorWorkspaceInstance(*rendererRuntime, "Example/CompositorWorkspace/Default/FirstScene");

		// Create the scene resource
		mSceneResource = rendererRuntime->getSceneResourceManager().loadSceneResourceByAssetId("Example/Scene/Default/FirstScene", this);

		// Load the material resource we're going to clone
		mMaterialResourceId = rendererRuntime->getMaterialResourceManager().loadMaterialResourceByAssetId(::detail::MaterialAssetId, this);
	}

	{ // Startup the VR-manager
		RendererRuntime::IVrManager& vrManager = rendererRuntime->getVrManager();
		if (vrManager.isHmdPresent())
		{
			vrManager.setSceneResource(mSceneResource);
			vrManager.startup();
		}
	}
}

void FirstScene::onDeinitialization()
{
	// Release the used resources
	delete mCompositorWorkspaceInstance;
	mCompositorWorkspaceInstance = nullptr;
	delete mSceneResource;

	// Call the base implementation
	IApplicationRendererRuntime::onDeinitialization();
}

void FirstScene::onUpdate()
{
	// Call the base implementation
	IApplicationRendererRuntime::onUpdate();

	// Stop the stopwatch
	mStopwatch.stop();

	{ // Tell the material blueprint resource manager about our global material properties
		RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
		if (nullptr != rendererRuntime)
		{
			RendererRuntime::MaterialProperties& globalMaterialProperties = rendererRuntime->getMaterialBlueprintResourceManager().getGlobalMaterialProperties();
			globalMaterialProperties.setPropertyById("SunLightColor", RendererRuntime::MaterialPropertyValue::fromFloat3(mSunLightColor));
			globalMaterialProperties.setPropertyById("Wetness", RendererRuntime::MaterialPropertyValue::fromFloat(mWetness));
		}
	}

	// Update the scene node rotation
	if (nullptr != mSceneNode && mRotationSpeed > 0.0f)
	{
		mSceneNode->setRotation(glm::angleAxis(mGlobalTimer, glm::vec3(0.0f, 1.0f, 0.0f)));

		// Update the global timer (FPS independent movement)
		mGlobalTimer += mStopwatch.getMilliseconds() * 0.0005f * mRotationSpeed;
	}

	// Start the stopwatch
	mStopwatch.start();

	// TODO(co) We need to get informed when the mesh scene item received the mesh resource loading finished signal
	trySetCustomMaterialResource();
}

void FirstScene::onDrawRequest()
{
	Renderer::IRenderer* renderer = getRenderer();
	if (nullptr != renderer)
	{
		Renderer::ISwapChain* swapChain = renderer->getMainSwapChain();
		if (nullptr != swapChain)
		{
			// Execute the compositor workspace instance
			if (nullptr != mCompositorWorkspaceInstance)
			{
				// Decide whether or not the VR-manager is used for rendering
				RendererRuntime::IVrManager& vrManager = mCompositorWorkspaceInstance->getRendererRuntime().getVrManager();
				if (vrManager.isRunning())
				{
					// No debug GUI in VR-mode for now

					// Update the VR-manager just before rendering
					vrManager.updateHmdMatrixPose();

					// Execute the compositor workspace instance
					vrManager.executeCompositorWorkspaceInstance(*mCompositorWorkspaceInstance, *swapChain, mCameraSceneItem);
				}
				else
				{
					createDebugGui(*swapChain);

					// Execute the compositor workspace instance
					mCompositorWorkspaceInstance->execute(*swapChain, mCameraSceneItem);
				}
			}
		}
	}
}


//[-------------------------------------------------------]
//[ Protected virtual RendererRuntime::IResourceListener methods ]
//[-------------------------------------------------------]
void FirstScene::onLoadingStateChange(const RendererRuntime::IResource& resource)
{
	const RendererRuntime::IResource::LoadingState loadingState = resource.getLoadingState();
	if (&resource == mSceneResource)
	{
		if (RendererRuntime::IResource::LoadingState::LOADED == loadingState)
		{
			// Loop through all scene nodes and grab the first found camera and mesh
			for (RendererRuntime::ISceneNode* sceneNode : mSceneResource->getSceneNodes())
			{
				// Loop through all scene items attached to the current scene node
				for (RendererRuntime::ISceneItem* sceneItem : sceneNode->getAttachedSceneItems())
				{
					if (sceneItem->getSceneItemTypeId() == RendererRuntime::MeshSceneItem::TYPE_ID)
					{
						// Grab the first found mesh scene item scene node
						if (nullptr == mSceneNode)
						{
							mSceneNode = sceneNode;
							trySetCustomMaterialResource();
						}
					}
					else if (sceneItem->getSceneItemTypeId() == RendererRuntime::CameraSceneItem::TYPE_ID)
					{
						// Grab the first found camera scene item
						if (nullptr == mCameraSceneItem)
						{
							mCameraSceneItem = static_cast<RendererRuntime::CameraSceneItem*>(sceneItem);
						}
					}
				}
			}
		}
		else
		{
			mCameraSceneItem = nullptr;
			mSceneNode = nullptr;
		}
	}
	else if (RendererRuntime::IResource::LoadingState::LOADED == loadingState && resource.getAssetId() == ::detail::MaterialAssetId)
	{
		// Create our material resource clone
		RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
		if (nullptr != rendererRuntime)
		{
			mCloneMaterialResourceId = rendererRuntime->getMaterialResourceManager().createMaterialResourceByCloning(resource.getId());
			trySetCustomMaterialResource();
		}
	}
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
void FirstScene::createDebugGui(Renderer::IRenderTarget& renderTarget)
{
	if (nullptr != mCompositorWorkspaceInstance && nullptr != mSceneResource)
	{
		mSceneResource->getRendererRuntime().getDebugGuiManager().newFrame(renderTarget);
		ImGui::Begin("Options");
			// Scene
			ImGui::SliderFloat("Rotation Speed", &mRotationSpeed, 0.0f, 2.0f, "%.3f");

			// Global material properties
			ImGui::ColorEdit3("Sun Light Color", mSunLightColor);
			ImGui::SliderFloat("Wetness", &mWetness, 0.0f, 2.0f, "%.3f");

			{ // Material properties
				ImGui::Checkbox("Perform Lighting", &mPerformLighting);
				ImGui::Checkbox("Use Diffuse Map", &mUseDiffuseMap);
				ImGui::Checkbox("Use Emissive Map", &mUseEmissiveMap);
				ImGui::Checkbox("Use Normal Map", &mUseNormalMap);
				ImGui::Checkbox("Use Specular Map", &mUseSpecularMap);

				// Tell the material resource instance
				RendererRuntime::IRendererRuntime* rendererRuntime = getRendererRuntime();
				if (nullptr != rendererRuntime)
				{
					const RendererRuntime::MaterialResources& materialResources = rendererRuntime->getMaterialResourceManager().getMaterialResources();
					RendererRuntime::MaterialResource* materialResource = materialResources.tryGetElementById(mMaterialResourceId);
					if (nullptr != materialResource)
					{
						materialResource->setPropertyById("Lighting", RendererRuntime::MaterialPropertyValue::fromBoolean(mPerformLighting));
					}
					materialResource = materialResources.tryGetElementById(mCloneMaterialResourceId);
					if (nullptr != materialResource)
					{
						materialResource->setPropertyById("UseDiffuseMap", RendererRuntime::MaterialPropertyValue::fromBoolean(mUseDiffuseMap));
						materialResource->setPropertyById("UseEmissiveMap", RendererRuntime::MaterialPropertyValue::fromBoolean(mUseEmissiveMap));
						materialResource->setPropertyById("UseNormalMap", RendererRuntime::MaterialPropertyValue::fromBoolean(mUseNormalMap));
						materialResource->setPropertyById("UseSpecularMap", RendererRuntime::MaterialPropertyValue::fromBoolean(mUseSpecularMap));
					}
				}
			}
		ImGui::End();
	}
}

void FirstScene::trySetCustomMaterialResource()
{
	if (nullptr != mSceneNode && RendererRuntime::isInitialized(mCloneMaterialResourceId))
	{
		for (RendererRuntime::ISceneItem* sceneItem : mSceneNode->getAttachedSceneItems())
		{
			if (sceneItem->getSceneItemTypeId() == RendererRuntime::MeshSceneItem::TYPE_ID)
			{
				// Tell the mesh scene item about our custom material resource
				RendererRuntime::MeshSceneItem* meshSceneItem = static_cast<RendererRuntime::MeshSceneItem*>(sceneItem);
				for (uint32_t subMeshIndex = 0; subMeshIndex < meshSceneItem->getNumberOfSubMeshes(); ++subMeshIndex)
				{
					meshSceneItem->setMaterialResourceIdOfSubMesh(subMeshIndex, mCloneMaterialResourceId);
				}
			}
		}
	}
}
