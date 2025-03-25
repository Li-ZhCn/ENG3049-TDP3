#include "barcode.h"

Barcode::Barcode(PinName bcs1, PinName bcs2, PinName bcs3)
    : bcs1(bcs1), bcs2(bcs2), bcs3(bcs3) {
  state = none;
  state_ = -1;
  printf("barcode sensors initialized successfully\n");
}

Barcode::~Barcode() {}

void Barcode::reset() {
  printf("reset bcs\n");
  state_ = -1;
  state = none;
}

void Barcode::showBCS() {
  bcs1_value = !bcs1.read();
  bcs2_value = !bcs2.read();
  bcs3_value = !bcs3.read();

  printf("bcs: %d, %d, %d\n", bcs1_value, bcs2_value, bcs3_value);
}

void Barcode::performTask() {
  switch (state) {
  case none: {
    break;
  }
  case wwb: {
    ThisThread::sleep_for(1s);
    printf("\n over 1s sleep \n");
    state = none;
    break;
  }
  case bwb: {
    ThisThread::sleep_for(1s);
    printf("\n over 5s sleep \n");
    state = none;
    break;
  }
  case wbb: {
    ThisThread::sleep_for(1s);
    printf("\n over 10s sleep \n");
    state = none;
    break;
  }
  case bbb: {
    ThisThread::sleep_for(1s);
    printf("\n over 15s sleep \n");
    state = none;
    break;
  }
  default: {
    break;
  }
  }
}

void Barcode::updateState() {
  bcs1_value = !bcs1.read();
  bcs2_value = !bcs2.read();
  bcs3_value = !bcs3.read();

  int new_state_ = bcs3_value + bcs2_value * 2 + bcs1_value * 4;
  //   printf("\n new state: %d \n", new_state_); // no this!!!!

  if (new_state_ == state_) {
    return;
  }

  switch (new_state_) {
    // 011 BWW
  case 3: {
    state = bww;
    break;
  }
  // 101 WBW
  case 5: {
    state = wbw;
    printf("\ntransit to WBW\n");
    break;
  }
  // 001 BBW
  case 1: {
    state = bbw;
    printf("\ntransit to BBW\n");
    break;
  }
  // 110 WWB
  case 6: {
    if (state == wbw) {
      state = wwb;
      printf("\ntransit to WWB and sleep for 1s\n");
    } else {
      printf("\n state_: wwb \n");
    }
    break;
  }
  // 010 wbw
  case 2: {
    if (state == wbw) {
      state = bwb;
      printf("\ntransit to BWB and sleep for 5s\n");
    } else {
      printf("\n state_: bwb \n");
    }
    break;
  }
  // wbb 100
  case 4: {
    if (state == bbw) {
      state = wbb;
      printf("\ntransit to WBB and sleep for 10s\n");
    } else {
      printf("\n state_: wbb \n");
    }
    break;
  }
  // 000 bbb
  case 0: {
    printf("\n state_: bbb \n");
    if (state == bbw) {
      state = bbb;
      printf("\ntransit to BBB and sleep for 15s\n");
    }
    break;
  }
  default: {
    printf("\n state_: %d \n", state_);
  }
  }

  state_ = new_state_;

  performTask();
}
