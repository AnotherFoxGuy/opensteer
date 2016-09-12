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

#ifndef __OGRESTEER_BASESCENARIO_
#define __OGRESTEER_BASESCENARIO_

#include "Ogre.h"
#include "OSCamera.h"
#include "OSGuiWindow.h"
#include "OSSimpleEntity.h"
#include "OgreHardwarePixelBuffer.h"

class OSBaseScenario: public Ogre::RenderTargetListener
{

public:

	// Base class constructor
	OSBaseScenario(const std::string& n, Ogre::SceneType type)
      : mSceneType(type),
		mScenarioName(n),
		mBoolOSCameraMainActive(true),
		mSelectedEntityName(""),
		mPreviewEntityName(""),
		mInitialNumEntities(2),
		mMaxNumEntities(20),
		mFogMode(FOG_NONE),
		mFogColour(ColourValue::White),
		mFogExpDensity(0.001), 
		mFogLinearStart(0.0), 
		mFoglinearEnd(1.0)
	{
		
	}

	// Base class destructor
	virtual ~OSBaseScenario()
	{
		removeSimpleEntities();
	}

	/*//////////////////////////////////////////////////////////////////////
	//
	// The following virtual methods MUST be overridden in child classes
	//
	/*//////////////////////////////////////////////////////////////////////

	// Update scene
	virtual void onUpdate(const float currentTime, const float elapsedTime) = 0;

	// Set up scene
	virtual void onEnter() = 0;

	// Reset scene
	virtual void onReset() = 0;

	// Destroy scene
	virtual void onExit() = 0;

	// Create scene specific Entities
	virtual void createSimpleEntities() = 0;


	/*--------------------------------------------------------*/

	// Public methods common to all BaseScenario children classes

	const std::string& getScenarioName() const { return mScenarioName; }
	/*--------------------------------------------------------*/

	void updateScenario(const float currentTime, const float elapsedTime)
	{
		mOSCameraMain->UpdateCamera(elapsedTime);

		// Only update if the preview window is showing
		if (!mBoolOSCameraMainActive)
		{
			mOSCameraPreview->UpdateCamera(elapsedTime);

			if (mPreviewEntity != 0)
			{
				std::stringstream stats;
				Ogre::Vector3 pos = mPreviewEntity->getPosition();

				stats << "Position: ";
				stats << RoundUp(pos.x,0) << " ";
				stats << RoundUp(pos.y,0) << " ";
				stats << RoundUp(pos.z,0);
				stats << "\nSpeed: ";
				stats << RoundUp(mPreviewEntity->speed());

				OSGuiWindow::getSingletonPtr()->updateEntityStats(stats.str());
			}

			// Look at annotationVelocityAcceleration
		}

		for (ent_it = mAvailableEntities.begin(); ent_it != mAvailableEntities.end(); ++ent_it)
		{
			(*ent_it)->update(currentTime, elapsedTime);
		}

		onUpdate(currentTime, elapsedTime);
	}
	/*--------------------------------------------------------*/

	void exitScenario()
	{
		mScenarioRoot->queueEndRendering();

		mOSCameraMain->setTargetNode(0);
		mOSCameraPreview->setTargetNode(0);

		removeSimpleEntities();
		removeOSCameras();

		mScenarioSceneMgr->clearScene();
		mScenarioSceneMgr->destroyAllCameras();
		mScenarioRoot->getAutoCreatedWindow()->removeAllViewports();

		onExit();
	}
	/*--------------------------------------------------------*/

	void resetScenario()
	{
		removeSimpleEntities();
		createSimpleEntities();

		std::vector<std::string> vecEntityNames;

		for (ent_it = mAvailableEntities.begin(); ent_it != mAvailableEntities.end(); ++ent_it)
		{
			(*ent_it)->resetEntity();

			vecEntityNames.push_back((*ent_it)->getEntityName());
		}

		setMainOSCamActive(true);
		setSelectedCam(OSCamera::OS_CAM_THIRDPERSON,false,true);

		OSGuiWindow::getSingletonPtr()->populateEntityList(vecEntityNames);

		mScenarioRoot->startRendering();
	}
	/*--------------------------------------------------------*/

