/**
 * @file custom_characters.h
 * @author Raphoulfifou (raphael.roger.92@gmail.com)
 * @brief Custom characters to use with arduino UNO REV3 LCD1602
 * @version 0.1
 * @date 2022-08-03
 *
 * @copyright Copyright (c) 2022
 *
 */

/*
    Representation of the custom symbols :

    - Heart -> ♡

    - Check -> ✔️

    - Cross -> ❌

    - Sigma -> Σ

    - Degree -> °
*/

#define byte uint8_t
//====================================
// CONSTANTS
// ===================================
const int bHeart = 1;
const int bCheck = 2;
const int bCross = 3;
const int bSigma = 4;
const int bDegree = 5;

//====================================
// BYTES
// ===================================
byte heart[8] = {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000};

byte check[8] = {
    0b00000,
    0b00000,
    0b00001,
    0b00010,
    0b10100,
    0b01000,
    0b00000,
    0b00000};

byte cross[8] = {
    0b00000,
    0b00000,
    0b01010,
    0b00100,
    0b00100,
    0b01010,
    0b00000,
    0b00000};

byte sigma[8] = {
    0b11111,
    0b10000,
    0b01000,
    0b00100,
    0b01000,
    0b10000,
    0b11111,
    0b00000};

byte degree[8] = {
    0b01100,
    0b10010,
    0b10010,
    0b01100,
    0b00000,
    0b00000,
    0b00000,
    0b00000};
