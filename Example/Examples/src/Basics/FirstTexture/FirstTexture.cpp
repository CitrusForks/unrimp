/*********************************************************\
 * Copyright (c) 2012-2015 Christian Ofenberg
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
#include "Basics/FirstTexture/FirstTexture.h"
#include "Framework/Color4.h"

#include <float.h> // For FLT_MAX
#include <string.h>
#include <stdlib.h> // For rand()


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FirstTexture::FirstTexture(const char *rendererName) :
	IApplicationRenderer(rendererName),
	mTextureUnit(0)
{
	// Nothing to do in here
}

FirstTexture::~FirstTexture()
{
	// The resources are released within "onDeinitialization()"
	// Nothing to do in here
}


//[-------------------------------------------------------]
//[ Public virtual IApplication methods                   ]
//[-------------------------------------------------------]
void FirstTexture::onInitialization()
{
	// Call the base implementation
	IApplicationRenderer::onInitialization();

	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		{ // Create the texture
			static const uint32_t TEXTURE_WIDTH   = 128;
			static const uint32_t TEXTURE_HEIGHT  = 128;
			static const uint32_t NUMBER_OF_BYTES = TEXTURE_WIDTH * TEXTURE_HEIGHT * 4;

			// Allocate memory for the texture
			uint8_t *data = new uint8_t[NUMBER_OF_BYTES];

			// TODO(co) Be a little bit more creative while filling the texture data
			// Random content
			uint8_t *dataCurrent = data;
			for (uint32_t i = 0; i < TEXTURE_WIDTH * TEXTURE_HEIGHT; ++i)
			{
				*dataCurrent = static_cast<uint8_t>(rand() % 255);
				++dataCurrent;
				*dataCurrent = static_cast<uint8_t>(rand() % 255);
				++dataCurrent;
				*dataCurrent = static_cast<uint8_t>(rand() % 255);
				++dataCurrent;
				*dataCurrent = static_cast<uint8_t>(rand() % 255);
				++dataCurrent;
			}

			// Create the texture instance
			mTexture2D = renderer->createTexture2D(128, 128, Renderer::TextureFormat::R8G8B8A8, data, Renderer::TextureFlag::GENERATE_MIPMAPS);

			// Free texture memory
			delete [] data;
		}

		{ // Create sampler state
			Renderer::SamplerState samplerState = Renderer::ISamplerState::getDefaultSamplerState();
			samplerState.addressU = Renderer::TextureAddressMode::WRAP;
			samplerState.addressV = Renderer::TextureAddressMode::WRAP;
			mSamplerState = renderer->createSamplerState(samplerState);
		}

		// Decide which shader language should be used (for example "GLSL" or "HLSL")
		Renderer::IShaderLanguagePtr shaderLanguage(renderer->getShaderLanguage());
		if (nullptr != shaderLanguage)
		{
			// Create the program
			Renderer::IProgramPtr program;
			{
				// Get the shader source code (outsourced to keep an overview)
				const char *vertexShaderSourceCode = nullptr;
				const char *fragmentShaderSourceCode = nullptr;
				#include "FirstTexture_GLSL_110.h"
				#include "FirstTexture_GLSL_ES2.h"
				#include "FirstTexture_HLSL_D3D9.h"
				#include "FirstTexture_HLSL_D3D10_D3D11.h"
				#include "FirstTexture_Null.h"

				// Create the program
				program = shaderLanguage->createProgram(
					shaderLanguage->createVertexShaderFromSourceCode(vertexShaderSourceCode),
					shaderLanguage->createFragmentShaderFromSourceCode(fragmentShaderSourceCode));
			}

			// Is there a valid program?
			if (nullptr != program)
			{
				// Create the vertex buffer object (VBO)
				// -> Clip space vertex positions, left/bottom is (-1,-1) and right/top is (1,1)
				static const float VERTEX_POSITION[] =
				{					// Vertex ID	Triangle on screen
					 0.0f, 1.0f,	// 0				0
					 1.0f, 0.0f,	// 1			   .   .
					-0.5f, 0.0f		// 2			  2.......1
				};
				Renderer::IVertexBufferPtr vertexBuffer(renderer->createVertexBuffer(sizeof(VERTEX_POSITION), VERTEX_POSITION, Renderer::BufferUsage::STATIC_DRAW));

				// Vertex input layout
				const Renderer::VertexArrayAttribute vertexArrayAttributes[] =
				{
					{ // Attribute 0
						// Data destination
						Renderer::VertexArrayFormat::FLOAT_2,	// vertexArrayFormat (Renderer::VertexArrayFormat::Enum)
						"Position",								// name[32] (char)
						"POSITION",								// semanticName[32] (char)
						0,										// semanticIndex (uint32_t)
						// Data source
						0,										// inputSlot (size_t)
						0,										// alignedByteOffset (uint32_t)
						// Data source, instancing part
						0										// instancesPerElement (uint32_t)
					}
				};
				const uint32_t numberOfVertexAttributes = sizeof(vertexArrayAttributes) / sizeof(Renderer::VertexArrayAttribute);

				{ // Create the pipeline state object (PSO)
					// Setup
					Renderer::PipelineState pipelineState;
					pipelineState.program = program;
					pipelineState.numberOfVertexAttributes = numberOfVertexAttributes;
					pipelineState.vertexAttributes = vertexArrayAttributes;

					// Create the instance
					mPipelineState = renderer->createPipelineState(pipelineState);
				}

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
				mVertexArray = program->createVertexArray(numberOfVertexAttributes, vertexArrayAttributes, sizeof(vertexArrayVertexBuffers) / sizeof(Renderer::VertexArrayVertexBuffer), vertexArrayVertexBuffers);

				// Tell the renderer API which texture should be bound to which texture unit (texture unit 0 by default)
				// -> When using OpenGL or OpenGL ES 2 this is required
				// -> OpenGL 4.2 or the "GL_ARB_explicit_uniform_location"-extension supports explicit binding points ("layout(binding = 0)"
				//    in GLSL shader) , for backward compatibility we don't use it in here
				// -> When using Direct3D 9, Direct3D 10 or Direct3D 11, the texture unit
				//    to use is usually defined directly within the shader by using the "register"-keyword
				// -> Usually, this should only be done once during initialization, this example does this
				//    every frame to keep it local for better overview
				mTextureUnit = program->setTextureUnit(program->getUniformHandle("DiffuseMap"), 0);
			}
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}

void FirstTexture::onDeinitialization()
{
	// Begin debug event
	RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(getRenderer())

	// Release the used resources
	mVertexArray = nullptr;
	mPipelineState = nullptr;
	mSamplerState = nullptr;
	mTexture2D = nullptr;

	// End debug event
	RENDERER_END_DEBUG_EVENT(getRenderer())

	// Call the base implementation
	IApplicationRenderer::onDeinitialization();
}

void FirstTexture::onDraw()
{
	// Get and check the renderer instance
	Renderer::IRendererPtr renderer(getRenderer());
	if (nullptr != renderer && nullptr != mPipelineState)
	{
		// Begin debug event
		RENDERER_BEGIN_DEBUG_EVENT_FUNCTION(renderer)

		// Begin scene rendering
		// -> Required for Direct3D 9
		// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
		if (renderer->beginScene())
		{
			// Clear the color buffer of the current render target with gray, do also clear the depth buffer
			renderer->clear(Renderer::ClearFlag::COLOR_DEPTH, Color4::GRAY, 1.0f, 0);

			// Set the used pipeline state object (PSO)
			renderer->setPipelineState(mPipelineState);

			{ // Setup input assembly (IA)
				// Set the used vertex array
				renderer->iaSetVertexArray(mVertexArray);

				// Set the primitive topology used for draw calls
				renderer->iaSetPrimitiveTopology(Renderer::PrimitiveTopology::TRIANGLE_LIST);
			}

			{ // Set diffuse map
				// Set the used texture at the texture unit
				renderer->fsSetTexture(mTextureUnit, mTexture2D);

				// Set the used sampler state at the texture unit
				renderer->fsSetSamplerState(mTextureUnit, mSamplerState);
			}

			// Render the specified geometric primitive, based on an array of vertices
			renderer->draw(0, 3);

			// End scene rendering
			// -> Required for Direct3D 9
			// -> Not required for Direct3D 10, Direct3D 11, OpenGL and OpenGL ES 2
			renderer->endScene();
		}

		// End debug event
		RENDERER_END_DEBUG_EVENT(renderer)
	}
}
