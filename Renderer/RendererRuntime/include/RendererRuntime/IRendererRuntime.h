/*********************************************************\
 * Copyright (c) 2012-2014 Christian Ofenberg
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
#ifndef __RENDERERRUNTIME_IRENDERERRUNTIME_H__
#define __RENDERERRUNTIME_IRENDERERRUNTIME_H__


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IRenderer;
}
namespace RendererRuntime
{
	class IFont;
	class AssetManager;
	class SceneManager;
	class IRendererRuntime;
	class CompositorManager;
	class FontResourceManager;
	class MeshResourceManager;
	class TextureResourceManager;
	class MaterialResourceManager;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract renderer runtime interface
	*/
	class IRendererRuntime : public Renderer::RefCount<IRendererRuntime>
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IRendererRuntime();

		/**
		*  @brief
		*    Return the used renderer instance
		*
		*  @return
		*    The used renderer instance, do not release the returned instance unless you added an own reference to it
		*/
		inline Renderer::IRenderer& getRenderer() const;

		//[-------------------------------------------------------]
		//[ Managers                                              ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the asset manager instance
		*
		*  @return
		*    The asset manager instance, do not release the returned instance
		*/
		inline AssetManager& getAssetManager() const;

		/**
		*  @brief
		*    Return the compositor manager instance
		*
		*  @return
		*    The compositor manager instance, do not release the returned instance
		*/
		inline CompositorManager& getCompositorManager() const;

		/**
		*  @brief
		*    Return the scene manager instance
		*
		*  @return
		*    The scene manager instance, do not release the returned instance
		*/
		inline SceneManager& getSceneManager() const;

		//[-------------------------------------------------------]
		//[ Resource managers                                     ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Return the font resource manager instance
		*
		*  @return
		*    The font resource manager instance, do not release the returned instance
		*/
		inline FontResourceManager& getFontResourceManager() const;

		/**
		*  @brief
		*    Return the texture resource manager instance
		*
		*  @return
		*    The texture resource manager instance, do not release the returned instance
		*/
		inline TextureResourceManager& getTextureResourceManager() const;

		/**
		*  @brief
		*    Return the material resource manager instance
		*
		*  @return
		*    The material resource manager instance, do not release the returned instance
		*/
		inline MaterialResourceManager& getMaterialResourceManager() const;

		/**
		*  @brief
		*    Return the mesh resource manager instance
		*
		*  @return
		*    The mesh resource manager instance, do not release the returned instance
		*/
		inline MeshResourceManager& getMeshResourceManager() const;


	//[-------------------------------------------------------]
	//[ Public virtual IRendererRuntime methods               ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Create a font instance
		*
		*  @param[in] filename
		*    The ASCII font filename, if null pointer or empty string a null pointer will be returned
		*
		*  @return
		*    The created font instance, a null pointer on error. Release the returned instance if you no longer need it.
		*/
		virtual IFont *createFont(const char *filename) = 0;


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Default constructor
		*/
		inline IRendererRuntime();

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit IRendererRuntime(const IRendererRuntime &source);

		/**
		*  @brief
		*    Copy operator
		*
		*  @param[in] source
		*    Source to copy from
		*
		*  @return
		*    Reference to this instance
		*/
		inline IRendererRuntime &operator =(const IRendererRuntime &source);


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		Renderer::IRenderer* mRenderer;	///< The used renderer instance (we keep a reference to it), always valid
		// Managers
		AssetManager*	   mAssetManager;
		CompositorManager* mCompositorManager;
		SceneManager*	   mSceneManager;
		// Resource managers
		FontResourceManager*	 mFontResourceManager;
		TextureResourceManager*	 mTextureResourceManager;
		MaterialResourceManager* mMaterialResourceManager;
		MeshResourceManager*	 mMeshResourceManager;


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef Renderer::SmartRefCount<IRendererRuntime> IRendererRuntimePtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/IRendererRuntime.inl"


//[-------------------------------------------------------]
//[ Header guard                                          ]
//[-------------------------------------------------------]
#endif // __RENDERERRUNTIME_IRENDERERRUNTIME_H__
