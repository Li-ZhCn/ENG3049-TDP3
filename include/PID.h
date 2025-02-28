#ifndef PID_H
#define PID_H

class PID {
public:
  int c;

  float error;
  float delta_v;

  PID();

  virtual ~PID();

  void init(float Kp, float Ki, float Kd, float int_atten);

  void updatePID(int lfs1, int lfs2, int lfs3);

  void resetPID();

  void setPID(float Kp, float Ki, float Kd);

private:
  float Kp;
  float Ki;
  float Kd;

  float int_atten;

  float integral;
  float derivative;

  float getError(int lfs1, int lfs2, int lfs3);
};

#endif /* PID_H */