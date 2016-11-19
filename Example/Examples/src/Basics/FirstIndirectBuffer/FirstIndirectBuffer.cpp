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
#include "Basics/FirstIndirectBuffer/FirstIndirectBuffer.h"
#include "Framework/Color4.h"

#include <string.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstIndirectBuffer::FirstIndirectBuffer(const char *rendererName) :
	FirstTriangle(rendererName)
{
	// Nothing here
}

FirstIndirectBuffer::~FirstIndirectBuffer()
{
	// The resources are released within "onDeinitialization()"
	// Nothing here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstIndirectBuffer::onInitialization()
{
	// Call the base implementation
	FirstTriangle::onInitialization();

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		{ // Create the indirect buffer
			const Renderer::DrawInstancedArguments drawInstancedArguments =
			{
				3,	// vertexCountPerInstance (uint32_t)
				1,	// instanceCount (uint32_t)
				0,	// startVertexLocation (uint32_t)
				0	// startInstanceLocation (uint32_t)
			};
			mIndirectBuffer = mBufferManager->createIndirectBuffer(sizeof(Renderer::DrawInstancedArguments), &drawInstancedArguments, Renderer::BufferUsage::STATIC_DRAW);
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstIndirectBuffer::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used resources
	mIndirectBuffer = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	FirstTriangle::onDeinitialization();
}

void FirstIndirectBuffer::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mPipelineState)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Clear the color buffer of the current render target with gray, do also clear the depth buffer
		renderer->clear(Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

		// Set the used graphics root signature
		renderer->setGraphicsRootSignature(mRootSignature);

		// Set the used pipeline state object (PSO)
		renderer->setPipelineState(mPipelineState);

		{ // Setup input assembly (IA)
			// Set the used vertex array
			renderer->iaSetVertexArray(mVertexArray);

			// Set the primitive topology used for draw calls
			renderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
		}

		// Set debug marker
		// -> Debug methods: When using Direct3D <11.1, these methods map to the Direct3D 9 PIX functions
		//    (D3DPERF_* functions, also works directly within VisualStudio 2012 out-of-the-box)
		RENDERER_SET_DEBUG_MARKER(renderer, L"Everyone ready for the upcoming triangle?")

		{
			// Begin debug event
			RENDERER_BEGIN_DEBUG_EVENT(renderer, L"Drawing the fancy triangle")

			// Render the specified geometric primitive, based on an array of vertices
			renderer->draw(*mIndirectBuffer);

			// End debug event
			RENDERER_END_DEBUG_EVENT(renderer)
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}
