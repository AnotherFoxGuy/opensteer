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
----------------------------------------------------------------------------
*/

#include "OgreSteerMain.h"

#include "OgreSingleton.h"
#include "OgreConfigFile.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "OgreEventQueue.h"

#include "OSGuiWindow.h"

template<> OgreSteerMain* Ogre::Singleton<OgreSteerMain>::ms_Singleton = 0;

using namespace Ogre;

OgreSteerMain::OgreSteerMain()
  : mRoot(0),
	mVersionString("OgreSteer V0.01"),
    mShutdown(false),
	mGuiInputMode(true),
	mGuiDisplayMode(true),
	mBoolPreviewPanelActive(false),
	mTimeUntilNextToggle(0),
	mTotalElapsedTime(0),
	mNumScreenShots(0),
	mCurrentScenarioID(-1)
{
}

OgreSteerMain::~OgreSteerMain()
{
	LogManager::getSingleton().logMessage( "__OgreSteer closing down cleanly.__");

	mRoot->queueEndRendering();

	destroy_Steer_Scenarios();

	if (mGuiWindowInst)
	{
		delete mGuiWindowInst;
		mGuiWindowInst = 0;
	}

	if (CEGUI::System::getSingletonPtr())
		delete CEGUI::System::getSingletonPtr();

	if (mGUIRenderer)
	{
		delete mGUIRenderer;
		mGUIRenderer = 0;
	}

	if (mEventProcessor)
	{
		delete mEventProcessor;
		mEventProcessor = 0;
	}

	if (mRoot)
	{
		delete mRoot;
		mRoot = 0;
	}
}


void OgreSteerMain::initApp()
{
	if (!setup())
	{
		return;
	}
}

void OgreSteerMain::shutDownApp()
{
	mShutdown = true;
}

bool OgreSteerMain::setup()
{
	mRoot =	new	Root();

	setupResources();

	bool carryOn = configure();
	if (!carryOn) return false;

	loadResources();

	setUpInput();

	mRoot->addFrameListener(this);

	showDebugOverlay(true);

	init_GUI_scene();

	init_Steer_Scenarios();

	return true;
}

bool OgreSteerMain::configure()
{
	if(mRoot->showConfigDialog())
	{
		mWindow	= mRoot->initialise( true, mVersionString );
		return true;
	}
	else
	{
		return false;
	}
}

void OgreSteerMain::init_GUI_scene(void)
{
	// setup GUI system
	mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000);
	new	CEGUI::System(mGUIRenderer);

	try
	{
		using namespace	CEGUI;
		Logger::getSingleton().setLoggingLevel(Informative); //Errors, Standard, Informative, Insane

		// load	schemes and imagesets, set up defaults and create fonts
		SchemeManager::getSingleton().loadScheme("WindowsLook.scheme");
		System::getSingleton().setDefaultMouseCursor("WindowsLook", "MouseArrow");

		CEGUI::Font* normalfont = CEGUI::FontManager::getSingleton().createFont("normalfont", "OMICZ___.TTF", 10, 0);
		normalfont->setAutoScalingEnabled(true);
		normalfont->setNativeResolution(Size(1024, 768));

		CEGUI::Font* titlefont = CEGUI::FontManager::getSingleton().createFont("titlefont", "OMICZ___.TTF", 12, 0);
		titlefont->setAutoScalingEnabled(true);
		titlefont->setNativeResolution(Size(1024, 768));

		System::getSingleton().setDefaultFont(normalfont->getName());

		WindowManager& winMgr =	WindowManager::getSingleton();
		Window*	sheet =	winMgr.createWindow("DefaultGUISheet", "OS_Root_GUI_Sheet");
		System::getSingleton().setGUISheet(sheet);

		// Create the GUI singleton class to setup all the GUI widgets and event handlers
		mGuiWindowInst = new OSGuiWindow();

	}
	// catch to	prevent	exit (errors will be logged).
	catch(CEGUI::Exception)
	{}
}

