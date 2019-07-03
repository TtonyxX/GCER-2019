#include <kipr/botball.h>
#include <drive.h>
#include <stdio.h>
#include <stdbool.h>

int camData[4];
int burning_building = 0; //close building is 0, further building is 1 
int timeout; //if camera does not sense burning timeout will be set to 1
int counter = 0; 
#define burning 0
#define claw 0
#define arm 1
#define arm_start 550
#define arm_down 1415
#define arm_down_people 1360
#define arm_up 800
#define arm_mid 700
#define claw_open_people 500
#define claw_open_firetruck 1050
#define claw_open_medical 800
#define claw_open_fireman 1000
#define claw_dropoff 600
#define claw_close 100
#define claw_start 350
#define left_light 0
#define right_light 1
#define black 3900
#define white 2500
#define grey 3700
#define THRESHOLD 3870 //need to change to 3600 during the day, 3800 at night

int left_motor = 1;
int right_motor = 0;

double gyrocalibrate() {
    double changeGyroZ = 0;//find average gyro value when still
    int i = 0;
    for(i = 0; i < 2000; i++) {
        changeGyroZ += gyro_z();
    }
    changeGyroZ /= 2000;

    return changeGyroZ;
}

/*
void move(int speed, int time, int changeGyroZ) {//speed -100 to 100, time is in miliseconds
    int i = 0;
    int anglechange = 0;
    for(i = 0; i < time; i++) {//gyro move
        anglechange += gyro_z()-changeGyroZ;
        motor(MOT_LEFT, speed+anglechange/300);
        motor(MOT_RIGHT, speed-anglechange/300);
        msleep(1);
    }
    ao();
}*/

void move(int speed, int time, double changeGyroZ) {//speed -1500 to 1500, time is in miliseconds
    int i = 0;
    double anglechange = 0;
    for(i = 0; i < time; i++) {//gyro move
        anglechange += gyro_z()-changeGyroZ;
        mav(MOT_LEFT, speed+anglechange/10);
        mav(MOT_RIGHT, speed-anglechange/10);
        msleep(1);
    }
    ao();
}


/*void turn(int direction, int degree, int changeGyroZ) {
    int i = 0;
    int anglechange = 0;
    int x = degree*57000/90;
    if(direction == 1) { // 1 = counterclockwise, 0 = clockwise
        for(i = 0; i < 100; i++) {
            motor(0, 100);
            motor(1, -100);
            msleep(1);
        }
        while(anglechange > -x) {
            anglechange += gyro_z()-changeGyroZ;
            motor(0, 100);
            motor(1, -100);
            msleep(1);
        }
    } else {
        for(i = 0; i < 100; i++) {
            motor(0, -100);
            motor(1, 100);
            msleep(1);
        }
        while(anglechange < x) {
            anglechange += gyro_z()-changeGyroZ;
            motor(0, -100);
            motor(1, 100);
            msleep(1);
        }
    }
    ao();
}
*/
void turn(int direction, int degree, int changeGyroZ) {
    double anglechange = 0;
    int x = degree*270000/90;
    x /= 10;
    if(direction == 1) { // 1 = left, 0 = right
        while(anglechange > -x) {
            anglechange -= gyro_z()-changeGyroZ;
            motor(0, 100);
            motor(1, -100);
            msleep(10);
        }
    } else {
        while(anglechange < x) {
            anglechange -= gyro_z()-changeGyroZ;
            motor(0, -100);
            motor(1, 100);
            msleep(10);
        }
    }
    ao();
}

void updateCam(){
    camData[1] = camera_update();
    camData[2] = get_object_count(burning);
    if(camData[2] > 0){
        camData[3] = get_object_center_x(burning, camData[2]-1);
        camData[4] = get_object_center_y(burning, camData[2]-1);
        printf("Camera Update: %d, Objects: %d, x: %d, y: %d ", camData[1], camData[2], camData[3], camData[4]); 
    }
    else{
        printf("Camera Update: %d, Objects: %d ", camData[1], camData[2]);  
    }
}

