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

#ifndef __OGRESTEER_MAIN_
#define __OGRESTEER_MAIN_

#include "CEGUI.h"
#include "Ogre.h"

#include "OgreSingleton.h"
#include "OgreKeyEvent.h"
#include "OgreEventListeners.h"
#include "OgreCEGUIRenderer.h"
#include "OgreCEGUIResourceProvider.h"

#include "OSCamera.h"
#include "OSScenarioRobots.h"
#include "OSScenarioRazors.h"

class OSGuiWindow;

class OgreSteerMain:	
	public Ogre::Singleton<OgreSteerMain>, 
	public Ogre::FrameListener, 
	public Ogre::KeyListener, 
	public Ogre::MouseMotionListener, 
	public Ogre::MouseListener
{

public:

	OgreSteerMain(); 
	virtual ~OgreSteerMain();

	void initApp();
	void shutDownApp();

	void showDebugOverlay(bool show);

	void setPreviewPanelActive(bool active);

	void setScenarioSelectedEntity(const std::string& entityName, bool previewEntity);

	void setScenarioSelectedCam(const std::string& camDescrip, bool previewCam);

	void changeScenarioByName(const std::string sceneName);

private:

	bool setup();
	bool configure();
	void setupResources();
	void loadResources();
	void setUpInput();
	void init_GUI_scene();
	void init_Steer_Scenarios();
	void destroy_Steer_Scenarios();
	void cycleScenario();
	void changeScenarioByID(int scenarioID);

	void takeScreenie();
	bool frameStarted(const Ogre::FrameEvent& evt);
	bool frameEnded(const Ogre::FrameEvent& evt);
	void updateGUIInputs(const float time);
	void updateStats();
	void setGUIMode(bool guiOn);
	void updateInputMode();
	void createSphere(const std::string& strName, const float r, const int nRings = 16, const int nSegments = 16);

	void keyPressed(Ogre::KeyEvent* e);
	void keyReleased(Ogre::KeyEvent* e);
	void keyClicked(Ogre::KeyEvent* e);

	void mouseReleased(Ogre::MouseEvent *e);
	void mousePressed(Ogre::MouseEvent *e);
	void mouseDragged(Ogre::MouseEvent *e);
	void mouseMoved(Ogre::MouseEvent *e);

	void mouseClicked(Ogre::MouseEvent *e) {}
	void mouseEntered(Ogre::MouseEvent *e) {}
	void mouseExited(Ogre::MouseEvent *e) {}

	CEGUI::MouseButton convertOgreButtonToCegui(int ogre_button_id);

	CEGUI::OgreCEGUIRenderer* mGUIRenderer;

	OSGuiWindow *mGuiWindowInst;

	// Scenario singletons
	OSScenarioRobots *mSceneBots; 
	OSScenarioRazors *mSceneRazors; 

	std::vector<OSBaseScenario*> mVecScenarios;

	Ogre::Root* mRoot;
	Ogre::RenderWindow* mWindow;
	Ogre::EventProcessor* mEventProcessor;
	Ogre::InputReader* mInputDevice;

	std::string mVersionString;

	bool mShutdown;
	bool mGuiInputMode;
	bool mGuiDisplayMode;
	bool mBoolAltDown;
	bool mBoolPreviewPanelActive;

	Ogre::Real mTimeUntilNextToggle;
	Ogre::Real mTotalElapsedTime;

	unsigned int mNumScreenShots;

	int mCurrentScenarioID;
};

#endif
