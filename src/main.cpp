/* mbed Microcontroller Library
 * Copyright (key) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

// #include "MMA8451Q.h"
#include "PID.h"
#include "helper.h"
#include "mbed.h"
#include "motor.h"
#include <cstdint>

/********************* Define all the pin names********************/
// For the accerometer
PinName const acc_SDA = PTE25;
PinName const acc_SCL = PTE24;

// For the motor
PinName const m_left_forward_ctrl = D5;
PinName const m_left_backward_ctrl = D4;

PinName const m_right_forward_ctrl = D2;
PinName const m_right_backward_ctrl = D3;

PinName const motor_enable = D13;

// For the line-following sensor
PinName const lfs1_p = D7;
PinName const lfs2_p = D8;
PinName const lfs3_p = D9;

// // For the barcode sensor
// PinName const bcs1_p = NC;
// PinName const bcs2_p = NC;
// PinName const bcs3_p = NC;

float const Kp = 0.5;
float const Ki = 0;
float const Kd = 0;
float const int_atten = 0;

/**************************** Define all the constants*****************/
#define LOOP_LENGTH 20ms

#define MMA8451_I2C_ADDRESS (0x1d << 1)

#define ACC 0.5         // linear acc
#define ANGULAR_ACC 0.2 // angular acc

#define MEM_SIZE 50

#define KEY_LED_R 'r'
#define KEY_ACC 'w'
#define KEY_DEACC 's'
#define KEY_L 'a'
#define KEY_R 'd'
#define KEY_MEM_UPDATING 'u'
#define KEY_MEM_LOADING 'l'
#define KEY_Q 'q'
#define KEY_LF 'f'
#define KEY_NULL '0'

enum CtrlMode {
  FREE_CTRL,
  MEM_UPDATING,         // green
  MEM_LOADING,          // blue
  LINE_FOLLOWING,       // red
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

// DigitalIn bcs1(bcs1_p);
// DigitalIn bcs2(bcs2_p);
// DigitalIn bcs3(bcs3_p);

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

  /*
    w: acc
    s: deacc
    a: turn left a certain angle
    d: turn right a certain angle
    q: stop and quit
  */
  char mem_buffer[MEM_SIZE] = "wwwaaaaaassssssdddddwwwwwwwwwwq";

  printf("successfully initailized\n");
  //   green_led = 1;

  /*************************** LOOP ***************************************/
  while (true) {
    PID pid = PID();
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
    case LINE_FOLLOWING: {
      red_led = 0;

      //   pid.updatePID(lfs1, lfs2, lfs3);
      //   left_v += pid.delta_v;
      //   right_v -= pid.delta_v;

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
      left_v += ACC;
      right_v += ACC;
      break;
    }
    case KEY_DEACC: {
      left_v -= ACC;
      right_v -= ACC;
      break;
    }
    case KEY_L: {
      left_v -= ANGULAR_ACC;
      right_v += ANGULAR_ACC;
      n_force_print = 2;
      break;
    }
    case KEY_R: {
      left_v += ANGULAR_ACC;
      right_v -= ANGULAR_ACC;
      n_force_print = 2;
      break;
    }
    case KEY_Q: {
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
    case KEY_LF: {
      if (mode != LINE_FOLLOWING) {
        mode = LINE_FOLLOWING;
        pid.init(Kp, Ki, Kd, int_atten);
      } else {
        mode = FREE_CTRL;
      }

      break;
    }
    case KEY_NULL: {
      red_led = 1;
      // if input is null, then cancel the voltage differnece to keep
      // the car going straightforward. choose the ave.
      right_v = (left_v + right_v) / 2;
      left_v = right_v;
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

    if (key != '0' or n_force_print) {
      printf("input: %c, mode: %d,\t", key, mode);
      printf("lf_sensors: %d, %d, %d,\t", lfs1.read(), lfs2.read(),
             lfs3.read());

      printf("left_v = %dm\t right_v = %dm\n", int(left_v * 1000),
             int(right_v * 1000));

      n_force_print -= 1;
      if (n_force_print <= 0) {
        n_force_print = 0;
      }
    }

    ThisThread::sleep_for(LOOP_LENGTH);
  }
}