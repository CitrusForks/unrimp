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
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	inline float CameraSceneItem::getFovY() const
	{
		return mFovY;
	}

	inline void CameraSceneItem::setFovY(float fovY)
	{
		mFovY = fovY;
	}

	inline float CameraSceneItem::getNearZ() const
	{
		return mNearZ;
	}

	inline void CameraSceneItem::setNearZ(float nearZ)
	{
		mNearZ = nearZ;
	}

	inline float CameraSceneItem::getFarZ() const
	{
		return mFarZ;
	}

	inline void CameraSceneItem::setFarZ(float farZ)
	{
		mFarZ = farZ;
	}

	inline bool CameraSceneItem::hasCustomWorldSpaceToViewSpaceMatrix() const
	{
		return mHasCustomWorldSpaceToViewSpaceMatrix;
	}

	inline void CameraSceneItem::unsetCustomWorldSpaceToViewSpaceMatrix()
	{
		mHasCustomWorldSpaceToViewSpaceMatrix = false;
	}

	inline void CameraSceneItem::setCustomWorldSpaceToViewSpaceMatrix(const glm::mat4& customWorldSpaceToViewSpaceMatrix)
	{
		mWorldSpaceToViewSpaceMatrix = customWorldSpaceToViewSpaceMatrix;
		mHasCustomWorldSpaceToViewSpaceMatrix = true;
	}

	inline const glm::mat4& CameraSceneItem::getViewSpaceToClipSpaceMatrix() const
	{
		return mViewSpaceToClipSpaceMatrix;
	}

	inline bool CameraSceneItem::hasCustomViewSpaceToClipSpaceMatrix() const
	{
		return mHasCustomViewSpaceToClipSpaceMatrix;
	}

	inline void CameraSceneItem::unsetCustomViewSpaceToClipSpaceMatrix()
	{
		mHasCustomViewSpaceToClipSpaceMatrix = false;
	}

	inline void CameraSceneItem::setCustomViewSpaceToClipSpaceMatrix(const glm::mat4& customViewSpaceToClipSpaceMatrix)
	{
		mViewSpaceToClipSpaceMatrix = customViewSpaceToClipSpaceMatrix;
		mHasCustomViewSpaceToClipSpaceMatrix = true;
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	inline SceneItemTypeId CameraSceneItem::getSceneItemTypeId() const
	{
		return TYPE_ID;
	}


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	inline CameraSceneItem::CameraSceneItem(SceneResource& sceneResource) :
		ISceneItem(sceneResource),
		mFovY(DEFAULT_FOV_Y),
		mNearZ(DEFAULT_NEAR_Z),
		mFarZ(DEFAULT_FAR_Z),
		mHasCustomWorldSpaceToViewSpaceMatrix(false),
		mHasCustomViewSpaceToClipSpaceMatrix(false)
	{
		// Nothing here
	}

	inline CameraSceneItem::~CameraSceneItem()
	{
		// Nothing here
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