	void enterScenario(CEGUI::OgreCEGUIRenderer* currentRenderer)
	{
		std::string strRenderTargetTexture = "RenderTargetTexture";

		mScenarioRoot = Root::getSingletonPtr();

		// Link up the scenario specific scenemanager
		mScenarioSceneMgr = mScenarioRoot->getSceneManager(mSceneType);

		// Update CEGUI with the new scenemanager.
		currentRenderer->setTargetSceneManager(mScenarioSceneMgr);

		mOgreCameraMain = mScenarioSceneMgr->createCamera("MainCamera");
		mOgreCameraMain->setNearClipDistance(2);

		mOSCameraMain = new OSCamera(mOgreCameraMain, true, true);

		mScenarioViewport = mScenarioRoot->getAutoCreatedWindow()->addViewport(mOgreCameraMain);
		mScenarioViewport->setBackgroundColour(ColourValue(0.0, 0.0, 0.0));

		mOgreCameraMain->setAspectRatio(Real(mScenarioViewport->getActualWidth()) / Real(mScenarioViewport->getActualHeight()));

		// Setup Render To Texture (cleaning up any pointers remaining from previous scenarios)

		if (TextureManager::getSingleton().resourceExists(strRenderTargetTexture))
		{
			mTexture = TextureManager::getSingleton().getByName(strRenderTargetTexture);

			rttTarget = mTexture->getBuffer()->getRenderTarget();
			rttTarget->removeAllListeners();
			rttTarget->removeAllViewports();

			TextureManager::getSingleton().remove(strRenderTargetTexture);
		}
        mTexture = TextureManager::getSingleton().createManual( strRenderTargetTexture, 
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 
			256, 256, 0, PF_R8G8B8, TU_RENDERTARGET );

		rttTarget = mTexture->getBuffer()->getRenderTarget();
		{
			mOgreCameraRTT = mScenarioSceneMgr->createCamera("RTTCamera");
			mOgreCameraRTT->setNearClipDistance(2);
			mOgreCameraRTT->setFarClipDistance(OSCamera::CONST_FLT_CAM_CUTOFF_HEIGHT);

			rttVP = rttTarget->addViewport(mOgreCameraRTT);
			rttVP->setOverlaysEnabled(false);
			rttVP->setClearEveryFrame(true);
			rttVP->setBackgroundColour(ColourValue::Black);

			rttTarget->addListener(this);
		}

		mOSCameraPreview = new OSCamera(mOgreCameraRTT, false, false);

		CEGUI::Texture* rttTexture = currentRenderer->createTexture(mTexture);

		if (CEGUI::ImagesetManager::getSingleton().isImagesetPresent("RttImageset"))
		{
			CEGUI::ImagesetManager::getSingleton().destroyImageset("RttImageset");
		}
        CEGUI::Imageset* rttImageSet = CEGUI::ImagesetManager::getSingleton().createImageset("RttImageset", rttTexture);

		if (rttImageSet->isImageDefined("RttImage"))
		{
			rttImageSet->undefineImage("RttImage");
		}
		rttImageSet->defineImage("RttImage", CEGUI::Point(0.0f, 0.0f), CEGUI::Size(rttTexture->getWidth(), rttTexture->getHeight()), CEGUI::Point(0.0f,0.0f));

		OSGuiWindow::getSingletonPtr()->setImageRTTStaticImage(rttImageSet,"RttImage");

		// Set the primary camera
		mScenarioSceneMgr->setOption("PrimaryCamera", mOgreCameraMain);

		// Set up the specific scene content
		onEnter();

		// store any fog values
		mFogMode = mScenarioSceneMgr->getFogMode();
		mFogColour = mScenarioSceneMgr->getFogColour();
		mFogExpDensity = mScenarioSceneMgr->getFogDensity();
		mFogLinearStart = mScenarioSceneMgr->getFogStart();
		mFoglinearEnd = mScenarioSceneMgr->getFogEnd();

		// Populate and reset the scene of entities
		resetScenario();
	}
	/*--------------------------------------------------------*/

