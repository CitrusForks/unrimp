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
#include "OpenGLRenderer/Buffer/VertexArrayVaoDsa.h"
#include "OpenGLRenderer/Buffer/IndexBuffer.h"
#include "OpenGLRenderer/Buffer/VertexBuffer.h"
#include "OpenGLRenderer/Mapping.h"
#include "OpenGLRenderer/Extensions.h"
#include "OpenGLRenderer/OpenGLRenderer.h"
#include "OpenGLRenderer/OpenGLRuntimeLinking.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	VertexArrayVaoDsa::VertexArrayVaoDsa(OpenGLRenderer& openGLRenderer, const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, IndexBuffer* indexBuffer) :
		VertexArrayVao(openGLRenderer, numberOfVertexBuffers, vertexBuffers, indexBuffer)
	{
		// Vertex buffer reference handling is done within the base class "VertexArrayVao"
		const bool isArbDsa = openGLRenderer.getExtensions().isGL_ARB_direct_state_access();
		if (isArbDsa)
		{
			// Create the OpenGL vertex array
			glCreateVertexArrays(1, &mOpenGLVertexArray);
		}
		else
		{
			// Create the OpenGL vertex array
			glGenVertexArrays(1, &mOpenGLVertexArray);
		}

		// Loop through all attributes
		// -> We're using "glBindAttribLocationARB()" when linking the program so we have known attribute locations (the vertex array can't know about the program)
		GLuint attributeLocation = 0;
		const Renderer::VertexAttribute* attributeEnd = vertexAttributes.attributes + vertexAttributes.numberOfAttributes;
		for (const Renderer::VertexAttribute* attribute = vertexAttributes.attributes; attribute < attributeEnd; ++attribute, ++attributeLocation)
		{
			// Set the OpenGL vertex attribute pointer
			// TODO(co) Add security check: Is the given resource one of the currently used renderer?
			const Renderer::VertexArrayVertexBuffer& vertexArrayVertexBuffer = vertexBuffers[attribute->inputSlot];

			if (isArbDsa)
			{
				// Enable attribute
				glEnableVertexArrayAttrib(mOpenGLVertexArray, attributeLocation);

				// Set up the format for my attribute
				if (Mapping::isOpenGLVertexAttributeFormatInteger(attribute->vertexAttributeFormat))
				{
					glVertexArrayAttribIFormat(mOpenGLVertexArray, attributeLocation, Mapping::getOpenGLSize(attribute->vertexAttributeFormat), Mapping::getOpenGLType(attribute->vertexAttributeFormat), static_cast<GLuint>(attribute->alignedByteOffset));
				}
				else
				{
					glVertexArrayAttribFormat(mOpenGLVertexArray, attributeLocation, Mapping::getOpenGLSize(attribute->vertexAttributeFormat), Mapping::getOpenGLType(attribute->vertexAttributeFormat), static_cast<GLboolean>(Mapping::isOpenGLVertexAttributeFormatNormalized(attribute->vertexAttributeFormat)), static_cast<GLuint>(attribute->alignedByteOffset));
				}
				glVertexArrayAttribBinding(mOpenGLVertexArray, attributeLocation, attributeLocation);

				// Bind vertex buffer to buffer point
				glVertexArrayVertexBuffer(mOpenGLVertexArray,
										  attributeLocation,
										  static_cast<VertexBuffer*>(vertexArrayVertexBuffer.vertexBuffer)->getOpenGLArrayBuffer(),
										  0,	// No offset to the first element of the buffer
										  static_cast<GLsizei>(attribute->strideInBytes));

				// Per-instance instead of per-vertex requires "GL_ARB_instanced_arrays"
				if (attribute->instancesPerElement > 0 && openGLRenderer.getExtensions().isGL_ARB_instanced_arrays())
				{
					glVertexArrayBindingDivisor(mOpenGLVertexArray, attributeLocation, attribute->instancesPerElement);
				}
			}
			else
			{
				glVertexArrayVertexAttribOffsetEXT(mOpenGLVertexArray,
												  static_cast<VertexBuffer*>(vertexArrayVertexBuffer.vertexBuffer)->getOpenGLArrayBuffer(),
												  attributeLocation, Mapping::getOpenGLSize(attribute->vertexAttributeFormat),
												  Mapping::getOpenGLType(attribute->vertexAttributeFormat),
												  static_cast<GLboolean>(Mapping::isOpenGLVertexAttributeFormatNormalized(attribute->vertexAttributeFormat)),
												  static_cast<GLsizei>(attribute->strideInBytes),
												  static_cast<GLintptr>(attribute->alignedByteOffset));

				// Per-instance instead of per-vertex requires "GL_ARB_instanced_arrays"
				if (attribute->instancesPerElement > 0 && openGLRenderer.getExtensions().isGL_ARB_instanced_arrays())
				{
					// Sadly, DSA has no support for "GL_ARB_instanced_arrays", so, we have to use the bind way
					// -> Keep the bind-horror as local as possible

					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Backup the currently bound OpenGL vertex array
						GLint openGLVertexArrayBackup = 0;
						glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &openGLVertexArrayBackup);
					#endif

					// Bind this OpenGL vertex array
					glBindVertexArray(mOpenGLVertexArray);

					// Set divisor
					if (attribute->instancesPerElement > 0)
					{
						glVertexAttribDivisorARB(attributeLocation, attribute->instancesPerElement);
					}

					#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
						// Be polite and restore the previous bound OpenGL vertex array
						glBindVertexArray(static_cast<GLuint>(openGLVertexArrayBackup));
					#endif
				}

				// Enable OpenGL vertex attribute array
				glEnableVertexArrayAttribEXT(mOpenGLVertexArray, attributeLocation);
			}
		}

		// Check the used index buffer
		// -> In case of no index buffer we don't bind buffer 0, there's not really a point in it
		if (nullptr != indexBuffer)
		{
			if (isArbDsa)
			{
				// Bind the index buffer
				glVertexArrayElementBuffer(mOpenGLVertexArray, indexBuffer->getOpenGLElementArrayBuffer());
			}
			else
			{
				// Sadly, EXT DSA has no support for element array buffer, so, we have to use the bind way
				// -> Keep the bind-horror as local as possible

				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					// Backup the currently bound OpenGL vertex array
					GLint openGLVertexArrayBackup = 0;
					glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &openGLVertexArrayBackup);

					// Backup the currently bound OpenGL element array buffer
					GLint openGLElementArrayBufferBackup = 0;
					glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB, &openGLElementArrayBufferBackup);
				#endif

				// Bind this OpenGL vertex array
				glBindVertexArray(mOpenGLVertexArray);

				// Bind OpenGL element array buffer
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer->getOpenGLElementArrayBuffer());

				#ifndef OPENGLRENDERER_NO_STATE_CLEANUP
					// Be polite and restore the previous bound OpenGL vertex array
					glBindVertexArray(static_cast<GLuint>(openGLVertexArrayBackup));

					// Be polite and restore the previous bound OpenGL element array buffer
					glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, static_cast<GLuint>(openGLElementArrayBufferBackup));
				#endif
			}
		}
	}

	VertexArrayVaoDsa::~VertexArrayVaoDsa()
	{
		// Vertex buffer reference handling is done within the base class "VertexArrayVao"
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
