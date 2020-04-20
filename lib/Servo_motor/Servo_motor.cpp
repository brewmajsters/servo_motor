#include "Servo_motor.hpp"

Servo_motor::Servo_motor(const char *uuid, const uint8_t pin, const uint16_t poll_rate,
                         const uint8_t leftmost_position_degree,
                         const uint8_t rightmost_position_degree)
    : uuid(uuid), pin(pin), poll_rate(poll_rate), leftmost_pos_deg(leftmost_position_degree),
      rightmost_pos_deg(rightmost_position_degree) {
  this->servo = Servo();
  this->servo.attach(pin);
}

const char *Servo_motor::get_uuid() {
  return this->uuid;
}

uint8_t Servo_motor::get_pin() {
  if (servo.attached()) return (uint8_t) this->servo.attachedPin();
}

uint8_t Servo_motor::move_to_leftmost_position() {
  if (not this->servo.attached()) return SERVO_IS_NOT_ATTACHED;

  this->servo.write(this->leftmost_pos_deg);
}

uint8_t Servo_motor::move_to_rightmost_position() {
  if (not this->servo.attached()) return SERVO_IS_NOT_ATTACHED;

  this->servo.write(this->rightmost_pos_deg);
}

uint8_t Servo_motor::get_current_position() {
  if (not this->servo.attached()) return SERVO_IS_NOT_ATTACHED;

  return this->servo.read();
}