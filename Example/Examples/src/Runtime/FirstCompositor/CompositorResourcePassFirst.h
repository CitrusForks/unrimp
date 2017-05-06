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
#include <RendererRuntime/Resource/CompositorNode/Pass/ICompositorResourcePass.h>


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
class CompositorResourcePassFirst : public RendererRuntime::ICompositorResourcePass
{


//[-------------------------------------------------------]
//[ Friends                                               ]
//[-------------------------------------------------------]
	friend class CompositorPassFactoryFirst;	// The only one allowed to create instances of this class


//[-------------------------------------------------------]
//[ Public definitions                                    ]
//[-------------------------------------------------------]
public:
	static const RendererRuntime::CompositorPassTypeId TYPE_ID;


//[-------------------------------------------------------]
//[ Public virtual RendererRuntime::ICompositorResourcePass methods ]
//[-------------------------------------------------------]
public:
	virtual RendererRuntime::CompositorPassTypeId getTypeId() const override;


//[-------------------------------------------------------]
//[ Protected methods                                     ]
//[-------------------------------------------------------]
protected:
	explicit CompositorResourcePassFirst(const RendererRuntime::CompositorTarget& compositorTarget);
	virtual ~CompositorResourcePassFirst();
	explicit CompositorResourcePassFirst(const CompositorResourcePassFirst&) = delete;
	CompositorResourcePassFirst& operator=(const CompositorResourcePassFirst&) = delete;


};
