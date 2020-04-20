#include "Servo_motor.hpp"

Servo_motor::Servo_motor(std::string uuid, const uint8_t pin, const uint16_t poll_rate,
                         const uint8_t leftmost_position_degree,
                         const uint8_t rightmost_position_degree)
    : Servo(), uuid(std::move(uuid)), poll_rate(poll_rate), leftmost_pos_deg(leftmost_position_degree),
      rightmost_pos_deg(rightmost_position_degree), wait(poll_rate) {
  this->attach(pin);
}

std::string Servo_motor::get_uuid() {
  return this->uuid;
}

uint8_t Servo_motor::move_to_leftmost_position() {
  if (not this->attached()) return PIN_NOT_ATTACHED;

  this->write(this->leftmost_pos_deg);
}

uint8_t Servo_motor::move_to_rightmost_position() {
  if (not this->attached()) return PIN_NOT_ATTACHED;

  this->write(this->rightmost_pos_deg);
}

uint8_t Servo_motor::get_current_position() {
  if (not this->attached()) return PIN_NOT_ATTACHED;

  return this->read();
}

bool Servo_motor::poll() {
  if (--this->wait) return false;

  this->wait = this->poll_rate;
  return true;
}

int Servo_motor::go_to(uint8_t angle) {
  if (not this->attached()) return PIN_NOT_ATTACHED;

  this->write(angle);
  return 0;
}

int Servo_motor::resolve_datapoint(const std::string& datapoint, const uint32_t value) {
  if (datapoint == "ANGLE") {
    return this->go_to(value);
  } else {
    return 1;
  }
}
