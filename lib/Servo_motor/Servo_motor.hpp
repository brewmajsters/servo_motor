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
  Servo_motor(std::string, uint8_t, uint16_t, uint8_t = 0, uint8_t = 180);

  std::string get_uuid();

  uint8_t move_to_rightmost_position();

  uint8_t move_to_leftmost_position();

  uint8_t get_current_position();

  bool poll();
};


#endif
