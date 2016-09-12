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

#include "OSSimpleRobot.h"

// Initialise Constants 
const std::string OSSimpleRobot::CONST_STR_MODEL_ROBOT("robot.mesh");
const std::string OSSimpleRobot::CONST_STR_MODEL_RAZOR("RZR-002.mesh");
const std::string OSSimpleRobot::CONST_STR_ANIM_NONE("None");
const std::string OSSimpleRobot::CONST_STR_ANIM_BOT_IDLE("Idle");
const std::string OSSimpleRobot::CONST_STR_ANIM_BOT_WALK("Walk");
const std::string OSSimpleRobot::CONST_STR_ANIM_BOT_SHOOT("Shoot");
const std::string OSSimpleRobot::CONST_STR_ANIM_BOT_SLUMP("Slump");
const std::string OSSimpleRobot::CONST_STR_ANIM_BOT_DIE("Die");


OSSimpleRobot::OSSimpleRobot(const std::string& name, Ogre::SceneManager* newSceneMgr, const std::string& meshName, Ogre::Vector3 startPosition)
 :	mBotName(name),
	mSceneMgr(newSceneMgr),
	mInitialBotPosition(startPosition),
	mLastBotPosition(startPosition),
	mBotNode(0),
	mBotEnt(0),
	mFirstPersonHeight(90.0f),
	mMeshName(meshName),
	mBotAnimString(CONST_STR_ANIM_NONE)
{
	assert(name != "");
	
	// Create robot parent sceneNode.
	mBotNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mInitialBotPosition);

	mBotEnt = mSceneMgr->createEntity(mBotName, mMeshName);
	mBotNode->attachObject(mBotEnt);

    resetBot();
}

OSSimpleRobot::~OSSimpleRobot()
{

}

void OSSimpleRobot::resetBot()
{
	mBotNode->setPosition(mInitialBotPosition);
	mBotNode->resetOrientation();

	// Setup the bots animation
    mBotAnimState = mBotEnt->getAnimationState(CONST_STR_ANIM_BOT_WALK);
    mBotAnimState->setEnabled(true);

	/*
	Vector3 nodeForwardVec = mBotNode->_getDerivedOrientation() * Vector3::NEGATIVE_UNIT_Z;

	OpenSteer::Vec3 startPos = OpenSteer::Vec3(initialBotPosition.x, initialBotPosition.y, initialBotPosition.z);

	SimpleVehicle::reset(); // reset the vehicle 

	static_cast<AbstractVehicle*>(this)->setUp(OpenSteer::Vec3(0, 1, 0));
	static_cast<AbstractVehicle*>(this)->setPosition(startPos);

    setSpeed (10.0f);         // speed along Forward direction.
    setMaxForce (100.0f);     // steering force is clipped to this magnitude
    setMaxSpeed (100.0f);     // velocity is clipped to this magnitude
	*/

}

void OSSimpleRobot::update(const float elapsedTime)
{

	mBotNode->setPosition(Ogre::Vector3(mBotNode->getPosition().x,mBotNode->getPosition().y,mBotNode->getPosition().z + (2.0f * elapsedTime)));

	mBotNode->yaw(Ogre::Degree(20.0f * elapsedTime));

	mBotAnimState->addTime(1.0f * elapsedTime);

	/*
	Vector3 newPos;
	Vector3 forwDir;

	// Apply Steering forces
	// ---------------------

    const OpenSteer::Vec3 wander2d = steerForWander(elapsedTime).setYtoZero();
    const OpenSteer::Vec3 steer = forward() + (wander2d * 6);
    applySteeringForce (steer, elapsedTime);
	
	//applySteeringForce(OpenSteer::Vec3(10, 0, -30), elapsedTime);

	
	// Update nodes and animations
	// ---------------------------

	AbstractVehicle* vehicle = this;

	Ogre::Vector3 nodePos(vehicle->position().x,vehicle->position().y,vehicle->position().z); 

	botTranslateNode->setPosition(newPos);

	forwDir.x = vehicle->forward().x;
	forwDir.y = vehicle->forward().y;
	forwDir.z = vehicle->forward().z;

	botRotateNode->setDirection(forwDir, Node::TS_LOCAL, Vector3::UNIT_X);

	botAnimState->addTime(elapsedTime * 1.0);

	lastBotPosition = newPos;

	*/
}