#include <kipr/botball.h>
#include <stdio.h>
#include <stdbool.h>
#define ET analog(3)
#define LIGHT_1 analog(1)
#define LIGHT_0 analog(0)
#define START 5
#define LEVER digital(0)
#define THRESHOLD 3870 //need to change to 3600 during the day, 3800 at night
#define OPEN 590
#define CLOSE 0
#define leftmotor 1
#define rightmotor 0
//#define SPEED 800
//#define TURNSPEED 700

double bias; //variable to hold the calibration value
int right_motor, left_motor;//ports
int counter = 0;
int i;

void snatch(){
    //surrounds the people
    drive_with_gyro(800, 2000.0);
    turn_with_gyro(-300, 300, 50000.0);
    drive_with_gyro(800, 1500.0);
    
    //closes claw
    set_servo_position(0, CLOSE);
    msleep(800);
    //back out until on black line
    drive_with_gyro(-800, 2300.0);
    turn_with_gyro(300, -300, 50000.0);
    gyro_light_stop(-800);
    drive_with_gyro(800, 400.0);

    counter++;

    if(counter < 4)
        square_up_black_line();
    //turn 
    //line follow black line
    //turn right towards next blue line (90 degrees)
    //forward until near blue line

}
void snatch_2(){
    //goes foward slightly
    drive_with_gyro(800, 2000.0);
    turn_with_gyro(-300, 300, 50000.0);
    drive_with_gyro(800, 1500.0);
    //closes claw
    set_servo_position(0, CLOSE);
    msleep(800);
    //back out until on black line
    drive_with_gyro(-800, 2100.0);
    turn_with_gyro(300, -300, 50000.0);
    gyro_light_stop(-800);
    drive_with_gyro(800, 400.0);

    counter++;

    if(counter < 4)
        square_up_black_line();
    //turn 
    //line follow black line
    //turn right towards next blue line (90 degrees)
    //forward until near blue line

}

void snatch_3(){
    //goes foward slightly
    drive_with_gyro(800, 2000.0);
    turn_with_gyro(-300, 300, 70000.0);
    drive_with_gyro(800, 1500.0);
    //closes claw
    set_servo_position(0, CLOSE);
    msleep(800);
    //back out until on black line
    drive_with_gyro(-800, 2100.0);
    turn_with_gyro(300, -300, 70000.0);
    gyro_light_stop(-800);
    drive_with_gyro(800, 400.0);

    counter++;

    if(counter < 4)
        square_up_black_line();
    //turn 
    //line follow black line
    //turn right towards next blue line (90 degrees)
    //forward until near blue line

}

int main()
{ 
    //wait_for_light(START);
	shut_down_in(119);
    declare_motors(1, 0);
    cmpc(0);
    cmpc(1);
    cg();
    enable_servos();
    set_servo_position(0, CLOSE); 

    //turn to be parallel with edge
    msleep(2000);
    turn_with_gyro(700, -700, 368000.0);
    //move forward a set distance until in front of first person line   
    drive_with_gyro(1110, 3400.0);
    //turn left then square up against back pvc pipe
    turn_with_gyro(-700, 700, 570000.0);
    drive_with_gyro(-1200, 2100.0);

    //move forward till black tape of black/grey line
    gyro_light_stop(1000);

    //move forward
    drive_with_gyro(800, 600.0);
    square_up_black_line();
    set_servo_position(0, OPEN - 20); //open arm
    turn_with_gyro(-700, 700, 10000.0);
    snatch(); //FIRST
    turn_with_gyro(-500, 500, 570000.0);

    drive_with_gyro(800, 4300.0);   //drive forward until second person line
    turn_with_gyro(500, -500, 540000.0);
    set_servo_position(0, OPEN - 50);
    
    drive_with_gyro(500, 300.0); //move forward
    square_up_black_line();
    turn_with_gyro(-300, 300, 9000.0);
    snatch_2(); //SECOND 
    turn_with_gyro(-500, 500, 570000.0); 

    drive_with_gyro(950, 2900.0);
    drive_with_gyro(800, 800.0); 
    turn_with_gyro(500, -500, 540000.0); 
    drive_with_gyro(500, 300.0);
    square_up_black_line();
    set_servo_position(0, OPEN - 45);
    turn_with_gyro(-700, 700, 11000.0);
    snatch_3(); //third
    turn_with_gyro(-500, 500, 570000.0);

    drive_with_gyro(800, 4330.0);
    turn_with_gyro(500, -500, 570000.0);
    drive_with_gyro(500, 300.0);
    square_up_black_line();
    set_servo_position(0, OPEN - 30);
    drive_with_gyro(800, 100.0);
    snatch(); //fourth
    square_up_black_line();

    set_servo_position(0, CLOSE);
    msleep(50);

    camera_open(LOW_RES);


    drive_with_gyro(1000, 200.0);
    mav(1, 0);
    mav(0, 0);
    
    //printf("\nmoved forwared\n");
    bool sawFire = false;
    int count = 0;
    msleep(3300);
    for(i = 0; i < 20; i++)
    {
        printf("\nupdating camera");
        camera_update();
        if(get_object_count(0) > 0)
        {
            count++;
        }
        msleep(10);
    }
    if(count > 10) {
        sawFire = true;
    }
    //drive_with_gyro(-3000, 1.0);
    //printf("drove backward\n");
    //msleep(10);
	square_up_black_line();
    printf("\nrobot squared up\n");


    if(!sawFire){ 
        printf("camera does not see fire\n");
        drive_with_gyro(1000, 1000.0);
        turn_with_gyro(-500, 500, 640000.0);    
        set_servo_position(0, OPEN + 100);
        drive_with_gyro(1000, 4100.0);


    } 
    else {
        printf("camera sees fire\n");
        turn_with_gyro(-700, 700, 400000.0);
        set_servo_position(0, OPEN + 300);
        drive_with_gyro(1500, 3500.0);
    }
    


    return 0;
}



