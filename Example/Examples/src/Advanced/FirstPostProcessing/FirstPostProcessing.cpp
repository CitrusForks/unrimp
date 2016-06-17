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
#include "Advanced/FirstPostProcessing/FirstPostProcessing.h"
#include "Framework/Color4.h"

#include <string.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstPostProcessing::FirstPostProcessing(const char *rendererName) :
	IApplicationRenderer(rendererName)
{
	// Nothing to do in here
}

FirstPostProcessing::~FirstPostProcessing()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstPostProcessing::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Create the framebuffer object (FBO) instance by using the current window size
		recreateFramebuffer();

		{ // Create sampler state: We don't use mipmaps
			Renderer::SamplerState samplerState = Renderer::ISamplerState::getDefaultSamplerState();
			samplerState.maxLOD = 0.0f;
			mSamplerState = renderer->createSamplerState(samplerState);
		}

		{ // Create the root signature
			Renderer::DescriptorRangeBuilder ranges[2];
			ranges[0].initializeSampler(1, 0);
			ranges[1].initialize(Renderer::DescriptorRangeType::SRV, 1, 0, "DiffuseMap", 0);

			Renderer::RootParameterBuilder rootParameters[2];
			rootParameters[0].initializeAsDescriptorTable(1, &ranges[0], Renderer::ShaderVisibility::FRAGMENT);
			rootParameters[1].initializeAsDescriptorTable(1, &ranges[1], Renderer::ShaderVisibility::FRAGMENT);

			// Setup
			Renderer::RootSignatureBuilder rootSignature;
			rootSignature.initialize(glm::countof(rootParameters), rootParameters, 0, nullptr, Renderer::RootSignatureFlags::ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			// Create the instance
			mRootSignature = renderer->createRootSignature(rootSignature);
		}

		// Vertex input layout
		const Renderer::VertexAttribute vertexAttributesLayout[] =
		{
			{ // Attribute 0
				// Data destination
				Renderer::VertexAttributeFormat::FLOAT_2,	// vertexAttributeFormat (Renderer::VertexAttributeFormat)
				"Position",									// name[32] (char)
				"POSITION",									// semanticName[32] (char)
				0,											// semanticIndex (uint32_t)
				// Data source
				0,											// inputSlot (uint32_t)
				0,											// alignedByteOffset (uint32_t)
				// Data source, instancing part
				0											// instancesPerElement (uint32_t)
			}
		};
		const Renderer::VertexAttributes vertexAttributes(glm::countof(vertexAttributesLayout), vertexAttributesLayout);

		{ // Create vertex array object (VAO) for scene rendering
			// Create the vertex buffer object (VBO)
			// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
			static const float VERTEX_POSITION[] =
			{					// Vertex ID	Triangle on screen
				 0.0f, 1.0f,	// 0				0
				 1.0f, 0.0f,	// 1			   .   .
				-0.5f, 0.0f		// 2			  2.......1
			};
			Renderer::IVertexBufferPtr vertexBuffer(renderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

			// Create vertex array object (VAO)
			// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
			// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
			// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
			//    reference of the used vertex buffer objects (VBO). If the reference counter of a
			//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
			const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
			{
				{ // Vertex buffer 0
					vertexBuffer,		// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 2	// strideInBytes (uint32_t)
				}
			};
			mVertexArraySceneRendering = renderer->createVertexArray(vertexAttributes, glm::countof(vertexArrayVertexBuffers), vertexArrayVertexBuffers);
		}

		{ // Create vertex array object (VAO) for post-processing
			// Create the vertex buffer object (VBO)
			// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
			static const float VERTEX_POSITION[] =
			{					// Vertex ID	Triangle strip on screen
				-1.0f, -1.0f,	// 0			  1.......3
				-1.0f,  1.0f,	// 1			  .	  .   .
				 1.0f, -1.0f,	// 2			  0.......2
				 1.0f,  1.0f	// 3
			};
			Renderer::IVertexBufferPtr vertexBuffer(renderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

			// Create vertex array object (VAO)
			// -> The vertex array object (VAO) keeps a reference to the used vertex buffer object (VBO)
			// -> This means that there's no need to keep an own vertex buffer object (VBO) reference
			// -> When the vertex array object (VAO) is destroyed, it automatically decreases the
			//    reference of the used vertex buffer objects (VBO). If the reference counter of a
			//    vertex buffer object (VBO) reaches zero, it's automatically destroyed.
			const Renderer::VertexArrayVertexBuffer vertexArrayVertexBuffers[] =
			{
				{ // Vertex buffer 0
					vertexBuffer,		// vertexBuffer (Renderer::IVertexBuffer *)
					sizeof(float) * 2	// strideInBytes (uint32_t)
				}
			};
			mVertexArrayPostProcessing = renderer->createVertexArray(vertexAttributes, glm::countof(vertexArrayVertexBuffers), vertexArrayVertexBuffers);
		}

		// Create the programs: Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// Create the programs
			Renderer::IProgramPtr programSceneRendering;
			Renderer::IProgramPtr programPostProcessing;
			{
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode_SceneRendering = nullptr;
				const char *fragmentShaderSourceCode_PostProcessing = nullptr;
				#include "FirstPostProcessing_GLSL_110.h"
				#include "FirstPostProcessing_GLSL_ES2.h"
				#include "FirstPostProcessing_HLSL_D3D9.h"
				#include "FirstPostProcessing_HLSL_D3D10_D3D11_D3D12.h"
				#include "FirstPostProcessing_Null.h"

				// In order to keep this example simple and to show that it's possible, we use the same vertex shader for both programs
				// -> Depending on the used graphics API and whether or not the shader compiler & linker is clever,
				//    the unused texture coordinate might get optimized out
				// -> In a real world application you shouldn't rely on shader compiler & linker behaviour assumptions
				Renderer::IVertexShaderPtr vertexShader(shaderLanguage->createVertexShaderFromSourceCode(vertexShaderSourceCode));
				programSceneRendering = shaderLanguage->createProgram(*mRootSignature, vertexAttributes, vertexShader, shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode_SceneRendering));
				programPostProcessing = shaderLanguage->createProgram(*mRootSignature, vertexAttributes, vertexShader, shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode_PostProcessing));
			}

			// Create the pipeline state objects (PSO)
			if (nullptr != programSceneRendering && nullptr != programPostProcessing)
			{
				{ // Scene rendering
					Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder(mRootSignature, programSceneRendering, vertexAttributes);
					pipelineState.depthStencilState.depthEnable = false;
					mPipelineStateSceneRendering = renderer->createPipelineState(pipelineState);
				}
				{ // Post-processing
					Renderer::PipelineState pipelineState = Renderer::PipelineStateBuilder(mRootSignature, programPostProcessing, vertexAttributes);
					pipelineState.depthStencilState.depthEnable = false;
					mPipelineStatePostProcessing = renderer->createPipelineState(pipelineState);
				}
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstPostProcessing::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used resources
	mVertexArrayPostProcessing = nullptr;
	mPipelineStatePostProcessing = nullptr;
	mVertexArraySceneRendering = nullptr;
	mPipelineStateSceneRendering = nullptr;
	mRootSignature = nullptr;
	mSamplerState = nullptr;
	mFramebuffer = nullptr;
	mTexture2D = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRenderer::onDeinitialization();
}

void FirstPostProcessing::onResize()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Call the base implementation
	IApplicationRenderer::onResize();

	// Recreate the framebuffer object (FBO) instance by using the current window size
	recreateFramebuffer();

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())
}