void OgreSteerMain::init_Steer_Scenarios(void)
{
	mCurrentScenarioID = 0;
	std::vector<std::string> vecSceneNames;

	Animation::setDefaultInterpolationMode(Animation::IM_LINEAR);
    Animation::setDefaultRotationInterpolationMode(Animation::RIM_LINEAR);

	mSceneBots = new OSScenarioRobots();
	mVecScenarios.push_back(mSceneBots);
	vecSceneNames.push_back(mSceneBots->getScenarioName());

	mSceneRazors = new OSScenarioRazors();
	mVecScenarios.push_back(mSceneRazors);
	vecSceneNames.push_back(mSceneRazors->getScenarioName());

	OSGuiWindow::getSingletonPtr()->populateScenarioList(vecSceneNames);

	mVecScenarios[mCurrentScenarioID]->enterScenario(mGUIRenderer);
}

void OgreSteerMain::destroy_Steer_Scenarios(void)
{
	if (mCurrentScenarioID > -1)
		mVecScenarios[mCurrentScenarioID]->exitScenario();

	if (mSceneBots)
	{
		delete mSceneBots;
		mSceneBots = 0;
	}

	if (mSceneRazors)
	{
		delete mSceneRazors;
		mSceneRazors = 0;
	}

	mVecScenarios.clear();
}

void OgreSteerMain::cycleScenario(void)
{
	// select next
	int nextScenario = mCurrentScenarioID + 1;

	// or first
	if (nextScenario >= mVecScenarios.size())
	{
		nextScenario = 0;
	}

	changeScenarioByID(nextScenario);
}

void OgreSteerMain::changeScenarioByID(int scenarioID)
{
	mVecScenarios[mCurrentScenarioID]->exitScenario();

	mCurrentScenarioID = scenarioID;

	mVecScenarios[mCurrentScenarioID]->enterScenario(mGUIRenderer);
}

void OgreSteerMain::changeScenarioByName(const std::string sceneName)
{
	// Very bad code here, I should loop through all scenarios comparing names, but I know there is only two, so...
	if (mVecScenarios[mCurrentScenarioID]->getScenarioName() == sceneName)
	{
		changeScenarioByID(mCurrentScenarioID);
	}
	else
	{
		cycleScenario();
	}
}

void OgreSteerMain::setupResources()
{
    // Load resource paths from config file
    ConfigFile cf;
    cf.load("resources.cfg");

    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}

void OgreSteerMain::loadResources()
{
	// Initialise, parse scripts etc
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void OgreSteerMain::showDebugOverlay(bool show)
{
	Overlay* o = (Overlay*)OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	if (show)
		o->show();
	else
		o->hide();
}

void OgreSteerMain::takeScreenie()
{
	char tmp[30];
	sprintf(tmp, "OgreSteer_Screenie_%d.png", ++mNumScreenShots);
	mWindow->writeContentsToFile(tmp);

	std::string filename = tmp;

	mTimeUntilNextToggle = 0.1f;
}

bool OgreSteerMain::frameStarted(const FrameEvent& evt)
{
	if (mShutdown)
	{
		return false;
	}

	if (mCurrentScenarioID != -1)
	{
		//mWindow->setDebugText(mVecScenarios[mCurrentScenarioID]->getScenarioName());
		//mWindow->setDebugText(mVecScenarios[mCurrentScenarioID]->getActiveOSCam()->getCurrentCamModeString());
	}

	if (mTimeUntilNextToggle >= 0)
		mTimeUntilNextToggle -= evt.timeSinceLastFrame;

	return true;
}

bool OgreSteerMain::frameEnded(const FrameEvent& evt)
{
	if (mShutdown)
	{
		return false;
	}

	float elapsedTime = evt.timeSinceLastFrame;

	mTotalElapsedTime = mTotalElapsedTime + elapsedTime;

	if (mCurrentScenarioID != -1)
	{
		mVecScenarios[mCurrentScenarioID]->updateScenario(mTotalElapsedTime, elapsedTime);
	
		if (!mGuiInputMode)
			updateGUIInputs(elapsedTime);
	}

	updateStats();

	return true;
}

void OgreSteerMain::updateGUIInputs(const float time)
{
	OSCamera::OSCameraEnumMode mCurrentCam = mVecScenarios[mCurrentScenarioID]->getActiveOSCam()->getCurrentCamModeEnum();

	if (mCurrentCam == OSCamera::OS_CAM_FREE)
	{
		float moveScale;
		moveScale = 200 * time;

		Vector3 vec;
		vec = Vector3::ZERO;

		if (mInputDevice->isKeyDown(KC_LEFT) || mInputDevice->isKeyDown(KC_A))
			vec.x = -moveScale; // Move camera left

		if (mInputDevice->isKeyDown(KC_RIGHT) || mInputDevice->isKeyDown(KC_D))
			vec.x = moveScale; // Move camera RIGHT

		if (mInputDevice->isKeyDown(KC_UP) || mInputDevice->isKeyDown(KC_W))
			vec.z = -moveScale; // Move camera forward

		if (mInputDevice->isKeyDown(KC_DOWN) || mInputDevice->isKeyDown(KC_S))
			vec.z = moveScale; // Move camera backward

		if (mInputDevice->isKeyDown(KC_PGUP))
			vec.y = moveScale; // Move camera up

		if (mInputDevice->isKeyDown(KC_PGDOWN))
			vec.y = -moveScale; // Move camera down
	
		mVecScenarios[mCurrentScenarioID]->getActiveOSCam()->setOgreCamVec(vec);

		Radian rotX, rotY;

		// Rotate view by mouse relative position
		rotX = Degree(-mInputDevice->getMouseRelativeX() * 0.12);
		rotY = Degree(-mInputDevice->getMouseRelativeY() * 0.12);

		mVecScenarios[mCurrentScenarioID]->getActiveOSCam()->setCamRot(rotX,rotY);
	}
}

void OgreSteerMain::updateStats()
{
    static String currFps = "Current FPS: ";
    static String avgFps = "Average FPS: ";
    static String bestFps = "Best FPS: ";
    static String worstFps = "Worst FPS: ";
    static String tris = "Triangle Count: ";

    // update stats when necessary
    try {
        OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
        OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
        OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
        OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

        const RenderTarget::FrameStats& stats = mWindow->getStatistics();

        guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
        guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
        guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
            +" "+StringConverter::toString(stats.bestFrameTime)+" ms");
        guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
            +" "+StringConverter::toString(stats.worstFrameTime)+" ms");

        OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
        guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

        OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
        guiDbg->setCaption(mWindow->getDebugText());
    }
    catch(...)
    {
        // ignore
    }
}

