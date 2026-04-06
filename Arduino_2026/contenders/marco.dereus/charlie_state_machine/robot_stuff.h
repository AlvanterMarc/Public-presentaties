#include <Servo.h>  // servo library

#define DEBUG  true

#define MAX_SPEED  210
#define MIN_SPEED  140

// Constants uit de voorbeeld libraries.

int Echo = A4;
int Trig = A5;
int sound = 15;

int motor_L_ENA = 6; //The left motor control end is connected to pins 6,9 and 11 of the Arduino board
int motor_L_IN1 = 9;
int motor_L_IN2 = 11;

int motor_R_ENB = 5; //The right motor control terminal is connected to pins 5, 7, and 8 of the Arduino board
int motor_R_IN3 = 7;
int motor_R_IN4 = 8;

int infrared_L = A0;  //The left infrared sensor is connected to the A0 pin of the Arduino board
int infrared_M = A2;  //The mid-side infrared sensor is connected to the A2 pin of the Arduino board
int infrared_R = A3;  //The right infrared sensor is connected to the A3 pin of the Arduino board

int sensor_L = 1;  //The detection value of the left infrared sensor, the default value is 1, outside the black line
int sensor_M = 1;  //The detection value of the middle infrared sensor, the default value is 1, is on the black line
int sensor_R = 1;  //The detection value of the infrared sensor on the right, the default value is 1, outside the black line

//
// SETUP
//

void setup_distance_sensor() {
  Serial.println("setup distance sensor");
  pinMode(Echo, INPUT);
  pinMode(Trig, OUTPUT);
  pinMode(sound,OUTPUT);
}

void setup_infrared_sensor() {
  Serial.println("setup infrared sensor");
  pinMode(infrared_L, INPUT);  //Left infrared sensor
  pinMode(infrared_M, INPUT);  //Intermediate infrared sensor
  pinMode(infrared_R, INPUT);  //Right infrared sensor
}

void setup_motors() {
  Serial.println("setup motors");
  pinMode(motor_L_ENA, OUTPUT); //Left motor enabler and PWM port
  pinMode(motor_L_IN1, OUTPUT);  //Left motor control port 1
  pinMode(motor_L_IN2, OUTPUT);  //Left motor control port 2
  pinMode(motor_R_ENB, OUTPUT); //Right motor enable and PWM speed control port
  pinMode(motor_R_IN3, OUTPUT);  //Right motor control port 3
  pinMode(motor_R_IN4, OUTPUT);  //Left motor control port 4
}

//
// MOVEMENT
//

void move_stop() {
  if (DEBUG) {
    Serial.print(" | stop!");
  }

  digitalWrite(motor_L_ENA, 0); // Left motor braking
  digitalWrite(motor_L_IN1, 0);
  digitalWrite(motor_L_IN2, 0);

  digitalWrite(motor_R_ENB, 0); // Right motor braking
  digitalWrite(motor_R_IN3, 0);
  digitalWrite(motor_R_IN4, 0);
}

void _move_forward(int speed_left, int speed_right) {
  if (DEBUG) {
    Serial.print(" | moving forward");
  }
  analogWrite(motor_L_ENA, speed_left); // Left motor forward
  digitalWrite(motor_L_IN1, 1);
  digitalWrite(motor_L_IN2, 0);

  analogWrite(motor_R_ENB, speed_right); // Right motor forward
  digitalWrite(motor_R_IN3, 1);
  digitalWrite(motor_R_IN4, 0);
}

void move_forward() {
  _move_forward(MIN_SPEED, MIN_SPEED);
}

void move_forward_left() {
  _move_forward(MIN_SPEED, MAX_SPEED);
}

void move_forward_right() {
  _move_forward(MAX_SPEED, MIN_SPEED);
}

void _move_backward(int speed_left, int speed_right) {
  if (DEBUG) {
    Serial.print(" | moving backward");
  }
  analogWrite(motor_L_ENA, speed_left); // Left motor backward
  digitalWrite(motor_L_IN1, 0);
  digitalWrite(motor_L_IN2, 1);

  analogWrite(motor_R_ENB, speed_right); // Right motor backward
  digitalWrite(motor_R_IN3, 0);
  digitalWrite(motor_R_IN4, 1);
}

void move_backward() {
  _move_backward(MIN_SPEED, MIN_SPEED);
}

void turn_left() {
  if (DEBUG) {
    Serial.print(" | turning left");
  }
  digitalWrite(motor_L_IN1, 0);
  digitalWrite(motor_L_IN2, 1);
  analogWrite(motor_L_ENA, MAX_SPEED);

  digitalWrite(motor_R_IN3, 1);
  digitalWrite(motor_R_IN4, 0);
  analogWrite(motor_R_ENB, MAX_SPEED);
}

void turn_right() {
  if (DEBUG) {
    Serial.print(" | turning right");
  }
  digitalWrite(motor_L_IN1, 1);
  digitalWrite(motor_L_IN2, 0);
  analogWrite(motor_L_ENA, MAX_SPEED);

  digitalWrite(motor_R_IN3, 0);
  digitalWrite(motor_R_IN4, 1);
  analogWrite(motor_R_ENB, MAX_SPEED);
}

//
// DISTANCE SENSOR
//

int _distance_sensor = 9999;

int get_distance() {
  return _distance_sensor;
}

void turn_distance_sensor_to(Servo sensor, int degrees) {
  sensor.write(degrees);
}

// proef ondervindelijk:
// - max distance lijkt 800
// - min distance waarop je echt niet verder wilt: 10
int calculate_distance() {
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);   
  _distance_sensor = pulseIn(Echo, HIGH)/58;
  if (DEBUG == true) {
    Serial.print(" | distance = ");
    Serial.print(_distance_sensor);
  }
  return (_distance_sensor);
}

//
// INFRARED SENSOR
//

bool _infrared_sensor_left  = false;
bool _infrared_sensor_mid   = false;
bool _infrared_sensor_right = false;

bool get_infrared_left() {
  return _infrared_sensor_left;
}

bool get_infrared_mid() {
  return _infrared_sensor_mid;
}

bool get_infrared_right() {
  return _infrared_sensor_right;
}

bool _read_infrared(int sensor) {
  // true: detected something, false: nothing
  return digitalRead(sensor) == 1;
}

bool detect_infrared() {
  _infrared_sensor_left = _read_infrared(infrared_L);
  _infrared_sensor_mid = _read_infrared(infrared_M);
  _infrared_sensor_right = _read_infrared(infrared_R);

  if (DEBUG == true) {
    Serial.print(" | infrared L, M, R: ");
    Serial.print(_infrared_sensor_left ? "true , " : "false, ");
    Serial.print(_infrared_sensor_mid ? "true , " : "false, ");
    Serial.print(_infrared_sensor_right ? "true" : "false");
  }
  
  bool any_sensor = _infrared_sensor_left || _infrared_sensor_mid || _infrared_sensor_right;
  return any_sensor;
}
