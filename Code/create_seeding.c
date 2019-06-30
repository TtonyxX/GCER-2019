/*
Created by Tony Xin for 2019 GCER
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

const int clawClose = 500;
const int clawOpen = 1900;
const int clawCloseWater = 850;

const int wristUp = 300;
const int wristDown = 650;
const int wristInitial = 2050;
const int wristMiddle = 190;

const int armUp = 0;
const int armDown = -3860;
const int armLevel = -4300;
const int armInitial = -3250;
const int armMiddle = -2130; //1950 --> 1860

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
    for(i = 0; i < 10000; i++) {
        changeGyroZ += gyro_x();
    }
    changeGyroZ /= 10000;
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

void detectBlackRightTwo() {
	while(get_create_rcliff_amt() > 2200){
    	create_drive_direct(0, 80);		//move to black line first
    }
    create_stop();
}

void squareBlackLineTwo() {
    thread thread = thread_create(detectBlackRightTwo);
    thread_start(thread);
  	
    while(get_create_lcliff_amt() > 2200){
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

void lightSense() {
	while(analog(lightPin) > 2000) {
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
    printf("connected");
    
    // clear motor position
    while(digital(btnPin) == 0) {
        mrp(motorPin, 500, 10);
        msleep(1);
    }
    
    cmpc(motorPin);
    
    msleep(1000); // Sleep to check if up is right
    
    set_servo_position(servoPin, servoUp);
    set_servo_position(wristPin, wristInitial);
    set_servo_position(clawPin, 900);
    moveArm(armInitial);
    
    msleep(1000); // Light sense here
    //lightSense();
     
    turnLeft(110);
    
    move(-200, 200, change);
    msleep(100);
    
    // Ready for Picking Up Things
    move(250, 250, change);
    msleep(400);
    senseLine();
    msleep(400);
    move(200, 120, change);
    msleep(100);
    set_servo_position(wristPin, wristMiddle-100);
    set_servo_position(clawPin, clawOpen);
    moveArm(armMiddle);
    msleep(300);
    turnLeft(160);
    msleep(100);
  
    // Raising up for first sense/pick up
   	move(-150, 145, change);
    msleep(600);
    if(scanForItem(20) == 0) {
		// First one is burning
        burningBuilding = 0;
        move(200, 300, change);
    } else {
    	set_servo_position(wristPin, wristMiddle);
        msleep(500);
        turnRight(2);
        msleep(100);
   		move(-150, 135, change);
        msleep(500);
    	set_servo_position(clawPin, clawClose);
        msleep(800);
        moveArm(gmpc(motorPin) + 1500);
        msleep(850);
        slow_servo(wristPin, 0);
        msleep(700);
        move(200, 300, change);
        turnRight(180);
        moveArm(armLevel-300);
        slow_servo(clawPin, clawOpen);
        msleep(300);
        turnLeft(180);
        move(150, 135, change);
    }
    
    // Moving and second pick up
    
    moveArm(armUp-300);
    msleep(200);
    set_servo_position(wristPin, wristUp+70);
    turnLeft(35);
    msleep(300);
    move(-200, 387/*390*/, change);
    msleep(500);
    if(scanForItem(45) == 0) {
		// Second one is burning
        burningBuilding = 1;
        move(200, 250, change);
        set_servo_position(wristPin, wristMiddle);
        moveArm(armMiddle+100);
    } else {
    	set_servo_position(wristPin, wristUp+100);
        turnRight(3);
        msleep(500);
   		move(-150, 170, change);
        msleep(500);
    	set_servo_position(clawPin, clawClose-300);
        msleep(400);
        moveArm(armUp);
        msleep(600);
        slow_servo(wristPin, 0);
        msleep(400);
        move(200, 250, change);
    	msleep(300);
        turnRight(170);
        moveArm(armLevel-300);
        slow_servo(clawPin, clawOpen);
        set_servo_position(wristPin, wristMiddle);
        moveArm(armMiddle+100);
   		move(-150, 160, change);
        msleep(500);
        turnLeft(170);
    }
    
   	// Moving and third pick up
    
    if(burningBuilding != 2) {
   	
        turnLeft(40);
        moveArm(armMiddle);
        msleep(100);
        set_servo_position(wristPin, wristMiddle);
        msleep(300);
        move(-200, 260, change);
        msleep(500);
        scanForItem(50);
        
        turnRight(0);
        msleep(500);
        move(-150, 100, change);
        msleep(500);
        set_servo_position(clawPin, clawClose);
        msleep(400);
        moveArm(gmpc(motorPin) + 1500);
        msleep(950);
        slow_servo(wristPin, 0);
        //slow_servo(wristPin, 0);
        msleep(400);
        move(200, 300, change);
        msleep(300);
        turnLeft(140);
        moveArm(armLevel-300);
        slow_servo(clawPin, clawOpen);
        set_servo_position(wristPin, wristMiddle);
        moveArm(armMiddle);
        msleep(500);
        turnRight(90);
        
    } else {
        
        turnLeft(90);
        
    }
    
    // Square up near block
    if(burningBuilding == 2) {
		move(-200, 940, change);
    } else {
        move(-200, 800, change);
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
    msleep(200);
    turnLeft(40);
    msleep(300);
    
    // Sense block
    
    scanForItem(50);
    turnRight(3);
    msleep(200);
    moveArm(armLevel + 300);
    msleep(200);
    set_servo_position(clawPin, clawCloseWater);
    msleep(200);
    moveArm(armUp);
    
    // Move to the burning building
    
   	turnRight(115);
    msleep(200);
    move(-200, 320, change);
    msleep(200);
    squareBlackLineBack();
    msleep(200);
    move(-180, 40, change);
    msleep(200);
    turnLeft(90);
    senseLineBackExternal();
    moveArm(armUp);
    msleep(50);
    slow_servo(wristPin, 0);
    msleep(100);
    move(100, 25, change);
    
    if(burningBuilding == 0) {
     
        msleep(50);
     	lineFollow(55, 100);
     	lineFollow(150, 250);
     	move(200, 900, change);
        msleep(200);
        turnRight(90);
        msleep(200);
        slow_servo(wristPin, wristMiddle);
        msleep(50);
        moveArm(armMiddle+500);
        msleep(200);
        move(100, 40, change);
        
    } else if(burningBuilding == 1) {
        
        msleep(50);
     	lineFollow(55, 100);
     	lineFollow(150, 250);
     	move(200, 470, change);
        msleep(200);
        turnRight(90);
        msleep(200);
        move(-150, 150, change);
        msleep(200);
        slow_servo(wristPin, wristUp+100);
        msleep(300);
        
    } else if(burningBuilding == 2) {
        
        msleep(50);
     	lineFollow(55, 100);
     	lineFollow(150, 250);
     	move(200, 7, change); // was 150
        msleep(200);
        turnRight(90);
        msleep(200);
        moveArm(armMiddle + 500);
        msleep(50);
        slow_servo(wristPin, wristMiddle);
        msleep(50);
    	
    }
    
    slow_servo(clawPin, clawOpen);
    set_servo_position(wristPin, wristUp);
   	set_servo_position(servoPin, servoUp);
    msleep(100);
    moveArm(armUp);
    msleep(100);
    if(burningBuilding != 1) {
    	move(-140, 130, change);
    }
    
    msleep(200);
    squareBlackLineTwo();
    msleep(200);
    move(150, 50, change);
    msleep(100);
    turnLeft(90);
    msleep(200);
    lineFollowDetect(160);
    msleep(200);
    move(-200, 280, change);
    msleep(200);
    turnRight(90);
    msleep(200);
    move(-200, 380, change);
    msleep(200);
    
    // Pick up container
    slow_servo(servoPin, servoDown);
    msleep(200);
    move(100, 400, change);
    
    create_disconnect();
    
    return 0;
}
