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

#include "OgreSteerMain.h"
#include "OSScenarioRobots.h"

using namespace Ogre;

template<> OSScenarioRobots* Ogre::Singleton<OSScenarioRobots>::ms_Singleton = 0;

OSScenarioRobots::OSScenarioRobots() 
  : OSBaseScenario("Robots Randomly Walk", ST_GENERIC)
{
	mSelectedEntityName = "";
	mPreviewEntityName = "";
	mInitialNumEntities = 10;
	mMaxNumEntities = 20;
	mLightNode = 0;
}

OSScenarioRobots::~OSScenarioRobots()
{}

void OSScenarioRobots::onUpdate(const float currentTime, const float elapsedTime)
{
	// Just move the light around above the camera
	mLightNode->setPosition(Vector3(mOgreCameraMain->getDerivedPosition().x, mOgreCameraMain->getDerivedPosition().y + 50.0f, mOgreCameraMain->getDerivedPosition().z));		
}

void OSScenarioRobots::onEnter()
{
	mScenarioRoot = Root::getSingletonPtr();

	mScenarioSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));

	mLightNode = mScenarioSceneMgr->getRootSceneNode()->createChildSceneNode();

	ColourValue colourLightBlue(0.5, 0.5, 0.7);

    mLight = mScenarioSceneMgr->createLight("CameraLight");
	mLight->setPosition(Vector3::ZERO);
	mLight->setDiffuseColour(colourLightBlue); 
	mLight->setSpecularColour(1, 1, 1);

	mScenarioSceneMgr->setFog(FOG_LINEAR, colourLightBlue, .001, 500, 5000);
	mScenarioViewport->setBackgroundColour(ColourValue(colourLightBlue));

	mLightNode->attachObject(mLight);

    Entity *ent;
    // Define a floor plane mesh
    Plane p;
    p.normal = Vector3::UNIT_Y;
    p.d = 0;
    MeshManager::getSingleton().createPlane(
        "FloorPlane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
        p, 10000, 10000, 10, 10, true, 1, 60, 60, Vector3::UNIT_Z);

    // Create an entity (the floor)
    ent = mScenarioSceneMgr->createEntity("floor", "FloorPlane");
    ent->setMaterialName("Scenario/Floor");

    // Attach to child of root node, better for culling (otherwise bounds are the combination of the 2)
    mScenarioSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

}

void OSScenarioRobots::onReset()
{

}

void OSScenarioRobots::onExit()
{
	
}

void OSScenarioRobots::createSimpleEntities()
{
	float seperation = 100.0f;
	float groundLevel = 0.0f;

	Vector3 newPosition;
	std::string newName;

	OSSimpleEntity* newEntity;

	for (int entityID = 0; entityID < mInitialNumEntities; entityID++)
    {
		newPosition = Vector3(0, groundLevel, (entityID*seperation) - (mInitialNumEntities * seperation/2));
		newName = "Wanderer Bot " + StringConverter::toString(entityID);
		newEntity = new OSSimpleEntity(newName, OSSimpleEntity::OSEnumEntiyType::OS_ENTITY_TYPE_WANDERER, mScenarioSceneMgr, OSSimpleEntity::CONST_STR_MODEL_ROBOT, newPosition);

		mAvailableEntities.push_back(newEntity);

		if (entityID == 0)
			setSelectedEntity(newName, false);
	}
}


