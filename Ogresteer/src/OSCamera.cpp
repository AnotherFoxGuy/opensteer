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

#include "OSCamera.h"

// Initialise Constants 
const std::string OSCamera::CONST_STR_CAM_FREE_DESCRIPT("Free Cam");
const std::string OSCamera::CONST_STR_CAM_THIRDPERSON_DESCRIPT("Third Person Cam");
const std::string OSCamera::CONST_STR_CAM_FIRSTPERSON_DESCRIPT("First Person Cam");
const std::string OSCamera::CONST_STR_CAM_TOPDOWN_DESCRIPT("Top Down Cam");
const std::string OSCamera::CONST_STR_CAM_ROTATE_DESCRIPT("Rotate Cam");
const float OSCamera::CONST_FLT_CAM_CUTOFF_HEIGHT(1000.0f);

using namespace Ogre;

OSCamera::OSCamera(Camera* cam, bool allowFreeCam, bool allowCatchupCam)
 :	mOSCamera(cam),
	mAllowUserControlledCam(allowFreeCam),
	mAllowCatchupCam(allowCatchupCam),
	mCurrentCamMode(OS_CAM_THIRDPERSON),
	mRotateAmount(25),
	mForwardVec(Vector3::UNIT_Z),
	mFreeMoveVec(Vector3::ZERO),
	mLastFreeCamPos(Vector3::ZERO),
	mLastFreeCamQuat(Quaternion::IDENTITY),  
	mRotX(0),
	mRotY(0),
	mFirstPersonHeight(100.0f),
	mThirdPersonDistance(200.0f),
	mTargetNode(0),
	mCamPathLength(2),
	mCatchUpTime(0),
	mNeedCatchup(false),
	mCaughtUp(false)
{
	SceneManager* mSceneMgr;
	mSceneMgr = mOSCamera->getSceneManager();

	// Create the camera nodes, which are all children of the parent position node.
	mCamPositionNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::ZERO);

	mCamRotationNode = mCamPositionNode->createChildSceneNode();
	mCamRotationNode->setPosition(Vector3::ZERO);

	mCamZoomNode = mCamRotationNode->createChildSceneNode();
	mCamZoomNode->setPosition(Vector3(0, mFirstPersonHeight, mThirdPersonDistance));

	// Attach the camera to the zoom node
	mCamZoomNode->attachObject(mOSCamera);
}

OSCamera::~OSCamera()
{

}

