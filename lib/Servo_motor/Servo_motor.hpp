#ifndef SERVO_SERVO_MOTOR_H
#define SERVO_SERVO_MOTOR_H

#include <Servo.h>


class Servo_motor : Servo {
private:
  const std::string uuid;
  const uint16_t poll_rate;
  const uint8_t leftmost_pos_deg;
  const uint8_t rightmost_pos_deg;
  uint32_t wait;
public:
  Servo_motor(std::string uuid, uint8_t pin, uint16_t ratelimit, uint8_t leftmost_pos = 0, uint8_t rightmost_pos = 180);

  std::string get_uuid();

  uint8_t move_to_rightmost_position();

  uint8_t move_to_leftmost_position();

  uint8_t get_current_position();

  bool poll();

  int go_to(uint8_t);

  int resolve_datapoint(const std::string&, uint32_t);
};


#endif
