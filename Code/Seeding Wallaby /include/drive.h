/* Drive.h	
	* The drive library contains code that should work on any robot running on two powered, forward-
	* facing motors. The main purpose of this library is to facilitate usage of four movements:
	* Forward, Backward, Left, and Right. Constants defined as $ need to be defined by the user.
	* !TODO! Left/Right with gyro.
	* Clean up right/left by using math.h's absolute value function.
	*/

// This segment prevents including Drive.h twice.
#ifndef DRIVE_H
#define DRIVE_H

#define PI 3.1415926539

// Hard constants
#define MAX_SPEED 1500

// Mathematically defined constants
#define CM_TO_BEMF (BEMFS_PER_ROTATION / (PI * WHEEL_DIAMETER)) //Centimeters travelled to motor ticks.
#define BEMF_TO_CM ((PI * WHEEL_DIAMETER) / BEMFS_PER_ROTATION) //Motor ticks to centimeters travelled.
#define SPD_L_TURN ((SPD_L_B + SPD_R_F) / 2)
#define SPD_R_TURN ((SPD_R_B + SPD_L_F) / 2)

// Robot-specific constants
#define MOT_LEFT 1 // Port the left drive motor is plugged into.
#define MOT_RIGHT 0 // Port the right drive motor is plugged into.
#define WHEEL_DIAMETER 5.5 // Diameter of the wheel. Common values in appendix
#define ROBOT_DIAMETER 19 // Distance from the center of one wheel to the center of the other.
#define BEMFS_PER_ROTATION 1500 // Motor ticks per rotation. KIPR says this value should be around 1500.

// Tuning Constants
#define SPD_L_F 1500 // Left forward speed. Max is 1500.
#define SPD_R_F 1450 // Right forward speed. Max is 1500.
#define SPD_L_B 1500 // Left backward speed. Max is 1500.
#define SPD_R_B 1410 // Right backward speed. Max is 1500.

// Gyro Constants
#define Kp 1 // Proportionality constant for Gyro driving
double GYRO_PER_ROT;
double GYRO_DEV;
#define RIGHT_OFFSET 1
#define LEFT_OFFSET 1

// Low-Level drive commands

/*
	* \brief Shuts down the two drive motors.
	*/
void drive_off();

/* (!NEEDS TESTING!)
	* \brief Actively brakes to shut off the two drive motors for more precision.
	*/
void drive_freeze();

/*
	* \brief Clears the position counter on the two drive motors.
	*/
void drive_clear();

/*
	* \brief Sets the two drive motors to move at certain speeds.
	* \param left_speed the speed of the left motor.
	* \param right_speed the speed of the right motor.
	*/
void drive(int left_speed, int right_speed);

// Main drive commands.

/*
	* \brief Turns right a certain amount of degrees on a certain radius
	* \param degrees the amount to turn, in degrees. Negative values do a reverse turn.
	* \param radius the radius of the turn, in centimeters. Only use values 0 and above.
	* NOTE: degrees is an int because the wallaby only has precision up to about 2-3 degrees.
	*/
void right(int degrees, double radius);

/*
	* \brief Turns right a certain amount of degrees on a certain radius
	* \param degrees the amount to turn, in degrees. Negative values do a reverse turn.
	* \param radius the radius of the turn, in centimeters. Only use values 0 and above.
	* \param speed the speed to travel at.
	* NOTE: degrees is an int because the wallaby only has precision up to about 2-3 degrees.
	*/
void right_speed(int degrees, double radius, int speed);

/*
	* \brief Turns left a certain amount of degrees on a certain radius
	* \param degrees the amount to turn, in degrees. Negative values do a reverse turn.
	* \param radius the radius of the turn, in centimeters. Only use values 0 and above.
	* NOTE: degrees is an int because the wallaby only has precision up to about 2-3 degrees.
	*/
void left(int degrees, double radius);

/*
	* \brief Turns left a certain amount of degrees on a certain radius
	* \param degrees the amount to turn, in degrees. Negative values do a reverse turn.
	* \param radius the radius of the turn, in centimeters. Only use values 0 and above.
	* \param speed the speed to travel at.
	* NOTE: degrees is an int because the wallaby only has precision up to about 2-3 degrees.
	*/
void left_speed(int degrees, double radius, int speed);

/*
	* \brief Drives forward a certain distance at default drive speed.
	* \param distance the distance to travel, in centimeters. Only use values 0 and above.
	* NOTE: distance is an integer as the wallaby only has precision up to about 1-2 centimeters.
	*/
void forward(int distance);

/*
	* \brief Drives forward a certain distance at a custom speed.
	* \param distance the distance to travel, in centimeters. Only use values 0 and above.
	* \param speed the speed to travel at.
	* NOTE: distance is an integer as the wallaby only has precision up to about 1-2 centimeters.
	*/
void forward_speed(int distance, int speed);

/*
	* \brief Drives backward a certain distance at default drive speed.
	* \param distance the distance to travel, in centimeters. Only use values 0 and above.
	* NOTE: distance is an integer as the wallaby only has precision up to about 1-2 centimeters.
	*/
void backward(int distance);

/*
	* \brief Drives backward a certain distance at default drive speed.
	* \param distance the distance to travel, in centimeters. Only use values 0 and above.
	* \param speed the speed to travel at.
	* NOTE: distance is an integer as the wallaby only has precision up to about 1-2 centimeters.
	*/
void backward_speed(int distance, int speed);

void left_turn(int speed, float degrees); 

void right_turn(int speed, float degrees); 


#endif

/* APPENDIX
	* Common wheel diameter values: (To be added at a future date)
	*
	*/