void openCam(){
    int open = camera_open();
    if(open == 1){
        printf("Successfully opened\n");
    }
    else{
        printf("Failed to open\n");
        openCam();
    }
}

int findObject(){
    int i, objects, final, av = 0;  

    for(i = 1; i <= 35; i++){
        updateCam();
        printf("EMPTY\n");
    }

    for(i = 1; i <= 20; i++){
        updateCam();
        objects = camData[2];
        if(objects > 0){
            printf("Item detected\n");
            final++;
            av = av + objects;
        }
        else{
            printf("Item not detected\n");
        }
        msleep(20);
    }
    if(final > 0){
        av = av/final;
    }
    else{
        av = 0;
    }
    printf("Successes: %d, Average: %d\n", final, av);

    if(final >= 15){
        return 1;
    }
    else{
        return 0;
    }
}

void scanBuilding(){
    drive_clear();
    camera_open_black();
    timeout = 0;
    printf("scan start\n");
    while(get_object_count(burning) < 1 && timeout == 0){
        camera_update();
        //printf("burning: %d \n", get_object_count(burning)); 
        mav(MOT_LEFT, 950);
        mav(MOT_RIGHT, 900);
        msleep(100);
        if(gmpc(MOT_LEFT) > 1200){
            printf("scan time out\n");
            timeout = 1;
        }
    }
    ao();
    if(gmpc(MOT_LEFT) < 1200){
        burning_building = 0; 
        printf("burning building is close\n");
    }
    else{
        burning_building = 1;
        printf("burning building is far\n");
    }
    printf("left motor: %d", gmpc(MOT_LEFT));
    printf("right motor: %d", gmpc(MOT_RIGHT));
    camera_close();
    printf("scan done\n"); 
}

void line_follow(int dist, int speed) { 
    drive(speed, speed);
    long leftTarg = gmpc(MOT_LEFT) + 150 * dist;
    long rightTarg = gmpc(MOT_RIGHT) + 150 * dist;
    while (gmpc(MOT_RIGHT) < rightTarg || gmpc(MOT_LEFT) < leftTarg) {
        if (analog(left_light) > black)
            drive(speed * .75, speed * 1.25);
        else if (analog(right_light) > black)
            drive(speed * 1.25, speed * .75);
        else
            drive(speed, speed);
        msleep(1);
    }
    ao();
}

void line_follow_middle_two(int dist, int speed) {  
    drive(speed, speed);
    long leftTarg = gmpc(MOT_LEFT) + 150 * dist;
    long rightTarg = gmpc(MOT_RIGHT) + 150 * dist;
    while (gmpc(MOT_RIGHT) < rightTarg || gmpc(MOT_LEFT) < leftTarg) {
        if (analog(left_light) > black)
            drive(speed * .85, speed * 1.15);
        else if (analog(right_light) > grey)
            drive(speed * 1.15, speed * .85);
        else
            drive(speed, speed);
        msleep(1);
    }
    ao();
}

void line_follow_middle(int dist, int speed) {  
    drive(speed, speed);
    long leftTarg = gmpc(MOT_LEFT) + 150 * dist;
    long rightTarg = gmpc(MOT_RIGHT) + 150 * dist;
    while (gmpc(MOT_RIGHT) < rightTarg || gmpc(MOT_LEFT) < leftTarg) {
        if (analog(left_light) > grey)
            drive(speed * .85, speed * 1.15);
        else if (analog(right_light) > black)
            drive(speed * 1.15, speed * .85);
        else
            drive(speed, speed);
        msleep(1);
    }
    ao();
}

void line_sense(int speed) {    
    drive(speed * SPD_L_F / MAX_SPEED, speed * SPD_R_F / MAX_SPEED);
    while (analog(left_light) < black || analog(right_light) < black) {
        if (analog(right_light) > black)
            drive(speed * SPD_L_F / MAX_SPEED, 0);
        else if (analog(left_light) > black)
            drive(0, speed * SPD_R_F / MAX_SPEED);
        else if (analog(left_light) <= black && analog(right_light) <= black)
            drive(speed, speed);
        msleep(1);
    }
    printf("black line sensed\n");
    ao();
}

