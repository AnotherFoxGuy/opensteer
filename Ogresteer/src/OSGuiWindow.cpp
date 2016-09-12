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

#include "OSGuiWindow.h"
#include "OgreSteerMain.h"

template<> OSGuiWindow* Ogre::Singleton<OSGuiWindow>::ms_Singleton = 0;

using namespace CEGUI;

// Initialise Constants 
const CEGUI::Point OSGuiWindow::CONST_INT_FRAME_POS(Point(0.59f, 0.01f));
const CEGUI::Size OSGuiWindow::CONST_INT_FRAME_SIZE(Size(0.4f, 0.5f));
const std::string OSGuiWindow::CONST_STR_FRAME_MAINWIN("MainFrameWindow");
const std::string OSGuiWindow::CONST_STR_TABCONTROL("MainFrameTabControl");
const std::string OSGuiWindow::CONST_STR_COMBO_SELECTEDENTITY("TabPane/SteerEntity/selectedEntity");
const std::string OSGuiWindow::CONST_STR_COMBO_CAMERAMODE("TabPane/SteerEntity/camMode");
const std::string OSGuiWindow::CONST_STR_COMBO_SELECT_SCENARIO("TabPane/SteerScenario/selectScenario");
const std::string OSGuiWindow::CONST_STR_HINT_ALL("All");
const std::string OSGuiWindow::CONST_STR_TABPANE_SCENARIO("ScrlTabPane/SteerScenario");
const std::string OSGuiWindow::CONST_STR_TABPANE_ENTITY("TabPane/SteerEntity");
const std::string OSGuiWindow::CONST_STR_BUTTON_APPLY_ENTITY("TabPane/SteerEntity/ApplyEntity");
const std::string OSGuiWindow::CONST_STR_BUTTON_APPLY_CAMERA("TabPane/SteerEntity/ApplyCam");
const std::string OSGuiWindow::CONST_STR_BUTTON_APPLY_SCENARIO("TabPane/SteerScenario/ApplyScenario");
const std::string OSGuiWindow::CONST_STR_STATICTXT_STATS("TabPane/SteerEntity/Stats");
const std::string OSGuiWindow::CONST_STR_STATICIMG_NAME("RTTStaticImage");

