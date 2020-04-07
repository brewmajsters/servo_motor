#ifndef SERVO_SERVO_MOTOR_H
#define SERVO_SERVO_MOTOR_H

#include <Servo.h>

// ERROR CODES
static const int SERVO_IS_NOT_ATTACHED = 1;

class Servo_motor : Servo {
private:
  Servo *servo = nullptr;
  const int leftmost_pos_deg;
  const int rightmost_pos_deg;
public:
  Servo_motor(const int rightmost_position_degree, const int leftmost_position_degree);
  ~Servo_motor();
  int rightmost_position();
  int leftmost_position();
  int get_position_degree();
};


#endif
