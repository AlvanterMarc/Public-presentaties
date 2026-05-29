/*
  Marco de Reus - marco.dereus@alvant.nl

  Mijn poging om Charlie door het doolhof te krijgen met een state machine achtige oplossing
*/

#include <Servo.h>  // servo library
#include "robot_stuff.h" // alle basic stuff van de robot 

Servo servo_distance_sensor; // create servo object to control servo

#define DISTANCE_SENSOR_TURN_MIDDLE   90 // distance sensor servo kan van 0-180, 90 graden is recht vooruit
#define DISTANCE_SENSOR_TURN_DEGREES   3 // links = -1 (neg) / rechts = 1 (pos)
#define DISTANCE_SENSOR_TURN_LIMIT    10
#define DISTANCE_SENSOR_MIN_DISTANCE  15

int distance_sensor_degrees_current = DISTANCE_SENSOR_TURN_MIDDLE;
int distance_sensor_degrees_left_right = DISTANCE_SENSOR_TURN_DEGREES; // links = neg / rechts = pos

typedef enum { NOTHING, TOO_CLOSE, WALL_LEFT, WALL_MID, WALL_RIGHT, WALL_ALL } danger_states;

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
  turn_distance_sensor_to(servo_distance_sensor, distance_sensor_degrees_current);
  move_stop();
}

int calculate_distance_sensor_turn() {
  if (distance_sensor_degrees_current <= (DISTANCE_SENSOR_TURN_MIDDLE - DISTANCE_SENSOR_TURN_LIMIT) && distance_sensor_degrees_left_right < 0) {
    distance_sensor_degrees_left_right *= -1; // draai beweging om
  } else if (distance_sensor_degrees_current >= (DISTANCE_SENSOR_TURN_MIDDLE + DISTANCE_SENSOR_TURN_LIMIT) && distance_sensor_degrees_left_right > 0) {
    distance_sensor_degrees_left_right *= -1; // draai beweging om
  }
  distance_sensor_degrees_current += distance_sensor_degrees_left_right;
  return distance_sensor_degrees_current;
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

  if (get_distance() < DISTANCE_SENSOR_MIN_DISTANCE) {
    return TOO_CLOSE;
  }

  if (get_infrared_left() && get_infrared_mid() && get_infrared_right()) {
    return WALL_ALL;
  }

  if (get_infrared_left() && get_infrared_mid()) {
    return WALL_LEFT;
  }

  if (get_infrared_mid() && get_infrared_right()) {
    return WALL_RIGHT;
  }

  if (get_infrared_left()) {
    return WALL_LEFT;
  }

  if (get_infrared_mid()) {
    return WALL_MID;
  }

  if (get_infrared_right()) {
    return WALL_RIGHT;
  }

  return NOTHING;
}

void update_moves(danger_states danger_state) {
  switch (danger_state) {
    case TOO_CLOSE:
      clear_stack();
      push_stack(BACKWARD);
      //push_stack(STOP);
      break;
    case WALL_ALL:
    case WALL_LEFT:
    case WALL_MID:
      clear_stack();
      push_stack(STOP);
      push_stack(RIGHT);
      push_stack(RIGHT);
      break;
    case WALL_RIGHT:
      // clear_stack();
      // push_stack(STOP);
      // push_stack(RIGHT);
      // push_stack(RIGHT);
      // break;
      clear_stack();
      push_stack(STOP);
      push_stack(LEFT);
      push_stack(LEFT);
      break;
    case NOTHING:
    default:
      if (is_stack_empty()) {
        // long rand = random(0,100);
        // if (rand < 10) {
        //   push_stack(RIGHT);
        // } else if (rand < 50) {
        //   push_stack(LEFT);
        // } else {
        //   push_stack(FORWARD);
        // }
        push_stack(LEFT);
        push_stack(STOP);
        push_stack(FORWARD);
        //push_stack(FORWARD);
        push_stack(STOP);
        // push_stack(FORWARD);
        // push_stack(STOP);
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
    case LEFT:
      turn_left();
      break;
    case RIGHT:
      turn_right();
      break;
    default:
      Serial.print("\n !! no mapping for move_state: ");
      Serial.print(move_state);
  }

  if (DEBUG) {
    Serial.println();
  }

}

void loop() {

  turn_distance_sensor_to(servo_distance_sensor, calculate_distance_sensor_turn());

  calculate_distance();
  detect_infrared();

  danger_state = update_danger_state();
  update_moves(danger_state);
  do_movement();

  delay(100);
}
