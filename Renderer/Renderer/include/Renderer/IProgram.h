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
//[ Header guard                                          ]
//[-------------------------------------------------------]
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "Renderer/PlatformTypes.h"	// For "handle"
#include "Renderer/IResource.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace Renderer
{
	class IVertexArray;
	class IIndexBuffer;
	struct VertexArrayAttribute;
	struct VertexArrayVertexBuffer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Abstract program interface
	*/
	class IProgram : public IResource
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Destructor
		*/
		inline virtual ~IProgram();


	//[-------------------------------------------------------]
	//[ Public virtual IProgram methods                       ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Create a vertex array instance
		*
		*  @param[in] numberOfAttributes
		*    Number of attributes (position, color, texture coordinate, normal...), having zero attributes is valid
		*  @param[in] attributes
		*    At least nNumberOfAttributes instances of vertex array attributes, can be a null pointer in case there are zero attributes, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] numberOfVertexBuffers
		*    Number of vertex buffers, having zero vertex buffers is valid
		*  @param[in] vertexBuffers
		*    At least numberOfVertexBuffers instances of vertex array vertex buffers, can be a null pointer in case there are zero vertex buffers, the data is internally copied and you have to free your memory if you no longer need it
		*  @param[in] indexBuffer
		*    Optional index buffer to use, can be a null pointer, the vertex array instance keeps a reference to the index buffer
		*
		*  @return
		*    The created vertex array instance, null pointer on error. Release the returned instance if you no longer need it.
		*
		*  @note
		*    - The created vertex array instance keeps a reference to the vertex buffers used by the vertex array attributes
		*    - It's valid that a vertex array implementation is adding a reference and releasing it again at once
		*      (this means that in the case of not having any more references, a vertex buffer might get destroyed when calling this method)
		*/
		virtual IVertexArray *createVertexArray(uint32_t numberOfAttributes, const VertexArrayAttribute *attributes, uint32_t numberOfVertexBuffers, const VertexArrayVertexBuffer *vertexBuffers, IIndexBuffer *indexBuffer = nullptr) = 0;

		// TODO(co) Cleanup
		inline virtual int getAttributeLocation(const char *attributeName);
		inline virtual uint32_t getUniformBlockIndex(const char *uniformBlockName, uint32_t defaultIndex);
		inline virtual handle getUniformHandle(const char *uniformName);
		inline virtual uint32_t setTextureUnit(handle uniformHandle, uint32_t unit);
		inline virtual void setUniform1i(handle uniformHandle, int value);
		inline virtual void setUniform1f(handle uniformHandle, float value);
		inline virtual void setUniform2fv(handle uniformHandle, const float *value);
		inline virtual void setUniform3fv(handle uniformHandle, const float *value);
		inline virtual void setUniform4fv(handle uniformHandle, const float *value);
		inline virtual void setUniformMatrix3fv(handle uniformHandle, const float *value);
		inline virtual void setUniformMatrix4fv(handle uniformHandle, const float *value);


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] renderer
		*    Owner renderer instance
		*/
		inline explicit IProgram(IRenderer &renderer);

		/**
		*  @brief
		*    Copy constructor
		*
		*  @param[in] source
		*    Source to copy from
		*/
		inline explicit IProgram(const IProgram &source);

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
		inline IProgram &operator =(const IProgram &source);


	};


	//[-------------------------------------------------------]
	//[ Type definitions                                      ]
	//[-------------------------------------------------------]
	typedef SmartRefCount<IProgram> IProgramPtr;


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Renderer/IProgram.inl"
