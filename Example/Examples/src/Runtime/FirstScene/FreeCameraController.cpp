/*********************************************************\
 * Copyright (c) 2012-2016 Christian Ofenberg
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
#include "PrecompiledHeader.h"
#include "Runtime/FirstScene/FreeCameraController.h"

#include <RendererRuntime/Core/GetUninitialized.h>
#include <RendererRuntime/Core/Math/Math.h>
#include <RendererRuntime/Core/Math/EulerAngles.h>
#include <RendererRuntime/Resource/Scene/Node/ISceneNode.h>
#include <RendererRuntime/Resource/Scene/Item/CameraSceneItem.h>


//[-------------------------------------------------------]
//[ Public methods                                        ]
//[-------------------------------------------------------]
FreeCameraController::FreeCameraController(RendererRuntime::CameraSceneItem& cameraSceneItem) :
	mCameraSceneItem(cameraSceneItem),
	mMousePositionX(RendererRuntime::getUninitialized<int>()),
	mMousePositionY(RendererRuntime::getUninitialized<int>()),
	mMouseMoveX(0),
	mMouseMoveY(0)
{
	// Nothing here
}

FreeCameraController::~FreeCameraController()
{
	// Nothing here
}

void FreeCameraController::onUpdate(float pastMilliseconds)
{
	RendererRuntime::ISceneNode* sceneNode = mCameraSceneItem.getParentSceneNode();
	if (nullptr != sceneNode && (!mPressedKeys.empty() || !mPressedMouseButtons.empty()))
	{
		// Get the current transform
		const RendererRuntime::Transform& transform = sceneNode->getTransform();

		// Keyboard
		if (!mPressedKeys.empty())
		{
			// Get the movement speed
			float movementSpeed = pastMilliseconds * 0.002f;
			{
				// Speed up = "left shift"-key
				if (isKeyPressed(16))
				{
					movementSpeed *= 10.0f;
				}

				// Slow down = "left strg"-key
				if (isKeyPressed(17))
				{
					movementSpeed *= 0.1f;
				}
			}

			// Get the movement vector
			glm::vec3 movementVector = RendererRuntime::Math::ZERO_VECTOR;
			{
				// Move forward = "w"-key
				if (isKeyPressed(87))
				{
					movementVector += transform.rotation * RendererRuntime::Math::FORWARD_VECTOR;
				}

				// Strafe left = "a"-key
				if (isKeyPressed(65))
				{
					movementVector += transform.rotation * RendererRuntime::Math::RIGHT_VECTOR;
				}

				// Move backward = "s"-key
				if (isKeyPressed(83))
				{
					movementVector -= transform.rotation * RendererRuntime::Math::FORWARD_VECTOR;
				}

				// Strafe right = "d"-key
				if (isKeyPressed(68))
				{
					movementVector -= transform.rotation * RendererRuntime::Math::RIGHT_VECTOR;
				}

				// Strafe up = "page up"-key
				if (isKeyPressed(33))
				{
					movementVector += transform.rotation * RendererRuntime::Math::UP_VECTOR;
				}

				// Strafe down = "page down"-key
				if (isKeyPressed(34))
				{
					movementVector -= transform.rotation * RendererRuntime::Math::UP_VECTOR;
				}
			}

			// Update the camera scene node position
			if (movementVector != RendererRuntime::Math::ZERO_VECTOR)
			{
				sceneNode->setPosition(transform.position + movementVector * movementSpeed);
			}
		}

		// Mouse: Camera look around = right mouse button pressed down + mouse move
		if (isMouseButtonPressed(1) && (0 != mMouseMoveX || 0 != mMouseMoveY))
		{
			// Get the rotation speed
			// -> Slow down = "q"-key
			float rotationSpeed = 0.2f;
			if (isKeyPressed(81))
			{
				rotationSpeed *= 0.2f;
			}

			// Calculate yaw and pitch from transformation
			// -> GLM 0.9.8.0 "glm::yaw()" and "glm::pitch" behave odd, so "RendererRuntime::EulerAngles::matrixToEuler()" is used instead
			float yaw = 0.0f, pitch = 0.0f;
			{
				const glm::vec3 eulerAngles = RendererRuntime::EulerAngles::matrixToEuler(glm::mat3_cast(transform.rotation));
				yaw = glm::degrees(eulerAngles.x);
				pitch = glm::degrees(eulerAngles.y);
			}

			// Apply rotation change
			if (0 != mMouseMoveX)
			{
				// X rotation axis: Update yaw (also called 'heading', change is turning to the left or right) - in degrees
				yaw -= mMouseMoveX * rotationSpeed;

				// Limit the yaw (too huge values may cause problems, so, bring them into a well known interval)
				yaw = RendererRuntime::Math::wrapToInterval(yaw, 0.0f, 360.0f);
			}
			if (0 != mMouseMoveY)
			{
				// Y rotation axis: Update pitch (also called 'bank', change is moving the nose down and the tail up or vice-versa) - in degrees
				pitch += mMouseMoveY * rotationSpeed;

				// Limit the pitch (no full 90� to avoid dead angles)
				pitch = glm::clamp(pitch, -89.9f, +89.9f);
			}

			// Update the camera scene node rotation
			sceneNode->setRotation(glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f)));
		}
	}

	// "Jedi gesture": There was no mouse movement
	mMouseMoveX = 0;
	mMouseMoveY = 0;
}

void FreeCameraController::onKeyDown(uint32_t key)
{
	mPressedKeys.insert(key);
}

void FreeCameraController::onKeyUp(uint32_t key)
{
	mPressedKeys.erase(key);
}

void FreeCameraController::onMouseButtonDown(uint32_t button)
{
	mPressedMouseButtons.insert(button);
}

void FreeCameraController::onMouseButtonUp(uint32_t button)
{
	mPressedMouseButtons.erase(button);
}

void FreeCameraController::onMouseMove(int x, int y)
{
	const static int MAXIMUM = 100;

	// X
	if (RendererRuntime::isInitialized(mMousePositionX))
	{
		mMouseMoveX = x - mMousePositionX;
		if (mMouseMoveX > MAXIMUM)
		{
			mMouseMoveX = MAXIMUM;
		}
	}
	mMousePositionX = x;

	// Y
	if (RendererRuntime::isInitialized(mMousePositionY))
	{
		mMouseMoveY = y - mMousePositionY;
		if (mMouseMoveY > MAXIMUM)
		{
			mMouseMoveY = MAXIMUM;
		}
	}
	mMousePositionY = y;
}


//[-------------------------------------------------------]
//[ Private methods                                       ]
//[-------------------------------------------------------]
bool FreeCameraController::isKeyPressed(uint32_t key) const
{
	return (mPressedKeys.find(key) != mPressedKeys.cend());
}

bool FreeCameraController::isMouseButtonPressed(uint32_t button) const
{
	return (mPressedMouseButtons.find(button) != mPressedMouseButtons.cend());
}
