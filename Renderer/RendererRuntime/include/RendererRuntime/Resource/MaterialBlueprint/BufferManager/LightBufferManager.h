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
#include "RendererRuntime/Core/Manager.h"

// Disable warnings in external headers, we can't fix them
PRAGMA_WARNING_PUSH
	PRAGMA_WARNING_DISABLE_MSVC(4201)	// warning C4201: nonstandard extension used: nameless struct/union
	PRAGMA_WARNING_DISABLE_MSVC(4464)	// warning C4464: relative include path contains '..'
	PRAGMA_WARNING_DISABLE_MSVC(4324)	// warning C4324: '<x>': structure was padded due to alignment specifier
	#include <glm/glm.hpp>
PRAGMA_WARNING_POP


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class CommandBuffer;
	class ITextureBuffer;
}
namespace RendererRuntime
{
	class SceneResource;
	class IRendererRuntime;
	class MaterialBlueprintResource;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef uint32_t TextureResourceId;	///< POD texture resource identifier


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Light buffer manager
	*
	*  @remarks
	*    The light buffer manager automatically generates some dynamic default texture assets one can reference e.g. inside material blueprint resources:
	*    - "Unrimp/Texture/DynamicByCode/LightClustersMap3D"
	*/
	class LightBufferManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] rendererRuntime
		*    Renderer runtime instance to use
		*/
		explicit LightBufferManager(IRendererRuntime& rendererRuntime);

		/**
		*  @brief
		*    Destructor
		*/
		~LightBufferManager();

		/**
		*  @brief
		*    Fill the light buffer
		*
		*  @param[in] sceneResource
		*    Scene resource to use
		*  @param[out] commandBuffer
		*    Command buffer to fill
		*/
		void fillBuffer(SceneResource& sceneResource, Renderer::CommandBuffer& commandBuffer);

		/**
		*  @brief
		*    Bind the light buffer manager into the given commando buffer
		*
		*  @param[in] materialBlueprintResource
		*    Material blueprint resource
		*  @param[out] commandBuffer
		*    Command buffer to fill
		*/
		void fillCommandBuffer(const MaterialBlueprintResource& materialBlueprintResource, Renderer::CommandBuffer& commandBuffer);

		/**
		*  @brief
		*    Get light clusters scale
		*
		*  @return
		*    Light clusters scale
		*/
		glm::vec3 getLightClustersScale() const;

		/**
		*  @brief
		*    Get light clusters bias
		*
		*  @return
		*    Light clusters bias
		*/
		glm::vec3 getLightClustersBias() const;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit LightBufferManager(const LightBufferManager&) = delete;
		LightBufferManager& operator=(const LightBufferManager&) = delete;
		void fillTextureBuffer(SceneResource& sceneResource, Renderer::CommandBuffer& commandBuffer);
		void fillClusters3DTexture(SceneResource& sceneResource, Renderer::CommandBuffer& commandBuffer);


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		typedef std::vector<uint8_t> ScratchBuffer;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		IRendererRuntime&		  mRendererRuntime;	///< Renderer runtime instance to use
		Renderer::ITextureBuffer* mTextureBuffer;	///< Texture buffer instance, always valid
		ScratchBuffer			  mTextureScratchBuffer;
		TextureResourceId		  mClusters3DTextureResourceId;
		glm::vec3				  mLightClustersAabbMinimum;
		glm::vec3				  mLightClustersAabbMaximum;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
