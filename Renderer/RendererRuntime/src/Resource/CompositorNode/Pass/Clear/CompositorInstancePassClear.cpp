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
#include "RendererRuntime/Resource/CompositorNode/Pass/Clear/CompositorInstancePassClear.h"
#include "RendererRuntime/Resource/CompositorNode/Pass/Clear/CompositorResourcePassClear.h"

#include <Renderer/Public/Renderer.h>


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Protected virtual RendererRuntime::ICompositorInstancePass methods ]
	//[-------------------------------------------------------]
	void CompositorInstancePassClear::onFillCommandBuffer(const Renderer::IRenderTarget&, const CompositorContextData&, Renderer::CommandBuffer& commandBuffer)
	{
		// Begin debug event
		COMMAND_BEGIN_DEBUG_EVENT_FUNCTION(commandBuffer)

		// Push the clear command
		const CompositorResourcePassClear& compositorResourcePassClear = static_cast<const CompositorResourcePassClear&>(getCompositorResourcePass());
		Renderer::Command::Clear::create(commandBuffer, compositorResourcePassClear.getFlags(), glm::value_ptr(compositorResourcePassClear.getClearColor()), compositorResourcePassClear.getZ(), compositorResourcePassClear.getStencil());

		// End debug event
		COMMAND_END_DEBUG_EVENT(commandBuffer)
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	CompositorInstancePassClear::CompositorInstancePassClear(const CompositorResourcePassClear& compositorResourcePassClear, const CompositorNodeInstance& compositorNodeInstance) :
		ICompositorInstancePass(compositorResourcePassClear, compositorNodeInstance)
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
