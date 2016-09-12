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

#ifndef __OSSCENARIO_SPACEPORT_
#define __OSSCENARIO_SPACEPORT_

#include "OgreSingleton.h"
#include "OSBaseScenario.h"

class OSScenarioRobots: 
	public Ogre::Singleton<OSScenarioRobots>, 
	public OSBaseScenario
{

public:

	OSScenarioRobots(); 
	virtual ~OSScenarioRobots();

	virtual void onUpdate(const float currentTime, const float elapsedTime);

	virtual void onEnter();

	virtual void onReset();

	virtual void onExit();

	virtual void createSimpleEntities();

private:

	Light* mLight;
	Ogre::SceneNode* mLightNode; 
};

#endif
