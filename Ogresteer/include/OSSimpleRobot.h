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

#ifndef __OGRESTEER_SIMPLEROBOT_
#define __OGRESTEER_SIMPLEROBOT_

#include "Ogre.h"

class OSSimpleRobot
{

public:

	// OSSimpleRobot Constants

	// Model Mesh Names
	static const std::string CONST_STR_MODEL_ROBOT;
	static const std::string CONST_STR_MODEL_RAZOR;
	// Model Animations
	static const std::string CONST_STR_ANIM_NONE;
	static const std::string CONST_STR_ANIM_BOT_IDLE;
	static const std::string CONST_STR_ANIM_BOT_WALK;
	static const std::string CONST_STR_ANIM_BOT_SHOOT;
	static const std::string CONST_STR_ANIM_BOT_SLUMP;
	static const std::string CONST_STR_ANIM_BOT_DIE;
    
	// Constructor / destructor
	OSSimpleRobot(const std::string& name, Ogre::SceneManager* newSceneMgr, const std::string& meshName, Ogre::Vector3 startPosition = Ogre::Vector3(0,0,0)); 
	~OSSimpleRobot();

	// Methods

	void resetBot();

	void update(const float elapsedTime);

	Ogre::SceneNode* getEntityNode(){return mBotNode;}

	const std::string& getEntityName(){return mBotName;}

	float getFirstPersonHeight(){return mFirstPersonHeight;}

private:

	Ogre::SceneManager* mSceneMgr;
	Ogre::SceneNode* mBotNode;
	Ogre::Entity* mBotEnt;
	Ogre::AnimationState* mBotAnimState;

	Ogre::Vector3 mInitialBotPosition;
	Ogre::Vector3 mLastBotPosition;

	float mFirstPersonHeight;

	std::string mBotName;
	std::string mBotAnimString;
	std::string mMeshName;

};

#endif