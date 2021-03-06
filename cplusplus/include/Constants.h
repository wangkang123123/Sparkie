#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

/** BUTTONS */
const uint8_t RED_BTN = 20;
const uint8_t BLUE_BTN = 19;
const uint8_t GREEN_BTN = 18;
const uint8_t ORANGE_BTN = 21;

/** LED indicators */
const uint8_t GREEN_LED = 12;
const uint8_t BLUE_LED = 11;
const uint8_t RED_LED = 10;
const uint8_t ORANGE_LED = 9;

/** Hardware serials for Odrive */
#define FRONT_RIGHT_SERIAL Serial1 // RX, TX (0, 1)
#define BACK_RIGHT_SERIAL Serial2  // RX, TX (7, 8)
#define FRONT_LEFT_SERIAL Serial3  // RX, TX (15, 14)
#define BACK_LEFT_SERIAL Serial4   // RX, TX (16, 17)

/** ROS Serial */
#define BAUDRATE 921600 // Bits per second

#endif // _CONSTANTS_H_
