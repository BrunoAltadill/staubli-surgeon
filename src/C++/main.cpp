// LIBRARIES
#include <thread>
#include <iostream>
#include "Control.h"
#include "Joystick.h"
using namespace std;


// MAIN
int main() {
    cout << "###############################" << endl;
    cout << "#     THE STAUBLI SURGEON     #" << endl;
    cout << "###############################" << endl;

    // Start control class for comunicate PC <-> CS8
    Control * ctrl = new Control();

    // Start joystick class for read movement commands
    Joystick * js = new Joystick(ctrl);
    thread t1(&Joystick::readJoystick, js);
    t1.detach();

    // Start monitorization socket
    thread t2(&Control::monitorizationSocket, ctrl);
    t2.detach();

    // Monitor until wait finish execution
    bool finish = false;
    while(!finish){
        finish = js->getFinishState();
    }

    ctrl->notifyExit();
    delete js;

    return 0;
}
