/*
Created by Tony Xin for 2019 GCER for Seeding
http://tonyxin.ga
*/

#include <kipr/botball.h>
#include "createDrive.h"
#include<time.h>

int change;
int building;

int burningBuilding = 2;

const int motorPin = 0;
const int clawPin = 0;
const int wristPin = 1;
const int etPin = 0;
const int btnPin = 0;
const int lightPin = 1;
const int tophatPin = 2;
const int servoPin = 2;

const int servoDown = 2047;
const int servoUp = 1100;

const int clawClose = 700;
const int clawOpen = 1900;
const int clawCloseWater = 850;

const int wristUp = 300;
const int wristDown = 650;
const int wristInitial = 2050;
const int wristMiddle = 190;
const int wristGas = 490;

const int armUp = 0;
const int armDown = -3860;
const int armLevel = -3900;
const int armInitial = -3300;
const int armMiddle = -2200;
const int armGas = -3550;

int timeWait = 0;
int timeDone = 0;

void lineFollow(int speed, int time) {
    int i = 0;

    int anglechange = 0;
    for(i = 0; i < time; i++) { // gyro move
        anglechange = get_create_rfcliff_amt()-1250;
        create_drive_direct(speed+anglechange/15, speed-anglechange/15);
        msleep(1);
    }
    create_stop();
}

void lineFollowOther(int speed, int time) {
    int i = 0;

    int anglechange = 0;
    for(i = 0; i < time; i++) { // gyro move
        anglechange = get_create_lfcliff_amt()-2150;
        create_drive_direct(speed+anglechange/15, speed-anglechange/15);
        msleep(1);
    }
    create_stop();
}

void lineFollowDetect(int speed) {
    int anglechange = 0;
    while(get_create_rbump() == 0) {
        anglechange = get_create_rfcliff_amt()-1250;
        create_drive_direct(speed+anglechange/15, speed-anglechange/15);
        msleep(1);
    }
    create_stop();
}

int gyroCalibrate() {
    int changeGyroZ = 0;//find average gyro value when still
    int i = 0;
    for(i = 0; i < 2000; i++) {
        changeGyroZ += gyro_x();
    }
    changeGyroZ /= 2000;
    return changeGyroZ;
}

void wait() {
    msleep(timeWait);
    timeDone = 1;
}

void move(int speed, int time, int changeGyroZ) {//speed -100 to 100, time is in miliseconds

    timeWait = time*3.78;
    timeDone = 0;
    thread thread = thread_create(wait);
    thread_start(thread);

    int anglechange = 0;
    while(timeDone == 0) {
        anglechange -= gyro_x()-changeGyroZ;
        create_drive_direct(speed+anglechange/200, speed-anglechange/200);
        msleep(1);
    }

    create_stop();
    thread_wait(thread);
    thread_destroy(thread);
}

void moveTwo(int speed, int time, int changeGyroZ) {

    int anglechange = 0;
    int i = 0;
    for(i = 0; i < time; i++) {
        anglechange -= gyro_x()-changeGyroZ;
        create_drive_direct(speed+anglechange/200, speed-anglechange/200);
        msleep(1);
    }
    create_stop();
}

int scanForItem(int times) {
    int i;
    for(i=0; i<times; i++) {
        create_drive_direct(30, -30);
        msleep(30);
        create_stop();
        if(analog(etPin) > 2000) {
            return 1;
        }
    }
    return 0;
}

int scanForItemSlow(int times) {
    int i;
    for(i=0; i<times; i++) {
        create_drive_direct(20, -20);
        msleep(30);
        create_stop();
        if(analog(etPin) > 2000) {
            return 1;
        }
    }
    return 0;
}


void moveArm(int pos) {

    while(gmpc(motorPin) > pos) {
        move_to_position(motorPin, 1800, gmpc(motorPin) - 10);
    }

    while(gmpc(motorPin) < pos) {
        move_to_position(motorPin, 1800, gmpc(motorPin) + 10);
    }

}

void detectBlackRight() {
    while(get_create_rfcliff_amt() > 2200){
        create_drive_direct(0, 80);		//move to black line first
    }
    create_stop();
}

