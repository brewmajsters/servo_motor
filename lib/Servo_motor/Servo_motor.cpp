#include "Servo_motor.hpp"

Servo_motor::Servo_motor(const int leftmost_position_degree, const int rightmost_position_degree)
    : leftmost_pos_deg(leftmost_position_degree), rightmost_pos_deg(rightmost_position_degree) {
  this->servo = new Servo();
}

int Servo_motor::leftmost_position() {
  if (not this->servo->attached()) return SERVO_IS_NOT_ATTACHED;

  this->servo->write(this->leftmost_pos_deg);
}

int Servo_motor::rightmost_position() {
  if (not this->servo->attached()) return SERVO_IS_NOT_ATTACHED;

  this->servo->write(this->rightmost_pos_deg);
}