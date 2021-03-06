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
#include "Direct3D11Renderer/State/IState.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
struct ID3D11DepthStencilState;
namespace Renderer
{
	struct DepthStencilState;
}
namespace Direct3D11Renderer
{
	class Direct3D11Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D11Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Direct3D 11 depth stencil state class
	*/
	class DepthStencilState : public IState
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] direct3D11Renderer
		*    Owner Direct3D 11 renderer instance
		*  @param[in] depthStencilState
		*    Depth stencil state to use
		*/
		DepthStencilState(Direct3D11Renderer& direct3D11Renderer, const Renderer::DepthStencilState& depthStencilState);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~DepthStencilState();

		/**
		*  @brief
		*    Return the Direct3D 11 depth stencil state
		*
		*  @return
		*    The Direct3D 11 depth stencil state, can be a null pointer, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3D11DepthStencilState* getD3D11DepthStencilState() const;


	//[-------------------------------------------------------]
	//[ Public virtual Direct3D11Renderer::IState methods     ]
	//[-------------------------------------------------------]
	public:
		virtual void setDebugName(const char* name) override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ID3D11DepthStencilState* mD3D11DepthStencilState;	///< Direct3D 11 depth stencil state, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D11Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D11Renderer/State/DepthStencilState.inl"
