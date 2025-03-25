/* mbed Microcontroller Library
 * Copyright (key) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

// #include "MMA8451Q.h"
#include "PID.h"
#include "barcode.h"
#include "helper.h"
#include "mbed.h"
#include "motor.h"

/********************* Define all the pin names********************/
// For the accerometer
PinName const acc_SDA = PTE25;
PinName const acc_SCL = PTE24;

// For the motor
PinName const m_left_forward_ctrl = D2;
PinName const m_left_backward_ctrl = D3;

PinName const m_right_forward_ctrl = D5;
PinName const m_right_backward_ctrl = D4;

PinName const motor_enable = D13;

// For the line-following sensor
PinName const lfs1_p = PTE30;
PinName const lfs2_p = PTE29;
PinName const lfs3_p = PTE23;

// // For the barcode sensor
PinName const bcs1_p = PTE20;
PinName const bcs2_p = PTE21;
PinName const bcs3_p = PTE22;

/**************************** Define all the constants*****************/
#define LOOP_LENGTH 50ms

#define MMA8451_I2C_ADDRESS (0x1d << 1)

#define MEM_SIZE 50

#define KEY_LED_R 'r'
#define KEY_ACC 'w'
#define KEY_DEACC 's'
#define KEY_L 'a'
#define KEY_R 'd'
#define KEY_MEM_UPDATING 'u'
#define KEY_MEM_LOADING 'l'
#define KEY_Q 'q'
#define KEY_LF_PID 'f'

#define KEY_CORRECTION 'c'

#define KEY_Kp_PLUS '1'
#define KEY_Kp_MINUS '2'
#define KEY_Ki_PLUS '3'
#define KEY_Ki_MINUS '4'
#define KEY_Kd_PLUS '5'
#define KEY_Kd_MINUS '6'
#define KEY_v0_PLUS '7'
#define KEY_v0_MINUS '8'

#define KEY_NULL '0'

enum CtrlMode {
  FREE_CTRL,
  MEM_UPDATING, // green
  MEM_LOADING,  // blue
  LF_PID,       // red
  DISABLED
};

/***************************** Define all the pin ********************/
DigitalOut red_led(LED1);
DigitalOut green_led(LED2);
DigitalOut blue_led(LED3);

// DigitalOut green_led(LED2);
// DigitalOut blue_led(LED3);

// MMA8451Q acc(acc_SDA, acc_SCL, MMA8451_I2C_ADDRESS);

static Motor motor(m_left_forward_ctrl, m_left_backward_ctrl,
                   m_right_forward_ctrl, m_right_backward_ctrl);

static BufferedSerial uart(USBTX, USBRX);

DigitalIn lfs1(lfs1_p);
DigitalIn lfs2(lfs2_p);
DigitalIn lfs3(lfs3_p);

Barcode bcs(bcs1_p, bcs2_p, bcs3_p);

// DigitalIn
DigitalIn force_mem_loading_input(D10, PullUp);

/****************************** Main *********************************/

