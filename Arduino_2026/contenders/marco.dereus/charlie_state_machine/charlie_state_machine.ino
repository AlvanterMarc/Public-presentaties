/*
  Marco de Reus - marco.dereus@alvant.nl

  Mijn poging om Charlie door het doolhof te krijgen met een state machine achtige oplossing
*/

#include <Servo.h>  // servo library
#include "robot_stuff.h" // alle basic stuff van de robot 

Servo servo_distance_sensor; // create servo object to control servo

// globale variablen
// koppie kan van 0-180, 90 graden is recht vooruit
int koppie_graden = 90;
int koppie_links_rechts = 1; // links = -1 (neg) / rechts = 1 (pos)
int koppie_links_bound = 90 - 10;
int koppie_rechts_bound = 90 + 10;

int linker_muur_gezien = 0;

#define MIN_DISTANCE  15

void setup() {
  Serial.begin(9600);
  Serial.println("\n\n\nSetup");

  servo_distance_sensor.attach(3,700,2400);  // attach servo on pin 3 to servo object

  setup_distance_sensor();
  setup_infrared_sensor();
  setup_motors();

  turn_distance_sensor_to(servo_distance_sensor, koppie_graden);
  move_stop();
}

void calculate_koppie() {
  koppie_graden += koppie_links_rechts;
  if (koppie_graden <= koppie_links_bound && koppie_links_rechts < 0) {
    koppie_links_rechts *= -1; // draai beweging om
  } else if (koppie_graden >= koppie_rechts_bound && koppie_links_rechts > 0) {
    koppie_links_rechts *= -1; // draai beweging om
  }
}

void loop() {

  calculate_koppie();
  //beweeg_koppie(koppie_graden);
  int distance = calculate_distance();
  bool koppie_ok = distance > MIN_DISTANCE;

  bool pootjes_ok = !detect_infrared();

  if (koppie_ok && pootjes_ok) {
    //kat_stop();
    // delay(10);
    // kat_forward_left();
    move_forward();
  }

  if (!koppie_ok) {
    move_stop();
    delay(20);
    move_backward();
    delay(200);
    turn_left();
    delay(200);
  }

  if (!pootjes_ok) {
    move_stop();
    delay(20);

    if (get_infrared_left()) {
      linker_muur_gezien = 0;
    }

    if (get_infrared_left() && get_infrared_right()) {
      move_stop();
      delay(100);
      move_backward();
      delay(300);
      turn_left();
      delay(600);
    }
    else if (get_infrared_left() || get_infrared_mid()) {
      move_stop();
      delay(100);
      // kat_backward();
      // delay(300);
      turn_right();
      delay(300);
    }
    else if (get_infrared_right()) {
      move_stop();
      delay(100);
      move_backward();
      delay(300);

      if (linker_muur_gezien < 20) {
        linker_muur_gezien = 0;
        turn_right();
        delay(300);
      } else {
        turn_left();
        delay(300);
      }
    }
    else {
      turn_distance_sensor_to(servo_distance_sensor, 0);
    }

    move_stop();
  }  

  delay(10);
  linker_muur_gezien += 1;

}