OSGuiWindow::OSGuiWindow()
{
	StaticText* stxt;
	PushButton* pBtn;
	Combobox* cmbBx;
	ListboxTextItem* lbitm;

	WindowManager& winMgr =	WindowManager::getSingleton();

	// Create main GUI Frame window (moveable, resizeable, collapsable)
	FrameWindow* FWWin = (FrameWindow*)winMgr.createWindow("WindowsLook/FrameWindow", CONST_STR_FRAME_MAINWIN);
	FWWin->setText("OGRE-STEER CONTROL");
	FWWin->setAlpha(1.0f);
	FWWin->setMaximumSize(Size(0.9f, 0.9f));
	FWWin->setMinimumSize(Size(0.1f, 0.1f));
	FWWin->setPosition(CONST_INT_FRAME_POS);
	FWWin->setSize(CONST_INT_FRAME_SIZE);
	FWWin->setCloseButtonEnabled(false);
	FWWin->setFont("titlefont");
	FWWin->subscribeEvent(FrameWindow::EventHidden, Event::Subscriber(&OSGuiWindow::handleVisibilityChanged, this));
	FWWin->subscribeEvent(FrameWindow::EventRollupToggled, Event::Subscriber(&OSGuiWindow::handleVisibilityChanged, this));

	// Add main GUI Frame window to the screens default sheet
	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(FWWin);

	// Calculate relative sizes (hack to compensate for FrameWindow's title bar using content space)
	Titlebar* FWWinTitlebar = (CEGUI::Titlebar*)winMgr.getWindow("MainFrameWindow__auto_titlebar__");
	
	float fltTitleHeight = FWWinTitlebar->getHeight(CEGUI::Relative); // Height of title bar
	float fltTabCntrlPadY = fltTitleHeight * 0.25f; // A quarter of title bar height
	float fltTabCntrlPosY = fltTitleHeight + fltTabCntrlPadY; // Position of tab control
	float fltTabCntrlSizeY = 1.0f - (fltTabCntrlPosY + (fltTabCntrlPadY * 2));

	// Create tab control and add to main GUI Frame window
	TabControl* TCWin = (TabControl*)winMgr.createWindow("WindowsLook/TabControl", CONST_STR_TABCONTROL);
	TCWin->setPosition(Point((fltTabCntrlPadY * 2), fltTabCntrlPosY));
	TCWin->setSize(Size((1 - (fltTabCntrlPadY * 4)), fltTabCntrlSizeY));
	TCWin->setFont("normalfont");
	TCWin->setTabHeight(0.09f);
	TCWin->setTabTextPadding(0.02f);
	FWWin->addChildWindow(TCWin);

	Size sizScrollTab = Size(0.98f, 0.98f);
	Point pntScrollTab = Point(0.01f, 0.01f);

	// Create 'Scenarios' scrollable tab panel and contents
	ScrollablePane* tabPaneScenarios = (ScrollablePane*)winMgr.createWindow("WindowsLook/ScrollablePane", CONST_STR_TABPANE_SCENARIO);
	tabPaneScenarios->setPosition(pntScrollTab);
	tabPaneScenarios->setSize(sizScrollTab);
	tabPaneScenarios->setText("Scenarios");

		stxt = (StaticText*)winMgr.createWindow("WindowsLook/StaticText", "TabPane/SteerScenario/stxt1");
		stxt->setFrameEnabled(false);
		stxt->setBackgroundEnabled(false);
		stxt->setWindowPosition(UVector2(cegui_reldim(0.01f), cegui_reldim(0.2f)));
		stxt->setWindowSize(UVector2(cegui_reldim(0.24f), cegui_reldim(0.1f)));
		stxt->setText("Scenario: ");
		tabPaneScenarios->addChildWindow(stxt);

		cmbBx = (Combobox*)winMgr.createWindow("WindowsLook/Combobox", CONST_STR_COMBO_SELECT_SCENARIO);
		cmbBx->setWindowPosition(UVector2(cegui_reldim(0.26f), cegui_reldim(0.2f)));
		cmbBx->setWindowSize(UVector2(cegui_reldim(0.55f), cegui_reldim(0.4f)));
		cmbBx->setReadOnly(true);
		cmbBx->subscribeEvent(Combobox::EventListSelectionAccepted, Event::Subscriber(&OSGuiWindow::handleComboAccepted, this));
		tabPaneScenarios->addChildWindow(cmbBx);

		pBtn = (PushButton*)winMgr.createWindow("WindowsLook/Button", CONST_STR_BUTTON_APPLY_SCENARIO);
		pBtn->setWindowPosition(UVector2(cegui_reldim(0.82f), cegui_reldim(0.2f)));
		pBtn->setWindowSize(UVector2(cegui_reldim(0.17f), cegui_reldim(0.1f)));
		pBtn->setText("Apply");
		pBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&OSGuiWindow::handleButtonClicked, this));
		tabPaneScenarios->addChildWindow(pBtn);
	
	// Create 'Entities And Cameras' tab panel and contents
	GUISheet* tabPaneEntitys = (GUISheet*)winMgr.createWindow("DefaultGUISheet", CONST_STR_TABPANE_ENTITY); 
	tabPaneEntitys->setPosition(pntScrollTab);
	tabPaneEntitys->setSize(sizScrollTab);
	tabPaneEntitys->setText("Entities and Cameras");
	tabPaneEntitys->subscribeEvent(TabControl::EventShown, Event::Subscriber(&OSGuiWindow::handleVisibilityChanged, this));
	tabPaneEntitys->subscribeEvent(TabControl::EventHidden, Event::Subscriber(&OSGuiWindow::handleVisibilityChanged, this));

		stxt = (StaticText*)winMgr.createWindow("WindowsLook/StaticText", "TabPane/SteerEntity/stxt1");
		stxt->setFrameEnabled(false);
		stxt->setBackgroundEnabled(false);
		stxt->setPosition(Point(0.01f, 0.01f));
		stxt->setSize(Size(0.28f, 0.1f));
		stxt->setText("Camera Mode: ");
		tabPaneEntitys->addChildWindow(stxt);

		cmbBx = (Combobox*)winMgr.createWindow("WindowsLook/Combobox", CONST_STR_COMBO_CAMERAMODE);
		cmbBx->setPosition(Point(0.3f, 0.01f));
		cmbBx->setSize(Size(0.5f, 0.4f));
		cmbBx->setReadOnly(true);
		cmbBx->subscribeEvent(Combobox::EventListSelectionAccepted, Event::Subscriber(&OSGuiWindow::handleComboAccepted, this));
		tabPaneEntitys->addChildWindow(cmbBx);

		lbitm = createComboBoxItem(OSCamera::CONST_STR_CAM_THIRDPERSON_DESCRIPT);
		cmbBx->addItem(lbitm);
		cmbBx->setItemSelectState(lbitm, true);
		cmbBx->setText(lbitm->getText());

		lbitm = createComboBoxItem(OSCamera::CONST_STR_CAM_FIRSTPERSON_DESCRIPT);
		cmbBx->addItem(lbitm);

		lbitm = createComboBoxItem(OSCamera::CONST_STR_CAM_TOPDOWN_DESCRIPT);
		cmbBx->addItem(lbitm);

		lbitm = createComboBoxItem(OSCamera::CONST_STR_CAM_ROTATE_DESCRIPT);
		cmbBx->addItem(lbitm);

		pBtn = (PushButton*)winMgr.createWindow("WindowsLook/Button", CONST_STR_BUTTON_APPLY_CAMERA);
		pBtn->setPosition(Point(0.82f, 0.01f));
		pBtn->setSize(Size(0.17f, 0.1f));
		pBtn->setText("Apply");
		pBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&OSGuiWindow::handleButtonClicked, this));
		tabPaneEntitys->addChildWindow(pBtn);

		stxt = (StaticText*)winMgr.createWindow("WindowsLook/StaticText", "TabPane/SteerEntity/stxt2");
		stxt->setFrameEnabled(false);
		stxt->setBackgroundEnabled(false);
		stxt->setPosition(Point(0.01f, 0.12f));
		stxt->setSize(Size(0.28f, 0.1f));
		stxt->setText("Selected Entity: ");
		tabPaneEntitys->addChildWindow(stxt);

		cmbBx = (Combobox*)winMgr.createWindow("WindowsLook/Combobox", CONST_STR_COMBO_SELECTEDENTITY);
		cmbBx->setPosition(Point(0.3f, 0.12f));
		cmbBx->setSize(Size(0.5f, 0.4f));
		cmbBx->setReadOnly(true);
		cmbBx->subscribeEvent(Combobox::EventListSelectionAccepted, Event::Subscriber(&OSGuiWindow::handleComboAccepted, this));
		tabPaneEntitys->addChildWindow(cmbBx);

		pBtn = (PushButton*)winMgr.createWindow("WindowsLook/Button", CONST_STR_BUTTON_APPLY_ENTITY);
		pBtn->setPosition(Point(0.82f, 0.12f));
		pBtn->setSize(Size(0.17f, 0.1f));
		pBtn->setText("Apply");
		pBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&OSGuiWindow::handleButtonClicked, this));
		tabPaneEntitys->addChildWindow(pBtn);

		StaticImage* siRTT = (StaticImage*)winMgr.createWindow("WindowsLook/StaticImage", CONST_STR_STATICIMG_NAME);
		siRTT->setPosition(Point(0.14f, 0.25f));
		siRTT->setSize(Size(0.72f, 0.72f));
		siRTT->setFrameEnabled(false);
		tabPaneEntitys->addChildWindow(siRTT);

		stxt = (StaticText*)winMgr.createWindow("WindowsLook/StaticText", CONST_STR_STATICTXT_STATS);
		stxt->setFrameEnabled(false);
		stxt->setBackgroundEnabled(false);
		stxt->setPosition(Point(0.16f, 0.27f));
		stxt->setSize(Size(0.70f, 0.70f));
		stxt->setText("Statistics");
		stxt->setVerticalFormatting(StaticText::TopAligned);
		stxt->setHorizontalFormatting(StaticText::WordWrapLeftAligned);
		stxt->setTextColours(0xFFFFFFFF);
		tabPaneEntitys->addChildWindow(stxt);

	// Add the tab panels to the tab control
	TCWin->addTab(tabPaneScenarios);
	TCWin->addTab(tabPaneEntitys);
}

