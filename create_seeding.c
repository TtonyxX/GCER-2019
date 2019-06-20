/*
==========================================================================
    Copyright 2019 All Rights Reserved - Tony Xin
	Name:          Create Seeding
	By:            Tony Xin

	PROPRIETARY and CONFIDENTIAL
============================================================================
*/
#include <kipr/botball.h>
#include "createDrive.h"

int change;
int building;

int burningBuilding = -1;

const int motorPin = 0;
const int clawPin = 0;
const int wristPin = 1;
const int etPin = 0;

const int clawClose = 550;
const int clawOpen = 1900;
const int clawCloseWater = 950;

const int wristUp = 150;
const int wristDown = 500;
const int wristInitial = 1900;
const int wristMiddle = 50;

const int armUp = 0;
const int armDown = -4200;
const int armLevel = -3950;
const int armInitial = -3250;
const int armMiddle = -1900;

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

void slow_servo(int port, int pos) {
    
	while(get_servo_position(port) > pos) {
		set_servo_position(port, get_servo_position(port)-10);
        msleep(8);
    }
    
    while(get_servo_position(port) < pos) {
		set_servo_position(port, get_servo_position(port)+10);
        msleep(8);
        if(get_servo_position(port) - pos < 100 && get_servo_position(port) - pos > -100) {
            break;
        }
    }
}

int main(){
    change = gyroCalibrate();
    enable_servos();
    create_connect();
    
    
    printf("connected");
    set_servo_position(wristPin, wristInitial);
    set_servo_position(clawPin, 900);
    moveArm(armInitial);
    
    msleep(1000);
    
     
    turnLeft(110);
    
    move(-200, 200, change);
    msleep(100);
    
    // Ready for Picking Up Things
    //move(350, 500, change);
    move(250, 250, change);
    msleep(300);
    senseLine();
    msleep(300);
    move(200, 120, change);
    msleep(100);
    set_servo_position(wristPin, wristMiddle);
    set_servo_position(clawPin, clawOpen);
    moveArm(armMiddle);
    msleep(300);
    turnLeft(160);
    msleep(100);
  
    // Raising up for first sense/pick up
   	move(-150, 215, change);
    msleep(600);
    if(scanForItem(20) == 0) {
		// First one is burning
        burningBuilding = 0;
        move(200, 300, change);
    } else {
        msleep(500);
   		move(-150, 100, change);
        msleep(500);
    	set_servo_position(clawPin, clawClose);
        msleep(800);
        mrp(motorPin, 700, 1500);
        msleep(600);
        set_servo_position(wristPin, 0);
        msleep(700);
        move(200, 300, change);
        turnRight(150);
        moveArm(armLevel);
        slow_servo(clawPin, clawOpen);
        msleep(300);
        turnLeft(150);
        move(150, 50, change);
    }
    
    // Moving and second pick up
    
    moveArm(armUp-300);
    msleep(200);
    set_servo_position(wristPin, wristUp+100);
    turnLeft(40);
    msleep(300);
    move(-200, 390, change);
    msleep(500);
    if(scanForItem(45) == 0) {
		// First one is burning
        burningBuilding = 1;
        move(200, 250, change);
        set_servo_position(wristPin, wristMiddle);
        moveArm(armMiddle);
    } else {
    	set_servo_position(wristPin, wristUp);
        turnRight(3);
        msleep(500);
   		move(-150, 100, change);
        msleep(500);
    	set_servo_position(clawPin, clawClose);
        msleep(400);
        mrp(motorPin, 500, 400);
        msleep(600);
        set_servo_position(wristPin, 0);
        //slow_servo(wristPin, 0);
        msleep(400);
        move(200, 250, change);
    	msleep(300);
        turnRight(180);
        moveArm(armLevel);
        slow_servo(clawPin, clawOpen);
        set_servo_position(wristPin, wristMiddle);
        moveArm(armMiddle);
   		move(-150, 100, change);
        msleep(500);
        turnLeft(180);
    }
    
   	// Moving and third pick up
   	
    turnLeft(40);
    msleep(100);
    set_servo_position(wristPin, wristMiddle);
    msleep(300);
    move(-200, 215, change);
    msleep(500);
    if(scanForItem(45) == 0) {
		// First one is burning
        burningBuilding = 2;
        move(200, 440, change);
   		move(-150, 100, change);
        set_servo_position(wristPin, wristMiddle);
        moveArm(armLevel);
        turnLeft(40);
    } else {
        turnRight(3);
        msleep(500);
   		move(-150, 180, change);
        msleep(500);
    	set_servo_position(clawPin, clawClose);
        msleep(400);
        mrp(motorPin, 700, 1500);
        msleep(600);
        set_servo_position(wristPin, 0);
        //slow_servo(wristPin, 0);
        msleep(400);
        move(200, 440, change);
    	msleep(300);
        turnLeft(130);
        moveArm(armLevel);
        slow_servo(clawPin, clawOpen);
        set_servo_position(wristPin, wristMiddle);
        moveArm(armMiddle);
        msleep(500);
        turnRight(90);
    }
    
    // Square up near block
    
    move(-200, 950, change);
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
    
    moveArm(armLevel + 830);
    set_servo_position(wristPin, wristDown);
    msleep(200);
    turnLeft(40);
    msleep(300);
    
    // Sense block
    
    scanForItem(50);
    turnRight(5);
    msleep(200);
    moveArm(armLevel + 100);
    msleep(200);
    set_servo_position(clawPin, clawCloseWater);
    msleep(200);
    moveArm(armUp);
    
    // Move to the burning building
    
   	turnRight(100);
    msleep(200);
    move(-200, 250, change);
    msleep(200);
    squareBlackLineBack();
    msleep(200);
    move(150, 100, change);
    msleep(200);
    turnRight(90);
    
    if(burningBuilding == 0) {
        
        moveArm(armMiddle+1000);
        msleep(50);
        set_servo_position(wristPin, wristMiddle);
        msleep(50);
     	lineFollow(200, 1400);
        
    } else if(burningBuilding == 1) {
        
        moveArm(armUp);
        msleep(50);
        set_servo_position(wristPin, 0);
        msleep(50);
     	lineFollow(200, 800);
        
    } else if(burningBuilding == 2) {
        
        moveArm(armMiddle + 1000);
        msleep(50);
        set_servo_position(wristPin, wristMiddle);
        msleep(50);
     	lineFollow(200, 200);
    	
    }
    
    
    create_disconnect();
    
    return 0;
}
