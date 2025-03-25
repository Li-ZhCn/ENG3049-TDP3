#include "motor.h"
#include "helper.h"

Motor::Motor(PinName m_left_forward, PinName m_left_backward,
             PinName m_right_forward, PinName m_right_backward)
    : m_left_forward(m_left_forward), m_left_backward(m_left_backward),
      m_right_forward(m_right_forward), m_right_backward(m_right_backward) {
  printf("motor initialzed succesfully\n");
  setDutycycle('A', 0);
}

void Motor::setDutycycle(char c, float dutycycle) {
  dutycycle = clamp(dutycycle, -1.0f, 1.0f);
  if (dutycycle >= 0) {
    /************* If going forward ****************/
    if (c == 'L') {
      m_left_forward.write(dutycycle);
      m_left_backward.write(0);
    } else if (c == 'R') {
      m_right_forward.write(dutycycle);
      m_right_backward.write(0);
    } else {
      m_left_forward.write(dutycycle);
      m_right_forward.write(dutycycle);
      m_left_backward.write(0);
      m_right_backward.write(0);
    }
  } else {
    /************* If going backward *************/
    if (c == 'L') {
      m_left_forward.write(0);
      m_left_backward.write(-dutycycle);
    } else if (c == 'R') {
      m_right_forward.write(0);
      m_right_backward.write(-dutycycle);
    } else {
      m_left_forward.write(0);
      m_right_forward.write(0);
      m_left_backward.write(-dutycycle);
      m_right_backward.write(-dutycycle);
    }
  }
}

void Motor::setDutycycle(char c, float left_dutycycle, float right_dutycycle) {
  left_dutycycle = clamp(left_dutycycle, -1.0f, 1.0f);
  right_dutycycle = clamp(right_dutycycle, -1.0f, 1.0f);
  setDutycycle('L', left_dutycycle);
  setDutycycle('R', right_dutycycle);
}