int main() {
  char input;
  char key;
  //   float x, y, z;
  float left_v = 0;
  float right_v = 0;

  enum CtrlMode mode = FREE_CTRL;

  uint8_t mem_index = 0;

  int8_t n_force_print = 0;
  bool is_first_loop_mem_loading = true;

  float const acc = 0.1; // linear acc
  float ang_acc = 0.4;   // angular acc

  // For PID: 0.2; 0.01; 0.002;
  // For PID: 0.01; 0.012; 0.001; v0 = 0.018; 0305 update for fully charged
  // battery
  float v0 = 0.018;
  float Kp = 0.0100;
  float Ki = 0.0120;
  float Kd = 0.0010;
  float int_atten = 0.9;

  bool lfs1_value = 1;
  bool lfs2_value = 1;
  bool lfs3_value = 1;

  /*
    w: acc
    s: deacc
    a: turn left a certain angle
    d: turn right a certain angle
    q: stop and quit
  */
  char mem_buffer[MEM_SIZE] = "wwssq";

  printf("all initailization finished successfully\n");
  //   green_led = 1;
  PID pid = PID();

  /*************************** LOOP ***************************************/
  while (true) {
    bcs.updateState();

    // x = abs(acc.getAccX());
    // y = abs(acc.getAccY());
    // z = abs(acc.getAccZ());
    // printf("accX: %1.2f, accY: %1.2f, accZ: %1.2f\n", x, y, z);

    /************************** UPDATE INPUT ********************************/

    // if update key by keyboard input
    if (uart.readable()) {
      uart.read(&key, 1);
    } else {
      key = '0';
    }

    if (lfs1_value == lfs1 or lfs2_value == lfs2 or lfs3_value == lfs3) {
      n_force_print = 1;
    }

    lfs1_value = !lfs1;
    lfs2_value = !lfs2;
    lfs3_value = !lfs3;

    /************************** UPDATE MODE ********************************/

    switch (mode) {
    case FREE_CTRL: {
      red_led = 1;
      green_led = 1;
      blue_led = 1;

      break;
    }
    case MEM_LOADING: {
      blue_led = 0;

      // if update key by mem
      key = mem_buffer[mem_index];
      mem_index += 1;
      printf("mem index %d\t", mem_index);
      // if out of index
      if (mem_index == MEM_SIZE - 1) {
        // is_mem_loading = false;
        mode = FREE_CTRL;

        blue_led = 1;
        mem_index = 0;
        // print mem_buffer at the end
        printf("\n");
        for (int i = 0; i < MEM_SIZE; i++) {
          printf("%c", mem_buffer[i]);
        }
        printf("\n");
      }
      break;
    }
    case MEM_UPDATING: {
      green_led = 0;

      mem_buffer[mem_index] = key;
      mem_index += 1;
      printf("mem index %d\t", mem_index);
      n_force_print = 2;

      if (mem_index == MEM_SIZE - 1) {
        mode = FREE_CTRL;
        // is_mem_updating = false;

        green_led = 1;
        mem_buffer[mem_index] = 'q';
        mem_index = 0;
        // print mem_buffer at the end
        for (int i = 0; i < MEM_SIZE; i++) {
          printf("%c", mem_buffer[i]);
        }
        printf("\n");
      }
      break;
    }
    case LF_PID: {
      red_led = 0;

      pid.updatePID(lfs1_value, lfs2_value, lfs3_value);

      left_v = v0 + pid.delta_v;
      right_v = v0 - pid.delta_v;

      if (bcs.state == Barcode::bwb or bcs.state == Barcode::bww or
          bcs.state == Barcode::wwb) {
        left_v = 0.01;
        right_v = 0.01;
      }
      //   bcs.updateState();

      //   key = '0';
      break;
    }

    case DISABLED: {
      red_led = 1;
      blue_led = 1;
      green_led = 1;

      key = '0';
      break;
    }
    }

    /**************************** OUTPUT ***********************************/
    switch (key) {
    /********************* READ LED *************************/
    case KEY_LED_R: {
      printf("red_led = %d\t", red_led.read());
    }
    /********************* CONTROL MOTOR *************************/
    case KEY_ACC: {
      left_v += acc;
      right_v += acc;
      break;
    }
    case KEY_DEACC: {
      left_v -= acc;
      right_v -= acc;
      break;
    }
    case KEY_L: {
      left_v -= ang_acc;
      right_v += ang_acc;
      n_force_print = 2;
      break;
    }
    case KEY_R: {
      left_v += ang_acc;
      right_v -= ang_acc;
      n_force_print = 2;
      break;
    }
    case KEY_CORRECTION: {
      if (left_v > right_v) {
        right_v = left_v;
      } else {
        left_v = right_v;
      }
    }
    case KEY_Q: {
      bcs.reset();

      mode = FREE_CTRL;
      mem_index = 0;

      left_v = 0;
      right_v = 0;
      break;
    }
    /********************* CONTROL MEM *************************/
    case KEY_MEM_UPDATING: {
      left_v = 0;
      right_v = 0;

      if (mode == MEM_UPDATING) {
        mode = FREE_CTRL;
      } else {
        mode = MEM_UPDATING;
      }

      // if try to set mem, clear mem_buffer first
      if (mode == MEM_UPDATING) {
        memset(&mem_buffer[0], 0, sizeof(mem_buffer));
        mem_index = 0;
      } else {
        // if cease to mem updating mode
        // is_mem_updating = false;
        red_led = 1;
        mem_buffer[mem_index - 1] = 'q';
        mem_index = 0;
        // print mem_buffer at the end
        printf("\n");
        for (int i = 0; i < MEM_SIZE; i++) {
          printf("%c", mem_buffer[i]);
        }
        printf("\n");
      }

      break;
    }
    case KEY_MEM_LOADING: {
      left_v = 0;
      right_v = 0;

      mem_index = 0;
      mode = MEM_LOADING;

      break;
    }
    /********************* LINE FOLLOWING *************************/
    case KEY_LF_PID: {
      if (mode != LF_PID) {
        mode = LF_PID;
        pid.init(Kp, Ki, Kd, int_atten);
        bcs.reset();
      } else if (mode == LF_PID) {
        mode = FREE_CTRL;
        left_v = 0;
        right_v = 0;
        bcs.reset();
      }

      break;
    }

    /********************* TESTING *************************/
    case KEY_Kp_PLUS: {
      Kp += 0.001;
      pid.setPID(Kp, Ki, Kd);
      break;
    }
    case KEY_Kp_MINUS: {
      Kp -= 0.001;
      pid.setPID(Kp, Ki, Kd);
      break;
    }

    case KEY_Ki_PLUS: {
      Ki += 0.0001;
      pid.setPID(Kp, Ki, Kd);
      break;
    }
    case KEY_Ki_MINUS: {
      Ki -= 0.0001;
      pid.setPID(Kp, Ki, Kd);
      break;
    }

    case KEY_Kd_PLUS: {
      Kd += 0.001;
      pid.setPID(Kp, Ki, Kd);
      break;
    }
    case KEY_Kd_MINUS: {
      Kd -= 0.001;
      pid.setPID(Kp, Ki, Kd);
      break;
    }
    case KEY_v0_PLUS: {
      v0 += 0.001;
      break;
    }
    case KEY_v0_MINUS: {
      v0 -= 0.001;
      break;
    }
    case KEY_NULL: {
      break;
    }
    }

    // force to load the mem whatever the input
    // printf("%d\t", force_mem_loading_input.read());

    if (force_mem_loading_input.read() == 0 or
        is_first_loop_mem_loading == true) {
      // maybe we can give it more time
      ThisThread::sleep_for(LOOP_LENGTH);

      // before loading memory, stop all the motion first
      is_first_loop_mem_loading = false;
      left_v = 0;
      right_v = 0;

      mem_index = 0;
      mode = MEM_LOADING;

      printf("force to go!\t");
    }

    /**************************** UPDATE MOTORS *****************************/
    left_v = clamp(left_v, -1.0f, 1.0f);
    right_v = clamp(right_v, -1.0f, 1.0f);
    motor.setDutycycle('A', left_v, right_v);

    /**************************** TEST LOG *****************************/

    if (key != '0' or n_force_print) {
      printf("input: %c, mode: %d,\t", key, mode);
    //   printf("debug: %d, %d, %d (1e-4), %d m\t", int(Kp * 10000), int(Ki * 10000), int(Kd * 10000), int(v0 * 1000));

      //   if (mode == LF_PID) {
      //     printf("\n c: %d, error: %d, dv: %d, \n", pid.c, int(pid.error *
      //     1000),
      //            int(pid.delta_v * 1000));
      //   }
      //   int value_lfs1 = lfs1;
    //   printf("lf_sensors: %d, %d, %d,\t", lfs1_value, lfs2_value, lfs3_value);

    //   printf("left_v = %dm\t right_v = %dm\n", int(left_v * 1000), int(right_v * 1000));

      n_force_print -= 1;
      if (n_force_print <= 0) {
        n_force_print = 0;
      }
    }

    ThisThread::sleep_for(LOOP_LENGTH);
  }
}