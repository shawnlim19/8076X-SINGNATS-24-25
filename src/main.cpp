#include "main.h"
#include <iostream>
#include <ostream>
#include "globals.hpp"

bool beep = false;

void initialize()
{   
    chassis.calibrate(true);

    //Mechanism Tasks (More information in Mechcontrol.cpp)
    Task ringTask(ringControl); //Detecting where the ring is
    Task intakeTask(intakeControl); //Changing the intake speed based on where the ring is 
    Task climbTask(climbControl); //Climbing sequence

    //Display Tasks (More information in display.cpp)
    Task displayTask(displayControl); //Brain screen with auton selector
    Task controllerTask(controllerControl);
}

void disabled() {}

void competition_initialize() {}

ASSET(path_txt);

void autonomous()
{
    double start = millis(); 
    autons = true;
    autoclampBool = true;

    //Switch case for auton selector
    switch (autonnum)
    {
    case 1:
        rush=true;
        goalSideSafe();
        break;

    case 2:
        elims = true;
        goalSideSafe();
        break;

    case 3:
        rush = false;
        ringSide();
        break;

    case 4:
        rush = true;
        ringSide();
        break;

    case 5:
        soloAWP();
        break;
    case 6:
        alliance = 1;
        skills();
        break;
    }

    chassis.waitUntilDone();
    screen::print(TEXT_SMALL, 300, 180, "Time elapsed: %f", (millis() - start)/1000);
}

void opcontrol() {
    stage=0;
    autons = false;
    bool shift = false;
    int intakeSpeed = 0;
    bool setheight = false;
    bool wallstakeBool = false;
    bool sense;

    while (true) {

        if (!master.get_digital(E_CONTROLLER_DIGITAL_UP)) {
            sense = false;
        } else {
            sense = true;
        }

        int leftY = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX;
        if (sense) {
            rightX = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        } else {
            rightX = (master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X)*0.7);
        }
        
        
        if (!climbing){
            chassis.setBrakeMode(MOTOR_BRAKE_COAST);
            left.move(leftY + rightX);
            right.move(leftY - rightX);
        }

        if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_B)){
            climbing = !climbing;
            climbsection += 1;
            setheight = true;
        }

        if (master.get_digital_new_press(DIGITAL_X)) {
            coloursort = !coloursort;
        }

        if (!setheight) {
            if (climbEnc.get_position() < -14000) {
                // climb.retract();
                beep = true;
                setheight=true;
            }
        }

        if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)) {
            wallstake = !wallstake;
        }

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2)){
            shift = true;
        } else {
            shift = false;
        }

        if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_R2)){
            if (shift) {
                climb.toggle();
            } else {
                clamp.toggle();
            }
        }


        if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_R1)){
            if (shift) {
                sweeper.toggle();
            } else {
                if (wallstake) {
                    intakeSpeed=127;
                    wallstakeExtend();
                    intakeSpeed=0;
                    wallstakeBool = true;
                } else if (wallstakearm.is_extended()) {
                    wallstakearm.retract();
                    wallstakeBool = false;
                    // manualintake=false;
                } else if (!wallstake) {
                    wallstake = true;
                    manualintake = true;
                }
            }
        }

        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && !shift) {
            intakeSpeed = 127;
        } else if (master.get_digital(E_CONTROLLER_DIGITAL_L1) && shift) {
            intakeSpeed = -127;
        } else {
            intakeSpeed = 0;
        }
        
        if (wallstakeBool) {
            setIntakeSpeed(0, intakeSpeed);
        } else if (manualintake) {
            setIntakeSpeed(intakeSpeed, intakeSpeed);
        } else {
            setIntakeSpeed(intakeSpeed, intakeSpeed);
        }

        if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
            intakestack.toggle();
        }

        if (master.get_digital_new_press(DIGITAL_Y)){
            wallstake = !wallstake;
        }

        pros::delay(10);
    }
}
