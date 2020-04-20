#ifndef SERVO_SERVO_MOTOR_H
#define SERVO_SERVO_MOTOR_H

#include <Servo.h>

// ERROR CODES
static const uint8_t SERVO_IS_NOT_ATTACHED = 255;

class Servo_motor : Servo {
private:
  Servo servo;
  const std::string uuid;
  const uint8_t pin;
  const uint16_t poll_rate;
  const uint8_t leftmost_pos_deg;
  const uint8_t rightmost_pos_deg;
public:
  Servo_motor(const std::string uuid, uint8_t pin, uint16_t poll_rate, uint8_t rightmost_position_degree = 0,
              uint8_t leftmost_position_degree = 180);

  const std::string get_uuid();

  uint8_t get_pin();

  uint8_t move_to_rightmost_position();

  uint8_t move_to_leftmost_position();

  uint8_t get_current_position();
};


#endif
