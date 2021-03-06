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
#include "RendererRuntime/Core/StringId.h"
#include "RendererRuntime/Core/Math/Transform.h"

#include <vector>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace RendererRuntime
{
	class ISceneItem;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace RendererRuntime
{


	//[-------------------------------------------------------]
	//[ Classes                                               ]
	//[-------------------------------------------------------]
	class SceneNode
	{


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
		friend class SceneResource;


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		typedef std::vector<SceneNode*> AttachedSceneNodes;
		typedef std::vector<ISceneItem*> AttachedSceneItems;


	//[-------------------------------------------------------]
	//[ Public methods                                        ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Local transform                                       ]
		//[-------------------------------------------------------]
		inline const Transform& getTransform() const;
		inline void setTransform(const Transform& transform);
		inline void setPosition(const glm::vec3& position);
		inline void setRotation(const glm::quat& rotation);
		inline void setPositionRotation(const glm::vec3& position, const glm::quat& rotation);
		inline void setScale(const glm::vec3& scale);

		//[-------------------------------------------------------]
		//[ Derived global transform                              ]
		//[-------------------------------------------------------]
		inline const Transform& getGlobalTransform() const;
		inline const Transform& getPreviousGlobalTransform() const;

		//[-------------------------------------------------------]
		//[ Attached scene nodes                                  ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT void attachSceneNode(SceneNode& sceneNode);
		RENDERERRUNTIME_API_EXPORT void detachAllSceneNodes();
		inline const AttachedSceneNodes& getAttachedSceneNodes() const;
		RENDERERRUNTIME_API_EXPORT void setVisible(bool visible);

		//[-------------------------------------------------------]
		//[ Attached scene items                                  ]
		//[-------------------------------------------------------]
		RENDERERRUNTIME_API_EXPORT void attachSceneItem(ISceneItem& sceneItem);
		RENDERERRUNTIME_API_EXPORT void detachAllSceneItems();
		inline const AttachedSceneItems& getAttachedSceneItems() const;
		RENDERERRUNTIME_API_EXPORT void setSceneItemsVisible(bool visible);


	//[-------------------------------------------------------]
	//[ Protected methods                                     ]
	//[-------------------------------------------------------]
	protected:
		inline explicit SceneNode(const Transform& transform);
		inline ~SceneNode();
		explicit SceneNode(const SceneNode&) = delete;
		SceneNode& operator=(const SceneNode&) = delete;


	//[-------------------------------------------------------]
	//[ Private methods                                       ]
	//[-------------------------------------------------------]
	private:
		RENDERERRUNTIME_API_EXPORT void updateGlobalTransformRecursive();


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		SceneNode*		   mParentSceneNode;			///< Parent scene node the scene node is attached to, can be a null pointer, don't destroy the instance
		Transform		   mTransform;					///< Local transform
		Transform		   mGlobalTransform;			///< Derived global transform - TODO(co) Will of course later on be handled in another way to be cache efficient and more efficient to calculate and incrementally update. But lets start simple.
		Transform		   mPreviousGlobalTransform;	///< Previous derived global transform
		AttachedSceneNodes mAttachedSceneNodes;
		AttachedSceneItems mAttachedSceneItems;


	};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // RendererRuntime


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "RendererRuntime/Resource/Scene/SceneNode.inl"
