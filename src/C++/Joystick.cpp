#include "Joystick.h"

Joystick::Joystick() {
    // Do nothing...
}

Joystick::Joystick(Control * ctrl) {
    mode = -1;
    joint = 1;
    finish = false;
    for (int i = 0; i < 7; i++) {
        js_axis[i] = 0;
    }
    this->ctrl = ctrl;

    string device = identifyDeviceName();
    if (device.length() != 0) {
        fd = open(device.c_str(), O_RDONLY);
        if (fd < 0) {
            cerr << "ERROR: Cannot open " << device << endl;
            exit(-1);
        }
    } else {
        cerr << "ERROR: Joystick device impossible to detect (default /dev/input/js0)!" << endl;
        exit(-1);
    }

    selectMovementMode();

    ctrl->finishInitMode();
}

Joystick::~Joystick() {
    close(fd);
    delete ctrl;
}

void Joystick::readJoystick () {
    // Read joystick events
    while (1) {
        read(fd, &js, sizeof(js));

        if (js.type == JS_EVENT_BUTTON || js.type == JS_EVENT_AXIS) {
            if (js.type == JS_EVENT_BUTTON) {
                int button = (int) js.number;
                int value = js.value;
                if (button == 0 && js_bt[button] == 1 && value == 0) {
                    ctrl->suspendResume();
                } else if (button == 1 && js_bt[button] == 1 && value == 0) {
                    finish = true;
                } else if ((button == 2 || button == 3) && js_bt[button] == 1 && value == 0) {
                    changeMode(button);
                } else if (button == 4 && js_bt[button] == 1 && value == 0) {
                    ctrl->setForces();
                } else if (button == 5 && js_bt[button] == 1 && value == 0) {
                    ctrl->setOperationMode();
                } else if (mode == 0 && js_bt[button] == 1 && value == 0 && button > 5 && button < 12) {
                    changeJoint(button);
                }
                js_bt[button] = value;
            } else {
                int axis = (int) js.number;
                int value = js.value;
                ctrl->movement(mode, joint, js_axis);
                js_axis[axis + 1] = value;
            }
        }
    }
}

bool Joystick::getFinishState() {
    return finish;
}

string Joystick::identifyDeviceName() {
    string str;
    bool found = false;

    ifstream devices("/proc/bus/input/devices");
    while (devices && !found && getline(devices, str)) {
        if (str.find("I:") == 0 && str.find("Vendor=046d") != string::npos && str.find("Product=c215") != string::npos) {
            while (devices && !found && getline(devices, str)) {
                if (str.find("H:") == 0 && str.find("js") != string::npos) {
                    found = true;
                    //Extract js device id
                    int pos = str.find("js");
                    str = str.substr(pos);
                    //Secure processing in case data exist after device name
                    if (str.find(" ") != string::npos) {
                        pos = str.find(" ");
                        str = str.substr(0, pos);
                    }
                }
            }
        }
    }

    return "/dev/input/" + str;
}

void Joystick::changeMode(int button) {
  if (button == 2) {
    mode = 0;
  } else if (button == 3) {
    mode = 1;
  }

  ctrl->setMovementMode(mode, joint);
  ctrl->showData();
}

void Joystick::changeJoint(int button) {
    if (mode == 0) {
        if (button == 6) {
            joint = 1;
        } else if (button == 7) {
            joint = 2;
        } else if (button == 8) {
            joint = 3;
        } else if (button == 9) {
            joint = 4;
        } else if (button == 10) {
            joint = 5;
        } else if (button == 11) {
            joint = 6;
        }
        ctrl->setMovementMode(mode, joint);
        ctrl->showData();
    }
}

void Joystick::selectMovementMode() {
    cout << "MOVEMENT MODE SETUP" << endl;
    cout << "-------------------" << endl;
    cout << "Button 3 -> JOINT" << endl;
    cout << "Button 4 -> FRAME" << endl;

    while (mode != 0 && mode != 1) {
        read(fd, &js, sizeof(js));
        if (js.type == JS_EVENT_BUTTON) {
            int button = (int) js.number;
            int value = js.value;
            if ((button == 2 || button == 3) && js_bt[button] == 1 && value == 0) {
                changeMode(button);
            }
            js_bt[button] = value;
        }
    }

    if (mode == 0) {
        cout << endl << "JOINT SETUP" << endl;
        cout << "-----------" << endl;
        cout << "Button 7 -> Joint 1" << endl;
        cout << "Button 8 -> Joint 2" << endl;
        cout << "Button 9 -> Joint 3" << endl;
        cout << "Button 10 -> Joint 4" << endl;
        cout << "Button 11 -> Joint 5" << endl;
        cout << "Button 12 -> Joint 6" << endl;
        while (joint < 1 || joint > 6) {
            read(fd, &js, sizeof(js));
            if (js.type == JS_EVENT_BUTTON) {
                int button = (int) js.number;
                int value = js.value;
                if (mode == 0 && js_bt[button] == 1 && value == 0 && button > 5 && button < 12) {
                    changeJoint(button);
                }
                js_bt[button] = value;
            }
        }
    }

    usleep(500000);

    ctrl->showData();
}
