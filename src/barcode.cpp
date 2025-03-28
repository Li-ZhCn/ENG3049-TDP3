#include "barcode.h"

Barcode::Barcode(PinName bcs1, PinName bcs2, PinName bcs3)
    : bcs1(bcs1), bcs2(bcs2), bcs3(bcs3) {
  state = none;
  bcs_value = -1;
  task = 0;
  delayed_snapshot_loop = -1;
  printf("barcode sensors initialized successfully\n");
}

Barcode::~Barcode() {}

void Barcode::reset() {
  printf("reset bcs\n");
  state = none;
  task = 0;
}

void Barcode::updateBCS() {
  bcs1_value = !bcs1.read();
  bcs2_value = !bcs2.read();
  bcs3_value = !bcs3.read();
  //   int new_bcs_value = bcs3_value + bcs2_value * 2 + bcs1_value * 4;

  //   printf("%d", new_bcs_value);
}

void Barcode::performTask() {
  switch (task) {
  case 0: {
    break;
  }
  case 1: {
    ThisThread::sleep_for(1s);
    printf("\n over 1s sleep \n");
    task = 0;
    break;
  }
  case 2: {
    ThisThread::sleep_for(5s);
    printf("\n over 5s sleep \n");
    task = 0;
    break;
  }
  case 3: {
    ThisThread::sleep_for(10s);
    printf("\n over 10s sleep \n");
    task = 0;
    break;
  }
  case 4: {
    ThisThread::sleep_for(15s);
    printf("\n over 15s sleep \n");
    task = 0;
    break;
  }
  default: {
    break;
  }
    state = none;
    reset();
  }
}

void Barcode::updateState() {
  updateBCS();

  int new_bcs_value = bcs3_value + bcs2_value * 2 + bcs1_value * 4;

  if (new_bcs_value == bcs_value) {
    return;
  }

  bcs_value = new_bcs_value;

  switch (bcs_value) {
    // A1 011 BWW
  case 3: {
    state = A1_bww;
    break;
  }
  // B1/D2 101 WBW
  case 5: {
    if (state == C1_wwb) {
      state = D2_wbw;
      printf("\n transit to D2 WBW and sleep for 5s (BWB)\n");
      task = 2;
      break;
    }
    state = B1_wbw;
    printf("\n transit to B1 WBW \n");
    break;
  }
  // B2 001 BBW
  case 1: {
    state = B2_bbw;
    printf("\n transit to B2 BBW \n");
    break;
  }
  // C1 110 WWB
  case 6: {
    if (state == B1_wbw) {
      state = C1_wwb;
      printf("\n transit to C1 WWB \n");
    } else if (state == C4_bbb) {
      state = D3_wwb;
      printf("\n transit to D3 WWB and sleep for 10s (WBB) \n");
      task = 3;
    } else {
      printf("\n bcs_value: wwb \n");
    }
    break;
  }
  // C2 010 BWB
  case 2: {
    if (state == B1_wbw or state == C1_wwb) {
      state = C2_bwb;
      printf("\n transit to C2 BWB and sleep for 5s (BWB) \n");
      task = 2;
    } else {
      printf("\n bcs_value: bwb \n");
    }
    break;
  }
  // C3 100 WBB
  case 4: {
    if (state == B2_bbw) {
      state = C3_wbb;
      printf("\n transit to C3 WBB and sleep for 10s (WBB) \n");
      task = 3;
    } else if (state == C4_bbb) {
      state = D4_wbb;
      printf("\n transit to D4 WBB and sleep for 15s (BBB) \n");
      task = 4;
    } else {
      printf("\n bcs_value: wbb \n");
    }
    break;
  }
  // C4 000 BBB
  case 0: {
    if (state == B2_bbw) {
      state = C4_bbb;
      printf("\n transit to C4 BBB \n");
      //   task = 4;
    } else {
      printf("\n bcs_value: bbb \n");
    }
    break;
  }
  // D1 111 WWW
  case 7: {
    if (state == C1_wwb) {
      state = D1_www;
      printf("\n transit to D1 WWW and sleep for 1s (WWB) \n");
      task = 1;
    }
  }
  default: {
    printf("\n bcs_value: %d \n", bcs_value);
  }
  }
}

void Barcode::updateState2() {
  updateBCS();

  int new_bcs_value = bcs3_value + bcs2_value * 2 + bcs1_value * 4;
  if (new_bcs_value == bcs_value) {
    // if bcs_value not change, decrement delayed_snapshot_loop until 0.
    if (delayed_snapshot_loop > 0) {
      delayed_snapshot_loop -= 1;
      return;
    } else if (delayed_snapshot_loop == 0) {
      delayed_snapshot = true;
      delayed_snapshot_loop = -1;
    } else {
      return;
    }
  } else {
    // if bcs_value change, get a delayed snapshot and update value.
    delayed_snapshot_loop = 2;
    bcs_value = new_bcs_value;
    return;
  }

  if (delayed_snapshot == true) {
    // step on FSM
    switch (new_bcs_value) {
      // A1 011 BWW
    case 3: {
      state = A1_bww;
      break;
    }
    // B1 101 WBW
    case 5: {
      state = B1_wbw;
      printf("\n transit to B1 WBW \n");
      break;
    }
    // B2 001 BBW
    case 1: {
      state = B2_bbw;
      printf("\n transit to B2 BBW \n");
      break;
    }
    // C1 110 WWB
    case 6: {
      if (state == B1_wbw) {
        state = C1_wwb;
        printf("\n transit to C1 WWB and sleep for 1s (WWB) \n");
        task = 1;
      } else {
        printf("\n bcs_value: wwb \n");
      }
      break;
    }
    // C2 010 BWB
    case 2: {
      if (state == B1_wbw) {
        state = C2_bwb;
        printf("\n transit to C2 BWB and sleep for 5s (BWB) \n");
        task = 2;
      } else {
        printf("\n bcs_value: bwb \n");
      }
      break;
    }
    // C3 100 WBB
    case 4: {
      if (state == B2_bbw) {
        state = C3_wbb;
        printf("\ntransit to C3 WBB and sleep for 10s (WBB) \n");
        task = 3;
      } else {
        printf("\n bcs_value: wbb \n");
      }
      break;
    }
    // C4 000 BBB
    case 0: {
      if (state == B2_bbw) {
        state = C4_bbb;
        printf("\n transit to C4 BBB and sleep for 15s (BBB) \n");
        task = 4;
      } else {
        printf("\n bcs_value: bbb \n");
      }
      break;
    }
    default: {
      printf("\n bcs_value: %d \n", bcs_value);
    }
    }

    delayed_snapshot = false;
  }
}