	// Cycle cameras
	void cycleSelectedCamera()
	{
		if (mBoolOSCameraMainActive)
		{
			mOSCameraMain->cycleCameraMode();

			if (mOSCameraMain->getCurrentCamModeEnum() != OSCamera::OS_CAM_FREE)
			{
				mOSCameraPreview->setCameraMode(mOSCameraMain->getCurrentCamModeEnum());
				OSGuiWindow::getSingletonPtr()->setListPreviewCam(mOSCameraPreview->getCurrentCamModeString());
			}
		}
		else
		{
			mOSCameraPreview->cycleCameraMode();
			OSGuiWindow::getSingletonPtr()->setListPreviewCam(mOSCameraPreview->getCurrentCamModeString());
		}
	}
	/*--------------------------------------------------------*/

	void setSelectedCam(const OSCamera::OSCameraEnumMode newMode, bool previewCam, bool updateGUI = false)
	{
		if (previewCam)
		{
			mOSCameraPreview->setCameraMode(newMode);

			if (updateGUI)
				OSGuiWindow::getSingletonPtr()->setListPreviewCam(mOSCameraPreview->getCurrentCamModeString());
		}
		else
		{
			mOSCameraMain->setCameraMode(newMode);

			if (updateGUI && (newMode != OSCamera::OS_CAM_FREE))
				OSGuiWindow::getSingletonPtr()->setListPreviewCam(mOSCameraMain->getCurrentCamModeString());
		}
	}
	/*--------------------------------------------------------*/

	OSCamera* getActiveOSCam() 
	{
		if (mBoolOSCameraMainActive)
			return mOSCameraMain;
		else
			return mOSCameraPreview;
	}
	/*--------------------------------------------------------*/

	void setMainOSCamActive(bool mainCamActive)
	{
		mBoolOSCameraMainActive = mainCamActive;
	}
	/*--------------------------------------------------------*/

	// Cycle targets
	void cycleSelectedEntity()
	{
		bool isPreviewEntity = false;
		bool getNextEntity = false;
		std::string currentEntityName = mSelectedEntityName;
		std::string nextEntityName = "";

		if (!mBoolOSCameraMainActive)
		{
			isPreviewEntity = true;
			currentEntityName = mPreviewEntityName;
		}

		for (int entityID = 0; entityID < getNumSimpleEntities(); ++entityID)
		{
			std::string tempEntityName = mAvailableEntities[entityID]->getEntityName();

			if (getNextEntity)
			{
				// Found the new 'next' entity
				nextEntityName = tempEntityName;
				break;
			}	
			else if (tempEntityName == currentEntityName)
			{
				// Found current entity, so use next one
				getNextEntity = true;
			}
		}

		if (nextEntityName == "")
		{
			// Not found a entity yet so it will be the first one!
			nextEntityName = mAvailableEntities[0]->getEntityName();
		}

		setSelectedEntity(nextEntityName, isPreviewEntity, true);
	}
	/*--------------------------------------------------------*/

	void setSelectedEntity(const std::string& entityName, bool ispreviewEntity, bool updateGUI = false)
	{
		OSSimpleEntity* selectedEntity = getSimpleEntity(entityName);
		Vector3 forwardVec = Vector3::UNIT_Z;

		if (selectedEntity->getForwardVector() == Vector3::UNIT_X)
		{
			forwardVec = Vector3::UNIT_Z;
		}
		else if (selectedEntity->getForwardVector() == Vector3::UNIT_Z)
		{
			forwardVec = Vector3::NEGATIVE_UNIT_X;
		}

		if (ispreviewEntity || updateGUI || (mOSCameraPreview->getTargetNode() == 0))
		{
			mPreviewEntityName = entityName;
			mPreviewEntity = getSimpleEntity(mPreviewEntityName);
			mOSCameraPreview->setTargetNode(selectedEntity->getEntityNode());
			mOSCameraPreview->setFirstPersonHeight(selectedEntity->getFirstPersonHeight());
			mOSCameraPreview->setThirdPersonDistance(selectedEntity->getThirdPersonDistance());
			mOSCameraPreview->setForwardVec(forwardVec);
		}

		if (!ispreviewEntity)
		{
			mSelectedEntityName = entityName;
			mOSCameraMain->setTargetNode(selectedEntity->getEntityNode());
			mOSCameraMain->setFirstPersonHeight(selectedEntity->getFirstPersonHeight());
			mOSCameraMain->setThirdPersonDistance(selectedEntity->getThirdPersonDistance());
			mOSCameraMain->setForwardVec(forwardVec);
		}

		if (updateGUI)
			OSGuiWindow::getSingletonPtr()->setListPreviewEntity(entityName);
	}
	/*--------------------------------------------------------*/

