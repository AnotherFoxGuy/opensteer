/*
-----------------------------------------------------------------------------
This source	file is	part of	the	OgreSteer Application.

Author: Spannerman (spannerworx@gmail.com)
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
#include "OSScenarioRazors.h"
//#include "OgreTerrainSceneManager.h"

using namespace Ogre;

template<> OSScenarioRazors* Ogre::Singleton<OSScenarioRazors>::ms_Singleton = 0;

OSScenarioRazors::OSScenarioRazors() 
  : OSBaseScenario("Razors Chase Wanderer", ST_EXTERIOR_CLOSE)
{
	mSelectedEntityName = "";
	mPreviewEntityName = "";
	mInitialNumEntities = 4;
	mMaxNumEntities = 20;
	raySceneQuery = 0;
}

OSScenarioRazors::~OSScenarioRazors()
{
	delete raySceneQuery;
}

void OSScenarioRazors::onUpdate(const float currentTime, const float elapsedTime)
{
	// Cast Ray down onto terrain from entities

	static Ray updateRay;
	Vector3 currentPosition;

	for (ent_it = mAvailableEntities.begin(); ent_it != mAvailableEntities.end(); ++ent_it)
	{
		currentPosition = (*ent_it)->getPosition();

		updateRay.setOrigin(currentPosition);
		updateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);

		raySceneQuery->setRay(updateRay);

		RaySceneQueryResult& qryResult = raySceneQuery->execute();
		RaySceneQueryResult::iterator i = qryResult.begin();

		if (i != qryResult.end() && i->worldFragment)
		{
			SceneQuery::WorldFragment* wf = i->worldFragment;
			(*ent_it)->setPosition(Ogre::Vector3(currentPosition.x, i->worldFragment->singleIntersection.y + 200, currentPosition.z));
		}

	}
}

void OSScenarioRazors::onEnter()
{
	mScenarioRoot = Root::getSingletonPtr();

	ColourValue fogColour(0.8, 0.9, 1); // 102 153 153

	mScenarioSceneMgr->setFog(FOG_LINEAR, fogColour, .01, 800, 5000);
    mScenarioViewport->setBackgroundColour(fogColour);
	mScenarioSceneMgr->setAmbientLight(fogColour);

	Light* sunLight;

	sunLight = mScenarioSceneMgr->createLight("CameraLight");
	sunLight->setPosition(Vector3(0,5000,0));
    sunLight->setType(Light::LT_POINT);
    sunLight->setDiffuseColour(0.5, 0.5, 0.5);
    sunLight->setSpecularColour(1, 1, 1);

	mScenarioSceneMgr->setWorldGeometry("terrain.cfg");

    if (mScenarioRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_INFINITE_FAR_PLANE))
    {
        mOgreCameraMain->setFarClipDistance(0);
    }

	raySceneQuery = mScenarioSceneMgr->createRayQuery(Ray(mOgreCameraMain->getPosition(), Vector3::NEGATIVE_UNIT_Y));
}

void OSScenarioRazors::onReset()
{

}

void OSScenarioRazors::onExit()
{

}

void OSScenarioRazors::createSimpleEntities()
{
	float seperation = 500.0f;
	float groundLevel = 600.0f;

	Vector3 newPosition;
	std::string newName;

	OSSimpleEntity* newEntity;
	OSSimpleEntity* wanderer;

	// Create Wanderer
	newPosition = Vector3(6000, groundLevel, 6000);

	newName = "Wanderer Razor";
	wanderer = new OSSimpleEntity(newName, OSSimpleEntity::OSEnumEntiyType::OS_ENTITY_TYPE_WANDERER, mScenarioSceneMgr, OSSimpleEntity::CONST_STR_MODEL_RAZOR, newPosition);
	mAvailableEntities.push_back(wanderer);

	setSelectedEntity(newName, false);

	// Create Pursuers
	for (int entityID = 1; entityID < mInitialNumEntities; ++entityID)
    {
		newPosition = Vector3(6000 + (entityID*seperation), groundLevel, 6000);
		newName = "Pursuer Razor " + StringConverter::toString(entityID);
		newEntity = new OSSimpleEntity(newName, OSSimpleEntity::OSEnumEntiyType::OS_ENTITY_TYPE_PURSUER, mScenarioSceneMgr, OSSimpleEntity::CONST_STR_MODEL_RAZOR, newPosition);

		newEntity->setTargetEntity(wanderer);

		mAvailableEntities.push_back(newEntity);
	}
}