//initializes the ports for the motors.
void declare_motors(int lmotor, int rmotor)
{
    right_motor = rmotor;
    left_motor = lmotor;
}

//Gyroscopes are always off by a specific angular velocity so we need to subtract this bias from all readings.
//Call calibrate_gyro to find what the gyroscope reads when the robot is at a complete stop (this is what the bias is). 
//The bias may change between turning the robot on when compared to the bias that the gyroscope has when it starts moving.
void calibrate_gyro()
{
    //takes the average of 50 readings
    int i = 0;
    double avg = 0;
    while(i < 50)
    {
        avg += gyro_z();
        msleep(1);
        i++;
    }
    bias = avg / 50.0;
    printf("New Bias: %f\n", bias);//prints out your bias. COMMENT THIS LINE OUT IF YOU DON'T WANT BIAS READINGS PRINTED OUT
}

//the same as calibrate_gyro() except faster to type
void cg()
{
    calibrate_gyro();
}

//turns the robot to reach a desired theta. 
//If you are expecting this function to work consistantly then don't take your turns too fast.
//The conversions from each wallaby to normal degrees varies but usually ~580000 KIPR degre80000 KIPR degrees = 90 degrees
void turn_with_gyro(int left_wheel_speed, int right_wheel_speed, double targetTheta)
{
    double theta = 0;//declares the variable that stores the current degrees
    mav(right_motor, right_wheel_speed);//starts the motors
    mav(left_motor, left_wheel_speed);
    //keeps the motors running until the robot reaches the desired angle
    while(theta < targetTheta)
    {
        msleep(10);//turns for .01 seconds
        theta += abs(gyro_z() - bias) * 10;//theta = omega(angular velocity, the value returned by gyroscopes) * time
       // printf("%d", theta);
    }

    //stops the motors after reaching the turn
    mav(right_motor, 0);
    mav(left_motor, 0);
}

//drives straight forward or backwards. The closer speed is to 0 the faster it will correct itself and the more consistent it will be but just do not go at max speed. Time is in ms. 
void drive_with_gyro(int speed, double time)
{ 
    double startTime = seconds();
    double theta = 0;
    while(seconds() - startTime < (time / 1000.0))
    {
        if(speed > 0)
        {
            mav(right_motor, (double)(speed - speed * (1.920137e-16 + 0.000004470956*theta - 7.399285e-28*pow(theta, 2) - 2.054177e-18*pow(theta, 3) + 1.3145e-40*pow(theta, 4))));  //here at NAR, we are VERY precise
            mav(left_motor, (double)(speed + speed * (1.920137e-16 + 0.000004470956*theta - 7.399285e-28*pow(theta, 2) - 2.054177e-18*pow(theta, 3) + 1.3145e-40*pow(theta, 4))));
        }
        else//reverses corrections if it is going backwards
        {
            mav(right_motor, (double)(speed + speed * (1.920137e-16 + 0.000004470956*theta - 7.399285e-28*pow(theta, 2) - 2.054177e-18*pow(theta, 3) + 1.3145e-40*pow(theta, 4)))); 
            mav(left_motor, (double)(speed - speed * (1.920137e-16 + 0.000004470956*theta - 7.399285e-28*pow(theta, 2) - 2.054177e-18*pow(theta, 3) + 1.3145e-40*pow(theta, 4))));
        }
        //updates theta
        msleep(10);
        theta += (gyro_z() - bias) * 10;
    }
}

