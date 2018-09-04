#ifndef CONTROL_H
#define CONTROL_H

// LIBRARIES
#include <cmath>
#include <math.h>
#include <thread>
#include <iostream>
#include <string>
#include <unistd.h>
#include <algorithm>
#include "TCPClient.h"
using namespace std;

// CLASS
class Control {
    public:
        // Constructor
        Control();
        // Destructor
        ~Control();
        // Set movement mode
        void setMovementMode(int mode, int joint);
        // Notify finish init mode
        void finishInitMode();
        // Send movement data to CS8
        void movement(int mode, int joint, int * axis);
        // Suspend movement
        void suspendResume();
        // Notify exit to CS8
        void notifyExit();
        // Select operation mode
        void setOperationMode();
        // Change maximum preasure force
        void setForces();
        // Method for monitorization TX60
        void monitorizationSocket();
        // Show data in window
        void showData();

    private:
        // Attributes
        TCPClient socket;
        bool init;
        bool suspended;
        bool blockTransmission;
        string vel;
        string frameCommand;
        string jointCommand;
        string velocityCommand;
        int maxForces[3];
        int currentJoint;
        int movementMode; // 0 -> Joint // 1 -> Frame
        int operationMode; //1 -> Standard //2 -> Follow irregular path //3 -> Puncture the vein
};

#endif // CONTROL_H