	OSSimpleEntity* getSimpleEntity(const std::string& entityName)
	{
		OSSimpleEntity* theEntity;

		for (int entityID = 0; entityID < getNumSimpleEntities(); ++entityID)
		{
			theEntity = mAvailableEntities[entityID];

			if (theEntity->getEntityName() == entityName)
				break;
		}

		return theEntity;
	}
	/*--------------------------------------------------------*/

	const std::vector<OSSimpleEntity*>& getAvailableEntities() const { return mAvailableEntities; }
	/*--------------------------------------------------------*/

	int getNumSimpleEntities() { return mAvailableEntities.empty() ? 0 : static_cast<int>(mAvailableEntities.size()); }
	/*--------------------------------------------------------*/
	
protected:

	// Protected member variables

	std::string mScenarioName;

	Ogre::Root *mScenarioRoot;
	Ogre::SceneManager* mScenarioSceneMgr;
	Ogre::Viewport* mScenarioViewport;
	Ogre::Camera* mOgreCameraMain;
	Ogre::Camera* mOgreCameraRTT;
	Ogre::TexturePtr mTexture;
	Ogre::RenderTarget *rttTarget;
	Ogre::Viewport *rttVP;
	Ogre::SceneType mSceneType;

	OSCamera *mOSCameraMain;
	OSCamera *mOSCameraPreview;

	bool mBoolOSCameraMainActive;

	std::vector<OSSimpleEntity*> mAvailableEntities;
	std::vector<OSSimpleEntity*>::const_iterator ent_it;

	std::string mSelectedEntityName;
	std::string mPreviewEntityName;

	OSSimpleEntity* mPreviewEntity;

	int mInitialNumEntities;
	int mMaxNumEntities;

	FogMode mFogMode;
	ColourValue mFogColour;
	Real mFogExpDensity;
	Real mFogLinearStart;
	Real mFoglinearEnd;

	// Protected methods

	void preRenderTargetUpdate(const RenderTargetEvent& evt)
	{
		mScenarioSceneMgr->setFog(FOG_LINEAR, ColourValue::Black, 0.1, OSCamera::CONST_FLT_CAM_CUTOFF_HEIGHT - (OSCamera::CONST_FLT_CAM_CUTOFF_HEIGHT * 0.2f), OSCamera::CONST_FLT_CAM_CUTOFF_HEIGHT + (OSCamera::CONST_FLT_CAM_CUTOFF_HEIGHT * 0.1f));
	}
	/*--------------------------------------------------------*/

	void postRenderTargetUpdate(const RenderTargetEvent& evt)
	{
		mScenarioSceneMgr->setFog(mFogMode, mFogColour, mFogExpDensity, mFogLinearStart, mFoglinearEnd);
	}
	/*--------------------------------------------------------*/

	void removeSimpleEntities()
	{
		for (ent_it = mAvailableEntities.begin(); ent_it != mAvailableEntities.end(); ++ent_it) 
			delete *ent_it;

		mAvailableEntities.clear();
	}
	/*--------------------------------------------------------*/

	void removeOSCameras()
	{
		if (mOSCameraMain)
		{
			delete mOSCameraMain;
			mOSCameraMain = 0;
		}

		if (mOSCameraPreview)
		{
			delete mOSCameraPreview;
			mOSCameraPreview = 0;
		}
	}
	/*--------------------------------------------------------*/

	float RoundUp(float Value, int NumPlaces = 1)
	{
		int k, Temp;
		float Factor = 1;

		for (k = 0; k < NumPlaces; k++)
			Factor = Factor * 10;
		      
		Temp = Value * Factor + 0.5;
		return Temp / Factor;
	}

	/*--------------------------------------------------------*/

};

#endif
