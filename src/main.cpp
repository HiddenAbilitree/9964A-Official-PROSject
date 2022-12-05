#include "main.h"
#include "okapi/api/chassis/controller/chassisControllerIntegrated.hpp"
#include "okapi/api/chassis/controller/odomChassisController.hpp"
#include "okapi/api/chassis/model/chassisModel.hpp"
#include "okapi/api/device/motor/abstractMotor.hpp"
#include "okapi/api/units/QLength.hpp"
#include "okapi/api/units/RQuantity.hpp"
#include "okapi/api/util/logging.hpp"
#include "okapi/api/util/mathUtil.hpp"
#include "okapi/impl/chassis/controller/chassisControllerBuilder.hpp"
#include "okapi/impl/device/motor/motorGroup.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "robot.h"

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */

// center button function.
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "Middle button pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

// left button function
void on_left_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "Left button pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

// right button function
void on_right_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "Right button pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// creates buttons on the cortex lcd display
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");
	pros::lcd::register_btn0_cb(on_right_button);
	pros::lcd::register_btn1_cb(on_center_button);
	pros::lcd::register_btn2_cb(on_left_button);

}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {

	// initalizing controller
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	
	// initializing motors
	pros::Motor lUFM(1,pros::E_MOTOR_GEAR_GREEN,0,pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor lUBM(2,pros::E_MOTOR_GEAR_GREEN,0,pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor lLFM(3,pros::E_MOTOR_GEAR_GREEN,0,pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor lLBM(4,pros::E_MOTOR_GEAR_GREEN,0,pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor rUFM(11,pros::E_MOTOR_GEAR_GREEN,0,pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor rUBM(12,pros::E_MOTOR_GEAR_GREEN,0,pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor rLFM(13,pros::E_MOTOR_GEAR_GREEN,0,pros::E_MOTOR_ENCODER_DEGREES);
	pros::Motor rLBM(14,pros::E_MOTOR_GEAR_GREEN,0,pros::E_MOTOR_ENCODER_DEGREES);
	
	// grouping motors into groups for readability
	pros::Motor_Group leftMotors({lUFM,lUBM,lLFM,lLBM});
	pros::Motor_Group rightMotors({rUFM,rUBM,rLFM,rLBM});

	// creating motor groups for odometry
	// port numbers stored in robot.h
	okapi::MotorGroup left_drive_motors({
		LEFT_DRIVE_MOTOR1_PORT,		
		LEFT_DRIVE_MOTOR2_PORT,
		LEFT_DRIVE_MOTOR3_PORT,
		LEFT_DRIVE_MOTOR4_PORT});
	okapi::MotorGroup right_drive_motors({
		LEFT_DRIVE_MOTOR1_PORT,
		LEFT_DRIVE_MOTOR2_PORT,
		LEFT_DRIVE_MOTOR3_PORT,
		LEFT_DRIVE_MOTOR4_PORT});


	//pros::Motor_Group leftMotors({lUFM,lUBM});
	//pros::Motor_Group rightMotors({rUFM,rUBM});
	/*std::shared_ptr<okapi::OdomChassisController> chassis = okapi::ChassisControllerBuilder()
	.withMotors(
		left_drive_motors,
		right_drive_motors
	)
	.withDimensions(DRIVE_GEARSET, {{3.25_in, 11_in}, okapi::imev5GreenTPR})
	.withOdometry()
	.buildOdometry();*/

	// creating the chassis using okapilib ChassisController
	chassis = okapi::ChassisControllerBuilder()
		.withMotors(
			left_drive_motors,					// left motors
			right_drive_motors)					// right motors
		.withGains(
        	{0.001, 0, 0.0001},	 				// Distance controller gains
        	{0.001, 0, 0.0001},			 		// Turn controller gains
        	{0.001, 0, 0.0001}  				// Angle controller gains (helps drive straight)
    	)
		.withDimensions(
			DRIVE_GEARSET,						// drive gearset stored in robot.h
			{	
				{
					CHASSIS_WHEELS,
					CHASSIS_TRACK
				}, // wheel size and drivetrain track size stored in robot.h
				DRIVE_TPR						// drivetrain ticks per rotation stored in robot.h
			}
		)
		.withOdometry()	
		.buildOdometry();

	// resets the state of the robot
	chassis->setState({0_in, 0_in, 0_deg});

	// moves the robot 1 foot forward to the right
	// orientation should be 45 degrees.
	chassis->driveToPoint({12_in,12_in});
	

	// main while loop
	/*
	while (true) {

		// prints buttons on the screen and sets numeric values for each button.
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);

		// stores controller analog stick positions into an int variable
		// ranges -127 to 127
		int left = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
		int right = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);


		// sets motor movement equal to the controller analog input
		leftMotors=left;
		rightMotors=right;

		// final delay
		pros::delay(8);
	}*/
}
