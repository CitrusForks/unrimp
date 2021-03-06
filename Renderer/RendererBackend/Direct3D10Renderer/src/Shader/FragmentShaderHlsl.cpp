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
#include "Direct3D10Renderer/Shader/FragmentShaderHlsl.h"
#include "Direct3D10Renderer/Shader/ShaderLanguageHlsl.h"
#include "Direct3D10Renderer/Guid.h"	// For "WKPDID_D3DDebugObjectName"
#include "Direct3D10Renderer/D3D10.h"
#include "Direct3D10Renderer/Direct3D10Renderer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace Direct3D10Renderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	FragmentShaderHlsl::FragmentShaderHlsl(Direct3D10Renderer& direct3D10Renderer, const Renderer::ShaderBytecode& shaderBytecode) :
		IFragmentShader(direct3D10Renderer),
		mD3D10PixelShader(nullptr)
	{
		// Create the Direct3D 10 vertex shader
		direct3D10Renderer.getD3D10Device()->CreatePixelShader(shaderBytecode.getBytecode(), shaderBytecode.getNumberOfBytes(), &mD3D10PixelShader);

		// Don't assign a default name to the resource for debugging purposes, Direct3D 10 automatically sets a decent default name
	}

	FragmentShaderHlsl::FragmentShaderHlsl(Direct3D10Renderer& direct3D10Renderer, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode) :
		IFragmentShader(direct3D10Renderer),
		mD3D10PixelShader(nullptr)
	{
		// Create the Direct3D 10 binary large object for the pixel shader
		ID3DBlob* d3dBlob = static_cast<ShaderLanguageHlsl*>(direct3D10Renderer.getShaderLanguage())->loadShaderFromSourcecode("ps_4_0", sourceCode, nullptr);
		if (nullptr != d3dBlob)
		{
			// Create the Direct3D 10 pixel shader
			direct3D10Renderer.getD3D10Device()->CreatePixelShader(d3dBlob->GetBufferPointer(), d3dBlob->GetBufferSize(), &mD3D10PixelShader);

			// Return shader bytecode, if requested do to so
			if (nullptr != shaderBytecode)
			{
				shaderBytecode->setBytecodeCopy(static_cast<uint32_t>(d3dBlob->GetBufferSize()), static_cast<uint8_t*>(d3dBlob->GetBufferPointer()));
			}

			// Release the Direct3D 10 shader binary large object
			d3dBlob->Release();
		}

		// Don't assign a default name to the resource for debugging purposes, Direct3D 10 automatically sets a decent default name
	}

	FragmentShaderHlsl::~FragmentShaderHlsl()
	{
		// Release the Direct3D 10 pixel shader
		if (nullptr != mD3D10PixelShader)
		{
			mD3D10PixelShader->Release();
		}
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IResource methods            ]
	//[-------------------------------------------------------]
	void FragmentShaderHlsl::setDebugName(const char* name)
	{
		#ifndef DIRECT3D10RENDERER_NO_DEBUG
			// Valid Direct3D 10 pixel shader?
			if (nullptr != mD3D10PixelShader)
			{
				// Set the debug name
				// -> First: Ensure that there's no previous private data, else we might get slapped with a warning
				mD3D10PixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, 0, nullptr);
				mD3D10PixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(strlen(name)), name);
			}
		#endif
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* FragmentShaderHlsl::getShaderLanguageName() const
	{
		return ShaderLanguageHlsl::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // Direct3D10Renderer