void FirstPostProcessing::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Scene rendering
		sceneRendering();

		// Post-processing
		postProcessing();

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
void FirstPostProcessing::recreateFramebuffer()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Get the window size
		int width  = 0;
		int height = 0;
		getWindowSize(width, height);

		// Due to the usage of smart pointers there's no need to explicitly free the previous resources, this is done automatically

		// Create the texture instance, but without providing texture data (we use the texture as render target)
		// -> Use the "Renderer::TextureFlag::RENDER_TARGET"-flag to mark this texture as a render target
		// -> Required for Direct3D 9, Direct3D 10, Direct3D 11 and Direct3D 12
		// -> Not required for OpenGL and OpenGL ES 2
		// -> The optimized texture clear value is a Direct3D 12 related option
		Renderer::ITexture *texture2D = mTexture2D = renderer->createTexture2D(static_cast<uint32_t>(width), static_cast<uint32_t>(height), Renderer::TextureFormat::R8G8B8A8, nullptr, Renderer::TextureFlag::RENDER_TARGET, Renderer::TextureUsage::DEFAULT, reinterpret_cast<const Renderer::OptimizedTextureClearValue*>(&Color4::BLUE));

		// Create the framebuffer object (FBO) instance
		mFramebuffer = renderer->createFramebuffer(1, &texture2D);

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstPostProcessing::sceneRendering()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mPipelineStateSceneRendering && nullptr != mPipelineStatePostProcessing)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// This in here is of course just an example. In a real application
		// there would be no point in constantly updating texture content
		// without having any real change.

		// TODO(co) Unbind our texture from the texture unit before rendering into it
		// -> Direct3D 9, OpenGL and OpenGL ES 2 don't mind as long as the texture is not used inside the shader while rendering into it
		// -> Direct3D 10 & 11 go crazy if you're going to render into a texture which is still bound at a texture unit:
		//    "D3D11: WARNING: ID3D11DeviceContext::OMSetRenderTargets: Resource being set to OM RenderTarget slot 0 is still bound on input! [ STATE_SETTING WARNING #9: DEVICE_OMSETRENDERTARGETS_HAZARD ]"
		//    "D3D11: WARNING: ID3D11DeviceContext::OMSetRenderTargets[AndUnorderedAccessViews]: Forcing PS shader resource slot 0 to NULL. [ STATE_SETTING WARNING #7: DEVICE_PSSETSHADERRESOURCES_HAZARD ]"

		// Backup the currently used render target
		Renderer::IRenderTargetPtr renderTarget(renderer->omGetRenderTarget());

		// Set the render target to render into
		renderer->omSetRenderTarget(mFramebuffer);

		// Clear the color buffer of the current render target with blue
		renderer->clear(Renderer::ClearFlag::COLOR, Color4::BLUE, 1.0f, 0);

		// Set the used graphics root signature
		renderer->setGraphicsRootSignature(mRootSignature);

		// Set the used pipeline state object (PSO)
		renderer->setPipelineState(mPipelineStateSceneRendering);

		{ // Setup input assembly (IA)
			// Set the used vertex array
			renderer->iaSetVertexArray(mVertexArraySceneRendering);

			// Set the primitive topology used for draw calls
			renderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
		}

		// Render the specified geometric primitive, based on indexing into an array of vertices
		renderer->draw(0, 3);

		// Restore the previously set render target
		renderer->omSetRenderTarget(renderTarget);

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstPostProcessing::postProcessing()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && mPipelineStatePostProcessing)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// We don't need to clear the current render target because our fullscreen quad covers the full screen

		// Set the used graphics root signature
		renderer->setGraphicsRootSignature(mRootSignature);

		// Set diffuse map
		renderer->setGraphicsRootDescriptorTable(0, mSamplerState);
		renderer->setGraphicsRootDescriptorTable(1, mTexture2D);

		// Set the used pipeline state object (PSO)
		renderer->setPipelineState(mPipelineStatePostProcessing);

		{ // Setup input assembly (IA)
			// Set the used vertex array
			renderer->iaSetVertexArray(mVertexArrayPostProcessing);

			// Set the primitive topology used for draw calls
			renderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_STRIP);
		}

		// Render the specified geometric primitive, based on indexing into an array of vertices
		renderer->draw(0, 4);

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}
