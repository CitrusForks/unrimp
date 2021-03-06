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
#include <Renderer/RenderTarget/ISwapChain.h>

#include "VulkanRenderer/VulkanRuntimeLinking.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{
	class VulkanRenderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace VulkanRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Vulkan swap chain class
	*/
	class SwapChain : public Renderer::ISwapChain
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] vulkanRenderer
		*    Owner Vulkan renderer instance
		*  @param[in] nativeWindowHandle
		*    Native window handle, must be valid
		*/
		SwapChain(VulkanRenderer& vulkanRenderer, handle nativeWindowHandle);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~SwapChain();

		/**
		*  @brief
		*    Return the Vulkan render pass
		*
		*  @return
		*    The Vulkan render pass
		*/
		inline VkRenderPass getVkRenderPass() const;

		/**
		*  @brief
		*    Return the current Vulkan image to render color into
		*
		*  @return
		*    The current Vulkan image to render color into
		*/
		inline VkImage getColorCurrentVkImage() const;

		/**
		*  @brief
		*    Return the Vulkan image to render depth into
		*
		*  @return
		*    The Vulkan image to render depth into
		*/
		inline VkImage getDepthVkImage() const;

		/**
		*  @brief
		*    Return the current Vulkan framebuffer to render into
		*
		*  @return
		*    The current Vulkan framebuffer to render into
		*/
		inline VkFramebuffer getCurrentVkFramebuffer() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IRenderTarget methods        ]
	//[-------------------------------------------------------]
	public:
		virtual void getWidthAndHeight(uint32_t& width, uint32_t& height) const override;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::ISwapChain methods           ]
	//[-------------------------------------------------------]
	public:
		virtual handle getNativeWindowHandle() const override;
		virtual void present() override;
		virtual void resizeBuffers() override;
		virtual bool getFullscreenState() const override;
		virtual void setFullscreenState(bool fullscreen) override;
		inline virtual void setRenderWindow(Renderer::IRenderWindow* renderWindow) override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit SwapChain(const SwapChain& source) = delete;
		SwapChain& operator =(const SwapChain& source) = delete;
		void createVulkanSwapChain();
		void destroyVulkanSwapChain();
		void acquireNextImage(bool recreateSwapChainIfNeeded);
		void createDepthRenderTarget(const VkExtent2D& vkExtent2D);
		void destroyDepthRenderTarget();


	//[-------------------------------------------------------]
	//[ Private definitions                                   ]
	//[-------------------------------------------------------]
	private:
		struct SwapChainBuffer
		{
			VkImage		  vkImage		= VK_NULL_HANDLE;	///< Vulkan image, don't destroy since we don't own it
			VkImageView   vkImageView	= VK_NULL_HANDLE;	///< Vulkan image view, destroy if no longer needed
			VkFramebuffer vkFramebuffer	= VK_NULL_HANDLE;	///< Vulkan framebuffer, destroy if no longer needed
		};
		typedef std::vector<SwapChainBuffer> SwapChainBuffers;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		// Operation system window
		handle					 mNativeWindowHandle;	///< Native window handle window, can be a null handle
		Renderer::IRenderWindow* mRenderWindow;			///< Render window instance, can be a null pointer, don't destroy the instance since we don't own it
		// Vulkan presentation surface
		VkSurfaceKHR mVkSurfaceKHR;	///< Vulkan presentation surface, destroy if no longer needed
		// Vulkan swap chain and color render target related
		VkSwapchainKHR	 mVkSwapchainKHR;				///< Vulkan swap chain, destroy if no longer needed
		VkRenderPass	 mVkRenderPass;					///< Vulkan render pass, destroy if no longer needed
		SwapChainBuffers mSwapChainBuffer;				///< Swap chain buffer for managing the color render targets
		VkSemaphore		 mImageAvailableVkSemaphore;	///< Vulkan semaphore, destroy if no longer needed
		VkSemaphore		 mRenderingFinishedVkSemaphore;	///< Vulkan semaphore, destroy if no longer needed
		uint32_t		 mCurrentImageIndex;			///< The index of the current Vulkan swap chain image to render into, ~0 if uninitialized
		// Depth render target related
		VkFormat		mDepthVkFormat;
		VkImage			mDepthVkImage;
		VkDeviceMemory  mDepthVkDeviceMemory;
		VkImageView		mDepthVkImageView;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // VulkanRenderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "VulkanRenderer/RenderTarget/SwapChain.inl"
