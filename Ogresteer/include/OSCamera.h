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

#ifndef __OSCAMERA_
#define __OSCAMERA_

#include "Ogre.h"
using namespace Ogre;

class OSCamera
{

public:

	// OSCamera Constants
	static const std::string CONST_STR_CAM_FREE_DESCRIPT;
	static const std::string CONST_STR_CAM_THIRDPERSON_DESCRIPT;
	static const std::string CONST_STR_CAM_FIRSTPERSON_DESCRIPT;
	static const std::string CONST_STR_CAM_TOPDOWN_DESCRIPT;
	static const std::string CONST_STR_CAM_ROTATE_DESCRIPT;

	static const float CONST_FLT_CAM_CUTOFF_HEIGHT;
	
	enum OSCameraEnumMode
	{
		// Free-Flight camera controlled via mouse and keyboard
		OS_CAM_FREE,
		// Follow in 3rd person
		OS_CAM_THIRDPERSON,
		// Follow in 1st person
		OS_CAM_FIRSTPERSON,
		// Follow from an elevated vantage point pointing straight down
		OS_CAM_TOPDOWN,
		// Follow and rotate around
		OS_CAM_ROTATE
	};

	OSCamera(Camera* cam, bool allowFreeCam = true, bool allowCatchupCam = true); 
	~OSCamera();

	void UpdateCamera(const float time);

	void setCameraMode(OSCameraEnumMode newMode);

	OSCameraEnumMode getCurrentCamModeEnum() {return mCurrentCamMode;}

	std::string getCurrentCamModeString();

	void cycleCameraMode();

	void setOgreCamVec(Vector3 moveVec) { mFreeMoveVec = moveVec; }

	void setCamRot(Radian RotX, Radian RotY) 
	{ 
		mRotX = RotX; 
		mRotY = RotY;
	}

	void setTargetNode(SceneNode* targetNode);

	Ogre::SceneNode* getTargetNode() {return mTargetNode;}

	void setFirstPersonHeight(float newHeight){mFirstPersonHeight = newHeight;}

	void setThirdPersonDistance(float newDistance){mThirdPersonDistance = newDistance;}

	void setForwardVec(Ogre::Vector3 newVec){mForwardVec = newVec;}

	bool isAllowedFreeCam() {return mAllowUserControlledCam;}

private:

	Ogre::Camera* mOSCamera;
	bool mAllowUserControlledCam;
	bool mAllowCatchupCam;

	Ogre::SimpleSpline mCatchUpSpline;

	Ogre::SceneNode* mCamPositionNode;
	Ogre::SceneNode* mCamRotationNode;
	Ogre::SceneNode* mCamZoomNode;

	Ogre::SceneNode* mTargetNode;

	OSCameraEnumMode mCurrentCamMode;
	Ogre::Vector3 mFreeMoveVec;
	Ogre::Vector3 mLastFreeCamPos;
	Ogre::Quaternion mLastFreeCamQuat;
	Ogre::Vector3 mLastNodeCamPos;
	Ogre::Vector3 mForwardVec;
	Ogre::Quaternion mLastNodeCamQuat;
	Ogre::Radian mRotX, mRotY;
	float mFirstPersonHeight;
	float mThirdPersonDistance;
	bool mNeedCatchup;
	bool mCaughtUp;
	Real mCamPathLength;
	Real mCatchUpTime;
	Real mRotateAmount;
};

#endif