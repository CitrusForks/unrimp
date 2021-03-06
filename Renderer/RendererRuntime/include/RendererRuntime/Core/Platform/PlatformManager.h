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
#include "RendererRuntime/Export.h"
#include "RendererRuntime/Core/Manager.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	/**
	*  @brief
	*    Platform specific functionality
	*/
	class PlatformManager : private Manager
	{


	//[-------------------------------------------------------]
	//[ Public static methods                                 ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Set the name of the current thread
		*
		*  @param[in] shortName
		*    Short ASCII name to set, up to 16 characters (including the terminating zero)
		*  @param[in] descriptiveName
		*    Descriptive ASCII name to set, not supported on some platforms, preferred name
		*/
		RENDERERRUNTIME_API_EXPORT static void setCurrentThreadName(const char* shortName, const char* descriptiveName);


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		explicit PlatformManager(const PlatformManager&) = delete;
		PlatformManager& operator=(const PlatformManager&) = delete;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Debug                                                 ]
//[-------------------------------------------------------]
// Debug macros
#ifdef RENDERER_NO_DEBUG
	/**
	*  @brief
	*    Set the name of the current thread
	*
	*  @param[in] shortName
	*    Short ASCII name to set, up to 16 characters (including the terminating zero)
	*  @param[in] descriptiveName
	*    Descriptive ASCII name to set, not supported on some platforms, preferred name
	*
	*  @note
	*    - Use this macro for debugging helpers only to make it possible to optimize it out in retail builds
	*/
	#define RENDERER_RUNTIME_SET_CURRENT_THREAD_DEBUG_NAME(shortName, descriptiveName)
#else
	/**
	*  @brief
	*    Set the name of the current thread
	*
	*  @param[in] shortName
	*    Short ASCII name to set, up to 16 characters (including the terminating zero)
	*  @param[in] descriptiveName
	*    Descriptive ASCII name to set, not supported on some platforms, preferred name
	*
	*  @note
	*    - Use this macro for debugging helpers only to make it possible to optimize it out in retail builds
	*/
	#define RENDERER_RUNTIME_SET_CURRENT_THREAD_DEBUG_NAME(shortName, descriptiveName) RendererRuntime::PlatformManager::setCurrentThreadName(shortName, descriptiveName);
#endif
