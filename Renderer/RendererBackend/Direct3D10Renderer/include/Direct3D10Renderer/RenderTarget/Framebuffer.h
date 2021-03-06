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
#include <Renderer/RenderTarget/IFramebuffer.h>


//[-------------------------------------------------------]
//[ Forward declaration                                   ]
//[-------------------------------------------------------]
struct ID3D10Device;
struct ID3D10RenderTargetView;
struct ID3D10DepthStencilView;
namespace Direct3D10Renderer
{
	class Direct3D10Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 10 framebuffer class
	*/
	class Framebuffer : public Renderer::IFramebuffer
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D10Renderer
		*    Owner Direct3D 10 renderer instance
		*  @param[in] numberOfColorFramebufferAttachments
		*    Number of color render target textures
		*  @param[in] colorFramebufferAttachments
		*    The color render target textures, can be a null pointer or can contain null pointers, if not a null pointer there must be at
		*    least "numberOfColorTextures" textures in the provided C-array of pointers
		*  @param[in] depthStencilFramebufferAttachment
		*    The depth stencil render target texture, can be a null pointer
		*
		*  @note
		*    - The framebuffer keeps a reference to the provided texture instances
		*/
		Framebuffer(Direct3D10Renderer& direct3D10Renderer, uint32_t numberOfColorFramebufferAttachments, const Renderer::FramebufferAttachment* colorFramebufferAttachments, const Renderer::FramebufferAttachment* depthStencilFramebufferAttachment);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Framebuffer();

		/**
		*  @brief
		*    Return the number of color textures
		*
		*  @return
		*    The number of color textures
		*/
		inline uint32_t getNumberOfColorTextures() const;

		/**
		*  @brief
		*    Return the color textures
		*
		*  @return
		*    The color textures, can be a null pointer, do not release the returned instances unless you added an own reference to it
		*/
		inline Renderer::ITexture** getColorTextures() const;

		/**
		*  @brief
		*    Return the depth stencil texture
		*
		*  @return
		*    The depth stencil texture, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline Renderer::ITexture* getDepthStencilTexture() const;

		/**
		*  @brief
		*    Return the Direct3D 10 render target views
		*
		*  @return
		*    The Direct3D 10 render target views, can be a null pointer, do not release the returned instances unless you added an own reference to it
		*/
		inline ID3D10RenderTargetView** getD3D10RenderTargetViews() const;

		/**
		*  @brief
		*    Return the Direct3D 10 depth stencil view
		*
		*  @return
		*    The Direct3D 10 depth stencil view, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D10DepthStencilView* getD3D10DepthStencilView() const;

		/**
		*  @brief
		*    Return whether or not mipmaps should be generated automatically
		*
		*  @return
		*    "true" if mipmaps should be generated automatically, else "false"
		*/
		inline bool getGenerateMipmaps() const;

		/**
		*  @brief
		*    Generate mipmaps
		*
		*  @param[in] d3d10Device
		*    Direct3D 10 device to use
		*/
		void generateMipmaps(ID3D10Device& d3d10Device) const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char* name) override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	public:
		virtual void getWidthAndHeight(uint32_t& width, uint32_t& height) const override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit Framebuffer(const Framebuffer& source) = delete;
		Framebuffer& operator =(const Framebuffer& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Generic part
		uint32_t			 mNumberOfColorTextures;	///< Number of color render target textures
		Renderer::ITexture** mColorTextures;			///< The color render target textures (we keep a reference to it), can be a null pointer or can contain null pointers, if not a null pointer there must be at least "mNumberOfColorTextures" textures in the provided C-array of pointers
		Renderer::ITexture*  mDepthStencilTexture;		///< The depth stencil render target texture (we keep a reference to it), can be a null pointer
		uint32_t			 mWidth;					///< The framebuffer width
		uint32_t			 mHeight;					///< The framebuffer height
		bool				 mGenerateMipmaps;			///< "true" if mipmaps should be generated automatically, else "false"
		// Direct3D 10 part
		ID3D10RenderTargetView** mD3D10RenderTargetViews;	///< The Direct3D 10 render target views (we keep a reference to it), can be a null pointer or can contain null pointers, if not a null pointer there must be at least "mNumberOfColorTextures" views in the provided C-array of pointers
		ID3D10DepthStencilView*  mD3D10DepthStencilView;	///< The Direct3D 10 depth stencil view (we keep a reference to it), can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D10Renderer/RenderTarget/Framebuffer.inl"