void squareBlackLine() {
    thread thread = thread_create(detectBlackRight);
    thread_start(thread);

    while(get_create_lfcliff_amt() > 2200){
        create_drive_direct(80, 0);		//move to black line first
    }

    create_stop();

    thread_wait(thread);
    thread_destroy(thread);
}

void detectBlackRightTwo() {
    while(get_create_rcliff_amt() > 2200) {
        create_drive_direct(0, 80);		//move to black line first
    }
    create_stop();
}

void squareBlackLineTwo() {
    thread thread = thread_create(detectBlackRightTwo);
    thread_start(thread);

    while(get_create_lcliff_amt() > 2200) {
        create_drive_direct(80, 0);		//move to black line first
    }

    create_stop();

    thread_wait(thread);
    thread_destroy(thread);
}

void detectBlackRightBack() {

    while(get_create_rcliff_amt() > 2000){
        create_drive_direct(0, -100);
    }
    create_stop();
}

void squareBlackLineBack() {
    thread thread = thread_create(detectBlackRightBack);
    thread_start(thread);

    while(get_create_lcliff_amt() > 2000){
        create_drive_direct(-100, 0);
    }

    create_stop();

    thread_wait(thread);
    thread_destroy(thread);
}

void senseLine() {
    while(get_create_lfcliff_amt() > 2200){
        create_drive_direct(200, 200);
    }
    create_stop();
}

void senseLineBack() {
    while(get_create_lcliff_amt() > 2200){
        create_drive_direct(-180, -180);
    }
    create_stop();
}

void senseLineBackExternal() {
    while(analog(tophatPin) < 1000){
        create_drive_direct(-180, -180);
    }
    create_stop();
}

void turnLeft(int angle) {
    create_drive_direct(-200, 200);
    msleep(10.7 * angle);
    create_stop();
}

void turnRight(int angle) {
    create_drive_direct(200, -200);
    msleep(10.7 * angle);
    create_stop();
}

void turnLeftSlow(int angle) {
    create_drive_direct(-100, 100);
    msleep(21.4 * angle);
    create_stop();
}

void turnRightSlow(int angle) {
    create_drive_direct(100, -100);
    msleep(21.4 * angle);
    create_stop();
}

void light_start() {
    while(right_button());
    int max = 0,
    min = 9999,
    curr = 0,
    avg = 0;

    display_clear();
    display_printf(0,0,"Max:");
    display_printf(0,1,"Min:");
    display_printf(0,2,"Curr:");
    display_printf(0,3,"avg:");

    while (!right_button()) {
        curr = analog(lightPin);

        if (curr > max) max = curr;
        if (curr < min) min = curr;

        avg = (max + min) / 2;

        display_printf(5, 0, "%d   ", max);
        display_printf(5, 1, "%d   ", min);
        display_printf(6, 2, "%d   ", curr);
        display_printf(5,3,"%d   ",avg);

        if (curr > avg) display_printf(10,5,"XX");
        else display_printf(10,5,"OO");

        msleep(50);
    }
    msleep(1000);
    display_clear();
    display_printf(0,4,"Prepared to begin: left to skip");

    while (analog(lightPin) > avg && !(right_button())) msleep(50);
}

void waitButton() {
    while(right_button() == 0) {
        msleep(2);
    }
}

void slow_servo(int port, int pos) {

    while(get_servo_position(port) > pos) {
        if(get_servo_position(port)-10 < 0) {
            break;
        }
        set_servo_position(port, get_servo_position(port)-10);
        msleep(8);
    }

    while(get_servo_position(port) < pos) {
        if(get_servo_position(port)+10 > 2047) {
            break;
        }
        set_servo_position(port, get_servo_position(port)+10);
        msleep(8);
    }

    set_servo_position(port, pos);

}

