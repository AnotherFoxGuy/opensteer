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

#include "CEGUI.h"
#include "Ogre.h"

#include "OgreSteerMain.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
#define WIN32_LEAN_AND_MEAN 
#include "windows.h" 
#endif 

#ifdef __cplusplus 
extern "C" { 
#endif 

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT ) 
#else 
int main(int argc, char **argv) 
#endif 
{ 
	// Create the instance of the OgreSteer app (Singleton)
	OgreSteerMain *app = new OgreSteerMain();

    try 
	{ 	
        app->initApp();
	} 
	catch( Exception& e ) 
	{ 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL); 
#else 
        std::cerr << "An exception has occured: " << e.getFullDescription(); 
#endif 
    } 

	delete app;

    return 0; 
} 

#ifdef __cplusplus 
} 
#endif 