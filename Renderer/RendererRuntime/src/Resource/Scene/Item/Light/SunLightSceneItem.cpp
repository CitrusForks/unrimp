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
#include "RendererRuntime/PrecompiledHeader.h"
#include "RendererRuntime/Resource/Scene/Item/Light/SunLightSceneItem.h"
#include "RendererRuntime/Resource/Scene/Loader/SceneFileFormat.h"
#include "RendererRuntime/Resource/Scene/SceneNode.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	const SceneItemTypeId SunLightSceneItem::TYPE_ID("SunLightSceneItem");


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	void SunLightSceneItem::calculatedDerivedSunLightProperties()
	{
		SceneNode* parentSceneNode = getParentSceneNode();
		if (nullptr != parentSceneNode)
		{
			{ // Calculate normalized world space sun direction vector
			  // -> Basing on https://raw.githubusercontent.com/aoighost/SkyX/master/SkyX/Source/BasicController.cpp - "SkyX::BasicController::update()"
			  // -> 24h day: 0______A(Sunrise)_______B(Sunset)______24
				float y = 0.0f;
				const float X = mTimeOfDay;
				const float A = mSunriseTime;
				const float B = mSunsetTime;
				const float AB  = A + 24.0f - B;
				const float AB_ = B - A;
				const float XB  = X + 24.0f - B;
				if (X < A || X > B)
				{
					if (X < A)
					{
						y = -XB / AB;
					}
					else
					{
						y = -(X - B) / AB;
					}
					if (y > -0.5f)
					{
						y *= 2.0f;
					}
					else
					{
						y = -(1.0f + y) * 2.0f;
					}
				}
				else
				{
					y = (X - A) / (B - A);
					if (y < 0.5f)
					{
						y *= 2.0f;
					}
					else
					{
						y = (1.0f - y) * 2.0f;
					}
				}

				// Get the east direction vector, clockwise orientation starting from north for zero
				glm::vec2 eastDirection = glm::vec2(-std::sin(mEastDirection), std::cos(mEastDirection));
				if (X > A && X < B)
				{
					if (X > (A + AB_ * 0.5f))
					{
						eastDirection = -eastDirection;
					}
				}
				else if (X <= A)
				{
					if (XB < (24.0f - AB_) * 0.5f)
					{
						eastDirection = -eastDirection;
					}
				}
				else if ((X - B) < (24.0f - AB_) * 0.5f)
				{
					eastDirection = -eastDirection;
				}

				// Calculate the sun direction vector
				const float ydeg = (glm::pi<float>() * 0.5f) * y;
				const float sn = std::sin(ydeg);
				const float cs = std::cos(ydeg);
				glm::vec3 sunDirection = glm::vec3(eastDirection.x * cs, sn, eastDirection.y * cs);

				// Modify the sun direction vector so one can control whether or not the light comes perpendicularly at 12 o'clock
				sunDirection = glm::angleAxis(mAngleOfIncidence, Math::VEC3_FORWARD) * sunDirection;

				// Tell the owner scene node about the new rotation
				// TODO(co) Can we simplify this?
				parentSceneNode->setRotation(glm::inverse(glm::quat(glm::lookAt(Math::VEC3_ZERO, sunDirection, Math::VEC3_UP))));
			}

			// TODO(co) Calculate sun color via "Solar Radiance Calculation" - https://www.gamedev.net/topic/671214-simple-solar-radiance-calculation/
		}
	}


	//[-------------------------------------------------------]
	//[ Public RendererRuntime::ISceneItem methods            ]
	//[-------------------------------------------------------]
	void SunLightSceneItem::deserialize(uint32_t numberOfBytes, const uint8_t* data)
	{
		assert(sizeof(v1Scene::SunLightItem) == numberOfBytes);
		std::ignore = numberOfBytes;

		// Read data
		const v1Scene::SunLightItem* sunLightItem = reinterpret_cast<const v1Scene::SunLightItem*>(data);
		mSunriseTime	  = sunLightItem->sunriseTime;
		mSunsetTime		  = sunLightItem->sunsetTime;
		mEastDirection	  = sunLightItem->eastDirection;
		mAngleOfIncidence = sunLightItem->angleOfIncidence;
		mTimeOfDay		  = sunLightItem->timeOfDay;

		// Sanity checks
		assert(mSunriseTime >= 00.00f && mSunriseTime < 24.00f);	// O'clock
		assert(mSunsetTime >= 00.00f && mSunsetTime < 24.00f);		// O'clock
		assert(mTimeOfDay >= 00.00f && mTimeOfDay < 24.00f);		// O'clock

		// Calculated derived sun light properties
		calculatedDerivedSunLightProperties();
	}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime
