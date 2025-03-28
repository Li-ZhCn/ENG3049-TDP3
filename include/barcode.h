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

  int bcs_value;

  int delayed_snapshot_loop;
  bool delayed_snapshot;

public:
  Barcode(PinName bcs1, PinName bcs2, PinName bcs3);
  virtual ~Barcode();

  void reset();

  void updateBCS();
  enum State : int {
    none,
    A1_bww,
    B1_wbw,
    B2_bbw,
    C1_wwb,
    C2_bwb,
    C3_wbb,
    C4_bbb,
    D1_www,
    D2_wbw,
    D3_wwb,
    D4_wbb
  };
  State state;
  int task = 0;
  void updateState();
  void updateState2();

  void performTask();
};

#endif