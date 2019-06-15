/*
==========================================================================
    Copyright 2019 All Rights Reserved - Tony Xin
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

const int clawClose = 200;
const int clawOpen = 830;

const int wristUp = 0;
const int wristDown = 500;
const int wristInitial = 2047;

const int armUp = 0;
const int armDown = -4200;
const int armLevel = -3500;

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



void turn(int direction, int degree, int changeGyroZ) {
    int anglechange = 0;
    int x = degree*170000/90;
    if(direction == 1) { // 1 = clockwise, 0 = counter-clockwise
    	while(anglechange > -x) {
        	anglechange -= gyro_x()-changeGyroZ;
            create_drive_direct(100, -100);
        	msleep(1);
    	}
    } else {
        while(anglechange < x) {
        	anglechange -= gyro_x()-changeGyroZ;
        	create_drive_direct(-100, 100);
        	msleep(1);
    	}
    }
    create_stop();
    msleep(200);
}



void pickUp(int armPos, int wristPos){  
    int i;
    for(i = 1; i < 63; i++){
        int clawPos = get_servo_position(clawPin);
    	set_servo_position(clawPin, clawPos - 10);  //close claw
    	msleep(5);
    }//claw close end
    
    printf("Moving arm up\n");
    move_to_position(motorPin, 1000, 2600);	//pick arm up   
    msleep(400);
    if(get_motor_position_counter(motorPin) < 2500 || get_motor_position_counter(motorPin) > 2700){
    		printf("Moving arm up again\n");
    		move_to_position(motorPin, 500, 2700);
    		msleep(400);
    	}//second arm down end
    
}//pickUp end

void moveArm(int pos) {
    
    while(gmpc(motorPin) > pos) {
		move_to_position(motorPin, 1000, gmpc(motorPin) - 10);
    }
    
    while(gmpc(motorPin) < pos) {
		move_to_position(motorPin, 1000, gmpc(motorPin) + 10);
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

void putDown(int armPos, int wristPos){
    set_servo_position(wristPin, wristDown);		//set wrist angle
    msleep(50);
    
    printf("Moving arm down\n");
	move_to_position(motorPin, 1000, armDown);		//lower arm
    msleep(4000);
    if(get_motor_position_counter(motorPin) < armDown - 100 || get_motor_position_counter(motorPin) > armDown + 100){
    	printf("Moving arm down again\n");
    	move_to_position(motorPin, 1000, armDown);
    	msleep(4600);
    }//second arm up end
    
    set_servo_position(clawPin, clawOpen);			//open claw
    msleep(50);
	
	
    move_to_position(motorPin, 1000, armPos);		//set arm height
    msleep(2000);
    set_servo_position(wristPin, wristPos);			//set wrist ange
    msleep(50);
    
}//putDown end

int main(){
    change = gyroCalibrate();
    enable_servos();
    create_connect();
    
    moveArm(armLevel);
    set_servo_position(wristPin, wristInitial);
    
    msleep(1000);
    
    turn(0, 120, change);
    move(-200, 200, change);
    msleep(100);
    
    // Ready for Picking Up Things
    move(350, 500, change);
    msleep(100);
    turn(0, 190, change);
    msleep(100);
    
    // Raising up for first sense/pick up
    set_servo_position(wristPin, wristUp);
    moveArm(armUp);
   	move(150, 50, change);
    if(analog(etPin) > 3000) {
		// First one is burning
        burningBuilding = 0;
    }
    
    // Moving 
    
    // Raising up for second sense/pick up
    
   	
   	
    
    create_disconnect();
    
    return 0;
}