void OSCamera::UpdateCamera(const float time)
{
	if (mCurrentCamMode == OS_CAM_FREE)
	{
		assert(mAllowUserControlledCam);

		// This camera is controlled by keyboard and mouse (set externally).
		mOSCamera->moveRelative(mFreeMoveVec);
		mOSCamera->yaw(mRotX);
		mOSCamera->pitch(mRotY);
	}
	else
	{
		if (mTargetNode != 0)
		{
			Ogre::Quaternion camOrientation;
			Ogre::Vector3 camPosVec;
			Ogre::Vector3 camDirVec;

			camPosVec = mTargetNode->getPosition();

			// Place the mCamPositionNode at the mTargetNode position to simulate 'follow'
			if (mCamPositionNode->getPosition() != camPosVec)
			{
				mCamPositionNode->setPosition(camPosVec);
			}

			if ((mCurrentCamMode == OS_CAM_THIRDPERSON) || (mCurrentCamMode == OS_CAM_FIRSTPERSON))
			{
				// Copy the orientation of the entity being followed
				camDirVec = mTargetNode->_getDerivedOrientation() * mForwardVec; //This value depends on the models initial direction
				camDirVec.normalise();

				// Test for opposite vectors
				Real md = 1.0f + Vector3::UNIT_X.dotProduct(camDirVec);
				if (fabs(md) < 0.00001)
				{
					camOrientation.FromAxes(Vector3::NEGATIVE_UNIT_X, Vector3::UNIT_Y, Vector3::NEGATIVE_UNIT_Z);
				}
				else
				{
					camOrientation = Vector3::UNIT_X.getRotationTo(camDirVec);
				}

                mCamRotationNode->setOrientation(camOrientation);

			}
			else if (mCurrentCamMode == OS_CAM_ROTATE)
			{
				mCamRotationNode->yaw((Ogre::Degree)mRotateAmount * time);
			}

			if (mNeedCatchup)
			{
				if (mCaughtUp)
				{
					mNeedCatchup = false;

					// Attach Ogre camera to camera node tree
					if (mCamZoomNode->numAttachedObjects() == 0)
					{
						mCamZoomNode->attachObject(mOSCamera);
					}

					mOSCamera->setDirection(Vector3::NEGATIVE_UNIT_Z);
					mOSCamera->setPosition(Vector3::ZERO);			
				}
				else
				{
					mCamPositionNode->_update(true,true);

					Ogre::Vector3 camTargetPos = mCamZoomNode->_getDerivedPosition();	
					Ogre::Quaternion camTargetOri = mCamZoomNode->_getDerivedOrientation();
					Ogre::Real interpolationTime = 0;

					camPosVec = mOSCamera->getPosition(); 

					mCatchUpTime += time;
						
					if ((mCatchUpTime >= mCamPathLength) || (camPosVec == camTargetPos))
					{
						mCaughtUp = true;
						
						mOSCamera->setOrientation(camTargetOri);
						mOSCamera->setPosition(camTargetPos);
					}
					else
					{
						interpolationTime = mCatchUpTime / mCamPathLength;

						camOrientation = mOSCamera->getOrientation();

						if (camOrientation != camTargetOri)
						{
							mOSCamera->setOrientation(Quaternion::nlerp(interpolationTime, camOrientation, camTargetOri, true));
						}
												
						mCatchUpSpline.updatePoint(1,camTargetPos);

						mOSCamera->setPosition(mCatchUpSpline.interpolate(interpolationTime));					
					}
					
				}
			} // End mNeedCatchup

		}
	}
}

void OSCamera::setCameraMode(OSCameraEnumMode newMode)
{
	if (mCurrentCamMode == OS_CAM_FREE)
	{
		// Store last free cam state
		mLastFreeCamPos = mOSCamera->getDerivedPosition();
		mLastFreeCamQuat = mOSCamera->getDerivedOrientation();

		// Attach Ogre camera to camera node tree
		if (mCamZoomNode->numAttachedObjects() == 0)
		{
			mCamZoomNode->attachObject(mOSCamera);
		}

		// Reset camera
		mOSCamera->setDirection(Vector3::NEGATIVE_UNIT_Z);	
		mOSCamera->setPosition(Vector3::ZERO);
	}

	if (newMode == OS_CAM_FREE)
	{
		// If the freecam has not yet been setup with user changed values, supply the current world space values
		if ((mLastFreeCamPos == Vector3::ZERO) && (mLastFreeCamQuat == Quaternion::IDENTITY))
		{
			mLastFreeCamPos = mOSCamera->getDerivedPosition();
			mLastFreeCamQuat = mOSCamera->getDerivedOrientation();
		}

		// Detach Ogre camera from camera node tree
		if (mCamZoomNode->numAttachedObjects() > 0)
		{
			mCamZoomNode->detachObject(mOSCamera);
		}

		// Set Ogre camera to previous freelook values
		mOSCamera->setPosition(mLastFreeCamPos);
		mOSCamera->setOrientation(mLastFreeCamQuat);
	}
	else
	{
		mCamPositionNode->_update(true,true);

		mLastNodeCamQuat = mOSCamera->getDerivedOrientation();
		mLastNodeCamPos = mOSCamera->getDerivedPosition();

		mCamRotationNode->resetOrientation();
		mCamZoomNode->resetOrientation();

		if (newMode == OS_CAM_FIRSTPERSON)
		{
			// Limit the Zoom node to the position of the targets head
			mCamZoomNode->setPosition(Vector3(0, mFirstPersonHeight, 0));
		}
		else if (newMode == OS_CAM_TOPDOWN)
		{
			// Position the Zoom node above the target and point straight downwards
			mCamZoomNode->setPosition(Vector3(0, mThirdPersonDistance + mFirstPersonHeight, 0));
			mCamZoomNode->setDirection(Vector3::NEGATIVE_UNIT_Y);
		}
		else
		{
			// Position the zoom at hoverHeight and look at target's head (mFirstPersonHeight), slightly down of
			float hoverHeight = mFirstPersonHeight + (mFirstPersonHeight * 0.25);

			mCamZoomNode->setPosition(0, hoverHeight, mThirdPersonDistance); 
			Vector3 lookatPos = Vector3(0, hoverHeight - (hoverHeight * 0.15f), 0) - mCamZoomNode->getPosition(); // Direction is destination minus Source
			mCamZoomNode->setDirection(lookatPos);
		}	

		if (mAllowCatchupCam)
		{
			mNeedCatchup = true;
			mCaughtUp = false;

			// Detach Ogre camera from camera node tree
			if (mCamZoomNode->numAttachedObjects() > 0)
			{
				mCamZoomNode->detachObject(mOSCamera);
			}

			mOSCamera->setPosition(mLastNodeCamPos);
			mOSCamera->setOrientation(mLastNodeCamQuat);

			// Setup path for camera to follow during catchup
			mCatchUpSpline.clear();
			mCatchUpSpline.addPoint(mLastNodeCamPos);
			mCatchUpSpline.addPoint(mCamZoomNode->_getDerivedPosition());

			// Init the time to zero
			mCatchUpTime = 0;
		}
		else
		{
			mNeedCatchup = false;

			mOSCamera->setDirection(Vector3::NEGATIVE_UNIT_Z);	
			mOSCamera->setPosition(Vector3::ZERO);
		}
	}
	
	mCurrentCamMode = newMode;
}

