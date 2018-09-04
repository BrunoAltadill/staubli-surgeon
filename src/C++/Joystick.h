#ifndef JOYSTICK_H
#define JOYSTICK_H

// LIBRARIES
#include <string>
#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include "Control.h"
using namespace std;

// CONSTANTS
#define JS_EVENT_BUTTON 0x01    // button pressed/released
#define JS_EVENT_AXIS 0x02      // joystick moved

// STRUCTS
struct js_event {
    unsigned int time;          // event timestamp in milliseconds
    short value;                // value
    unsigned char type;         // event type
    unsigned char number;       // axis/button number
};

// CLASS
class Joystick {
    public:
        // Default constructor
        Joystick();
        // Parameter constructor
        Joystick(Control * ctrl);
        // Destructor
        ~Joystick();
        // Read joystick data
        void readJoystick();
        // Return finish state
        bool getFinishState();

    private:
        // Attributes
        int fd;
        struct js_event js;
        int js_bt[13];
        int js_axis[7];
        int mode; // 0 -> Joint // 1 -> Frame
        int joint;
        bool finish;
        Control * ctrl;
        // Methods
        string identifyDeviceName();
        void selectMovementMode();
        void changeMode(int button);
        void changeJoint(int button);
};

#endif // JOYSTICK_H