void OgreSteerMain::setGUIMode(bool guiOn)
{
	mGuiDisplayMode = guiOn;

	// Center mouse
	CEGUI::Renderer* rend =	CEGUI::System::getSingleton().getRenderer();
	CEGUI::System::getSingleton().injectMousePosition(0.5f * rend->getWidth(), 0.5f	* rend->getHeight());

	// hide or show all CEGUI renderables
	CEGUI::System::getSingleton().getGUISheet()->setVisible(mGuiDisplayMode);

	// Set the correct camera active
	if (!mGuiDisplayMode)
	{
		setPreviewPanelActive(false);
	}
	else
	{
		OSGuiWindow::getSingletonPtr()->checkPreviewPanelActive(); 
	}

	updateInputMode();
}

void OgreSteerMain::updateInputMode()
{
	if (!mGuiDisplayMode)
	{
		mGuiInputMode = false;
		CEGUI::MouseCursor::getSingleton().hide();
	}
	else
	{
		if (mCurrentScenarioID != -1)
		{
			OSCamera::OSCameraEnumMode mCurrentCam = mVecScenarios[mCurrentScenarioID]->getActiveOSCam()->getCurrentCamModeEnum();

			if (mCurrentCam == OSCamera::OS_CAM_FREE)
			{
				mGuiInputMode = false;
				CEGUI::MouseCursor::getSingleton().hide();
			}
			else
			{
				mGuiInputMode = true;
				CEGUI::MouseCursor::getSingleton().show();
			}
			}
	}
	
	mInputDevice->setBufferedInput(true, mGuiInputMode);
}

void OgreSteerMain::setUpInput()
{
	mEventProcessor	= new EventProcessor();
	mEventProcessor->initialise(mWindow);
	mEventProcessor->startProcessingEvents();
	mEventProcessor->addKeyListener(this);
	mEventProcessor->addMouseMotionListener(this);
	mEventProcessor->addMouseListener(this);
	mInputDevice = mEventProcessor->getInputReader();
	mInputDevice->setBufferedInput(true, true);
}

void OgreSteerMain::keyPressed(KeyEvent* e)
{
	// give	'quitting' priority
	if (e->getKey()	== KC_ESCAPE)
	{

		shutDownApp();

		return;
	}

	// Alt keys
	if (e->getKey()	== KC_LMENU || e->getKey() == KC_RMENU)
	{
		mBoolAltDown = true;
	}

	if (mGuiInputMode)
	{
		// do event	injection
		CEGUI::System& cegui = CEGUI::System::getSingleton();
		// key down
		cegui.injectKeyDown(e->getKey());
		// now character
		cegui.injectChar(e->getKeyChar());

		// Zoom keys
		if (e->getKey()	== KC_PGUP)
		{
			
		}

		if (mBoolAltDown)
		{
		}
	}

	e->consume();
}