OSGuiWindow::~OSGuiWindow()
{

}

void OSGuiWindow::resetGUI()
{
	FrameWindow* FWWin = (FrameWindow*)WindowManager::getSingleton().getWindow(CONST_STR_FRAME_MAINWIN);
	FWWin->setPosition(CONST_INT_FRAME_POS);
	FWWin->setSize(CONST_INT_FRAME_SIZE);

	TabControl* TCWin = (TabControl*)WindowManager::getSingleton().getWindow(CONST_STR_TABCONTROL);
	TCWin->setSelectedTabAtIndex(0);
}

void OSGuiWindow::setImageRTTStaticImage(const CEGUI::Imageset* const iset, const CEGUI::String& imageName)
{
    CEGUI::StaticImage* mStatImg = (CEGUI::StaticImage*)WindowManager::getSingleton().getWindow(CONST_STR_STATICIMG_NAME);
    mStatImg->setImage(&iset->getImage(imageName));	
}

void OSGuiWindow::updateEntityStats(const CEGUI::String& strStats)
{
	CEGUI::StaticText* mStatTxt = (StaticText*)WindowManager::getSingleton().getWindow(CONST_STR_STATICTXT_STATS);
	mStatTxt->setText(strStats);
}

void OSGuiWindow::setListPreviewEntity(const CEGUI::String& entityName)
{
	Combobox* cbox = (Combobox*)WindowManager::getSingleton().getWindow(CONST_STR_COMBO_SELECTEDENTITY);
	ListboxItem* listItem = cbox->findItemWithText(entityName, NULL);

	if (listItem != NULL)
	{
		cbox->setItemSelectState(listItem, true);
		cbox->setText(listItem->getText());
	}
}

