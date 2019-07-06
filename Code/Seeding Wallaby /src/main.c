#include <kipr/botball.h>
#include <drive.h>

int camData[4];
int burning_building = 1; //close building is 0, further building is 1 
int timeout; //if camera does not sense burning timeout will be set to 1
int counter; 
#define yellow 0
#define red 1
#define burning 0
#define claw 0
#define arm 1
#define light 2
#define arm_start 400
#define arm_down 1415
#define arm_down_people 1320
#define arm_up 800
#define arm_mid 700
#define claw_open_people 945
#define claw_open_firetruck 1400
#define claw_open_medical 1500
#define claw_open_fireman 1700
#define claw_dropoff 1300
#define claw_close 800
#define claw_start 950
#define left_light 0
#define right_light 1
#define black 3900
#define white 2500
#define grey 3600
#define tv 90  //in degrees, adjust before running

double gyrocalibrate() {
    double changeGyroZ = 0;//find average gyro value when still
    int i = 0;
    for(i = 0; i < 2000; i++) {
        changeGyroZ += gyro_z();
    }
    changeGyroZ /= 2000;

    return changeGyroZ;
}

void waitButton() {
    while(right_button() == 0) {
        msleep(2);
    }
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
    int open = camera_open_black();
    if(open == 1){
        printf("Successfully opened\n");
    }
    else{
        printf("Failed to open\n");
        //openCam();
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
        if (analog(left_light) < black)
            drive(speed * 1.15, speed * .85);
        else if (analog(right_light) > grey)
            drive(speed * .85, speed * 1.15);
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
        set_servo_position(port, get_servo_position(port)-5);
        msleep(8);
    }

    while(get_servo_position(port) < pos) {
        set_servo_position(port, get_servo_position(port)+5);
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

void light_start () {
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
    curr = analog(LIGHT);

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

  while (analog(LIGHT) > avg && !(right_button())) msleep(50);
}

void get_people(double change, int claw_open, int first) {
    set_servo_position(arm, arm_down_people);
    set_servo_position(claw, claw_open);
    move(1000, 600, change);
    set_servo_position(claw, claw_open+500);
    set_servo_position(arm, arm_down);
    msleep(200);
    move(1500, 130, change);
    slow_servo(claw, claw_close);
    msleep(200);
    if(first==1){
        move(1500, 100, change);
        msleep(6000); 
    }

    move(-1500, 500, change);
    line_sense(-1000);
    turn(1, tv, change);
    msleep(500);

    if(first==0) {
        set_servo_position(claw, claw_open+400);
    }
    else{
        set_servo_position(claw, claw_open_fireman);
        line_follow_middle(10, 1200); 
    }
    if(burning_building == 1) {
        while(analog(left_light) < black){
            line_follow_middle(1, 1200);
        } 

        move(1500, 125, change);         
        msleep(200);
        move(-1500, 125, change);
        if(counter==3){}
        else{
            set_servo_position(arm, arm_up);
            set_servo_position(claw, claw_close);
            counter++;
        }
    } 
    else if(burning_building == 0){
        turn(0, tv/3, change);
        msleep(200); 
        move(1500, 425, change); 
        msleep(100);
        if(counter==3){}
        else{  
            move(-1500, 375, change);
            turn(1, tv/3, change);
            msleep(400);
            set_servo_position(arm, arm_up);
            set_servo_position(claw, claw_close); 
            counter++;
        }
    }
}

void get_people_middle(int change, int claw_open) {
    set_servo_position(arm, arm_down_people);
    msleep(100);
    move(1500, 350, change);
    set_servo_position(claw, claw_open+500);
    set_servo_position(arm, arm_down);
    move(1500, 25, change);
    move(1500, 200, change);
    msleep(100);
    set_servo_position(claw, claw_close);
    if(counter == 1){
        move(1500, 100, change);
        msleep(20000);
    }
    move(-1500, 450, change);
    line_sense(-1200);
    turn(1, tv, change);
    msleep(500);
    set_servo_position(claw, claw_open);
    set_servo_position(arm, arm_up);
    counter++;
}

void get_people_main(int change){
    set_servo_position(arm, arm_up);
    msleep(100);
    move(1500, 360, change);
    turn(1, tv-10, change);
    msleep(500);
    move(1500, 75, change); 
    line_sense(1200);
    move(-1500, 450, change);
    turn(0, tv, change);
    msleep(500);
    line_sense(-1200);

    //goes after the first people
    get_people(change, claw_open_people, 1);

    //goes towards the second building
    turn(0, tv, change);
    msleep(500); 
    move(1500, 15, change); 
    line_sense(-1200);
    turn(0, tv-15, change);
    msleep(500);
    line_follow_middle_two(42, 1200);
    msleep(200);
    turn(1, tv, change);
    msleep(500);
    move(1500, 100, change);
    msleep(100);
    line_sense(-1200);

    //goes after the second people
    get_people_middle(change, claw_open_people-125);

    //goes towards the third building
    turn(0, tv, change);
    msleep(500);
    move(1500, 30, change);
    line_sense_grey(-1200);
    turn(0, tv, change);
    msleep(500);
    line_follow_middle_two(25, 1200);
    turn(1, tv, change);
    msleep(500);
    move(1500, 100, change);
    line_sense(-1200);

    //goes after the third people
    get_people_middle(change, claw_open_people-125);
	/*
    //goes towards the fourth building
    turn(0, tv, change);
    msleep(500);
    move(1500, 30, change);
    line_sense_grey(-1200);
    turn(0, tv, change);
    msleep(500);
    line_follow_middle_two(22, 1200);
    turn(1, tv, change);
    msleep(500);
    move(1500, 100, change);
    line_sense(-1200);
	*/
    //goes after the fourth people
    get_people(change, claw_open_people-75, 0);
    line_follow_middle(75, 1200); 
    if(burning_building == 1){
        while(analog(left_light) < black){
            line_follow_middle(1, 1200);
        } 
        move(1500, 50, change);
        set_servo_position(claw, claw_close+300);
    }
    else if(burning_building == 0){
        while(analog(left_light) < black){
            line_follow_middle(1, 1200);
        } 
        turn(0, tv/3, change); 
        msleep(200);
        move(1500, 250, change); 
    }

}

void scanBuilding(double change){
    drive_clear();
     while(analog(left_light) < black){
        line_follow_middle(1, 1200);   
        //msleep(10);
    }
    camera_open_black();  
    printf("scan start\n");
    camera_update();
    msleep(50);
    camera_update();
    msleep(50);
    camera_update();
    msleep(50);
    printf("yellow objects: %d \n", get_object_count(yellow));
    printf("red objects: %d \n", get_object_count(red));
    int i;
    for(i = get_object_count(yellow); i > 0; i--){
        printf("yellow object: %d \n", i);
        printf("yellow object area: %d \n", get_object_area(yellow, i));  
        if(get_object_area(yellow, i) > 600 && get_object_area(yellow, i) < 1600){
            burning_building = 0; 
            printf("burning building is close\n");
        }
    }
    printf("burning building is: %d ", burning_building); 
    camera_close();
    printf("scan done\n");

}

int main(){
    //wallaby starts in the left corner against the back with claw closed facing towards create
    //servo starting positions
    set_servo_position(claw, claw_start);
    set_servo_position(arm, arm_start);
    enable_servos(); 
    counter = 0;
    double change = gyrocalibrate();
    //msleep(1000);
    //test get_people_main(change);
    waitButton();
    shut_down_in(119);
    //people and ambulance go on ground of nonburning building, medical supplies go on top of nonburning building
    //firetruck goes on ground of burning building, firefighter goes on top of burning building 
    move(-1500, 25, change);
    turn(1, tv, change); 
    msleep(500); 
    move(-1500, 200, change); //square up
    set_servo_position(claw, claw_open_medical);
    set_servo_position(arm, arm_down_people+75);
    move(1500, 600, change);
    line_sense(1200);
    //get medical supplies and ambulance 
    move(1500, 35, change);
    //msleep(200);
    //slow_servo(claw, claw_close+350); 
    turn(1, tv, change);
    msleep(500);
    set_servo_position(claw, claw_open_medical);
    msleep(200);
    line_follow_middle(35, 1200); 
    msleep(500);
    scanBuilding(change); 
    //drop off medical supplies  
    if(burning_building == 1){
        move(1500, 50, change); 
        set_servo_position(claw, claw_open_medical); //drops off medical supplies and ambulance 
        msleep(500); 
        slow_servo(arm, arm_up); 
        msleep(200);
        move(-1500, 150, change);
        msleep(500);
        turn(0, tv, change);
        msleep(500);
        line_sense(-1000);//square on middle line
        set_servo_position(arm, arm_down_people + 100);
    }
    else if(burning_building == 0){
        set_servo_position(claw, claw_close+300); 
        msleep(200); 
        turn(0, tv/2, change); 
        msleep(500);
        move(1500, 350, change); //moves forward to burning building 
        move(-1500, 50, change); 
        msleep(500);                
        slow_servo(claw, claw_open_medical); //drops off medical supplies and ambulance 
        slow_servo(arm, arm_up); 
        move(-1500, 100, change); 
        line_sense(-1000); 
    }
    //grabs firetruck 
    set_servo_position(arm, arm_down);
    msleep(100);
    turn(0, tv, change);
    msleep(400);
    turn(0, tv, change);
    msleep(400);
    turn(0, tv/3, change);
    msleep(200);
    move(1500, 400, change); 
    slow_servo(claw, claw_close);
    msleep(100);
    move(-1500, 450, change); 
    turn(1, tv/3, change); 
    msleep(200);
    line_sense(-1200);
    //drop off firetruck 
    if(burning_building == 1){
        turn(1, tv, change); 
        msleep(400); 
        turn(1, tv, change);
        msleep(400); 
        turn(1, tv/3, change); 
        msleep(200);
        move(1500, 425, change); 
        set_servo_position(claw, claw_dropoff+100); //drops off firetruck 
        msleep(100);
        move(-1500, 375, change); 
        turn(0, tv/3, change); 
        line_sense(-1200); 
    }else if(burning_building == 0){
        turn(0, tv, change); 
        msleep(400); 
        /*while(analog(left_light) < black){
            line_follow_middle(1, 1200); 
        }*/
        move(1500, 50, change);
        set_servo_position(claw, claw_dropoff+100); //drops off firetruck 
        msleep(100);
        move(-1500, 500, change);
        turn(0, tv, change);
        move(1500, 25, change); 
        line_sense(-1200);
    }

    //get people 
    get_people_main(change);


    return 0;
}
