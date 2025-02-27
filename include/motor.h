#ifndef MOTOR_H
#define MOTOR_H

// #include "encoder.h"
// #include "helper.h"
#include "mbed.h"
#include <utility>

/** @brief Fast duty cycle constant.*/
#define FAST_PWM 0.6
/** @brief Half duty cycle constant.*/
#define HALF_PWM 0.5
/** @brief Slow duty cycle constant.*/
#define SLOW_PWM 0.3

/**
 * @brief Motor class to control the motors.
 * @details This class includes configuration method of left and right motors.
 * It also features cruising methods to move forward, backwards, turn left and
 * turn left.
 *
 */
class Motor {
private:
  /** @brief Speed for the left motor to go forward. */
  PwmOut m_left_forward;

  /** @brief Speed for the left motor to go backward. */
  PwmOut m_left_backward;

  /** @brief Speed for the left motor to go forward. */
  PwmOut m_right_forward;

  /** @brief Speed for the left motor to go backward. */
  PwmOut m_right_backward;

public:
  /** @brief Left motor speed.*/
  float left_motor_v;

  /** @brief Right motor speed.*/
  float right_motor_v;

  /**
   * @brief Constructor.
   *
   * Construct a motor object.
   * Using unipolar mode (0), pins are sunk to ground instead of using
   * DigitalOut.write(0).
   * @param enable Enable pin.
   */
  Motor(PinName m_left_forward, PinName m_left_backward,
        PinName m_right_forward, PinName m_right_backward);

  void setDutycycle(char c, float dutycycle);

  /**
   * @brief Set the duty cycle of the motors.
   *
   * @param c Which motor to set duty cycle.
   * - 'L' Left motor.
   * - 'R' Right motor.
   * - otherwise both motors.
   * @param left_dutycycle Duty cycle of left motor in percentange within
   * [-1.0,1.0]. If duty cycle is out of range, it saturates at -1 and 1.
   * @param right_dutycycle Duty cycle of right motor in percentange within
   * [-1.0,1.0]. If duty cycle is out of range, it saturates at -1 and 1.
   *
   */
  void setDutycycle(char c, float left_dutycycle, float right_dutycycle);

  /**
   * @brief Get the duty cycle of the motors.
   *
   * @return std::pair<float,float> {Left motor duty cycle, Right motor duty
   * cycle}.
   */
  std::pair<float, float> getDutycycle();
};

#endif
