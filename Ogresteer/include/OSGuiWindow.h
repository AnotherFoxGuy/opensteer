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

#ifndef __OGRESTEER_GUIWINDOW_
#define __OGRESTEER_GUIWINDOW_

#include "CEGUI.h"
#include "OgreSingleton.h"


class OSGuiWindow: public Ogre::Singleton<OSGuiWindow>
{

public:

	OSGuiWindow(); 
	virtual ~OSGuiWindow();

	void resetGUI();

	void setImageRTTStaticImage(const CEGUI::Imageset* const iset, const CEGUI::String& imageName);

	void updateEntityStats(const CEGUI::String& strStats);

	void setListPreviewEntity(const CEGUI::String& entityName);

	void setListPreviewCam(const CEGUI::String& camMode);

	void populateEntityList(std::vector<std::string>& vecEntityNames);

	void populateScenarioList(std::vector<std::string>& vecSceneNames);

	void checkPreviewPanelActive();

private:

	// OSGuiWindow Constants
	static const CEGUI::Point CONST_INT_FRAME_POS; 
	static const CEGUI::Size CONST_INT_FRAME_SIZE;
	static const std::string CONST_STR_FRAME_MAINWIN;
	static const std::string CONST_STR_TABCONTROL;
	static const std::string CONST_STR_COMBO_SELECTEDENTITY;
	static const std::string CONST_STR_COMBO_CAMERAMODE;
	static const std::string CONST_STR_COMBO_SELECT_SCENARIO;
	static const std::string CONST_STR_HINT_ALL;
	static const std::string CONST_STR_TABPANE_SCENARIO;
	static const std::string CONST_STR_TABPANE_ENTITY;
	static const std::string CONST_STR_STATICIMG_NAME;
	static const std::string CONST_STR_BUTTON_APPLY_ENTITY;
	static const std::string CONST_STR_BUTTON_APPLY_CAMERA;
	static const std::string CONST_STR_BUTTON_APPLY_SCENARIO;
	static const std::string CONST_STR_STATICTXT_STATS;

	bool handleButtonClicked(const CEGUI::EventArgs& e);
	bool handleComboAccepted(const CEGUI::EventArgs& e);
	bool handleVisibilityChanged(const CEGUI::EventArgs& e);

	CEGUI::ListboxTextItem* createComboBoxItem(CEGUI::String itemText);

	const std::string getComboBoxSelectedText(CEGUI::String comboBoxName);

};

#endif
