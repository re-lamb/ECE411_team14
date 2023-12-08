/*
 *  graphics.h - Basic bitmaps/icons/splash screen
 *
 *  Abstract:
 *      Contains one-bit monochrome (and 4bpp gray?) bitmaps in
 *      a C array format, compiled in rather than loaded from a
 *      separate flash partition.  Less flexible but easier to
 *      manage!
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#ifndef _GM_GFX_H_
#define _GM_GFX_H_

#define SPLASH_SM   32
#define SPLASH_MED  64
#define SPLASH_LG   128

#define WHITE       0x0F    // SSD1327_WHITE
#define HALF_BRIGHT 0x07    // should be a medium gray?
#define DARK_GRAY   0x03    // about 3/4 dark?  season to taste
#define BLACK       0x00    // SSD1327_BLACK

// "About" menu icon
static const uint8_t PROGMEM about16_bmp[] = {
  0xff, 0xff, 0xf8, 0x1f, 0xf6, 0x0f, 0xe8, 0x07, 
  0xc0, 0x03, 0xc7, 0xf3, 0xdf, 0x3d, 0xfc, 0x3d, 
  0xdc, 0xb9, 0xc7, 0xf3, 0xc1, 0x83, 0xe0, 0x07, 
  0xf0, 0x6f, 0xfa, 0x0f, 0xfc, 0x7f, 0xff, 0xff
};

// "SysInfo" menu icon
static const uint8_t PROGMEM info16_bmp[] = {
  0xf8, 0x1f, 0xe3, 0xe7, 0xcf, 0x33, 0x9f, 0x19,
  0xbf, 0xfd, 0x3e, 0x3c, 0x7e, 0x3e, 0x7c, 0x7e,
  0x7c, 0x7e, 0x78, 0xfe, 0x38, 0xfe, 0xb8, 0xbd,
  0x9c, 0x79, 0xcf, 0xf3, 0xe3, 0xc7, 0xf8, 0x1f
};

// "TicTacToe" menu icon
static const uint8_t PROGMEM ttt16_bmp[] = {
  0xff, 0xff, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf,
  0x80, 0x01, 0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf,
  0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0x80, 0x01,
  0xfb, 0xdf, 0xfb, 0xdf, 0xfb, 0xdf, 0xff, 0xff
};

// "Battleship" menu icon
static const uint8_t PROGMEM bship16_bmp[] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xbf, 0xfe, 0xbf, 0xfe, 0x3f, 0xff, 0x3f,
  0xc3, 0x11, 0x78, 0x0f, 0x00, 0x00, 0x80, 0x00,
  0xc0, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// Mazewar mid-size
static const uint8_t PROGMEM maze32_bmp[] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xf0, 0x0f, 0xff, 0xff, 0xc4, 0x01, 0xff, 
  0xff, 0x3c, 0x00, 0xff, 0xfe, 0x78, 0x00, 0x7f, 
  0xfc, 0xc0, 0x00, 0x3f, 0xf9, 0x88, 0x00, 0x1f, 
  0xfa, 0x60, 0x00, 0x0f, 0xf0, 0x03, 0xe0, 0x0f, 
  0xf0, 0x1f, 0xfc, 0x0f, 0xf0, 0xfe, 0x3f, 0x07, 
  0xe3, 0xff, 0x9f, 0xc7, 0xe7, 0xfe, 0x4f, 0xf7, 
  0xef, 0xf5, 0x0f, 0xe7, 0xef, 0xf6, 0x3f, 0xd7, 
  0xe7, 0xfb, 0x4f, 0xe7, 0xf1, 0xf9, 0x9e, 0xc7, 
  0xf0, 0xfe, 0x3f, 0x07, 0xf0, 0x1f, 0xf8, 0x0f, 
  0xf0, 0x0f, 0xf0, 0x0f, 0xf8, 0x00, 0x00, 0x0f, 
  0xf8, 0x00, 0x00, 0x9f, 0xfc, 0x00, 0x03, 0x3f, 
  0xff, 0x00, 0x0e, 0xff, 0xff, 0x00, 0x3d, 0x5f, 
  0xff, 0x88, 0x04, 0xff, 0xff, 0xe2, 0x41, 0xff, 
  0xff, 0xf8, 0x0f, 0xff, 0xff, 0xfe, 0x5f, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// array size is 512
static const uint8_t PROGMEM maze64_bmp[] = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xe0, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x7f, 0xff, 0xff, 
  0xff, 0xff, 0xf0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x38, 0x00, 0x03, 0xff, 0xff, 
  0xff, 0xff, 0x8c, 0xf8, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x03, 0xe0, 0x00, 0x00, 0x7f, 0xff, 
  0xff, 0xfe, 0x1f, 0xe0, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x7f, 0x00, 0x00, 0x00, 0x1f, 0xff, 
  0xff, 0xf9, 0xf9, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf1, 0xe0, 0x80, 0x00, 0x00, 0x07, 0xff, 
  0xff, 0xe3, 0xc1, 0x80, 0x00, 0x00, 0x03, 0xff, 0xff, 0xc7, 0x60, 0x80, 0x00, 0x00, 0x01, 0xff, 
  0xff, 0xce, 0x38, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x8c, 0x18, 0x00, 0x00, 0x00, 0x00, 0xff, 
  0xff, 0x80, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, 0x7f, 
  0xff, 0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x7f, 0xfe, 0x00, 0x1f, 0xff, 0xff, 0xfc, 0x00, 0x7f, 
  0xfe, 0x00, 0x7f, 0xfc, 0x1f, 0xff, 0x00, 0x3f, 0xfe, 0x01, 0xff, 0xfa, 0x47, 0xff, 0xc0, 0x3f, 
  0xfe, 0x07, 0xff, 0xff, 0x43, 0xff, 0xf0, 0x3f, 0xfc, 0x1f, 0xff, 0xfe, 0xc9, 0xff, 0xfc, 0x1f, 
  0xfc, 0x1f, 0xff, 0xde, 0x31, 0xff, 0xf4, 0x1f, 0xfc, 0x7f, 0xff, 0x9e, 0x10, 0xff, 0x9f, 0x1f, 
  0xfc, 0xed, 0xff, 0xb7, 0x06, 0xff, 0xff, 0x1f, 0xfc, 0xff, 0xff, 0x87, 0x80, 0xff, 0xfc, 0x9f, 
  0xfc, 0xfd, 0xff, 0x80, 0x0c, 0xff, 0xf6, 0x1f, 0xfc, 0x7f, 0xff, 0xb4, 0x06, 0xff, 0xe7, 0x1f, 
  0xfc, 0x3f, 0xff, 0x86, 0x18, 0xff, 0xfe, 0x1f, 0xfc, 0x1f, 0xff, 0xc8, 0x21, 0xff, 0xfc, 0x1f, 
  0xfe, 0x0f, 0xff, 0xc8, 0xeb, 0xff, 0xf8, 0x3f, 0xfe, 0x07, 0xff, 0xe3, 0x43, 0xff, 0xf0, 0x3f, 
  0xfe, 0x00, 0xff, 0xf8, 0x0f, 0xff, 0xc0, 0x3f, 0xfe, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x3f, 
  0xff, 0x00, 0x1f, 0xff, 0xff, 0xcc, 0x00, 0x7f, 0xff, 0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x7f, 
  0xff, 0x00, 0x00, 0x7f, 0xff, 0xc0, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0xff, 
  0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x61, 0xff, 
  0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xc3, 0xff, 
  0xff, 0xf0, 0x00, 0x00, 0x00, 0x01, 0x87, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x07, 0x0f, 0xff, 
  0xff, 0xfc, 0x00, 0x00, 0x00, 0x1e, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 
  0xff, 0xfe, 0x00, 0x00, 0x0f, 0xf3, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x03, 0x86, 0x01, 0xff, 
  0xff, 0xff, 0x80, 0x40, 0x00, 0x3c, 0x7f, 0xff, 0xff, 0xff, 0xc0, 0x60, 0x01, 0xb0, 0xff, 0xff, 
  0xff, 0xff, 0xf8, 0x64, 0x13, 0x81, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x07, 0xf0, 0x0b, 0xff, 0xff, 
  0xff, 0xff, 0xfe, 0x03, 0xe0, 0x5f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd0, 0x0a, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// array size is 2048
static const uint8_t PROGMEM maze128_bmp[]  = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xc0, 0xf0, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x80, 0xf1, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfc, 0x03, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xe0, 0x3f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xe1, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xc3, 0xff, 0xe7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0x83, 0xff, 0xc3, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xfe, 0x03, 0xfe, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 
  0xff, 0xff, 0xfe, 0x07, 0xfc, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 
  0xff, 0xff, 0xf8, 0x0f, 0xe0, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 
  0xff, 0xff, 0xf8, 0x1f, 0xf0, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 
  0xff, 0xff, 0xe0, 0x3f, 0xb8, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 
  0xff, 0xff, 0xe0, 0x7f, 0x1f, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 
  0xff, 0xff, 0xf0, 0x7e, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 
  0xff, 0xff, 0xe0, 0x7c, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 
  0xff, 0xff, 0xc0, 0x78, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
  0xff, 0xff, 0xc1, 0xf0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
  0xff, 0xff, 0xc1, 0x80, 0x00, 0x00, 0x00, 0x30, 0xc1, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x39, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x3f, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x3f, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x3f, 0xff, 
  0xff, 0xfc, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x0f, 0xff, 
  0xff, 0xfc, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x1f, 0xff, 
  0xff, 0xfc, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xff, 0x00, 0x00, 0x0f, 0xff, 
  0xff, 0xfc, 0x00, 0x01, 0xff, 0xff, 0xff, 0xe6, 0x11, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x1f, 0xff, 
  0xff, 0xfc, 0x00, 0x01, 0xff, 0xff, 0xff, 0xc6, 0x38, 0x3f, 0xff, 0xff, 0xf0, 0x00, 0x1f, 0xff, 
  0xff, 0xfc, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xe6, 0x18, 0x1f, 0xff, 0xff, 0xfe, 0x00, 0x0f, 0xff, 
  0xff, 0xfc, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xf6, 0x18, 0x0f, 0xff, 0xff, 0xfe, 0x00, 0x0f, 0xff, 
  0xff, 0xfc, 0x01, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x70, 0xc3, 0xff, 0xf7, 0xff, 0xe0, 0x07, 0xff, 
  0xff, 0xf0, 0x01, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x60, 0xe3, 0xff, 0xf7, 0xff, 0xf0, 0x03, 0xff, 
  0xff, 0xf0, 0x01, 0xff, 0xbf, 0xff, 0xf7, 0xf8, 0x07, 0x83, 0xff, 0xff, 0x7e, 0x38, 0x03, 0xff, 
  0xff, 0xf0, 0x01, 0xff, 0xbf, 0xff, 0xf7, 0xf8, 0x07, 0x83, 0xff, 0xff, 0x7f, 0x38, 0x03, 0xff, 
  0xff, 0xf0, 0x1e, 0x6e, 0xff, 0xff, 0xc7, 0xfc, 0x03, 0x80, 0xff, 0xff, 0xc3, 0xfe, 0x03, 0xff, 
  0xff, 0xf0, 0x1e, 0x66, 0xff, 0xff, 0xc7, 0xfc, 0x03, 0x00, 0xff, 0xff, 0xc3, 0xfe, 0x03, 0xff, 
  0xff, 0xf0, 0x78, 0x63, 0xbf, 0xff, 0xdf, 0x7e, 0x00, 0x3c, 0xff, 0xfe, 0x7f, 0xff, 0x83, 0xff, 
  0xff, 0xf0, 0x7c, 0xe3, 0xbf, 0xff, 0xcf, 0x7f, 0x00, 0x3c, 0xff, 0xff, 0x7f, 0xff, 0x83, 0xff, 
  0xff, 0xf1, 0xff, 0xdf, 0xff, 0xff, 0xc0, 0x1f, 0x80, 0x00, 0xff, 0xff, 0xff, 0xe0, 0xc3, 0xff, 
  0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x1f, 0x80, 0x40, 0xff, 0xff, 0xff, 0xf0, 0x43, 0xff, 
  0xff, 0xf1, 0xff, 0xe1, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xe7, 0xb8, 0x03, 0xff, 
  0xff, 0xf1, 0xff, 0xf3, 0xff, 0xff, 0xc8, 0x20, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xbf, 0x03, 0xff, 
  0xff, 0xf0, 0x7f, 0xfe, 0xff, 0xff, 0xdf, 0x60, 0x00, 0x3c, 0xff, 0xff, 0xf8, 0x3f, 0x83, 0xff, 
  0xff, 0xf0, 0x3f, 0xff, 0xff, 0xff, 0xce, 0x38, 0x01, 0xfc, 0xff, 0xff, 0xff, 0xff, 0x03, 0xff, 
  0xff, 0xf0, 0x1f, 0xff, 0xbf, 0xff, 0xc0, 0x18, 0x03, 0xe0, 0xff, 0xff, 0xff, 0xfe, 0x03, 0xff, 
  0xff, 0xf0, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0x7e, 0x07, 0x43, 0xff, 0xff, 0xff, 0xfc, 0x03, 0xff, 
  0xff, 0xf0, 0x07, 0xff, 0xff, 0xff, 0xf0, 0x66, 0x0e, 0x03, 0xff, 0xff, 0x9b, 0xf8, 0x03, 0xff, 
  0xff, 0xf0, 0x03, 0xff, 0xff, 0xff, 0xf1, 0xc2, 0x7e, 0x43, 0xff, 0xff, 0xff, 0xf0, 0x03, 0xff, 
  0xff, 0xf8, 0x01, 0xff, 0xff, 0xff, 0xf1, 0xc0, 0x7e, 0xe3, 0xff, 0xdf, 0x7e, 0xe0, 0x07, 0xff, 
  0xff, 0xfc, 0x00, 0xff, 0xff, 0xff, 0xfc, 0x1e, 0x70, 0x0f, 0xff, 0xf1, 0xff, 0xc0, 0x0f, 0xff, 
  0xff, 0xfc, 0x00, 0x1f, 0xff, 0xff, 0xfc, 0x1e, 0x60, 0x0f, 0xff, 0xf1, 0xff, 0x80, 0x0f, 0xff, 
  0xff, 0xfc, 0x00, 0x1b, 0xff, 0xff, 0xff, 0x0e, 0x38, 0x3f, 0xfd, 0xff, 0xfc, 0x00, 0x0f, 0xff, 
  0xff, 0xfc, 0x00, 0x01, 0xff, 0xff, 0xff, 0x06, 0x18, 0x3f, 0xfd, 0xff, 0xf8, 0x00, 0x0f, 0xff, 
  0xff, 0xfc, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xe0, 0x03, 0xfb, 0xff, 0x7f, 0xc0, 0x00, 0x0f, 0xff, 
  0xff, 0xfc, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xf0, 0x03, 0xfb, 0xff, 0x7f, 0x80, 0x00, 0x0f, 0xff, 
  0xff, 0xfc, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x3d, 0xfc, 0x00, 0x00, 0x0f, 0xff, 
  0xff, 0xfe, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7c, 0xfc, 0x00, 0x00, 0x1f, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xc1, 0xc0, 0x00, 0x00, 0x3f, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xe0, 0xc0, 0x00, 0x00, 0x3f, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x3f, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x3f, 0xff, 
  0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x3f, 0xff, 
  0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
  0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
  0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
  0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
  0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x03, 0xff, 0xff, 
  0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x03, 0xff, 0xff, 
  0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x03, 0xff, 0xff, 
  0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 
  0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0f, 0xff, 0xff, 
  0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x0f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x3f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2f, 0xc0, 0x7f, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x7f, 0x01, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x03, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x27, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x67, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x07, 0x9f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x0f, 0x9f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xf6, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x1c, 0x00, 0x03, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc3, 0xf8, 0x19, 0x13, 0xff, 0xff, 
  0xff, 0xff, 0xd9, 0x99, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x19, 0xbb, 0xbf, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x30, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x7f, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0x66, 0x66, 0xe0, 0x00, 0x30, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x66, 0xee, 0xcf, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xfb, 0x00, 0x3c, 0x00, 0x00, 0x0f, 0xc7, 0x01, 0xff, 0xfb, 0xbf, 0xff, 
  0xff, 0xfd, 0xd9, 0x99, 0xbb, 0x80, 0x3c, 0x00, 0x00, 0x0f, 0xe0, 0x01, 0xd9, 0xbb, 0xbb, 0xff, 
  0xff, 0xff, 0x7f, 0x6f, 0xef, 0xc0, 0x18, 0x78, 0x07, 0x83, 0x00, 0x07, 0x76, 0xfe, 0xff, 0xff, 
  0xff, 0xf7, 0x76, 0x66, 0xee, 0xe0, 0x00, 0x7c, 0x07, 0x80, 0x00, 0x06, 0x66, 0x6e, 0xef, 0xff, 
  0xff, 0xfd, 0xd9, 0xd9, 0xbb, 0xb0, 0x00, 0x1f, 0xfe, 0x00, 0x00, 0xdd, 0xdf, 0xbb, 0xbf, 0xff, 
  0xff, 0xfd, 0xd9, 0x99, 0xbb, 0x38, 0x00, 0x1f, 0xfe, 0x00, 0x01, 0xdd, 0xd9, 0xbb, 0xff, 0xff, 
  0xff, 0xff, 0x76, 0x66, 0xee, 0xec, 0xc0, 0x00, 0x00, 0x00, 0x33, 0x77, 0x66, 0xee, 0xff, 0xff, 
  0xff, 0xff, 0x77, 0x66, 0xee, 0xcf, 0xe0, 0x00, 0x00, 0x04, 0x77, 0x37, 0x77, 0xef, 0xff, 0xff, 
  0xff, 0xff, 0xfd, 0xd9, 0xbb, 0x33, 0x37, 0x00, 0x00, 0x6c, 0xcc, 0xdd, 0xdb, 0xbb, 0xff, 0xff, 
  0xff, 0xff, 0xfd, 0xff, 0xbb, 0xb3, 0x37, 0x00, 0x00, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xe6, 0xee, 0xec, 0xcd, 0xd9, 0x9b, 0xbb, 0x33, 0x77, 0x77, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0x37, 0x76, 0x66, 0xee, 0xcd, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

#endif
