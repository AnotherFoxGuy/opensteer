// ----------------------------------------------------------------------------
//
//
// OpenSteer -- Steering Behaviors for Autonomous Characters
//
// Copyright (c) 2002-2003, Sony Computer Entertainment America
// Original author: Craig Reynolds <craig_reynolds@playstation.sony.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//
// ----------------------------------------------------------------------------
//
//
// "Low Speed Turn" test fixture
//
// Used to evaluate vehicle response at low speed to backward-directed
// steering force
//
// 08-20-02 cwr: created 
//
//
// ----------------------------------------------------------------------------


#include "OpenSteer/SimpleVehicle.h"
#include "OpenSteer/SteerTest.h"
#include <iomanip>
#include <strstream>


// ----------------------------------------------------------------------------


class LowSpeedTurn : public SimpleVehicle
{
public:

    // constructor
    LowSpeedTurn () {reset ();}

    // reset state
    void reset (void)
    {
        // reset vehicle state
        SimpleVehicle::reset ();

        // speed along Forward direction.
        setSpeed (startSpeed);

        // initial position along X axis
        setPosition (startX, 0, 0);

        // steering force clip magnitude
        setMaxForce (0.3);

        // velocity  clip magnitude
        setMaxSpeed (1.5);

        // for next instance: step starting location
        startX += 2;

        // for next instance: step speed
        startSpeed += 0.15;

        // 15 seconds and 150 points along the trail
        setTrailParameters (15, 150);
    }

    // draw into the scene
    void draw (void)
    {
        drawBasic2dCircularVehicle (*this, gGray50);
        drawTrail ();
    }

    // per frame simulation update
    void update (const float currentTime, const float elapsedTime)
    {
        applySteeringForce (steering (), elapsedTime);

        // annotation
        annotationVelocityAcceleration ();
        recordTrailVertex (currentTime, position());
    }

    // reset starting positions
    static void resetStarts (void)
    {
        startX = 0;
        startSpeed = 0;
    }

    // constant steering force
    Vec3 steering (void) {return Vec3 (1, 0, -1);}

    // for stepping the starting conditions for next vehicle
    static float startX;
    static float startSpeed;
};


float LowSpeedTurn::startX;
float LowSpeedTurn::startSpeed;


// ----------------------------------------------------------------------------
// PlugIn for SteerTest


const int lstCount = 5;
const float lstLookDownDistance = 18;
const Vec3 lstViewCenter (7, 0, -2);
const Vec3 lstPlusZ (0, 0, 1);


class LowSpeedTurnPlugIn : public PlugIn
{
public:

    const char* name (void) {return "Low Speed Turn";}

    float selectionOrderSortKey (void) {return 0.05;}

    // be more "nice" to avoid a compiler warning
    virtual ~LowSpeedTurnPlugIn() {}

    void open (void)
    {
        // create a given number of agents with stepped inital parameters,
        // store pointers to them in an array.
        LowSpeedTurn::resetStarts ();
        for (int i = 0; i < lstCount; i++) all.push_back (new LowSpeedTurn);

        // initial selected vehicle
        SteerTest::selectedVehicle = *all.begin();

        // initialize camera
        SteerTest::camera.mode = Camera::cmFixed;
        SteerTest::camera.fixedUp = lstPlusZ;
        SteerTest::camera.fixedTarget = lstViewCenter;
        SteerTest::camera.fixedPosition = lstViewCenter;
        SteerTest::camera.fixedPosition.y += lstLookDownDistance;
        SteerTest::camera.lookdownDistance = lstLookDownDistance;
        SteerTest::camera.fixedDistVOffset = SteerTest::camera2dElevation;
        SteerTest::camera.fixedDistDistance = SteerTest::cameraTargetDistance;
    }

    void update (const float currentTime, const float elapsedTime)
    {
        // update, draw and annotate each agent
        for (iterator i = all.begin(); i != all.end(); i++)
        {
            (**i).update (currentTime, elapsedTime);
        }
    }

    void redraw (const float currentTime, const float elapsedTime)
    {
        // selected vehicle (user can mouse click to select another)
        AbstractVehicle& selected = *SteerTest::selectedVehicle;

        // vehicle nearest mouse (to be highlighted)
        AbstractVehicle& nearMouse = *SteerTest::vehicleNearestToMouse ();

        // update camera
        SteerTest::updateCamera (currentTime, elapsedTime, selected);

        // draw "ground plane"
        SteerTest::gridUtility (selected.position());
      
        // update, draw and annotate each agent
        for (iterator i = all.begin(); i != all.end(); i++)
        {
            // draw this agent
            LowSpeedTurn& agent = **i;
            agent.draw ();

            // display speed near agent's screen position
            const Vec3 textColor (0.8, 0.8, 1.0);
            const Vec3 textOffset (0, 0.25, 0);
            const Vec3 textPosition = agent.position() + textOffset;
            ostrstream annote;
            annote << setprecision (2)
                   << setiosflags (ios::fixed)
                   << agent.speed()
                   << ends;
            draw2dTextAt3dLocation (*annote.str(), textPosition, textColor);
        }

        // highlight vehicle nearest mouse
        SteerTest::highlightVehicleUtility (nearMouse);
    }

    void close (void)
    {
        for (iterator i = all.begin(); i!=all.end(); i++) delete (*i);
        all.clear ();
    }

    void reset (void)
    {
        // reset each agent
        LowSpeedTurn::resetStarts ();
        for (iterator i = all.begin(); i!=all.end(); i++) (**i).reset();
    }

    const AVGroup& allVehicles (void) {return (const AVGroup&) all;}

    vector<LowSpeedTurn*> all; // for allVehicles
    typedef vector<LowSpeedTurn*>::const_iterator iterator;
};


LowSpeedTurnPlugIn gLowSpeedTurnPlugIn;


// ----------------------------------------------------------------------------