void line_sense_grey(int speed) {   
    drive(speed * SPD_L_F / MAX_SPEED, speed * SPD_R_F / MAX_SPEED);
    while (analog(left_light) < grey || analog(right_light) < grey) {
        if (analog(right_light) > grey){
            drive(speed * SPD_L_F / MAX_SPEED, 0);
        }
        else if (analog(left_light) > grey){
            drive(0, speed * SPD_R_F / MAX_SPEED);
        }    
        else if (analog(left_light) <= grey && analog(right_light) <= grey){
            drive(speed, speed);
        }
        msleep(1);
    }
    printf("grey line sensed\n");
    ao();
}

void slow_servo(int port, int pos) {

    while(get_servo_position(port) > pos) {
        set_servo_position(port, get_servo_position(port)-10);
        msleep(8);
    }

    while(get_servo_position(port) < pos) {
        set_servo_position(port, get_servo_position(port)+10);
        msleep(8);
    }
}

/*void slow_servo(int port, int start, int end) {
    int i = 0;
    for(i; i < 30; i++) {
        start += (end - start)/30;
        set_servo_position(port, start);        
        msleep(100);
    }
}
*/
void turn_left(int speed, int time){
    mav(MOT_LEFT, -speed);
    mav(MOT_RIGHT, speed);
    msleep(time);
    ao();
}

void turn_right(int speed, int time){
    mav(MOT_LEFT, speed);
    mav(MOT_RIGHT, -speed);
    msleep(time);
    ao();
}

void turn_left90(){
    mav(MOT_LEFT, -1500);
    mav(MOT_RIGHT, 1500);
    msleep(575);
    ao();
}

void turn_right90(){
    mav(MOT_LEFT, 1500);
    mav(MOT_RIGHT, -1500);
    msleep(615);
    ao();
}

void get_people(double change, int claw_open, int first) {
    set_servo_position(arm, arm_down_people);
    msleep(500);
    move(1500, 200, change);
    msleep(500);
    set_servo_position(claw, claw_open);
    msleep(300);
    move(1500, 60, change);
    msleep(300);
    set_servo_position(claw, claw_open+70);
    msleep(300);
    move(1500, 110, change);
    msleep(400);
    set_servo_position(arm, arm_down);
    slow_servo(claw, claw_close);
    msleep(200);
    move(-1500, 560, change);
    turn(1, 90, change);
    if(first==0) {
        slow_servo(claw, claw_open+400);
    }
    msleep(500);
    while(analog(left_light) < black){
        line_follow_middle(1, 750);
    } 
    msleep(200);
    if(burning_building == 1) {
        move(-1500, 30, change);
        if(counter==3){}
        else{
            set_servo_position(claw, claw_open_fireman);
            msleep(100);
            slow_servo(arm, arm_up);
            counter++;
        }
    } 
    else if(burning_building == 0){
        move(-1500, 100, change); 
        msleep(200); 
        turn(0, 75, change);
        msleep(500);
        move(1500, 380, change);
        msleep(100);
        if(counter==3){}
        else {
        	move(-1500, 380, change);
            msleep(300);
            set_servo_position(claw, claw_open_fireman);
            msleep(100);
            turn(1, 75, change);
            msleep(100);
            slow_servo(arm, arm_up);
            counter++;
        }
    }
    

}

void get_people_middle(int change, int claw_open) {
    set_servo_position(arm, arm_down_people);
    msleep(500);
    move(1500, 200, change);
    msleep(500);
    set_servo_position(claw, claw_open);
    msleep(300);
    move(1500, 60, change);
    msleep(300);
    set_servo_position(claw, claw_open+130);
    msleep(300);
    move(1500, 90, change);
    msleep(400);
    set_servo_position(arm, arm_down);
    slow_servo(claw, claw_close);
    msleep(200);
    move(-1500, 550, change);
    turn(1, 90, change);
    msleep(400);
    slow_servo(claw, claw_open);
    slow_servo(arm, arm_up);
}

