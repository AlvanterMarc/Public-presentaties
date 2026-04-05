/*
  Marco de Reus - marco.dereus@alvant.nl

  Mijn poging om Charlie door het doolhof te krijgen...
*/

#include <Servo.h>  // servo library
Servo servo_koppie; // create servo object to control servo

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

// globale variablen
// koppie kan van 0-180, 90 graden is recht vooruit
int koppie_graden = 90;
int koppie_links_rechts = 1; // links = -1 (neg) / rechts = 1 (pos)
int koppie_links_bound = 90 - 10;
int koppie_rechts_bound = 90 + 10;

int linker_muur_gezien = 0;

bool pootje_L = false;
bool pootje_M = false;
bool pootje_R = false;

#define MAX_SPEED  210
#define MIN_SPEED  140

#define MIN_DISTANCE  15

void setup() {
  Serial.begin(9600);

  servo_koppie.attach(3,700,2400);  // attach servo on pin 3 to servo object

  pinMode(Echo, INPUT);
  pinMode(Trig, OUTPUT);

  pinMode(motor_L_ENA, OUTPUT); //Left motor enabler and PWM port
  pinMode(motor_L_IN1, OUTPUT);  //Left motor control port 1
  pinMode(motor_L_IN2, OUTPUT);  //Left motor control port 2
  pinMode(motor_R_ENB, OUTPUT); //Right motor enable and PWM speed control port
  pinMode(motor_R_IN3, OUTPUT);  //Right motor control port 3
  pinMode(motor_R_IN4, OUTPUT);  //Left motor control port 4

  pinMode(infrared_L, INPUT);  //Left infrared sensor
  pinMode(infrared_M, INPUT);  //Intermediate infrared sensor
  pinMode(infrared_R, INPUT);  //Right infrared sensor

  pinMode(sound,OUTPUT);

  beweeg_koppie(koppie_graden);
  kat_stop();
}

void kat_forward_left() {
  kat_forward(MIN_SPEED, MAX_SPEED);
}

void kat_forward_right() {
  kat_forward(MAX_SPEED, MIN_SPEED);
}

void kat_forward(int speed_left, int speed_right) {
  analogWrite(motor_L_ENA, speed_left); // Left motor forward
  digitalWrite(motor_L_IN1, 1);
  digitalWrite(motor_L_IN2, 0);

  analogWrite(motor_R_ENB, speed_right); // Right motor forward
  digitalWrite(motor_R_IN3, 1);
  digitalWrite(motor_R_IN4, 0);
}

void kat_backward() {
  analogWrite(motor_L_ENA, MIN_SPEED); // Left motor backward
  digitalWrite(motor_L_IN1, 0);
  digitalWrite(motor_L_IN2, 1);

  analogWrite(motor_R_ENB, MIN_SPEED); // Right motor backward
  digitalWrite(motor_R_IN3, 0);
  digitalWrite(motor_R_IN4, 1);
}

void kat_turn_left() {
  digitalWrite(motor_L_IN1, 0);
  digitalWrite(motor_L_IN2, 1);
  analogWrite(motor_L_ENA, MAX_SPEED);

  digitalWrite(motor_R_IN3, 1);
  digitalWrite(motor_R_IN4, 0);
  analogWrite(motor_R_ENB, MAX_SPEED);
}

void kat_turn_right() {
  digitalWrite(motor_L_IN1, 1);
  digitalWrite(motor_L_IN2, 0);
  analogWrite(motor_L_ENA, MAX_SPEED);

  digitalWrite(motor_R_IN3, 0);
  digitalWrite(motor_R_IN4, 1);
  analogWrite(motor_R_ENB, MAX_SPEED);
}

void kat_stop() {
  digitalWrite(motor_L_ENA, 0); // Left motor braking
  digitalWrite(motor_L_IN1, 0);
  digitalWrite(motor_L_IN2, 0);

  digitalWrite(motor_R_ENB, 0); // Right motor braking
  digitalWrite(motor_R_IN3, 0);
  digitalWrite(motor_R_IN4, 0);
}

void calculate_koppie() {
  koppie_graden += koppie_links_rechts;
  if (koppie_graden <= koppie_links_bound && koppie_links_rechts < 0) {
    koppie_links_rechts *= -1; // draai beweging om
  } else if (koppie_graden >= koppie_rechts_bound && koppie_links_rechts > 0) {
    koppie_links_rechts *= -1; // draai beweging om
  }
}

void beweeg_koppie(int graden) {
  servo_koppie.write(graden);
}

// proef ondervindelijk:
// - max distance lijkt 800
// - distance waarop je echt niet verder wilt: 10
int calculate_distance() {
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);   
  float Fdistance = pulseIn(Echo, HIGH)/58;  
  // Serial.print("distance = ");
  // Serial.println(Fdistance);
  return (Fdistance);
}

bool feel_kat_pootjes() {
  pootje_L = feel_kat_pootje(infrared_L);
  pootje_M = feel_kat_pootje(infrared_M);
  pootje_R = feel_kat_pootje(infrared_R);
  
  bool any_sensor = pootje_L || pootje_M || pootje_R;
  return any_sensor;
}

bool feel_kat_pootje(int sensor) {
  return digitalRead(sensor) == 1;
}

void loop() {

  calculate_koppie();
  beweeg_koppie(koppie_graden);
  int distance = calculate_distance();
  bool koppie_ok = distance > MIN_DISTANCE;

  bool pootjes_ok = !feel_kat_pootjes();

  if (koppie_ok && pootjes_ok) {
    //kat_stop();
    // delay(10);
    // kat_forward_left();
    kat_forward(MIN_SPEED, MIN_SPEED);
  }

  if (!koppie_ok) {
    kat_stop();
    delay(20);
    kat_backward();
    delay(200);
    kat_turn_left();
    delay(200);
  }

  if (!pootjes_ok) {
    kat_stop();
    delay(20);

    if (pootje_L) {
      linker_muur_gezien = 0;
    }

    if (pootje_L && pootje_R) {
      kat_stop();
      delay(100);
      kat_backward();
      delay(300);
      kat_turn_left();
      delay(600);
    }
    else if (pootje_L || pootje_M) {
      kat_stop();
      delay(100);
      // kat_backward();
      // delay(300);
      kat_turn_right();
      delay(300);
    }
    else if (pootje_R) {
      kat_stop();
      delay(100);
      kat_backward();
      delay(300);

      if (linker_muur_gezien < 20) {
        linker_muur_gezien = 0;
        kat_turn_right();
        delay(300);
      } else {
        kat_turn_left();
        delay(300);
      }
    }
    else {
      beweeg_koppie(0);
    }

    kat_stop();
  }  

  delay(10);
  linker_muur_gezien += 1;

}