int main(){
    change = gyroCalibrate();
    enable_servos();
    create_connect();
    create_full();
    printf("connected");

    // clear motor position
    while(digital(btnPin) == 0) {
        mrp(motorPin, 500, 20);
        msleep(1);
    }

    cmpc(motorPin);
    
    
    moveArm(armLevel + 950);
    set_servo_position(wristPin, wristDown);
    msleep(100000);

    msleep(1000); // Sleep to check if up is right

    set_servo_position(wristPin, wristInitial);
    set_servo_position(clawPin, 900);
    moveArm(armInitial);

    slow_servo(servoPin, 0); //bin gripper down

    msleep(1000); // Light sense here
    wait_for_light(lightPin);
    //waitButton();
    shut_down_in(119);

    turnRight(90);
    msleep(2800);
    turnRight(165);

    move(-200, 200, change);
    msleep(100);

    // Ready for Picking Up Things
    move(250, 250, change);
    msleep(100);
    senseLine();
    msleep(100);
    move(200, 120, change);
    msleep(200);
    set_servo_position(wristPin, wristMiddle-100);
    set_servo_position(clawPin, clawOpen);
    moveArm(armMiddle+50);
    msleep(200);
    turnLeft(156);
    msleep(100);

    // Raising up for first sense/pick up
    move(-150, 135, change);
    msleep(500);
    if(scanForItem(20) == 0) {
        // First one is burning
        burningBuilding = 0;
        move(200, 300, change);
    } else {
        turnRight(2);
        msleep(100);
        move(-150, 125, change);
        msleep(200);
        set_servo_position(clawPin, clawClose);
        msleep(300);
        moveArm(gmpc(motorPin) + 1500);
        msleep(800);
        slow_servo(wristPin, 0);
        msleep(400);
        move(200, 300, change);
        turnRight(170);
        moveArm(armLevel-300);
        slow_servo(clawPin, clawOpen);
        msleep(200);
        turnLeft(170);
        move(150, 135, change);
    }

    // Moving and second pick up

    moveArm(armUp-300);
    msleep(100);
    set_servo_position(wristPin, wristUp+70);
    turnLeft(38);
    msleep(50);
    moveArm(armUp-300);
    msleep(50);
    move(-200, 387, change);
    msleep(300);
    if(scanForItem(40) == 0) {
        // Second one is burning
        burningBuilding = 1;
        move(200, 250, change);
        set_servo_position(wristPin, wristMiddle);
        moveArm(armMiddle+100);
    } else {
        turnRight(3);
        msleep(200);
        move(-150, 170, change);
        msleep(200);
        set_servo_position(clawPin, clawClose-210);
        msleep(200);
        while(digital(btnPin) == 0) {
            mrp(motorPin, 500, 20);
            msleep(1);
        }
        msleep(200);
        slow_servo(wristPin, 0);
        msleep(200);
        move(200, 250, change);
        msleep(100);
        turnRight(170);
        moveArm(armLevel-300);
        slow_servo(clawPin, clawOpen);
        set_servo_position(wristPin, wristMiddle);
        if(burningBuilding != 0) msleep(4300);
        moveArm(armMiddle+100);
        move(-150, 160, change);
        msleep(300);
        turnLeft(180);
    } 

    // Moving and third pick up

    if(burningBuilding != 2) {

        msleep(800);
        if(burningBuilding != 1) {
            turnLeft(40);
        } else {
            turnLeft(58);
        }
        moveArm(armMiddle+150);
        msleep(100);
        set_servo_position(wristPin, wristMiddle-60);
        msleep(200);
        move(-200, 215, change);
        msleep(300);
        scanForItem(65);
        turnRight(2);
        msleep(300);
        move(-150, 128, change);
        msleep(300);
        set_servo_position(clawPin, clawClose);
        msleep(300);
        moveArm(gmpc(motorPin) + 1500);
        msleep(800);
        slow_servo(wristPin, 0);
        //slow_servo(wristPin, 0);
        msleep(300);
        move(200, 300, change);
        msleep(200);
        turnLeft(150);
        moveArm(armLevel-300);
        slow_servo(clawPin, clawOpen);
        set_servo_position(wristPin, wristMiddle);
        msleep(3300);
        moveArm(armMiddle);
        msleep(300);
        turnRight(100);

    } else {

        turnLeft(90);

    }

    // Square up near block
    if(burningBuilding == 2) {
        move(-200, 880, change);
    } else {
        move(-200, 750, change);
    }
    msleep(200);
    turnLeft(90);
    msleep(200);
    move(-200, 900, change);
    msleep(200);
    move(200, 200, change);
    turnRight(90);
    msleep(200);
    squareBlackLine();
    msleep(200);

    // Move towards block

    moveArm(armLevel + 950);
    set_servo_position(wristPin, wristDown);
    set_servo_position(clawPin, 2047);
    msleep(100);
    turnLeft(40);
    msleep(100);

    // Sense block

    scanForItem(50);
    turnRight(3);
    msleep(100);
    moveArm(armLevel + 300);
    msleep(100);
    set_servo_position(clawPin, clawCloseWater);
    msleep(100);
    moveArm(armUp);
    msleep(100);

    // Move to the burning building

    turnRight(115);
    msleep(100);
    move(-200, 355, change);
    msleep(100);	

    squareBlackLineBack();
    msleep(100);
    move(200, 50, change);
    msleep(100);
    //move(-180, 40, change);
    //msleep(100);
    turnLeft(90);
    msleep(100);
    senseLineBackExternal();
    moveArm(armUp);
    msleep(50);
    slow_servo(wristPin, 0);
    msleep(100);
    turnRight(90);
    msleep(200);
    move(200, 50, change);
    msleep(200);
    turnLeft(90);
    msleep(100);

    if(burningBuilding == 0) {

        msleep(50);
        lineFollowOther(55, 100);
        lineFollowOther(150, 250);
        move(200, 885, change);
        msleep(100);
        turnRight(87);
        msleep(100);
        move(-170, 80, change);
        msleep(200);
        slow_servo(wristPin, wristMiddle);
        msleep(50);
        moveArm(armMiddle+400);

    } else if(burningBuilding == 1) {

        msleep(50);
        lineFollowOther(55, 100);
        lineFollowOther(150, 250);
        move(200, 454, change);
        msleep(200);
        turnRight(90);
        msleep(100);
        move(-150, 290, change);
        msleep(100);
        slow_servo(wristPin, wristUp+100);

    } else if(burningBuilding == 2) {

        msleep(50);
        lineFollowOther(55, 100);
        lineFollowOther(150, 250);
        msleep(100);
        turnRight(87);
        msleep(100);
        move(-170, 100, change);
        msleep(200);
        moveArm(armMiddle + 400);
        msleep(50);
        slow_servo(wristPin, wristMiddle);

    }

    slow_servo(clawPin, clawOpen);
    set_servo_position(servoPin, servoUp);
    msleep(100);
    moveArm(armUp);
    msleep(100);

    if(burningBuilding == 2 || burningBuilding == 0) {
        move(-170, 70, change);
        msleep(100);
    }

    squareBlackLineTwo();
    msleep(100);
    move(150, 60, change);
    msleep(200);
    turnLeft(90);
    msleep(100);
    lineFollow(180, 500);
    set_servo_position(clawPin, clawOpen);
    set_servo_position(wristPin, wristUp);
    lineFollowDetect(230);
    msleep(100);
    move(-200, 280, change);
    msleep(100);
    turnRight(90);
    msleep(100);
    move(-200, 380, change);
    msleep(100);

    // Pick up container
    slow_servo(servoPin, servoDown);
    msleep(200);
    move(100, 400, change);

    // Square to get gas
    squareBlackLine();
    set_servo_position(servoPin, servoUp);
    msleep(100);
    move(200, 220, change);
    msleep(100);
    set_servo_position(wristPin, wristGas);
    moveArm(armGas);
    msleep(100);
    slow_servo(clawPin, clawClose);
    msleep(100);

    // Get gas
    slow_servo(clawPin, clawClose);
    msleep(100);
    moveArm(armUp);
    msleep(400);
    turnLeftSlow(160);
    msleep(400);
    move(-150, 150, change);
    msleep(200);
    moveArm(armLevel-100);
    msleep(100);
    slow_servo(clawPin, clawOpen);

    create_disconnect();

    return 0;
}