void OgreSteerMain::keyReleased(KeyEvent *e)
{
	if (mGuiInputMode)
	{

	}
}

void OgreSteerMain::keyClicked(KeyEvent *e)
{
	if ((e->getKey() == KC_F1) && (mTimeUntilNextToggle <= 0))
	{
		setGUIMode(!mGuiDisplayMode);
	}
	else if ((e->getKey() == KC_TAB) && (mTimeUntilNextToggle <= 0))
	{
		cycleScenario();
	}
	else if ((e->getKey() == KC_SYSRQ) && (mTimeUntilNextToggle <= 0))
	{
		takeScreenie();
	}
	else if ((e->getKey() == KC_C) && (mTimeUntilNextToggle <= 0))
	{
		mVecScenarios[mCurrentScenarioID]->cycleSelectedCamera();
		updateInputMode();
	}
	else if ((e->getKey() == KC_T) && (mTimeUntilNextToggle <= 0))
	{
		if (mVecScenarios[mCurrentScenarioID]->getActiveOSCam()->getCurrentCamModeEnum() != OSCamera::OS_CAM_FREE)
			mVecScenarios[mCurrentScenarioID]->cycleSelectedEntity();
	}

	mTimeUntilNextToggle = 0.2f;
}

void OgreSteerMain::setScenarioSelectedEntity(const std::string& entityName, bool previewEntity)
{
	if (mCurrentScenarioID != -1)
		mVecScenarios[mCurrentScenarioID]->setSelectedEntity(entityName, previewEntity);
}

void OgreSteerMain::setScenarioSelectedCam(const std::string& camDescrip, bool previewCam)
{
	if (mCurrentScenarioID != -1)
	{
		OSCamera::OSCameraEnumMode newMode = OSCamera::OS_CAM_THIRDPERSON;

		if (camDescrip == OSCamera::CONST_STR_CAM_FREE_DESCRIPT) 
		{
			newMode = OSCamera::OS_CAM_FREE;
		}
		else if (camDescrip == OSCamera::CONST_STR_CAM_THIRDPERSON_DESCRIPT)
		{
			newMode = OSCamera::OS_CAM_THIRDPERSON;
		}
		else if (camDescrip == OSCamera::CONST_STR_CAM_FIRSTPERSON_DESCRIPT)
		{
			newMode = OSCamera::OS_CAM_FIRSTPERSON;
		}
		else if (camDescrip == OSCamera::CONST_STR_CAM_TOPDOWN_DESCRIPT)
		{
			newMode = OSCamera::OS_CAM_TOPDOWN;
		}
		else if (camDescrip == OSCamera::CONST_STR_CAM_ROTATE_DESCRIPT)
		{
			newMode = OSCamera::OS_CAM_ROTATE;
		}

		mVecScenarios[mCurrentScenarioID]->setSelectedCam(newMode, previewCam);
	}
}

void OgreSteerMain::mouseMoved(MouseEvent *e)
{
	if (mGuiInputMode)
	{
		CEGUI::Renderer* rend =	CEGUI::System::getSingleton().getRenderer();
		CEGUI::System::getSingleton().injectMouseMove(e->getRelX() * rend->getWidth(), e->getRelY()	* rend->getHeight());
	}
	e->consume();
}

void OgreSteerMain::mouseDragged(MouseEvent *e)
{
	mouseMoved(e);
}

void OgreSteerMain::mousePressed(MouseEvent *e)
{
	if (mGuiInputMode)
	{
		CEGUI::System::getSingleton().injectMouseButtonDown(convertOgreButtonToCegui(e->getButtonID()));
	}
	e->consume();
}

void OgreSteerMain::mouseReleased(MouseEvent *e)
{
	if (mGuiInputMode)
	{
		CEGUI::System::getSingleton().injectMouseButtonUp(convertOgreButtonToCegui(e->getButtonID()));
	}
	e->consume();
}

