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
#include <Renderer/Shader/ITessellationControlShader.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
typedef __interface ID3D10Blob *LPD3D10BLOB;
typedef ID3D10Blob ID3DBlob;
namespace Renderer
{
	class ShaderBytecode;
}
namespace Direct3D12Renderer
{
	class Direct3D12Renderer;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D12Renderer
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    HLSL tessellation control shader ("hull shader" in Direct3D terminology) class
	*/
	class TessellationControlShaderHlsl : public Renderer::ITessellationControlShader
	{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor for creating a tessellation control shader from shader bytecode
		*
		*  @param[in] direct3D12Renderer
		*    Owner Direct3D 12 renderer instance
		*  @param[in] shaderBytecode
		*    Shader bytecode
		*/
		TessellationControlShaderHlsl(Direct3D12Renderer& direct3D12Renderer, const Renderer::ShaderBytecode& shaderBytecode);

		/**
		*  @brief
		*    Constructor for creating a tessellation control shader from shader source code
		*
		*  @param[in] direct3D12Renderer
		*    Owner Direct3D 12 renderer instance
		*  @param[in] sourceCode
		*    Shader ASCII source code, must be valid
		*/
		TessellationControlShaderHlsl(Direct3D12Renderer& direct3D12Renderer, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode = nullptr);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~TessellationControlShaderHlsl();

		/**
		*  @brief
		*    Return the Direct3D 12 hull shader blob
		*
		*  @return
		*    Direct3D 12 hull shader blob, can be a null pointer on error, do not release the returned instance unless you added an own reference to it
		*/
		inline ID3DBlob* getD3DBlobHullShader() const;


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	public:
		virtual const char* getShaderLanguageName() const override;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit TessellationControlShaderHlsl(const TessellationControlShaderHlsl& source) = delete;
		TessellationControlShaderHlsl& operator =(const TessellationControlShaderHlsl& source) = delete;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		ID3DBlob* mD3DBlobHullShader;	///< Direct3D 12 hull shader blob, can be a null pointer


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D12Renderer


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "Direct3D12Renderer/Shader/TessellationControlShaderHlsl.inl"
