#include "PID.h"
#include "mbed.h"

PID::PID() { printf("PID initialzed successfully\n"); }

PID::~PID() {}

void PID::init(float Kp, float Ki, float Kd, float int_atten) {
  PID::Kp = Kp;
  PID::Ki = Ki;
  PID::Kd = Kd;
  PID::int_atten = int_atten;

  error = 0.0;

  integral = 0.0;
  derivative = 0.0;

  delta_v = 0.0;
}
float PID::getError(int lfs1, int lfs2, int lfs3) {
  c = lfs3 + lfs2 * 2 + lfs1 * 4;

  //   printf("\n in the PID, c = %d\n", c);

  float current_error;

  switch (c) {
  case 5:
    current_error = error * 0.5;
    break;
  case 1:
    current_error = -1;
    break;
  case 3:
    current_error = -1;
    break;
  case 6:
    current_error = 1;
  case 4:
    current_error = 1;
    break;
  case 7:
    current_error = error;
    break;
  case 0:
    current_error = error;
    break;
  }

  return current_error;
}

void PID::updatePID(int lfs1, int lfs2, int lfs3) {
  float current_error = getError(lfs1, lfs2, lfs3);

  integral = current_error + int_atten * integral;
  derivative = current_error - error;

  error = current_error;
  delta_v = Kp * error + Ki * integral + Kd * derivative;
}

void PID::resetPID() {
  error = 0.0;

  integral = 0.0;
  derivative = 0.0;

  delta_v = 0.0;
}

void PID::setPID(float Kp_, float Ki_, float Kd_) {
  Kp = Kp_;
  Ki = Ki_;
  Kd = Kd_;
}