void OgreSteerMain::setPreviewPanelActive(bool active)
{
	mBoolPreviewPanelActive = active;

	if (mCurrentScenarioID != -1)
	{
		mVecScenarios[mCurrentScenarioID]->setMainOSCamActive(!mBoolPreviewPanelActive);
	}
}

CEGUI::MouseButton OgreSteerMain::convertOgreButtonToCegui(int ogre_button_id)
{
	switch (ogre_button_id)
	{
	case MouseEvent::BUTTON0_MASK:
		return CEGUI::LeftButton;
		break;

	case MouseEvent::BUTTON1_MASK:
		return CEGUI::RightButton;
		break;

	case MouseEvent::BUTTON2_MASK:
		return CEGUI::MiddleButton;
		break;

	case MouseEvent::BUTTON3_MASK:
		return CEGUI::X1Button;
		break;

	default:
		return CEGUI::LeftButton;
		break;
	}
}

// createSphere method taken from the OGRE Wiki
void OgreSteerMain::createSphere(const std::string& strName, const float r, const int nRings , const int nSegments)
{
	MeshPtr pSphere = MeshManager::getSingleton().createManual(strName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	SubMesh *pSphereVertex = pSphere->createSubMesh();

	pSphere->sharedVertexData = new VertexData();
	VertexData* vertexData = pSphere->sharedVertexData;

	// define the vertex format
	VertexDeclaration* vertexDecl = vertexData->vertexDeclaration;
	size_t currOffset = 0;
	// positions
	vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_POSITION);
	currOffset += VertexElement::getTypeSize(VET_FLOAT3);
	// normals
	vertexDecl->addElement(0, currOffset, VET_FLOAT3, VES_NORMAL);
	currOffset += VertexElement::getTypeSize(VET_FLOAT3);
	// two dimensional texture coordinates
	vertexDecl->addElement(0, currOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
	currOffset += VertexElement::getTypeSize(VET_FLOAT2);

	// allocate the vertex buffer
	vertexData->vertexCount = (nRings + 1) * (nSegments+1);
	HardwareVertexBufferSharedPtr vBuf = HardwareBufferManager::getSingleton().createVertexBuffer(vertexDecl->getVertexSize(0), vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
	VertexBufferBinding* binding = vertexData->vertexBufferBinding;
	binding->setBinding(0, vBuf);
	Real* pVertex = static_cast<Real*>(vBuf->lock(HardwareBuffer::HBL_DISCARD));

	// allocate index buffer
	pSphereVertex->indexData->indexCount = 6 * nRings * (nSegments + 1);
	pSphereVertex->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(HardwareIndexBuffer::IT_16BIT, pSphereVertex->indexData->indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
	HardwareIndexBufferSharedPtr iBuf = pSphereVertex->indexData->indexBuffer;
	unsigned short* pIndices = static_cast<unsigned short*>(iBuf->lock(HardwareBuffer::HBL_DISCARD));

	float fDeltaRingAngle = (Math::PI / nRings);
	float fDeltaSegAngle = (2 * Math::PI / nSegments);
	unsigned short wVerticeIndex = 0 ;

	// Generate the group of rings for the sphere
	for( int ring = 0; ring <= nRings; ring++ ) {
		float r0 = r * sinf (ring * fDeltaRingAngle);
		float y0 = r * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++) {
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			*pVertex++ = x0;
			*pVertex++ = y0;
			*pVertex++ = z0;

			Vector3 vNormal = Vector3(x0, y0, z0).normalisedCopy();
			*pVertex++ = vNormal.x;
			*pVertex++ = vNormal.y;
			*pVertex++ = vNormal.z;

			*pVertex++ = (float) seg / (float) nSegments;
			*pVertex++ = (float) ring / (float) nRings;

			if (ring != nRings) {
                               // each vertex (except the last) has six indicies pointing to it
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex;               
				*pIndices++ = wVerticeIndex + nSegments;
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex + 1;
				*pIndices++ = wVerticeIndex;
				wVerticeIndex ++;
			}
		}; // end for seg
	} // end for ring

	// Unlock
	vBuf->unlock();
	iBuf->unlock();
	// Generate face list
	pSphereVertex->useSharedVertices = true;

	// the original code was missing this line:
	pSphere->_setBounds( AxisAlignedBox( Vector3(-r, -r, -r), Vector3(r, r, r) ), false );
	pSphere->_setBoundingSphereRadius(r);
        // this line makes clear the mesh is loaded (avoids memory leakes)
        pSphere->load();
 }
