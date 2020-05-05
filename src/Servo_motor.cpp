#include "Servo_motor.hpp"

Servo_motor::Servo_motor(const std::string uuid,
                         const uint8_t pin,
                         const uint16_t poll_rate,
                         const uint8_t leftmost_position_degree,
                         const uint8_t rightmost_position_degree)
    : Servo(),
      uuid(uuid),
      poll_rate(poll_rate),
      leftmost_pos_deg(leftmost_position_degree),
      rightmost_pos_deg(rightmost_position_degree) {
  this->attach(pin);
  this->wait = poll_rate;
}

uint8_t Servo_motor::move_to_leftmost_position() {
  if (not this->attached()) return NOT_ATTACHED;

  this->write(this->leftmost_pos_deg);
  return 0;
}

uint8_t Servo_motor::move_to_rightmost_position() {
  if (not this->attached()) return NOT_ATTACHED;

  this->write(this->rightmost_pos_deg);
  return 0;
}

uint8_t Servo_motor::get_current_position() {
  if (not this->attached()) return NOT_ATTACHED;

  return this->read();
}

bool Servo_motor::poll() {
  if (--this->wait) return false;

  this->wait = this->poll_rate;
  return true;
}

uint8_t Servo_motor::go_to(uint8_t angle) {
  if (not this->attached()) return NOT_ATTACHED;

  this->write(angle);
  return 0;
}

uint8_t Servo_motor::resolve_datapoint(const std::string datapoint, const uint8_t value) {
  if (datapoint == "ANGLE") {
    return this->go_to(value);
  } else {
    return Servo_motor::UKNOWN_DATAPOINT;
  }
}