void simple_drive_with_gyro(int speed, double time)
{
    double startTime = seconds(); //used to keep track of time
    double theta = 0; //keeps track of how much the robot has turned
    while(seconds() - startTime < time)
    {
        //if the robot is essentially straight then just drive straight
        if(theta < 1000 && theta > -1000)
        {
            mav(right_motor, speed);
            mav(left_motor, speed);
        }
        //if the robot is off to the right then drift to the left
        else if(theta < 1000)
        {
            mav(right_motor, speed + 100);
            mav(left_motor, speed - 100);
        }
        //if the robot is off to the left then drift to the right
        else
        {
            mav(right_motor, speed - 100);
            mav(left_motor, speed + 100);
        }
        //updates theta
        msleep(10);
        theta += (gyro_z() - bias) * 10;
    }
}

//keeps driving until detects black line
void gyro_light_stop(int speed)
{ 
    double startTime = seconds();
    double theta = 0;
    while(LIGHT_0 < THRESHOLD)
    {
        if(speed > 0)
        {
            mav(right_motor, (double)(speed - speed * (1.920137e-16 + 0.000004470956*theta - 7.399285e-28*pow(theta, 2) - 2.054177e-18*pow(theta, 3) + 1.3145e-40*pow(theta, 4))));  //here at NAR, we are VERY precise
            mav(left_motor, (double)(speed + speed * (1.920137e-16 + 0.000004470956*theta - 7.399285e-28*pow(theta, 2) - 2.054177e-18*pow(theta, 3) + 1.3145e-40*pow(theta, 4))));
        }
        else//reverses corrections if it is going backwards
        {
            mav(right_motor, (double)(speed + speed * (1.920137e-16 + 0.000004470956*theta - 7.399285e-28*pow(theta, 2) - 2.054177e-18*pow(theta, 3) + 1.3145e-40*pow(theta, 4)))); 
            mav(left_motor, (double)(speed - speed * (1.920137e-16 + 0.000004470956*theta - 7.399285e-28*pow(theta, 2) - 2.054177e-18*pow(theta, 3) + 1.3145e-40*pow(theta, 4))));
        }
        //updates theta
        msleep(10);
        theta += (gyro_z() - bias) * 10;
    }
}

//moves servo slowly
void slow_servo(int start, int end) {
    int i = 0;
    for(i; i < 30; i++) {
        start += (end - start)/30;
        set_servo_position(0, start);        
        msleep(100);
    }
}
void square_up_black_line () {
    while (LIGHT_0 < THRESHOLD || LIGHT_1 < THRESHOLD) {
        if(LIGHT_0 < THRESHOLD && LIGHT_1 < THRESHOLD) {
            mav(1, -200);
            mav(0, -200);
            msleep(1);
        }

        else if(LIGHT_0 < THRESHOLD && LIGHT_1 > THRESHOLD) {
            mav(1, 200);
            mav(0, -200);
            msleep(1);
        }
        else if (LIGHT_0 > THRESHOLD && LIGHT_1 < THRESHOLD) {
            mav(1, -200);
            mav(0, 200);
            msleep(1);
        } 
    }
    mav(0, 0);
    mav(1, 0);
}       

/*void square_up_black_line_2 () {
    while (LIGHT_0 < THRESHOLD || LIGHT_1 < THRESHOLD) {
        if(LIGHT_0 < THRESHOLD && LIGHT_1 < THRESHOLD) {
            mav(1, -150);
            mav(0, -150);
            msleep(1);
        }

        else if(LIGHT_0 < THRESHOLD && LIGHT_1 > THRESHOLD) {
            mav(1, 150);
            mav(0, -150);
            msleep(1);
        }
        else if (LIGHT_0 > THRESHOLD && LIGHT_1 < THRESHOLD) {
            mav(1, -150);
            mav(0, 150);
            msleep(1);
        } 
    }
    mav(0, 0);
    mav(1, 0);
}   


*/



