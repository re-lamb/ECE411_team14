/*
 *  hardware.h - Buttons and pin assignments
 *
 *  Abstract:
 *      Defines the mapping of IO pins to the hardware as laid down
 *      on the GameMan PCB rev A3.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#ifndef _GM_HARD_H_
#define _GM_HARD_H_

// Should match the schematic and PCB layout!
#define HW_VERSION  "B1"

// Pins used for software SPI (display connection)
#define OLED_CLK 5
#define OLED_MOSI 18

// Used for software or hardware SPI
#define OLED_CS 17
#define OLED_DC 16

// Used for I2C or SPI
#define OLED_RESET 21

// Define the GPIO pins for the direction pad, a/b/c buttons
#define IO_BTN_N  25    // north == up    -> pin labeled A1 on the Huzzah32
#define IO_BTN_E  26    // east == right  -> pin A0
#define IO_BTN_S  27    // south == down  -> pin IO27
#define IO_BTN_W  14    // west == left   -> pin IO14

#define IO_BTN_A  32    // -> pin IO32 on the Huzzah32
#define IO_BTN_B  33    // -> pin IO33
#define IO_BTN_C   4    // -> pin A5

// The ESP32-WROOM-32E has voltage sensing built in!
#define VBAT_SENSE  35

#endif
