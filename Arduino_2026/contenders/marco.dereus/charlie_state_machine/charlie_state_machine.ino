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

#define MIN_DISTANCE  15

typedef enum { NOTHING, TOO_CLOSE } danger_states;

danger_states danger_state = NOTHING;

typedef enum { STOP, FORWARD, BACKWARD, LEFT, RIGHT } move_states;

int move_stack_latest;
move_states move_stack[10];

void setup() {
  Serial.begin(9600);
  Serial.println("\n\n\nSetup");

  servo_distance_sensor.attach(3,700,2400);  // attach servo on pin 3 to servo object

  setup_distance_sensor();
  setup_infrared_sensor();
  setup_motors();

  clear_stack();
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

move_states pop_stack() {
  move_states state = move_stack[move_stack_latest];
  move_stack_latest--;
  return state;
}

void push_stack(move_states move_state) {
  move_stack_latest++;
  move_stack[move_stack_latest] = move_state;
}

void clear_stack() {
  move_stack_latest = -1;
}

bool is_stack_empty() {
  return move_stack_latest == -1;
}

danger_states update_danger_state() {
  if (DEBUG) {
    Serial.print(" | danger_state: ");
    Serial.print(danger_state);
  }

  if (get_distance() < MIN_DISTANCE) {
    return TOO_CLOSE;
  }

  return NOTHING;
}

void update_moves(danger_states danger_state) {
  switch (danger_state) {
    case TOO_CLOSE:
      clear_stack();
      push_stack(BACKWARD);
      push_stack(STOP);
      break;
    case NOTHING:
    default:
      if (is_stack_empty()) {
        push_stack(FORWARD);
        push_stack(STOP);
      }
  }

  if (DEBUG) {
    Serial.print(" | move_stack_latest: ");
    Serial.print(move_stack_latest);
    Serial.print(" | move stack:");
    for (int i=0; i <= move_stack_latest; i++) {
      Serial.print(" ");
      Serial.print(move_stack[i]);
    }
  }

}

void do_movement() {
  move_states move_state = pop_stack();

  switch (move_state) {
    case STOP:
      move_stop();
      break;
    case FORWARD:
      move_forward();
      break;
    case BACKWARD:
      move_backward();
      break;
  }

  if (DEBUG) {
    Serial.println();
  }

}

void loop() {

  calculate_distance();
  detect_infrared();

  danger_state = update_danger_state();
  update_moves(danger_state);
  do_movement();

  delay(1000);
}