int main(){
    //wallaby starts in the left corner against the back with claw closed facing towards create
    //servo starting positions
    set_servo_position(claw, claw_start);
    set_servo_position(arm, arm_start);
    double change = gyrocalibrate();
    enable_servos();
    msleep(2000);
    shut_down_in(119);
    //people and ambulance go on ground of nonburning building, medical supplies go on top of nonburning building
    //firetruck goes on ground of burning building, firefighter goes on top of burning building 
    //turn_left90();

    // Test

    set_servo_position(arm, arm_up);
    msleep(200);
    move(1500, 100, change);
    msleep(200);
    turn(1, 110, change);
    msleep(200);
    while(analog(right_light) < black){
        move(1500, 10, change);
    } 
    msleep(200);
    move(-1500, 585, change);
    turn(0, 110, change);
    msleep(200);
    line_sense(-700);

    // Goes after the first people
    get_people(change, claw_open_people, 1);

    // Goes towards the second building
    msleep(1000);
    turn(0, 270, change);
    msleep(200);
    line_follow_middle_two(42, 1200);
    msleep(100);
    turn(1, 110, change);
    msleep(100);
    move(1500, 100, change);
    msleep(100);
    line_sense(-700);
    msleep(100);

    // Goes after the second people
    get_people_middle(change, claw_open_people-30);
    
    // Goes towards the third building
    msleep(1000);
    turn(0, 250, change);
    msleep(200);
    line_follow_middle_two(23, 1200);
    msleep(100);
    turn(1, 110, change);
    msleep(100);
    move(1500, 100, change);
    msleep(100);
    line_sense(-700);
    msleep(100);

    // Goes after the third people
    get_people_middle(change, claw_open_people-50);
    
    // Goes towards the fourth building
    msleep(1000);
    turn(0, 260, change);
    msleep(200);
    line_follow_middle_two(20, 1200);
    msleep(100);
    turn(1, 110, change);
    msleep(100);
    move(1500, 100, change);
    msleep(100);
    line_sense(-700);
    msleep(100);

    // Goes after the fourth people
    get_people(change, claw_open_people-40, 0);

    msleep(100000);
    // End Test

    move(-1500, 25, change);
    turn(1, 90, change); 
    msleep(500); 
    move(-1500, 400, change); //square up
    set_servo_position(arm, arm_down_people);
    msleep(1000);
    move(1500, 300, change);
    set_servo_position(claw, claw_open_medical);
    msleep(300);
    move(1500, 300, change);
    line_sense(500);
    //move(1500, 10, change); 
    //get medical supplies 
    //set_servo_position(claw, claw_close); //grabs medical supplies 1
    msleep(500);
    //set_servo_position(arm, arm_mid);
    move(1500, 50, change); 
    turn(1, 90, change);
    msleep(500);
    set_servo_position(arm, arm_down);
    msleep(300);
    slow_servo(claw, claw_open_medical);
    line_follow_middle(40, 1200); 
    slow_servo(claw, claw_close);
    while(analog(left_light) < black){
        line_follow_middle(1, 750); 
    }
    move(-1500, 50, change); 
    //set_servo_position(arm, arm_mid);
    msleep(300);
    //turn_right90(); //turns right onto the black line by the building 
    turn(0, 90, change);
    msleep(500);
    //move(-1500, 175, change);
    //line_sense(-500); //backs up to prepare for camera 
    move(-1500, 75, change); 
    scanBuilding(); 
    //drop off medical supplies 
    if(burning_building == 1){
        msleep(500);
        turn(1, 90, change);//turns to face nonburning building
        msleep(500);
        move(1500, 50, change); 
        slow_servo(claw, claw_open_medical); //drops off medical supplies 
        msleep(500); 
        move(-1500, 150, change);
        msleep(500);
        slow_servo(arm, arm_up); 
        msleep(500); 
        turn(0, 90, change);
        msleep(500);
        line_sense(-500);//square on middle line
        msleep(500); 

    }
    else if(burning_building == 0){
        move(1500, 620, change); //moves forward to burning building 
        move(-1500, 25, change); 
        msleep(500);				
        //move(1500, 50, change);
        turn_left(1500, 300); 
        msleep(500);
        move(1500, 100, change); 
        slow_servo(claw, claw_open_medical); //drops off medical supplies 
        //set_servo_position(claw, claw_close);
        msleep(500);
        move(-1500, 200, change);
        msleep(200);
        slow_servo(arm, arm_up);
        msleep(200);
        turn_right(1500, 300);
        msleep(500);
        line_sense(-750); 
        msleep(500);

    }

    //get ambulance
    move(-1500, 1200, change); //square up
    move(1500, 500, change);
    turn(0, 90, change);
    msleep(500);
    set_servo_position(arm, arm_down);
    msleep(500);
    move(1500, 200, change);
    //turn_right90();
    msleep(200);
    set_servo_position(claw, claw_open_fireman); 
    msleep(200);
    move(1500, 200, change);
    msleep(500); 
    set_servo_position(claw, claw_close);
    msleep(500); 
    move(-1500, 150, change); 
    msleep(200);
    turn(1, 90, change);
    msleep(500);
    move(1500, 200, change);
    line_sense(500);
    //drop off ambulance
    if(burning_building == 1){
        move(1500, 40, change); 
        msleep(200);
        turn(1, 90, change);
        msleep(500);
        line_follow_middle(20, 1000);
        msleep(200);
        while(analog(left_light) < black){
            line_follow_middle(1, 750); 
        }
        slow_servo(claw, claw_dropoff); //drops off ambulance
        msleep(500);
        move(-1500, 450, change);
        msleep(200);
        //get firetruck 
        turn(1, 10, change);
        msleep(500);
        set_servo_position(claw, claw_open_firetruck); 
        move(1500, 600, change);
        msleep(200);
        set_servo_position(claw, claw_close); //grabs firetruck 
        msleep(200);
        move(-1500, 300, change);
        msleep(200);
        turn(1, 80, change);
        msleep(200);
        turn(1, 90, change); 
        msleep(200);
        move(1500, 50, change);
        msleep(500);
        turn(1, 90, change); 
        msleep(500);
    }
    else if(burning_building == 0){
        move(1500, 200, change);
        msleep(200);
        turn(1, 45, change);
        msleep(500);
        move(1500, 500, change); 
        slow_servo(claw, claw_dropoff); //drops off ambulance 
        move(-1500, 600, change);
        turn(0, 40, change);
        line_sense(-1000); //squares up on middle line 
        msleep(200);
        turn(1, 90, change);
        msleep(500);
        turn(1, 30, change);
        msleep(200);
        set_servo_position(claw, claw_open_firetruck); 
        move(1500, 600, change); 
        msleep(200);
        set_servo_position(claw, claw_close); //grabs firetruck 
        msleep(500);
    }
    //drop off firetruck 
    if(burning_building == 1){
        line_sense(1000); //moves to middle line 
        //move(1500, 350, change);
        msleep(200);
        turn(1, 30, change);
        msleep(200);
        move(1500, 550, change);
        set_servo_position(claw, claw_dropoff+100); //drops off firetruck 
        msleep(200);
        move(-1500, 200, change);
        msleep(200);
        turn(0, 30, change);
        line_sense(-1000); 
    }
    else if(burning_building == 0){
        move(-1500, 300, change);
        msleep(200);
        turn(1, 30, change); 
        line_sense(-750);
        move(1500, 25, change); 
        msleep(200);
        turn(0, 90, change);
        msleep(500);
        line_follow_middle(25, 1000);
        while(analog(left_light) < black){
            line_follow_middle(1, 750); 
        }
        set_servo_position(claw, claw_dropoff+100); 
        move(-1500, 500, change);
        turn(0, 90, change);
        line_sense(-500);

    }

    //get people 
    msleep(200);
    move(1500, 200, change);
    msleep(200);
    turn(1, 90, change);
    msleep(200);
    while(analog(right_light) < black){
        line_follow_middle(0, 750); 
    } 
    msleep(200);



    return 0;
}