void OSCamera::cycleCameraMode()
{
	if (mCurrentCamMode == OS_CAM_FREE)
	{
		setCameraMode(OS_CAM_THIRDPERSON);
	}
	else if (mCurrentCamMode == OS_CAM_THIRDPERSON)
	{
		setCameraMode(OS_CAM_FIRSTPERSON);
	}
	else if (mCurrentCamMode == OS_CAM_FIRSTPERSON)
	{
		setCameraMode(OS_CAM_TOPDOWN);
	}
	else if (mCurrentCamMode == OS_CAM_TOPDOWN)
	{
		setCameraMode(OS_CAM_ROTATE);
	}
	else if (mCurrentCamMode == OS_CAM_ROTATE)
	{
		if (mAllowUserControlledCam)
		{
			setCameraMode(OS_CAM_FREE);
		}
		else
		{
			// Skip FREE cam
			setCameraMode(OS_CAM_THIRDPERSON);
		}
	}
}

void OSCamera::setTargetNode(SceneNode* targetNode)
{
	// If its the first time setting the target, try to place camera at a decent relative position
	if ((mTargetNode == 0) && (targetNode != 0))
	{
		mCamPositionNode->setPosition(targetNode->getPosition());
	}

	mTargetNode = targetNode;

	// Refresh Camera Mode
	setCameraMode(mCurrentCamMode);
}

std::string OSCamera::getCurrentCamModeString()
{
	switch(mCurrentCamMode) 
    { 
	case OS_CAM_FREE:
		return CONST_STR_CAM_FREE_DESCRIPT;
		break; 
	case OS_CAM_THIRDPERSON:
		return CONST_STR_CAM_THIRDPERSON_DESCRIPT;
		break; 
	case OS_CAM_FIRSTPERSON:
		return CONST_STR_CAM_FIRSTPERSON_DESCRIPT;
		break; 
	case OS_CAM_TOPDOWN:
		return CONST_STR_CAM_TOPDOWN_DESCRIPT;
		break; 
	case OS_CAM_ROTATE:
		return CONST_STR_CAM_ROTATE_DESCRIPT;
		break; 
	default:
		return "No Camera Mode Found!";
		break; 
	}
}
