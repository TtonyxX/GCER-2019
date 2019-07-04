#include <kipr/botball.h>
#include <drive.h>
#include <stdio.h>
#include <stdbool.h>

double gyrocalibrate() {
    double changeGyroZ = 0;//find average gyro value when still
    int i = 0;
    for(i = 0; i < 2000; i++) {
        changeGyroZ += gyro_z();
    }
    changeGyroZ /= 2000;

    return changeGyroZ;
}

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

void turn(int direction, int degree, int changeGyroZ) {
    double anglechange = 0;
    int x = degree*270000/90;
    x /= 10;
    if(direction == 1) { // 1 = left, 0 = right
        while(anglechange > -x) {
            anglechange -= gyro_z()-changeGyroZ;
            motor(0, -100);
            motor(1, 100);
            msleep(10);
        }
    } else {
        while(anglechange < x) {
            anglechange -= gyro_z()-changeGyroZ;
            motor(0, 100);
            motor(1, -100);
            msleep(10);
        }
    }
    ao();
}

void turnLeft(int degrees) { 
    motor(0, 100);
    motor(1, -100);
    msleep(degrees*8.2);
    ao();
}

int main() {
    
    double change = gyrocalibrate();
    
    
    turnLeft(90);
    msleep(100000);

    move(1500, 3600, change);
    
    
    move(1500, 5000, change);
    
    return 0;
}
