/*
-----------------------------------------------------------------------------
This source	file is	part of	the	OgreSteer Application.

Author: Spannerman (spannerworx AT gmail DOT com)
URL: http://www.spannerworx.com/

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

#ifndef __OGRESTEER_SIMPLEENTITY_
#define __OGRESTEER_SIMPLEENTITY_

#include "Ogre.h"

#include "OSSteerableBase.h"

class OSSimpleEntity: public OSSteerableBase
{

public:

	// Model Mesh Names
	static const std::string CONST_STR_MODEL_ROBOT;
	static const std::string CONST_STR_MODEL_RAZOR;
	// Model Animations
	static const std::string CONST_STR_ANIM_NONE;
	static const std::string CONST_STR_ANIM_ENTITY_IDLE;
	static const std::string CONST_STR_ANIM_ENTITY_WALK;
	static const std::string CONST_STR_ANIM_ENTITY_SHOOT;
	static const std::string CONST_STR_ANIM_ENTITY_SLUMP;
	static const std::string CONST_STR_ANIM_ENTITY_DIE;

	enum OSEnumEntiyType
	{
		OS_ENTITY_TYPE_WANDERER,
		OS_ENTITY_TYPE_PURSUER
	};

	OSSimpleEntity(const std::string& name, 
		OSEnumEntiyType type, 
		Ogre::SceneManager* newSceneMgr, 
		const std::string& meshName, 
		Ogre::Vector3 startPosition = Ogre::Vector3::ZERO,
		Ogre::Quaternion startOrientation = Ogre::Quaternion::IDENTITY); 

	virtual ~OSSimpleEntity();

	void resetEntity();

	void update(const float currentTime, const float elapsedTime);

	Ogre::SceneNode* getEntityNode(){return mEntityNode;}

	const std::string& getEntityName(){return mEntityName;}

	float getFirstPersonHeight(){return mFirstPersonHeight;}

	float getThirdPersonDistance(){return mThirdPersonDistance;}

	OSEnumEntiyType getEntityType(){return mEntiyType;}

	Ogre::Vector3 getPosition();
	void setPosition(Ogre::Vector3 newPos);

	Ogre::Vector3 getDirection();

	Ogre::Vector3 getForwardVector() {return mForwardVector;}

	void setTargetEntity(OSSimpleEntity* target) {mTargetEntity = target;}

private:

	Ogre::SceneManager* mSceneMgr;
	Ogre::SceneNode* mEntityNode;
	Ogre::Entity* mEntityEnt;
	Ogre::AnimationState* mEntityAnimState;

	Ogre::Vector3 mMeshScale;
	Ogre::Vector3 mInitialEntityPos;
	Ogre::Quaternion mInitialEntityOrient;
	Ogre::Vector3 mForwardVector;

	OSSimpleEntity* mTargetEntity;

	OSEnumEntiyType mEntiyType;

	float mFirstPersonHeight;
	float mThirdPersonDistance;
	float mRadius;
	float mMaxForce;
	float mMaxSpeed;

	std::string mEntityName;
	std::string mEntityAnimString;
	std::string mMeshName;

	bool hasAnimation;
};

#endif