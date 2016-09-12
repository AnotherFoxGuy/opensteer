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

#include "OSSimpleEntity.h"

const std::string OSSimpleEntity::CONST_STR_MODEL_ROBOT("robot.mesh");
const std::string OSSimpleEntity::CONST_STR_MODEL_RAZOR("RZR-002.mesh");
const std::string OSSimpleEntity::CONST_STR_ANIM_NONE("None");
const std::string OSSimpleEntity::CONST_STR_ANIM_ENTITY_IDLE("Idle");
const std::string OSSimpleEntity::CONST_STR_ANIM_ENTITY_WALK("Walk");
const std::string OSSimpleEntity::CONST_STR_ANIM_ENTITY_SHOOT("Shoot");
const std::string OSSimpleEntity::CONST_STR_ANIM_ENTITY_SLUMP("Slump");
const std::string OSSimpleEntity::CONST_STR_ANIM_ENTITY_DIE("Die");

using namespace Ogre;

OSSimpleEntity::OSSimpleEntity(const std::string& name, 
						       OSEnumEntiyType type,
							   Ogre::SceneManager* newSceneMgr, 
							   const std::string& meshName, 
							   Ogre::Vector3 startPosition, 
							   Ogre::Quaternion startOrientation)
 :	mEntityName(name),
	mEntiyType(type),
	mSceneMgr(newSceneMgr),
	mMeshName(meshName),
	mInitialEntityPos(startPosition),
	mInitialEntityOrient(startOrientation),
	mEntityNode(0),
	mEntityEnt(0),
	mTargetEntity(0),
	mFirstPersonHeight(90.0f),
	mThirdPersonDistance(200.0f),
	mForwardVector(Ogre::Vector3::UNIT_X),
	mEntityAnimString(CONST_STR_ANIM_NONE),
	hasAnimation(false),
	mMeshScale(Vector3::ZERO)
{
	assert(name != "");

	// Create parent sceneNode.
	mEntityNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mInitialEntityPos);

	mEntityEnt = mSceneMgr->createEntity(mEntityName, mMeshName);
	mEntityNode->attachObject(mEntityEnt);

	if (mMeshName == CONST_STR_MODEL_ROBOT)
	{
		hasAnimation = true;
		mFirstPersonHeight = 90.0f;
		mThirdPersonDistance = 180.0f;
		mForwardVector = Ogre::Vector3::UNIT_X;
		mMeshScale = Vector3::ZERO;
		mRadius = 10.0f;
		mMaxForce = 40.0f;
		mMaxSpeed = 30.0f;
	}
	else if (mMeshName == CONST_STR_MODEL_RAZOR)
	{
		hasAnimation = false;
		mFirstPersonHeight = 120.0f;
		mThirdPersonDistance = 500.0f;
		mForwardVector = Ogre::Vector3::UNIT_Z;
		mMeshScale = Vector3(10,10,10);
		mRadius = 60.0f;
		mMaxForce = 80.0f;
		mMaxSpeed = 60.0f;
	}	

	if (mMeshScale != Vector3::ZERO)
	{
		mEntityNode->setScale(mMeshScale);
		mEntityEnt->setNormaliseNormals(true);
	}

    resetEntity();
}

OSSimpleEntity::~OSSimpleEntity()
{

}

void OSSimpleEntity::resetEntity()
{
	mEntityNode->setPosition(mInitialEntityPos);
	mEntityNode->setOrientation(mInitialEntityOrient);

	// Setup the Entitys animation
	if (hasAnimation)
	{
		mEntityAnimState = mEntityEnt->getAnimationState(CONST_STR_ANIM_ENTITY_WALK);
		mEntityAnimState->setEnabled(true);
	}

	Vector3 nodeForwardVec = mEntityNode->_getDerivedOrientation() * Vector3::NEGATIVE_UNIT_Z;

	OpenSteer::Vec3 startPos = OpenSteer::Vec3(mInitialEntityPos.x, mInitialEntityPos.y, mInitialEntityPos.z);

	OSSteerableBase::reset(); // reset the vehicle 

	static_cast<AbstractVehicle*>(this)->setUp(OpenSteer::Vec3(0, 1, 0));
	static_cast<AbstractVehicle*>(this)->setPosition(startPos);

    setSpeed (1.0f);         // speed along Forward direction.
    setMaxForce (mMaxForce);     // steering force is clipped to this magnitude
    setMaxSpeed (mMaxSpeed);     // velocity is clipped to this magnitude
	setRadius (mRadius);
}

void OSSimpleEntity::update(const float currentTime, const float elapsedTime)
{
	OpenSteer::AbstractVehicle* vehicle = this;

	// Apply Steering forces
	// ---------------------

	if (mEntiyType == OSEnumEntiyType::OS_ENTITY_TYPE_WANDERER)
	{
		int intTurningness = 6;

		const OpenSteer::Vec3 wander2d = steerForWander(elapsedTime * 24).setYtoZero();
		const OpenSteer::Vec3 steer = forward() + (wander2d * intTurningness);

		applySteeringForce (steer, elapsedTime);

	}
	else if (mEntiyType == OSEnumEntiyType::OS_ENTITY_TYPE_PURSUER)
	{
		if (mTargetEntity != 0)
		{
			OpenSteer::AbstractLocalSpace* targetLocalSpace = mTargetEntity;
			OpenSteer::AbstractVehicle* targetVehicle = mTargetEntity;

			const float d = Vec3::distance (position(), targetLocalSpace->position());
			const float r = radius() + targetVehicle->radius();
	        
			if (d < r) resetEntity();

			applySteeringForce (steerForPursuit (*targetVehicle), elapsedTime);
		}
	}
	
	// Update nodes and animations
	// ---------------------------
	mEntityNode->setPosition(Ogre::Vector3(vehicle->position().x,vehicle->position().y,vehicle->position().z));

	Ogre::Vector3 forwDir = Ogre::Vector3(vehicle->forward().x, vehicle->forward().y, vehicle->forward().z);

	mEntityNode->setDirection(forwDir, Ogre::SceneNode::TS_WORLD, mForwardVector);  

	if (hasAnimation)
		mEntityAnimState->addTime(elapsedTime * 1.0);
}

Ogre::Vector3 OSSimpleEntity::getPosition()
{
	OpenSteer::AbstractVehicle* vehicle = this;
	return Ogre::Vector3(vehicle->position().x,vehicle->position().y,vehicle->position().z);
}

void OSSimpleEntity::setPosition(Ogre::Vector3 newPos)
{
	mEntityNode->setPosition(newPos);
	static_cast<AbstractVehicle*>(this)->setPosition(OpenSteer::Vec3(newPos.x, newPos.y, newPos.z));
}

Ogre::Vector3 OSSimpleEntity::getDirection()
{
	OpenSteer::AbstractVehicle* vehicle = this;
	return Ogre::Vector3(vehicle->forward().x, vehicle->forward().y, vehicle->forward().z);
}
