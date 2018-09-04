#include "Control.h"

Control::Control() {
    operationMode = 1;
    init = false;
    suspended = false;
    blockTransmission = false;
    vel = "1.0";
    velocityCommand = "V;0;";
    jointCommand = "J;0;0;";
    frameCommand = "F;0;0;0;0;0;0;";
    socket = TCPClient("84.88.129.201", 2000);
    setOperationMode();
    setForces();
}

Control::~Control() {
    socket.exit();
}

void Control::setMovementMode(int mode, int joint) {
    movementMode = mode;
    currentJoint = joint;
}

void Control::finishInitMode () {
    init = true;
}

void Control::movement(int mode, int joint, int * axis) {
    if (!suspended) {
        //Velocity control
        string command = "V;";
        vel = "1.0";
        if (axis[4] < -30000) {
            vel = "12.0";
        } else if (axis[4] >= -30000 && axis[4] < -20000) {
            vel = "10.0";
        } else if (axis[4] >= -20000 && axis[4] < -10000) {
            vel = "7.0";
        } else if (axis[4] >= -10000 && axis[4] < -5000) {
            vel = "5.0";
        } else if (axis[4] >= -5000 && axis[4] < 5000) {
            vel = "2.5";
        } else if (axis[4] >= 5000 && axis[4] < 10000) {
            vel = "2.0";
        } else if (axis[4] >= 10000 && axis[4] < 20000) {
            vel = "1.5";
        } else if (axis[4] >= 20000 && axis[4] < 30000) {
            vel = "1.0";
        } else if (axis[4] > 30000) {
            vel = "0.5";
        }
        command += vel + ";";
        if (command != velocityCommand) {
            socket.sendData(command);
            velocityCommand = command;
            showData();
        }

        //Movement control
        command = "";
        if (mode == 0) {
            command = "J;";
            command += to_string(joint) + ";";
            if (axis[1] > 15000) {
                command += "1;";
            } else if (axis[1] < -15000) {
                command += "-1;";
            } else {
                command += "0;";
            }
        } else if (mode == 1) {
            command = "F;";
            for (int i = 1; i < 4; i++) {
                double data = axis[i];
                if (data > 15000) {
                    command += "-1;";
                } else if (data < -15000) {
                    command += "1;";
                } else {
                    command += "0;";
                }
            }
        }
        if (mode == 0 && command != jointCommand) {
            socket.sendData(command);
            jointCommand = command;
        } else if (mode == 1 && command != frameCommand) {
            socket.sendData(command);
            frameCommand = command;
        }
    }
}

void Control::suspendResume() {
    if (!blockTransmission) {
        suspended = !suspended;
        socket.sendData("S;");
        showData();
    }
}

void Control::notifyExit() {
    socket.sendData("E;");
}

void Control::setOperationMode() {
    blockTransmission = true;
    if (!suspended) {
        suspendResume();
    }

    cout << "OPERATION MODE SETUP" << endl;
    cout << "--------------------" << endl;
    cout << "1.- Standard operation" << endl;
    cout << "2.- Follow irregular path" << endl;
    cout << "3.- Click the vein" << endl << endl;
    cout << "Select operation mode: ";
    cin >> operationMode;
    while (operationMode < 1 || operationMode > 3) {
        cout << "ERROR: Invalid option!" << endl;
        cout << "Select operation mode: ";
        cin >> operationMode;
    }
    string command = "O;" + to_string(operationMode) + ";";
    socket.sendData(command);
    cout << endl << endl;

    blockTransmission = false;
    if (suspended) {
        suspendResume();
    }

    showData();
}

void Control::setForces() {
    blockTransmission = true;
    if (!suspended) {
        suspendResume();
    }

    int optionForce;
    cout << "FORCE SETUP" << endl;
    cout << "-----------" << endl;
    cout << "1.- Standard forces" << endl;
    cout << "2.- Custom forces" << endl << endl;
    cout << "Select force option: ";
    cin >> optionForce;
    while (optionForce != 1 && optionForce != 2) {
        cout << "ERROR: Invalid option!" << endl;
        cout << "Select force option: ";
        cin >> optionForce;
    }

    string command = "P;";
    if (optionForce == 1) {
        maxForces[0] = -10;
        maxForces[1] = -10;
        maxForces[2] = -10;
        command += "-10;-10;-10;";
    } else if (optionForce == 2) {
        int force = -10;
        string axis [3] = {"X", "Y", "Z"};
        for (int i = 0; i < 3; i++) {
            cout << "Enter force for " + axis[i] + " axis: ";
            cin >> force;
            if (cin.fail()) {
                force = -10;
            }
            command += to_string(force) + ";";
            maxForces[i] = force;
        }
    }
    socket.sendData(command);
    cout << endl << endl;

    blockTransmission = false;
    if (suspended) {
        suspendResume();
    }

    showData();
}

void Control::monitorizationSocket() {
    while (true) {
        string data = socket.read();
        cout << "Data recived: " << data << endl;
        if (data == "A") {
            suspended = true;
            string click;
            cout << "Start click the vein operation? (y/n): ";
            cin >> click;
            while (click != "y" && click != "n") {
                cout << "Start click the vein operation? (y/n): ";
                cin >> click;
            }
            string command = "D;" + click + ";";
            socket.sendData(command);
            suspended = false;
        }
        usleep(1000000);
    }
}

void Control::showData() {
    if (init) {
        system("clear");

        // TITLE
        cout << "##################################" << endl;
        cout << "#     OPERATION IN PROGRESS!     #" << endl;
        cout << "#     ----------------------     #" << endl;
        cout << "#        THIS IS SPARTA!!        #" << endl;
        cout << "#    Try not to kill someone!    #" << endl;
        cout << "##################################" << endl << endl;

        // OPERATION MODE
        string opMode = "";
        if (operationMode == 1) {
            opMode = "Standard";
        } else if (operationMode == 2) {
            opMode = "Follow irregular path";
        } else if (operationMode == 3) {
            opMode = "Click the vein";
        }
        cout << "OPERATION MODE" << endl;
        cout << "--------------" << endl;
        cout << "  -> " + opMode << endl << endl << endl;

        // FORCE SETTINGS
        cout << "MAX. FORCES [N]" << endl;
        cout << "---------------" << endl;
        cout << "  -> X: " << maxForces[0] << endl;
        cout << "  -> Y: " << maxForces[1] << endl;
        cout << "  -> Z: " << maxForces[2] << endl<< endl << endl;

        // MOVEMENT MODE
        string mvMode = "";
        if (movementMode == 0) {
            mvMode = "JOINT (" + to_string(currentJoint) + ")";
        } else if (movementMode == 1) {
            mvMode = "FRAME";
        }
        cout << "MOVEMENT MODE" << endl;
        cout << "-------------" << endl;
        cout << "  -> " + mvMode << endl << endl << endl;

        // VELOCITY
        cout << "VELOCITY [%]" << endl;
        cout << "------------" << endl;
        cout << "  -> " + vel << endl << endl << endl;

        // ROBOT STATUS
        string status = "ARM MOVEMENT DISABLED";
        if (!suspended) {
            status = "ARM MOVEMENT ENABLED";
        }
        cout << "ROBOT STATUS" << endl;
        cout << "------------" << endl;
        cout << "  -> " + status << endl << endl << endl;

        cout << "MODIFY SETTINGS" << endl;
        cout << "---------------" << endl;
    }
}
