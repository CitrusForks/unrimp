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
#include "OpenGLRenderer/Buffer/VertexArray.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	struct VertexAttribute;
	struct VertexAttributes;
	struct VertexArrayVertexBuffer;
}
namespace OpenGLRenderer
{
	class IndexBuffer;
	class OpenGLRenderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    OpenGL vertex array class, traditional version
	*/
	class VertexArrayNoVao : public VertexArray
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] openGLRenderer
		*    Owner OpenGL renderer instance
		*  @param[in] vertexAttributes
		*    Vertex attributes ("vertex declaration" in Direct3D 9 terminology, "input layout" in Direct3D 10 & 11 terminology)
		*  @param[in] numberOfVertexBuffers
		*    Number of vertex buffers, having zero vertex buffers is valid
		*  @param[in] vertexBuffers
		*    At least numberOfVertexBuffers instances of vertex array vertex buffers, can be a null pointer in case there are zero vertex buffers, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] indexBuffer
		*    Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer
		*/
		VertexArrayNoVao(OpenGLRenderer& openGLRenderer, const Renderer::VertexAttributes& vertexAttributes, uint32_t numberOfVertexBuffers, const Renderer::VertexArrayVertexBuffer* vertexBuffers, IndexBuffer* indexBuffer);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~VertexArrayNoVao();

		/**
		*  @brief
		*    Enable OpenGL vertex attribute arrays
		*/
		void enableOpenGLVertexAttribArrays();

		/**
		*  @brief
		*    Disable OpenGL vertex attribute arrays
		*/
		void disableOpenGLVertexAttribArrays();


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit VertexArrayNoVao(const VertexArrayNoVao& source) = delete;
		VertexArrayNoVao& operator =(const VertexArrayNoVao& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		uint32_t						   mNumberOfAttributes;			///< Number of attributes (position, color, texture coordinate, normal...), having zero attributes is valid
		Renderer::VertexAttribute*		   mAttributes;					///< At least "numberOfAttributes" instances of vertex attributes, can be a null pointer in case there are zero attributes
		uint32_t						   mNumberOfVertexBuffers;		///< Number of vertex buffers, having zero vertex buffers is valid
		Renderer::VertexArrayVertexBuffer* mVertexBuffers;				///< At least mNumberOfVertexBuffers instances of vertex array vertex buffers, can be a null pointer in case there are zero vertex buffers, the data is internally copied and you have to free your memory if you no longer need it
		bool							   mIsGL_ARB_instanced_arrays;	///< Is the "GL_ARB_instanced_arrays"-extension supported?


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
