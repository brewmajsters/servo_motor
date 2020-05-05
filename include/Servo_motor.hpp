#pragma once

#include <Servo.h>

class Servo_motor : Servo {
private:
  uint16_t wait;
public:
  const std::string uuid;
  const uint16_t poll_rate;
  const uint8_t leftmost_pos_deg;
  const uint8_t rightmost_pos_deg;
  static const uint8_t NOT_ATTACHED = 255;
  static const uint8_t UKNOWN_DATAPOINT = 254;

  Servo_motor(std::string uuid,
              uint8_t pin,
              uint16_t poll_rate,
              uint8_t leftmost_pos = 0,
              uint8_t rightmost_pos = 180);

  uint8_t move_to_rightmost_position();

  uint8_t move_to_leftmost_position();

  uint8_t get_current_position();

  bool poll();

  uint8_t go_to(uint8_t);

  uint8_t resolve_datapoint(std::string datapoint, uint8_t value);
};
