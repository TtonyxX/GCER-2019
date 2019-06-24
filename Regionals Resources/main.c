#include <kipr/botball.h>
#include <drive.h>

//analog ports
#define lightL 0
#define lightR 1

//digital ports
#define touchL 0
#define touchR 1

//servo ports
#define claw 0
#define food 1
#define spinner 2
#define grabber 3 

//claw pos values 
#define claw_down 1600
#define claw_up 600
#define spinner_up 1
#define spinner_down 1
#define grabber_open 1 
#define grabber_close 1
//need values for food supply effectors 

//light sensor values
#define black 4000
#define white 2200
#define blue 2700
#define grey 3300

int main()
{
    set_servo_position(claw, claw_up); //claw starts up 
    set_servo_position(spinner, spinner_down); //spinner starts down 
    enable_servos();
    set_servo_position(claw,claw_down); //put claw down 
    msleep(1000);
    forward(15); //move forward out of starting box
    line_sense(50); //go forward until sensing black line in middle 
    left(90,0); //turn left onto middle line 
    line_follow_special(100, 100); //line follow middle line 
    left(90, 0); //turns so it can square up 
    square_up(100); //squares up with the pipe adjacent to the sky scrapers 
    line_sense(50); //line sense to black line 
    left(90,0); //turn left to face blue poms on blue tape
    line_sense_blue(50); //goes forward until it senses the blue tape 
    line_follow_blue(200,100); //line follows on the blue tape so it can collect blue poms 
    forward(50); //go foward to avoid food supplies 
    right(90, 0); //turn right
    line_sense(100); //move forward to black line 
    forward(20);
    line_sense(100); //move forward to second black line adjacent to starting box 
    right(90,0);
    line_sense(100); //moves to starting box 
    forward(25); //puts blue poms in starting box in the middle 
    backward(50); //backs out of starting box and squares up with pipe 
    square_up(100); 
    right(90,0); 
    line_sense(100); //turns right and goes until it sees middle black line 
    left(90, 0);
    line_follow_special(15, 100); //line follows middle line 
    get_food_supplies(); //gets food supplies 
    left(90, 0); 
    forward(30); 
    right(90,0); //turns toward starting box 
    line_sense(100); 
    forward(20); //puts food supplies in front of blue poms in middle of starting box stacked 
	backward(40); 
    right(90,0);
    line_sense(100);
    left(90,0); //goes forward to the middle line and turns onto it so it can line follow 
    line_follow_special(200,100); //bulldozes the rest of the food supplies 
    left(90,0);
    line_sense(100);
    left(90,0);
    forward(15);
    
    
    
    disable_servos();

    ao();
    return 0;
}

void get_food_supplies(){
     set_servo_position(grabber, grabber_close);
     set_servo_position(spinner, spinner_up);
}


void square_up(int speed){
    while(touchL == 1 || touchR == 1){
        backward(1);
        if(touchR == 1){
            drive(speed * SPD_L_F / MAX_SPEED, 0);

        }
        else if (touchL == 1){
            drive(0, speed * SPD_R_F / MAX_SPEED);

        }
    }
}

void slow_servo(int start, int servo, int end, int time){ //time is in MS
    set_servo_position(servo, start);


    int difference = end - start;
    float timeper = difference/(time/1000.0);
    printf("timeper: %f", timeper);
    int i;
    for (i = 1; i <= (time/1000); i++){
        set_servo_position(servo, start+i*timeper);
        msleep(1000);
    }
}

void line_sense(int speed) {
    drive(speed * SPD_L_F / MAX_SPEED, speed * SPD_R_F / MAX_SPEED);
    while (analog(lightL) < black || analog(lightR) < black) {
        if (analog(lightR) > black)
            drive(speed * SPD_L_F / MAX_SPEED, 0);
        else if (analog(lightL) > black)
            drive(0, speed * SPD_R_F / MAX_SPEED);
        else if (analog(lightL) <= black && analog(lightR) <= black)
            drive(speed, speed);
        msleep(1);
    }
    ao();
}

void line_sense_blue(int speed) {
    drive(speed * SPD_L_F / MAX_SPEED, speed * SPD_R_F / MAX_SPEED);
    while (analog(lightL) < blue || analog(lightR) < blue) {
        if (analog(lightR) > blue)
            drive(speed * SPD_L_F / MAX_SPEED, 0);
        else if (analog(lightL) > blue)
            drive(0, speed * SPD_R_F / MAX_SPEED);
        else if (analog(lightL) <= blue && analog(lightR) <= blue)
            drive(speed, speed);
        msleep(1);
    }
    ao();
}

void line_follow(int dist, int speed) {
    drive(speed, speed);
    long leftTarg = gmpc(MOT_LEFT) + 150 * dist;
    long rightTarg = gmpc(MOT_RIGHT) + 150 * dist;
    while (gmpc(MOT_RIGHT) < rightTarg || gmpc(MOT_LEFT) < leftTarg) {
        if (analog(lightL) > black)
            drive(speed * .75, speed * 1.25);
        else if (analog(lightR) > black)
            drive(speed * 1.25, speed * .75);
        else
            drive(speed, speed);
        msleep(1);
    }
    ao();
}

void line_follow_special(int dist, int speed){
    drive(speed, speed);
    long leftTarg = gmpc(MOT_LEFT) + 150 * dist;
    long rightTarg = gmpc(MOT_RIGHT) + 150 * dist;
    while (gmpc(MOT_RIGHT) < rightTarg || gmpc(MOT_LEFT) < leftTarg) {
        if (analog(lightL) > grey)
            drive(speed * .75, speed * 1.25);
        else if (analog(lightR) > black)
            drive(speed * 1.25, speed * .75);
        else
            drive(speed, speed);
        msleep(1);
    }
    ao();
}

void line_follow_blue(int dist, int speed) {
    drive(speed, speed);
    long leftTarg = gmpc(MOT_LEFT) + 150 * dist;
    long rightTarg = gmpc(MOT_RIGHT) + 150 * dist;
    while (gmpc(MOT_RIGHT) < rightTarg || gmpc(MOT_LEFT) < leftTarg) {
        if (analog(lightL) > blue)
            drive(speed * .75, speed * 1.25);
        else if (analog(lightR) > blue)
            drive(speed * 1.25, speed * .75);
        else
            drive(speed, speed);
        msleep(1);
    }
    ao();
}


