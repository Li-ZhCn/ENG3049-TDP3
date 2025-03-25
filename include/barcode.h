#ifndef BARCODE_H
#define BARCODE_H

#include "mbed.h"

class Barcode {
private:
  DigitalIn bcs1;
  DigitalIn bcs2;
  DigitalIn bcs3;

  int bcs1_value;
  int bcs2_value;
  int bcs3_value;

  int state_;

  void performTask();

public:
  Barcode(PinName bcs1, PinName bcs2, PinName bcs3);
  virtual ~Barcode();

  void reset();

  void showBCS();
  enum State : int { none, bww, bwb, bbw, bbb, wbw, wbb, wwb};
  State state;
  void updateState();
};

#endif