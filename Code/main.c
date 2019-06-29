#include <kipr/botball.h>
#include <drive.h>

int camData[4];
int burning_building; //close building is 0, further building is 1 
int timeout; //if camera does not sense burning timeout will be set to 1
#define burning 0
#define claw 0
#define arm 1
#define arm_start 2000
#define arm_down 0
#define arm_up 2047
#define arm_mid 700
#define claw_open_people 530
#define claw_open_firetruck 950
#define claw_open_medical 850
#define claw_open_fireman 1000
#define claw_close 100
#define claw_start 350
#define left_light 0
#define right_light 1
#define black 3900
#define white 2500
#define grey 3400

int gyrocalibrate() {
    int changeGyroZ = 0;//find average gyro value when still
    int i = 0;
    for(i = 0; i < 5000; i++) {
        changeGyroZ += gyro_z();
    }
    changeGyroZ /= 5000;

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

void move(int speed, int time, int changeGyroZ) {//speed -1500 to 1500, time is in miliseconds
    int i = 0;
    int anglechange = 0;
    for(i = 0; i < time; i++) {//gyro move
        anglechange += gyro_z()-changeGyroZ;
        mav(MOT_LEFT, speed+anglechange/10);
        mav(MOT_RIGHT, speed-anglechange/10);
        msleep(1);
    }
    ao();
}


void turn(int direction, int degree, int changeGyroZ) {
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

void turn90(int direction, int changeGyroZ) {
    int anglechange = 0;
    int x = 90*270000/90;
    x /= 50;
    if(direction == 1) { // 1 = left, 0 = right
        while(anglechange > -x) {
            anglechange -= gyro_z()-changeGyroZ;
            motor(0, 100);
            motor(1, -100);
            msleep(50);
        }
    } else {
        while(anglechange < x) {
            anglechange -= gyro_z()-changeGyroZ;
            motor(0, -100);
            motor(1, 100);
            msleep(50);
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
        if(gmpc(MOT_LEFT) > 2200){
            printf("scan time out\n");
            timeout = 1;
        }
    }
    ao();
    if(gmpc(MOT_LEFT) < 2200){
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
    msleep(615);
    ao();
}

void turn_right90(){
    mav(MOT_LEFT, 1500);
    mav(MOT_RIGHT, -1500);
    msleep(615);
    ao();
}

int main(){
    //wallaby starts in the left corner against the back with claw closed facing towards create
    //servo starting positions
    set_servo_position(claw, claw_start);
    set_servo_position(arm, arm_start);
    int change = gyrocalibrate();
    enable_servos();
    msleep(1000);
    shut_down_in(119);

    //people and ambulance go on ground of nonburning building, medical supplies go on top of nonburning building
    //firetruck goes on ground of burning building, firefighter goes on top of burning building 
    turn_left90();
    //left_turn(1500, 90); 
    msleep(300); 
    move(-1500, 400, change); //square up
    set_servo_position(claw, claw_open_medical);
    set_servo_position(arm, arm_down);
    msleep(300);
    move(1500, 500, change);
    line_sense_grey(500);
    move(-1500, 10, change); 
    //get medical supplies 
    set_servo_position(claw, claw_close); //grabs medical supplies 1
    msleep(500);
    set_servo_position(arm, arm_up);
    msleep(300);
    line_sense_grey(500); //get to middle line
    move(-1500, 200, change); 
    line_sense(-500);
    move(-1500, 50, change); 
    turn_left(1500, 570); 
    //left_turn(1500, 90);
    msleep(500);
    line_sense(500); 
    move(1500, 280, change);
    set_servo_position(arm, arm_down); 
    msleep(500);
    //right_turn(1500, 90);
    turn_right90();
    msleep(500);
    slow_servo(claw, claw_open_medical); 
    move(1500, 420, change); 
    msleep(500);
    slow_servo(claw, claw_close); //grabs medical supplies 2
    msleep(500);
    set_servo_position(arm, arm_up);
    msleep(500);
    move(-1500, 100, change); 
    line_sense_grey(1000); //squares up on line 
    move(1500, 150, change);
    //left_turn(1500, 90); //turns onto middle line 
    turn_left90();
    msleep(500); 
    line_follow_middle(25, 800); 
    move(1500, 100, change); 
    msleep(500);
    //right_turn(1500, 90); //turns right onto the black line by the building 
    turn_right90();
    msleep(500);
    //line_sense(-1000); //backs up to prepare for camera  
    move(-1500, 150, change);
    scanBuilding(); 
    //drop off medical supplies 
    if(burning_building == 1){
        move(-1500, 100, change); //backs up a little to get in front of nonburning building 
        msleep(500);
        //left_turn(1500, 90); 
        turn_left90();
        msleep(500);
        move(1500, 50, change);
        slow_servo(claw, claw_open_medical); //drops off medical supplies 
        msleep(1000);
        move(-1500, 150, change);
        //left_turn(1500, 90); //faces firetruck 
        turn_left90();
        msleep(500);
    }
    else if(burning_building == 0){
        move(1500, 775, change); //moves forward to burning building 
        move(-1500, 25, change); 
        msleep(500);
        //left_turn(1500, 90); 
        turn_left90(); 
        msleep(500);
        move(1500, 50, change);
        slow_servo(claw, claw_open_medical); //drops off medical supplies 
        msleep(1000);
        move(-1500, 150, change); 
        //left_turn(1500, 90); //faces firetruck
        turn_left90();
        msleep(500);
        //line_follow(70, 1500); 
        move(1500, 800, change); 
        msleep(500);
    }
    //get firetruck 
    set_servo_position(claw, claw_open_firetruck);  
    msleep(500); 
    set_servo_position(arm, arm_down); 
    move(1500, 200, change); 
    msleep(500);
    set_servo_position(claw, claw_close); //grabs firetruck 
    msleep(500); 
    //drop off firetruck
    if(burning_building == 0){
        move(-1500, 300, change); 
        //left_turn(1500, 90);
        turn_left90(); 
        msleep(500);
        //left_turn(1500, 90); //turns to face forward
        turn_left90();
        msleep(500);
        move(1500, 600, change); //moves forward to non burning building 
        turn_left(1500, 200); 
        msleep(200);
        slow_servo(claw, claw_open_firetruck);  
        turn_right(1500, 200);
        msleep(200);
        move(-1500, 1000, change);     
    }
    else if(burning_building == 1){
        move(-1500, 300, change); 
        //left_turn(1500, 90); 
        turn_left90();
        msleep(500);
        //left_turn(1500, 90); //turns to face forward 
        turn_left90();
        msleep(500);
        move(1500, 200, change); //moves to non burning building 
        turn_left(1500, 200); 
        msleep(200);
        slow_servo(claw, claw_open_firetruck);  
        turn_right(1500, 200);
        msleep(200);
        move(-1500, 300, change); 
    }
    //get fireman
    msleep(500);
    turn_right90();
    msleep(500);
    turn_right90();
    msleep(500);
    move(1500, 100, change);
    turn_right90();
    msleep(500); 
    set_servo_position(claw, claw_open_fireman); 
    move(1500, 50, change); 
    set_servo_position(claw, claw_close);
    msleep(500); 
    move(-1500, 150, change); 
    
    
    
    





    return 0;
}
