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
#include "OpenGLRenderer/Shader/Separate/FragmentShaderSeparate.h"
#include "OpenGLRenderer/Shader/Separate/ShaderLanguageSeparate.h"
#include "OpenGLRenderer/Extensions.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace OpenGLRenderer
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	FragmentShaderSeparate::FragmentShaderSeparate(OpenGLRenderer& openGLRenderer, const Renderer::ShaderBytecode& shaderBytecode) :
		IFragmentShader(reinterpret_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLShaderProgram(ShaderLanguageSeparate::loadShaderProgramFromBytecode(openGLRenderer, GL_FRAGMENT_SHADER_ARB, shaderBytecode))
	{
		// Nothing here
	}

	FragmentShaderSeparate::FragmentShaderSeparate(OpenGLRenderer& openGLRenderer, const char* sourceCode, Renderer::ShaderBytecode* shaderBytecode) :
		IFragmentShader(reinterpret_cast<Renderer::IRenderer&>(openGLRenderer)),
		mOpenGLShaderProgram(ShaderLanguageSeparate::loadShaderProgramFromSourceCode(openGLRenderer, GL_FRAGMENT_SHADER_ARB, sourceCode))
	{
		// Return shader bytecode, if requested do to so
		if (nullptr != shaderBytecode)
		{
			ShaderLanguageSeparate::shaderSourceCodeToShaderBytecode(GL_FRAGMENT_SHADER_ARB, sourceCode, *shaderBytecode);
		}
	}

	FragmentShaderSeparate::~FragmentShaderSeparate()
	{
		// Destroy the OpenGL shader program
		// -> Silently ignores 0's and names that do not correspond to existing buffer objects
		glDeleteProgram(mOpenGLShaderProgram);
	}


	//[-------------------------------------------------------]
	//[ Public virtual Renderer::IShader methods              ]
	//[-------------------------------------------------------]
	const char* FragmentShaderSeparate::getShaderLanguageName() const
	{
		return ShaderLanguageSeparate::NAME;
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // OpenGLRenderer
