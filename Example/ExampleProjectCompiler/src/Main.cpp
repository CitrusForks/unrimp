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
#include "Main.h"

#include <RendererRuntime/Core/File/StdFileManager.h>

#include <RendererToolkit/Public/RendererToolkit.h>
#include <RendererToolkit/Public/RendererToolkitInstance.h>

#include <exception>

#include <iostream>


//[-------------------------------------------------------]
//[ Platform independent program entry point              ]
//[-------------------------------------------------------]
int programEntryPoint(CommandLineArguments& args)
{
	RendererRuntime::StdFileManager stdFileManager;
	RendererToolkit::RendererToolkitInstance rendererToolkitInstance(stdFileManager);
	RendererToolkit::IRendererToolkit* rendererToolkit = rendererToolkitInstance.getRendererToolkit();
	if (nullptr != rendererToolkit)
	{
		// TODO(co) Experiments
		RendererToolkit::IProject* project = rendererToolkit->createProject();
		try
		{
			project->loadByFilename("../DataSource/Example.project");

			if (args.empty())
			{
				//	project->compileAllAssets("Direct3D9_30");
				//	project->compileAllAssets("Direct3D11_50");
				//	project->compileAllAssets("Direct3D12_50");
				//	project->compileAllAssets("OpenGLES3_300");
					project->compileAllAssets("OpenGL_440");
			}
			else
			{
				// For now all given arguments are interpreted as render target
				for (auto& renderTarget : args)
				{
					std::cout<<"compile for target: "<<renderTarget<<'\n';
					project->compileAllAssets(renderTarget.c_str());
					std::cout<<"compilation done\n";
				}
			}
		}
		catch (const std::exception& e)
		{
			std::cout<<"Project compilation failed: "<<e.what()<<"\n";
		}
		delete project;
	}

	// No error
	return 0;
}
