/*
==========================================================================
    Copyright 2019 All Rights Reserved - Tony Xin and Ben Kurzion -- the coolest Jewish man of all time
	Name:          Create Seeding
	By:            Tony Xin

	PROPRIETARY and CONFIDENTIAL
============================================================================
*/
#include <kipr/botball.h>
int change;
int building;

int burningBuilding = -1;

const int motorPin = 0;
const int clawPin = 0;
const int wristPin = 1;
const int etPin = 0;

const int clawClose = 1100;
const int clawOpen = 2047;

const int wristUp = 150;
const int wristDown = 710;
const int wristInitial = 2047;
const int wristMiddle = 260;

const int armUp = 0;
const int armDown = -4200;
const int armLevel = -3560;
const int armInitial = -3100;
const int armMiddle = -1680;

void linefollow(int speed, int time, int changeGyroZ) {
    int i = 0;
    
    int anglechange = 0;
    for(i = 0; i < time; i++) {//gyro move
        anglechange = get_create_rfcliff_amt()-1250;
        create_drive_direct(speed+anglechange/15, speed-anglechange/15);
        msleep(1);
    }
    create_stop();
}

int gyroCalibrate() {
    int changeGyroZ = 0;//find average gyro value when still
    int i = 0;
    for(i = 0; i < 10000; i++) {
        changeGyroZ += gyro_x();
    }
    changeGyroZ /= 10000;
   	return changeGyroZ;
}

void move(int speed, int time, int changeGyroZ) {//speed -100 to 100, time is in miliseconds
    int i = 0;
    int anglechange = 0;
    for(i = 0; i < time; i++) {//gyro move
        anglechange -= gyro_x()-changeGyroZ;
        create_drive_direct(speed+anglechange/200, speed-anglechange/200);
        msleep(1);
    }
    create_stop();
}

int scanForItem() {
	int i;
    for(i=0; i<10; i++) {
        create_drive_direct(70, -70);
        msleep(23);
        create_stop();
        if(analog(etPin) > 2000) {
            return 1;
        }
    }
    return 0;
}

void moveArm(int pos) {
    
    while(gmpc(motorPin) > pos) {
		move_to_position(motorPin, 1100, gmpc(motorPin) - 10);
    }
    
    while(gmpc(motorPin) < pos) {
		move_to_position(motorPin, 1100, gmpc(motorPin) + 10);
    }
    
}

void detectBlackRight() {
	while(get_create_rfcliff_amt() > 2200){
    	create_drive_direct(0, 80);		//move to black line first
    }
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


int main(){
    change = gyroCalibrate();
    enable_servos();
    create_connect();
    
    moveArm(armInitial);
    set_servo_position(wristPin, wristInitial);
    set_servo_position(clawPin, clawClose);
    
    msleep(1000);
    
    turnLeft(110);
    
    move(-200, 200, change);
    msleep(100);
    
    // Ready for Picking Up Things
    move(350, 500, change);
    msleep(100);
    set_servo_position(wristPin, wristMiddle);
    set_servo_position(clawPin, clawOpen);
    moveArm(armMiddle);
    turnLeft(160);
    msleep(100);
  
    // Raising up for first sense/pick up
   	move(-150, 155, change);
    msleep(300);
    if(scanForItem() == 0) {
		// First one is burning
        burningBuilding = 0;
        move(200, 300, change);
    } else {
        msleep(500);
    	set_servo_position(clawPin, clawClose);
        msleep(800);
        mrp(motorPin, 700, 500);
        move(200, 400, change);
        turnRight(180);
        moveArm(armLevel);
        set_servo_position(clawPin, clawOpen);
        turnLeft(180);
        move(-200, 100, change);
    }
    
    // Moving 
    moveArm(armUp);
    set_servo_position(wristPin, wristUp);
    turnLeft(40);
    
    // Raising up for second sense/pick up
    
   	
   	
    
    create_disconnect();
    
    return 0;
}