void OSGuiWindow::setListPreviewCam(const CEGUI::String& camMode)
{
	Combobox* cbox = (Combobox*)WindowManager::getSingleton().getWindow(CONST_STR_COMBO_CAMERAMODE);
	ListboxItem* listItem = cbox->findItemWithText(camMode, NULL);

	if (listItem != NULL)
	{
		cbox->setItemSelectState(listItem, true);
		cbox->setText(listItem->getText());
	}
}

void OSGuiWindow::populateEntityList(std::vector<std::string>& vecEntityNames)
{
	ListboxTextItem* lbitm;

	Combobox* cbox = (Combobox*)WindowManager::getSingleton().getWindow(CONST_STR_COMBO_SELECTEDENTITY);
	cbox->resetList();
	cbox->setText("");

	std::vector<std::string>::iterator it;
	int i = 0;

	while (i < static_cast<int>(vecEntityNames.size()))
	{
		lbitm = createComboBoxItem(vecEntityNames[i]);
		cbox->addItem(lbitm);

		if (i == 0)
		{
			cbox->setItemSelectState(lbitm, true);
			cbox->setText(lbitm->getText());
		}
		i++;
	}

	TabControl* TCWin = (TabControl*)WindowManager::getSingleton().getWindow(CONST_STR_TABCONTROL);
	TCWin->setSelectedTabAtIndex(0);
}

void OSGuiWindow::populateScenarioList(std::vector<std::string>& vecSceneNames)
{
	ListboxTextItem* lbitm;

	Combobox* cbox = (Combobox*)WindowManager::getSingleton().getWindow(CONST_STR_COMBO_SELECT_SCENARIO);
	cbox->resetList();
	cbox->setText("");

	std::vector<std::string>::iterator it;
	int i = 0;

	while (i < static_cast<int>(vecSceneNames.size()))
	{
		lbitm = createComboBoxItem(vecSceneNames[i]);
		cbox->addItem(lbitm);

		if (i == 0)
		{
			cbox->setItemSelectState(lbitm, true);
			cbox->setText(lbitm->getText());
		}
		i++;
	}
}

