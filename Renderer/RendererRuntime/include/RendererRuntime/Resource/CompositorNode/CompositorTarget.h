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
#include "RendererRuntime/Core/StringId.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class ICompositorPassFactory;
	class ICompositorResourcePass;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Global definitions                                    ]
	//[-------------------------------------------------------]
	typedef StringId CompositorChannelId;		///< Compositor channel identifier, internally just a POD "uint32_t"
	typedef StringId CompositorFramebufferId;	///< Compositor framebuffer identifier, internally just a POD "uint32_t"
	typedef StringId CompositorPassTypeId;		///< Compositor pass type identifier, internally just a POD "uint32_t"


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Compositor node resource target
	*
	*  @note
	*    - Named in short just compositor target because there's no instance variant
	*    - Render target might be compositor channel (external interconnection) or compositor framebuffer (node internal processing)
	*/
	class CompositorTarget
	{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<ICompositorResourcePass*> CompositorResourcePasses;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		inline explicit CompositorTarget(CompositorChannelId compositorChannelId, CompositorFramebufferId compositorFramebufferId);
		inline explicit CompositorTarget(const CompositorTarget& compositorTarget);
		inline ~CompositorTarget();

		inline CompositorChannelId getCompositorChannelId() const;
		inline CompositorFramebufferId getCompositorFramebufferId() const;

		//[-------------------------------------------------------]
		//[ Passes                                                ]
		//[-------------------------------------------------------]
		inline void setNumberOfCompositorResourcePasses(uint32_t numberOfCompositorResourcePasses);
		ICompositorResourcePass* addCompositorResourcePass(const ICompositorPassFactory& compositorPassFactory, CompositorPassTypeId compositorPassTypeId);
		inline const CompositorResourcePasses& getCompositorResourcePasses() const;
		void removeAllCompositorResourcePasses();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		CompositorTarget& operator=(const CompositorTarget&) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		CompositorChannelId		 mCompositorChannelId;
		CompositorFramebufferId	 mCompositorFramebufferId;
		CompositorResourcePasses mCompositorResourcePasses;	///< We're responsible for destroying the instances if we no longer need them


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/CompositorNode/CompositorTarget.inl"