bool OSGuiWindow::handleButtonClicked(const CEGUI::EventArgs& e)
{
	std::string strBtnName = (((const CEGUI::WindowEventArgs&)e).window)->getName().c_str();
	std::string strSelectedItem;

	if (strBtnName == CONST_STR_BUTTON_APPLY_ENTITY)
	{
		strSelectedItem = getComboBoxSelectedText(CONST_STR_COMBO_SELECTEDENTITY);
		OgreSteerMain::getSingletonPtr()->setScenarioSelectedEntity(strSelectedItem, false);
	}
	else if (strBtnName == CONST_STR_BUTTON_APPLY_CAMERA)
	{
		strSelectedItem = getComboBoxSelectedText(CONST_STR_COMBO_CAMERAMODE);
		OgreSteerMain::getSingletonPtr()->setScenarioSelectedCam(strSelectedItem, false);
	}
	else if (strBtnName == CONST_STR_BUTTON_APPLY_SCENARIO)
	{
		strSelectedItem = getComboBoxSelectedText(CONST_STR_COMBO_SELECT_SCENARIO);
		OgreSteerMain::getSingletonPtr()->changeScenarioByName(strSelectedItem);
	}

	return true;
}

bool OSGuiWindow::handleVisibilityChanged(const CEGUI::EventArgs& e)
{
	checkPreviewPanelActive();
	return true;
}

bool OSGuiWindow::handleComboAccepted(const CEGUI::EventArgs& e)
{
	CEGUI::String comboName = (((const CEGUI::WindowEventArgs&)e).window)->getName();

	if (comboName == CONST_STR_COMBO_SELECTEDENTITY)
	{
		OgreSteerMain::getSingletonPtr()->setScenarioSelectedEntity(getComboBoxSelectedText(comboName), true);
	}
	else if (comboName == CONST_STR_COMBO_CAMERAMODE)
	{
		OgreSteerMain::getSingletonPtr()->setScenarioSelectedCam(getComboBoxSelectedText(comboName), true);
	}

	//CONST_STR_COMBO_SELECT_SCENARIO
	return true;
}

void OSGuiWindow::checkPreviewPanelActive()
{
	bool boolBotPanelActive = false;

	FrameWindow* FWWin = (FrameWindow*)WindowManager::getSingleton().getWindow(CONST_STR_FRAME_MAINWIN);

	if (FWWin->isVisible() && !FWWin->isRolledup())
	{
		GUISheet* tabPaneEntitys = (GUISheet*)WindowManager::getSingleton().getWindow(CONST_STR_TABPANE_ENTITY); 

		// Is Preview panel selected
		boolBotPanelActive = tabPaneEntitys->isVisible();
	}

	OgreSteerMain::getSingletonPtr()->setPreviewPanelActive(boolBotPanelActive);
}

ListboxTextItem* OSGuiWindow::createComboBoxItem(CEGUI::String itemText)
{
	const CEGUI::Image* sel_img_default = &ImagesetManager::getSingleton().getImageset("WindowsLook")->getImage("TooltipMiddle");

	ListboxTextItem* lbitm = new ListboxTextItem(itemText);
	lbitm->setSelectionBrushImage(sel_img_default);
	lbitm->setSelectionColours(0xFF99CC99,0xFF99CC99,0xFF99CC99,0xFFFFFFFF);
	lbitm->setTextColours(0xFF000000);

	return lbitm;
}

const std::string OSGuiWindow::getComboBoxSelectedText(CEGUI::String comboBoxName)
{
	Combobox* cbox = (Combobox*)WindowManager::getSingleton().getWindow(comboBoxName);
	ListboxItem* listItem = cbox->getSelectedItem();
	std::string strItemText = "";

	if (listItem != NULL)
	{
		strItemText = listItem->getText().c_str();
	}

	return strItemText;
}