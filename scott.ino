/*
  ____________________

  - Welcome to SCOTT -
  (Scientific RPN Calculator with OLED display and ATTINY85 microcontroller)
  Version 1.0, (c) 2019 by deetee/zooxo

  SCOTT is free software. You can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software Foundation.

 
    KEYS:
      [SHIFT]            7 [(MENU)] (F1)     8 [SUM+] (F2)      9   [/](F3)
      EE [ROT](UP)       4 [CONST]           5 [CMD]            6   [*]
      +- [ROT](DOWN)     1 [RCL]             2 [STO]            3   [-]
      C  [OFF]           0 [BATT]            . [SWAP]         ENTER [+]

    MENU:
       SQRT  POW   INV   ... Basic Functions
       EXP   LN    !
       ->P   ->R   PV    ... Polar/Rectangular Conversion, Present Value (Annuity)
       ND    STA   LR    ... Normal Distribution, Statistics, Linear Regression
       SIN   COS   TAN   ... Trigonometric Functions
       ASIN  ACOS  ATAN
       SINH  COSH  TANH  ... Hyperbolic Functions
       ASINH ACOSH ATANH
       CONST CMD   LIT   ... Set Constant, User Defined Command Key and Brightness
       REC1  REC2  REC3  ... Record user keypress sequence ("Type Recorder")
       PLAY1 PLAY2 PLAY3 ... Play user keypress sequence ("Type Recorder")

    POWER MANAGEMENT

      As SCOTT is supplied with battery power a severe power management is
      essential. After 10 seconds without pressing a key the display brightness
      will be reduced to a minimum. After 10 more seconds the display will be
      deactivated. Till now every further keypress will be interpreted as
      normal keypress for calculations.
      After 10 more seconds without pressing a key SCOTT falls into a deep sleep
      mode. Only a pin change (pressing any key in the upper part of the keyboard)
      wakes SCOTT up, but this keypress will not be interpreted as normal keypress
      for calculations.

      If demanded the user can enter the sleep mode by pressing SHIFT and CLR.
      Additionally SCOTT saves the stack and brightness values to the EEPROM.
      By pressing SHIFT and DOT the battery voltage will be shown.
      The default brightness of the display can be set by entering a value
      (0...255) followed by SHIFT and 0.

      Running on a single battery (CR2032) SCOTT draws a current of 10 mA
      (bright display with a lot of "8s"). With a battery capacity of at least
      200 mAh SCOTT can calculate approximately 20 hours.
      After dimming the current falls to 5.5 mA, after deactivating the display
      1.1 mA are needed.
      In sleep mode SCOTT consumes less than 0.25 mA. With a battery capacity of
      at least 200 mAh SCOTT lasts more than a month in sleep mode.


    USER DEFINED FUNCTION KEYs

    COMMANDS (Numbers and corresponding ASCII characters)

      NR ASC COMMAND         NR ASC COMMAND         NR ASC COMMAND
      ------------------     ------------------     ------------------
      00 0-9 Numbers
      01  :  DOT             21  N  nop             41  b  COSH
      02  ;  CLX             22  O  SQRT            42  c  TANH
      03  <  EE              23  P  POW(y^x)        43  d  ASINH
      04  =  ENTER           24  Q  INV(1/x)        44  e  ACOSH
      05  >  CHS             25  R  EXP             45  f  ATANH
      06  ?  BATT            26  S  LN              46  g  SETCONST
      07  @  RCL             27  T  GAMMA(!)        47  h  SETCMD
      08  A  STO             28  U  R2P             48  i  CONTR
      09  B  SUB(-)          29  V  P2R             49  j  REC1
      10  C  nop             30  W  PV              50  k  REC2
      11  D  CMD(user)       31  X  ND              51  l  REC3
      12  E  MULT(*)         32  Y  STAT            52  m  PLAY1
      13  F  MENU            33  Z  SETCMD          53  n  PLAY2
      14  G  nop             34  [  SIN             54  o  PLAY3
      15  H  DIV(/)          35  \  COS             55  p  SUM1
      16  I  SWAP            36  ]  TAN             56  q  SUM2STACK
      17  J  SLEEP           37  ^  ASIN            57  r  SHADOWSAVE
      18  K  RORup           38  _  ACOS            58  s  SHADOWLOAD1
      19  L  ADD(+)          39  `  ATAN            59  t  SHADOWLOAD2
      20  M  ROT             40  a  SINH            60  u  ENTER3

    CIRCUIT DIAGRAM:
                ____                   _________
               |*   |--Vcc       Vcc--| DISPLAY |
       +---D3--| AT |--SCK-------SCK--| SSD1306 |
       |       |TINY|                 | 128x64  |
       |  GND--|_85_|--SDA-------SDA--|__OLED___|
       |
       |
       |                  ONE WIRE KEYBOARD         ____
       |  Vcc--X----------X----------X----------X--|_Ra_|--+
       |       |          |          |          |          |   ____
       |       X----------X----------X----------X----------+--|_Rb_|--+
       |       |          |          |          |                     |   ____
       |       X----------X----------X----------X---------------------+--|_Rc_|--+
       |       |          |          |          |                                |
       |       X----------X----------X----------X--------------------------------+
       |       |          |          |          |
       |       |   ____   |   ____   |   ____   |          R = 10k
       +-------+--|_R1_|--+--|_R2_|--+--|_R3_|--+          R1 = R2 = R3 = 820
       |   ____                                            Ra = Rb = Rc = 3k3
       +--|_R__|---GND

             Approximate keycode with quadratic function:  keyvalue = 2 * keynr^2 + 470


    EEPROM usage (512 bytes):

      | CONTRAST | STACK | CMD | CONST | REC1 | REC2 | REC3 | EOREC | EOEE
      0          1       21    31      71     217    363    509     511
      | +1       | +20   | +10 | +40   | +146 | +146 | +146 | +2    |


  ____________________

        PLAYSTRING
  ____________________

    To save flash memory a lot of functions are calculated with SCOTT itself
    (and not with libraries). Every command of SCOTT can be dispatched with one
    character (one byte command - see chapter COMMANDS). So a sequence of commands
    can be represented and "played" with a sequence of characters in a string.
    After playing a string the result(s) is/are shown in the register(s) X
    and/or Y. Other parts of the stack (including mem) will be restored from a
    "shadow stack".


    PLAYSTRING - PSEUDO CODES

      COS (Cosine)
        COMMANDS (5): CHS 90 + SIN

        Converts    4 mem  -    to:     -
        stack[]     ---------         -----
        (incl.      3  t   -            -        cos(x) = sqrt(1 - sin(x)*sin(x))
        mem)        2  z   -            -
        from:       1  y   -            -
                    0  x   x          cos(x)


      TAN (Tangent)
        COMMANDS (9): SIN ENTER ENTER * CHS 1 + SQRT /

        Converts    4 mem  -    to:     -
        stack[]     ---------         -----               sin(x)          sin(x)
        (incl.      3  t   -            -        tan(x) = ------ = -----------------------
        mem)        2  z   -            -                 cos(x)   sqrt(1 - sin(x)*sin(x))
        from:       1  y   -            -
                    0  x   x          tan(x)


      ACOS (Arcus Cosine)
        COMMANDS (4): ASIN CHS 90 +

        Converts    4 mem  -    to:     -
        stack[]     ---------         ------
        (incl.      3  t   -            -        acos(x) = 90 - asin(x)
        mem)        2  z   -            -
        from:       1  y   -            -
                    0  x   x          acos(x)


      ATAN (Arcus Tangent)
        COMMANDS (10): ENTER ENTER ENTER * 1 + SQRT INV * ASIN

        Converts    4 mem  -    to:     -
        stack[]     ---------         ------                           x
        (incl.      3  t   -            -        atan(x) = asin( ------------- )
        mem)        2  z   -            -                        sqrt(1 + x*x)
        from:       1  y   -            -
                    0  x   x          atan(x)


      PV (Present Value)
        COMMANDS (14): CHS SWAP ENTER 1 + SWAP ROT SWAP PWR CHS 1 + ROTup /

        Converts    4 mem  -    to:     -
        stack[]     ---------         -------              1 - (1+i)^-n
        (incl.      3  t   -            -        PV(i,n) = ------------
        mem)        2  z   -            -                       i
        from:       1  y   i            -
                    0  x   n          PV(i,n)


      GAMMA (due to formula of Nemes)
        COMMANDS (33): 1 + ENTER ENTER ENTER 12 * SWAP 10 * INV - INV +
                     1 EXP / SWAP PWR 2.506628 ROTup SQRT / *


        Converts    4 mem  -    to:    -
        stack[]     ---------         ---                                 x + 1/(12*x - 1/10/x)
        (incl.      3  t   -           -         (x-1)! = sqrt(2*PI/x)* (----------------------)^x
        mem)        2  z   -           -                                            e
        from:       1  y   -           -
                    0  x   x           x!


      SINH (Hyperbolic Sine)
        COMMANDS (7): EXP ENTER INV CHS + 2 /

        Converts    4 mem  -    to:      -
        stack[]     ---------         -------              exp(x) - exp(-x)
        (incl.      3  t   -             -       sinh(x) = ----------------
        mem)        2  z   -             -                        2
        from:       1  y   -             -
                    0  x   x          sinh(x)


      COSH (Hyperbolic Cosine)
        COMMANDS (6): EXP ENTER INV + 2 /

        Converts    4 mem  -    to:      -
        stack[]     ---------         -------              exp(x) + exp(-x)
        (incl.      3  t   -             -       cosh(x) = ----------------
        mem)        2  z   -             -                        2
        from:       1  y   -             -
                    0  x   x          cosh(x)


      TANH (Hyperbolic Tangent)
        //COMMANDS (12): 2 * CHS EXP ENTER CHS 1 + SWAP 1 + /
        COMMANDS (9): 2 * EXP 1 - ENTER 2 + /

        Converts    4 mem  -    to:      -
        stack[]     ---------         -------              exp(x) - exp(-x)
        (incl.      3  t   -             -       tanh(x) = ----------------
        mem)        2  z   -             -                 exp(x) + exp(-x)
        from:       1  y   -             -
                    0  x   x          tanh(x)


      ASINH (Area Hyperbolic Sine)
        COMMANDS (8): ENTER ENTER * 1 + SQRT + LN

        Converts    4 mem  -    to:      -
        stack[]     ---------         --------
        (incl.      3  t   -             -       asinh(x) = ln(x + sqrt(x*x + 1))
        mem)        2  z   -             -
        from:       1  y   -             -
                    0  x   x          asinh(x)


      ACOSH (Area Hyperbolic Cosine)
        COMMANDS (8): ENTER ENTER * 1 - SQRT + LN

        Converts    4 mem  -    to:      -
        stack[]     ---------         --------
        (incl.      3  t   -             -       acosh(x) = ln(x + sqrt(x*x - 1))
        mem)        2  z   -             -
        from:       1  y   -             -
                    0  x   x          acosh(x)


      ATANH (Area Hyperbolic Tangent)
        COMMANDS (11): ENTER ENTER 1 + SWAP CHS 1 + / SQRT LN

        Converts    4 mem  -    to:      -
        stack[]     ---------         --------                      1 + x
        (incl.      3  t   -             -       atanh(x) = ln(sqrt(-----))
        mem)        2  z   -             -                          1 - x
        from:       1  y   -             -
                    0  x   x          atanh(x)


      SUM (Prepare stack/arguments for summarizing)
        COMMANDS (12): 1 STO ROT ENTER ENTER * ROT * ROT SWAP SQRT SUM1

        Converts    4 mem  -    to:    1    and where
        stack[]     ---------         ---   SUM1 adds
        (incl.      3  t   -          x*y   stack[i]
        mem)        2  z   -          x*x   to
        from:       1  y   y           y    sum[i]
                    0  x   x           x


      ND (Normal Distribution: Probability Density and Cumulative Distribution Function)
        //COMMANDS (38): ENTER ENTER ENTER * * .07 * CHS SWAP 1.6 * CHS + EXP 1 + INV
        //               SWAP ENTER * CHS 2 / EXP 0.3989423 *
        COMMANDS (37): ENTER ENTER ENTER * * .07 * CHS SWAP 1.6 * - EXP 1 + INV
                       SWAP ENTER * CHS 2 / EXP 0.3989423 *

        Converts    4 mem  -    to:    -                  (x)
        stack[]     ---------         ---        CDF = integral(PDF) = 1/(1 + exp(-0.07*x^3 - 1.6*x))
        (incl.      3  t   -           -                 (-inf)
        mem)        2  z   -           -
        from:       1  y   -          CDF        PDF = 1/sqrt(2*PI) * exp(-x*x/2)
                    0  x   x          PDF


      R2P (Rectangular to Polar Coordinates)
        COMMANDS (14): ENTER * SWAP ENTER ENTER ROT * + SQRT ENTER ROT / ASIN ROTup

        Converts    4 mem  -    to:    -
        stack[]     ---------         ---      r = sqrt(x*x + y*y)
        (incl.      3  t   -           -
        mem)        2  z   -           -                 y
        from:       1  y   y           a       a = atan(---)
                    0  x   x           r                 x


      P2R (Polar to Rectangular Coordinates)
        COMMANDS (15): SWAP SIN ENTER ENTER * CHS 1 + SQRT ROT * ROT * SWAP ROT

        Converts    4 mem  -    to:    -
        stack[]     ---------         ---      y = r * sin(a)
        (incl.      3  t   -           -
        mem)        2  z   -           -
        from:       1  y   a           y       x = r * cos(a)
                    0  x   r           x


      STAT (Statistik, Mean Value, Standard Deviation)
        COMMANDS (16): SWAP ROT ENTER RCL / ENTER ROT * CHS + RCL 1 - / SQRT SWAP

        Converts    4 mem  n    to:    -                 XX - X^2 / n
        stack[]     ---------         ---      d = sqrt(--------------)
        (incl.      3  t   XY          -                     n - 1
        mem)        2  z   XX          -
        from:       1  y   Y           d       m = X / n
                    0  x   X           m


      QE (Quadratic Equation)
        COMMANDS (25): SWAP 2 / CHS ENTER ENTER * SWAP ROT SWAP - SQRT ROT ROT
                       ENTER ROTup ENTER ROT SWAP ROT - ROT + SWAP ROT

        Converts    4 mem  -    to:    -
        stack[]     ---------         ---      y = x*x + p *x + q
        (incl.      3  t   -           -
        mem)        2  z   -           -       x1 = -p/2 + sqrt((p/2)^2 - q)
        from:       1  y   p           x2
                    0  x   q           x1      x2 = -p/2 - sqrt((p/2)^2 - q)


      LR (Linear Regression)
        COMMANDS (33): SUM2STACK * SWAP RCL * ROTup RCL * ROTup SHADOWSAVE
                       SUM2STACK ENTER * SHADOWLOAD1
                       SWAP ROT - ROT - ROTup / ENTER ENTER ENTER SHADOWSAVE
                       SUM2STACK SHADOWLOAD2 ROTup * - RCL / SWAP

        Converts    4 mem  n    to:    -
        stack[]     ---------         ---      y = a * x + b
        (incl.      3  t   XY          -
        mem)        2  z   XX          -       a = (XY*n - X*Y) / (XX*n - X*X)
        from:       1  y   Y           b
                    0  x   X           a       b = (Y - X*a) / n


*/


// ***** I N C L U D E S

#include <TinyWireM.h> // I2C wire communication with display
#include <avr/power.h> // Needed for power management
#include <avr/sleep.h> // Needed for sleeping
#include <EEPROM.h> // For saving data to EEPROM


// ***** F O N T

#define FONTOFFSET ','
#define FONTWIDTH 5
const byte font [] PROGMEM = { // Edged font
  /*0x00, 0x00, 0x00, 0x00, 0x00, // space // No ascii signs below 44 (',') to save memory (flash)
    0x00, 0x00, 0x2f, 0x00, 0x00, // !
    0x00, 0x07, 0x00, 0x07, 0x00, // "
    0x14, 0x7f, 0x14, 0x7f, 0x14, // #
    0x2e, 0x2a, 0x7f, 0x2a, 0x3a, // $
    0x62, 0x64, 0x08, 0x13, 0x23, // %
    0x36, 0x49, 0x55, 0x22, 0x50, // &
    0x00, 0x07, 0x03, 0x00, 0x00, // '
    0x00, 0x1c, 0x22, 0x41, 0x00, // (
    0x00, 0x41, 0x22, 0x1c, 0x00, // )
    0x14, 0x08, 0x3E, 0x08, 0x14, // *
    0x08, 0x08, 0x3E, 0x08, 0x08, // + */
  //0x00, 0x00, 0xe0, 0x60, 0x00, // ,
  0x10, 0x7f, 0x01, 0x01, 0x01, // , squareroot
  0x08, 0x08, 0x08, 0x08, 0x08, // -
  0x00, 0x60, 0x60, 0x00, 0x00, // .
  0x20, 0x10, 0x08, 0x04, 0x02, // /
  0x7f, 0x41, 0x41, 0x41, 0x7f, // 0
  0x00, 0x00, 0x02, 0x7f, 0x00, // 1
  0x79, 0x49, 0x49, 0x49, 0x4f, // 2
  0x41, 0x49, 0x49, 0x49, 0x7f, // 3
  0x0f, 0x08, 0x08, 0x08, 0x7f, // 4
  0x4f, 0x49, 0x49, 0x49, 0x79, // 5
  0x7f, 0x49, 0x49, 0x49, 0x79, // 6
  0x03, 0x01, 0x01, 0x01, 0x7f, // 7
  0x7f, 0x49, 0x49, 0x49, 0x7f, // 8
  0x4f, 0x49, 0x49, 0x49, 0x7f, // 9
  //0x00, 0x36, 0x36, 0x00, 0x00, // :
  0x00, 0x00, 0x00, 0x00, 0x00, // : space
  //0x00, 0x76, 0x36, 0x00, 0x00, // ;
  0x00, 0x1b, 0x04, 0x1b, 0x00, // ; raised x
  //0x08, 0x14, 0x22, 0x41, 0x00, // <
  0x00, 0x7f, 0x3e, 0x1c, 0x08, // < play
  //0x14, 0x14, 0x14, 0x14, 0x14, // =
  0x04, 0xbe, 0xbf, 0xbe, 0x04, // = shift sign
  //0x00, 0x41, 0x22, 0x14, 0x08, // >
  0x08, 0x08, 0x3e, 0x1c, 0x08, // > arrow to right
  //0x03, 0x01, 0x59, 0x09, 0x0f, // ?
  0x00, 0x00, 0x2f, 0x00, 0x00, // ? !
  //0x7b, 0x49, 0x79, 0x41, 0x7f, // @
  0x1c, 0x3e, 0x3e, 0x3e, 0x1c, // @ record
  0x7f, 0x09, 0x09, 0x09, 0x7f, // A
  0x7f, 0x49, 0x49, 0x4f, 0x78, // B
  0x7f, 0x41, 0x41, 0x41, 0x40, // C
  0x41, 0x7F, 0x41, 0x41, 0x7F, // D
  0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x7F, 0x09, 0x09, 0x09, 0x01, // F
  0x7f, 0x41, 0x49, 0x49, 0x79, // G
  0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x41, 0x7F, 0x41, 0x00, // I
  0x40, 0x40, 0x40, 0x40, 0x7f, // J
  0x7F, 0x08, 0x08, 0x0f, 0x78, // K
  0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x7F, 0x01, 0x07, 0x01, 0x7F, // M
  0x7F, 0x01, 0x7f, 0x40, 0x7F, // N
  0x7f, 0x41, 0x41, 0x41, 0x7f, // O
  0x7F, 0x09, 0x09, 0x09, 0x0f, // P
  0x7f, 0x41, 0x71, 0x41, 0x7f, // Q
  0x7F, 0x09, 0x09, 0x79, 0x4f, // R
  0x4f, 0x49, 0x49, 0x49, 0x78, // S
  0x01, 0x01, 0x7F, 0x01, 0x01, // T
  0x7F, 0x40, 0x40, 0x40, 0x7F, // U
  0x1F, 0x70, 0x40, 0x70, 0x1F, // V
  0x7F, 0x40, 0x70, 0x40, 0x7F, // W
  0x78, 0x0f, 0x08, 0x0f, 0x78, // X
  0x0f, 0x08, 0x78, 0x08, 0x0f, // Y
  0x71, 0x51, 0x5d, 0x45, 0x47, // Z
  /*0x00, 0x7F, 0x41, 0x41, 0x00, // [ // No low case letters to save flash memory
    0x02, 0x04, 0x08, 0x10, 0x20, // b
    0x00, 0x41, 0x41, 0x7F, 0x00, // ]
    0x04, 0x02, 0x01, 0x02, 0x04, // ^
    0x40, 0x40, 0x40, 0x40, 0x40, // _
    0x00, 0x00, 0x03, 0x00, 0x00, // `
    0x70, 0x54, 0x54, 0x54, 0x7c, // a
    0x7F, 0x44, 0x44, 0x44, 0x7c, // b
    0x7c, 0x44, 0x44, 0x44, 0x40, // c
    0x7c, 0x44, 0x44, 0x44, 0x7F, // d
    0x7c, 0x54, 0x54, 0x54, 0x1c, // e
    0x08, 0x7f, 0x09, 0x01, 0x00, // f
    0x3c, 0xa4, 0xa4, 0xa4, 0xfc, // g
    0x7F, 0x04, 0x04, 0x04, 0x7c, // h
    0x00, 0x00, 0x7D, 0x00, 0x00, // i
    0x00, 0x80, 0x80, 0xfa, 0x00, // j
    0x7F, 0x10, 0x1c, 0x70, 0x00, // k
    0x00, 0x7f, 0x40, 0x40, 0x00, // l
    0x7C, 0x04, 0x1c, 0x04, 0x7c, // m
    0x7C, 0x04, 0x04, 0x04, 0x7c, // n
    0x7c, 0x44, 0x44, 0x44, 0x7c, // o
    0xFC, 0x24, 0x24, 0x24, 0x3c, // p
    0x3c, 0x24, 0x24, 0x24, 0xfc, // q
    0x7C, 0x04, 0x04, 0x04, 0x00, // r
    0x5c, 0x54, 0x54, 0x54, 0x70, // s
    0x04, 0x7F, 0x44, 0x40, 0x00, // t
    0x7C, 0x40, 0x40, 0x40, 0x7C, // u
    0x04, 0x7c, 0x40, 0x7c, 0x04, // v
    0x7C, 0x40, 0x70, 0x40, 0x7C, // w
    0x70, 0x1c, 0x10, 0x1c, 0x70, // x
    0x3C, 0xA0, 0xA0, 0xA0, 0xfC, // y
    0x74, 0x54, 0x54, 0x54, 0x5c, // z
    0x08, 0x1c, 0x3e, 0x7f, 0x00, // { left arrow
    0x00, 0x00, 0xff, 0x00, 0x00, // |
    0x00, 0x7f, 0x3e, 0x1c, 0x08, // } right arrow
    0x07, 0x7f, 0x01, 0x7f, 0x01  // ~ paragraph sign*/
};


// *****  D I S P L A Y

// DEFINES
#define DISPLAY_ADDRESS 0x3C  // I2C slave address
#define DISPLAY_PAGES 4       // Lines of screen
#define DISPLAY_COMMAND 0x00  // Command byte
#define DISPLAY_DATA 0x40     // Data byte
#define SCREENWIDTH 128       // Screen width in pixel


// VARIABLES
static byte dx = 0, dy = 0; // Display cursor coordinates
static byte renderram = 0xB0, drawram = 0x40; // Masks to address GDDRAM of display


// SUBPROGRAMS
static const byte inits [] PROGMEM = { // Initialization sequence
  0xC8,       // Set scan direction (C0 scan from COM0 to COM[N-1] or C8 mirroring)
  0xA1,       // Set segment remap (A0 regular or A1 flip)
  0xA8, 0x1F, // Set mux ratio (N+1) where: 14<N<64 ... 3F or 1F
  0xDA, 0x02, // COM config pin mapping:            right/left left/right
  //          //                         sequential     02        22
  //          //                         alternate      12        32
  0x20, 0x00, // Horizontal addressing mode (line feed after EOL)
  0x8D, 0x14  // Charge pump (0x14 enable or 0x10 disable)
};

static void dbegin(void) { // Initialize communication
  TinyWireM.begin();
}

static void dsendstart(void) { // Start communication
  TinyWireM.beginTransmission(DISPLAY_ADDRESS);
}

static bool dsendbyte(byte b) { // Send byte
  return (TinyWireM.write(b));
}

static void dsendstop(void) { // Stop communication
  TinyWireM.endTransmission();
}

static void dsenddatastart(void) { // Start data transfer
  dsendstart();
  dsendbyte(DISPLAY_DATA);
}

static void dsenddatabyte(byte b) { // Send data byte
  if (!dsendbyte(b)) {
    dsendstop();
    dsenddatastart();
    dsendbyte(b);
  }
}

static void dsendcmdstart(void) { // Start command transfer
  dsendstart();
  dsendbyte(DISPLAY_COMMAND);
}

static void dsendcmd(byte cmd) { // Send command
  dsendcmdstart();
  dsendbyte(cmd);
  dsendstop();
}

static void drender(void) { // Render current half of GDDRAM to oled display
  renderram ^= 0x04;
}

static void dswap(void) { // Swap GDDRAM to other half and render
  drawram ^= 0x20;
  dsendcmd(drawram);
  drender();
}

static void dinit(void) { // Run initialization sequence
  dbegin();
  dsendstart();
  dsendbyte(DISPLAY_COMMAND);
  for (byte i = 0; i < sizeof(inits); i++) dsendbyte(pgm_read_byte(&inits[i]));
  dsendstop();
}

static void don(void) { // Display on
  dsendcmd(0xAF);
}

static void doff(void) { // Display off
  dsendcmd(0xAE);
}

void dcontrast(byte contrast) { // Set contrast
  dsendcmdstart();
  dsendbyte(0x81);
  dsendbyte(contrast);
  dsendstop();
}

static void dsetcursor(byte x, byte y) { // Set cursor to position (x|y)
  dsendcmdstart();
  dsendbyte(renderram | (y & 0x07));
  dsendbyte(0x10 | (x >> 4));
  dsendbyte(x & 0x0f);
  dsendstop();
  dx = x;
  dy = y;
}

static void dfill(byte b) { // Fill screen with byte/pattern b
  dsetcursor(0, 0);
  dsenddatastart();
  for (int i = 0; i < SCREENWIDTH * DISPLAY_PAGES; i++) dsenddatabyte(b);
  dsendstop();
}


// ***** K E Y B O A R D

// DEFINES
#define KPIN 3  // Pin to read analog keyboard  (H2 = D3)
#define BITEXP 0x01

// Keychars
#define KEY_1 '?' // SHIFT    1-?  2-7  3-8  4-9
#define KEY_2 '7' //          5-<  6-4  7-5  8-6
#define KEY_3 '8' //          9-> 10-1 11-2 12-3
#define KEY_4 '9' //         13-; 14-0 15-: 16-=
#define KEY_5 '<' // EE
#define KEY_6 '4'
#define KEY_7 '5'
#define KEY_8 '6'
#define KEY_9 '>' // CHS
#define KEY_10 '1'
#define KEY_11 '2'
#define KEY_12 '3'
#define KEY_13 ';' // CLX
#define KEY_14 '0'
#define KEY_15 ':' // DOT
#define KEY_16 '=' // ENTER

static int getbutton(void) { // Returns analog value measured on keyboard pin
  return (analogRead(KPIN));
}

static byte getkeycode(void) { // Returns key character due to analog value
  int b = getbutton();
  const byte keys[] = {KEY_16, KEY_15, KEY_14, KEY_13, KEY_12, KEY_11, KEY_10, KEY_9, KEY_8, KEY_7, KEY_6, KEY_5, KEY_4, KEY_3, KEY_2, KEY_1};
  if (b < 231)return (NULL); // No key pressed
  else return (keys[(byte)(_exp_sin_asin(0.5 * log(b / 2 - 235), BITEXP) - 1)]); // Approximate keys with quadratic function
  /*if (b < 231)return (NULL); // No key pressed
    else if (b < 470) return (KEY_16);
    else if (b < 488) return (KEY_15);
    else if (b < 508) return (KEY_14);
    else if (b < 529) return (KEY_13);
    else if (b < 552) return (KEY_12);
    else if (b < 578) return (KEY_11);
    else if (b < 606) return (KEY_10);
    else if (b < 637) return (KEY_9);
    else if (b < 661) return (KEY_8);
    else if (b < 708) return (KEY_7);
    else if (b < 751) return (KEY_6);
    else if (b < 799) return (KEY_5);
    else if (b < 853) return (KEY_4);
    else if (b < 915) return (KEY_3);
    else if (b < 986) return (KEY_2);
    else return (KEY_1);*/
}


// *****  S Y S T E M

// DEFINES
#define FRAMERATE 10 // Maximal number of screen refreshes per second (>3)
#define SIZES 0x01 // Printing size
#define SIZEM 0x02 // Printing size
#define SIZEL 0x04 // Printing size


// VARIABLES
static byte eachframemillis, thisframestart, lastframedurationms; // Framing times
static boolean justrendered; // True if frame was just rendered
static byte printbitshift = 0; // Shifts printed character down


// SUBPROGRAMS
static byte expand4bit(byte b) { // 0000abcd  Expand 4 bits (lower nibble)
  b = (b | (b << 2)) & 0x33;     // 00ab00cd
  b = (b | (b << 1)) & 0x55;     // 0a0b0c0d
  return (b | (b << 1));         // aabbccdd
}

static byte expand2bit(byte b) { // Expand 2 bits 000000ab
  b = (b | (b << 3)) & 0x11;                   // 000a000b
  for (byte i = 0; i < 3; i++) b |= (b << 1);  // aaaabbbb
  return (b);
}

static void delayshort(byte ms) { // Delay (with timer) in ms with 8 bit duration
  long t = millis();
  while ((byte)(millis() - t) < ms) ;
}

static void printc(byte c, byte w, byte h) { // Print char c with width and height (1, 2 or 4)
  byte tmpx = dx;
  for (byte k = 0; k < h; k++) { // One byte, two nibbles or 4 pairs of bits
    if (k > 0) { // Manage cursor position, if size >1
      dx = tmpx; // Remember x position
      dy++; // Increment y position/page
      dsetcursor(dx, dy);
    }
    for (byte j = 0; j < FONTWIDTH; j++) { // Fontbyte - shifted one pixel down (if applicable)
      byte tmp = pgm_read_byte(&font[FONTWIDTH * (c - FONTOFFSET) + j]) << printbitshift; // Fontbyte
      if (h == SIZEM)      tmp = expand4bit((tmp >> (k * 4)) & 0x0f); // Expand 0000abcd
      else if (h == SIZEL) tmp = expand2bit((tmp >> (k * 2)) & 0x03); // Expand 000000ab
      dsenddatastart();
      for (byte i = 0; i < w; i++) dsenddatabyte(tmp);
      dsendstop();
    }
  }
}

static void printcat(byte c, byte w, byte h, byte x, byte y) { // Print sized char c at (x|y)
  dsetcursor(x, y);
  printc(c, w, h);
}

static void prints(char * s, byte w, byte h) { // Print string
  byte tmpx = dx, tmpy = dy;
  for (byte i = 0; i < strlen(s); i++) printcat(s[i], w, h, tmpx + i * (FONTWIDTH + 1) * w, tmpy);
}

static void printsat(char * s, byte w, byte h, byte x, byte y) { // Print sized string s at (x|y)
  dsetcursor(x, y);
  prints(s, w, h);
}

static void cls(void) { // Clear screen
  dfill(0x00);
}

static void display(void) { // Swap GDDRAM and render
  dswap();
}

static void setframerate(byte rate) { // Calculate frameduration
  eachframemillis = 1000 / rate;
}

ISR (PCINT0_vect) { // External interrupt 0 wakes up ATTINY
}

static void execsleep(void) { // Execute sleep mode
  sleep_enable();
  sleep_cpu();
  sleep_disable();
  power_all_enable();
  power_timer1_disable(); // Never using timer1
}

static void sleep (void) { // Goto deep sleep mode
  doff();
  delayshort(200); // Prevent instant wakeup
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_all_disable (); // Power off ADC, timer 0 and 1
  execsleep();
  don();
}

static void idle(void) { // Idle, while waiting for next frame
  set_sleep_mode(SLEEP_MODE_IDLE);
  power_adc_disable(); // Disable ADC (do not disable timer0; timer1 is disabled from last idle)
  execsleep();
}

static boolean nextframe(void) { // Wait (idle) for next frame
  byte now = (byte) millis(), framedurationms = now - thisframestart;
  if (justrendered) {
    lastframedurationms = framedurationms;
    justrendered = false;
    return false;
  }
  else if (framedurationms < eachframemillis) {
    if (++framedurationms < eachframemillis) idle();
    return false;
  }
  justrendered = true;
  thisframestart = now;
  return true;
}


// ***** A P P L I C A T I O N

// DEFINES
#define CHARSPACE ':' // Character for space symbol
#define CHARSHIFT '=' // Character for shift symbol
#define CHARREC '@' // Character for recording symbol
#define POWEROFFTIME 30 // Time for auto poweroff in s
#define DISPLAYOFFTIME 20 // Time for auto displayoff in s
#define DIMTIME 10 // Time for auto displaydim in s
#define MAXSTRBUF 12 // Maximal length of string buffer sbuf[]
#define STACKSIZE 5 // Size of floatstack
#define TINYNUMBER 1e-7 // Number for rounding to 0
#define RAD 57.29578 // 180/PI
#define MAXITERATE 100 // Maximal number of Taylor series loops to iterate
#define FKEYNR 3 // 3 function keys
#define KEY_DUMMY 0xff // Needed to enter key-loop and printstring
#define EECONTRAST 0 // EEPROM address of brightness (1 byte)
#define EESTACK 1 // EEPROM address of stack ((4+1)*4 bytes)
#define EECMDKEY 21 // EEPROM address of command keys (10 bytes)
#define EECONST 31 // EEPROM address of constants (10*4 bytes)
#define EEREC 71 // EEPROM address Starting EE-address for saving "type recorder"
#define MAXREC 146 // Number of record steps per slot
//#define EECONST 61 // EEPROM address of constants (10*4 bytes)
//#define EEREC 101 // EEPROM address Starting EE-address for saving "type recorder"
//#define MAXREC 136 // Number of record steps per slot
#define MAXRECSLOT 3 // Maximal slots to record
#define BITEXP 1 // Bit for exp()
#define BITSIN 2 // Bit for sin()
#define BITASIN 4 // Bit for asin

// Macros
#define _abs(x) ((x<0)?(-x):(x)) // abs()-substitute macro
#define _ones(x) ((x)%10)        // Calculates ones unit
#define _tens(x) (((x)/10)%10)   // Calculates tens unit
#define _huns(x) (((x)/100)%10)  // Calculates hundreds unit
#define _tsds(x) (((x)/1000)%10) // Calculates thousands unit


// VARIABLES
static byte key; // Holds entered key
static byte oldkey; // Old key - use for debouncing
static char sbuf[MAXSTRBUF]; // Holds string to print
static double stack[STACKSIZE]; // Float stack (XYZT) and memory
static boolean isnewnumber = true; // True if stack has to be lifted before entering a new number
static boolean ispushed = false; // True if stack was already pushed by ENTER
static byte decimals = 0; // Number of decimals entered - used for input after decimal dot
static boolean isdot = false; // True if dot was pressed and decimals will be entered
static boolean isf = false; // true if f is pressed
static boolean ismenu = false; // True if menu demanded
static byte select = 0; // Selection number or playstring position
static boolean isplaystring = false; // True if string should be played
static byte brightness; // Contrast
static boolean isfirstrun = true; // Allows first run of loop and printscreen without key query
static long timestamp = 0; // Needed for timing of power manangement
static int recptr = 0; // Pointer to recording step
static byte recslot = 0; // Slot number for recording to EEPROM
static boolean isrec = false, isplay = false; // True, if "Type Recorder" records or plays
static double sum[STACKSIZE] = {0.0, 0.0, 0.0, 0.0, 0.0}; // Memory to save statistic sums
static double shadow[STACKSIZE] = {0.0, 0.0, 0.0, 0.0, 0.0}; // Shadow memory (buffer) for stack
static byte restore; // Position of stack salvation (including mem)


// COMMANDS
const char c0[] PROGMEM = ",X";   // Squareroot
const char c1[] PROGMEM = "Y;";   // Raise to the power of
const char c2[] PROGMEM = "1/X";  // Reciprocal
const char c3[] PROGMEM = "EXP";  // Exponential
const char c4[] PROGMEM = "LN";   // Natural logarithm
const char c5[] PROGMEM = "?";    // Gamma function (due to Nemes)
const char c6[] PROGMEM = ">P";   // Rectangular to polar coordinates
const char c7[] PROGMEM = ">R";   // Polar to rectangular coordinates
const char c8[] PROGMEM = "PV";   // Present value (annuity)
const char c9[] PROGMEM = "ND";   // Normal distribution (CDF/PDF)
const char c10[] PROGMEM = "STA"; // Statistics
const char c11[] PROGMEM = "LR";  // Linear regression
const char c12[] PROGMEM = "SIN"; // Sine
const char c13[] PROGMEM = "COS"; // Cosine
const char c14[] PROGMEM = "TAN"; // Tangent
const char c15[] PROGMEM = "ASN"; // Inverse sine
const char c16[] PROGMEM = "ACS"; // Inverse cosine
const char c17[] PROGMEM = "ATN"; // Inverse tangent
const char c18[] PROGMEM = "SNH"; // Hyperbolic sine
const char c19[] PROGMEM = "CSH"; // Hyperbolic cosine
const char c20[] PROGMEM = "TNH"; // Hyperbolic tangent
const char c21[] PROGMEM = "ASH"; // Inverse hyperbolic sine
const char c22[] PROGMEM = "ACH"; // Inverse hyperbolic cosine
const char c23[] PROGMEM = "ATH"; // Inverse hyperbolic tangent
const char c24[] PROGMEM = "CST"; // Set constant key
const char c25[] PROGMEM = "CMD"; // Set command key
const char c26[] PROGMEM = "LIT"; // Set contrast/brightness
const char c27[] PROGMEM = "@1"; //  ... Record user keys
const char c28[] PROGMEM = "@2"; //
const char c29[] PROGMEM = "@3"; //
const char c30[] PROGMEM = "<1"; //  ... Play user keys
const char c31[] PROGMEM = "<2"; //
const char c32[] PROGMEM = "<3"; //
const char* const cmd[] PROGMEM = {
  c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20,
  c21, c22, c23, c24, c25, c26, c27, c28, c29, c30, c31, c32
};
#define numberofcommands (sizeof(cmd)/sizeof(const char *))


// PLAYSTRINGS (Don't play a playstring!)
#define PSCOS 0
#define PSTAN 1
#define PSACOS 2
#define PSATAN 3
#define PSPV 4
#define PSGAMMA 5
#define PSSINH 6
#define PSCOSH 7
#define PSTANH 8
#define PSASINH 9
#define PSACOSH 10
#define PSATANH 11
#define PSSUM 12
#define RESTORE2 13 // Leave stack[0] and stack[1] unchanged when salvaging from shadow[]
#define PSND 13
#define PSR2P 14
#define PSP2R 15
#define PSSTAT 16
#define PSLR 17
const char s0[] PROGMEM = ">90L["; // COS
const char s1[] PROGMEM = "[==E>1LOH"; // TAN
const char s2[] PROGMEM = "^>90L"; // ACOS
const char s3[] PROGMEM = "===E1LOQE^"; // ATAN
const char s4[] PROGMEM = ">I=1LIMIP>1LKH"; // PV
const char s5[] PROGMEM = "1L===12EI10EQBQL1RHIP2:506628KOHE"; // GAMMA
const char s6[] PROGMEM = "R=Q>L2H"; // SINH
const char s7[] PROGMEM = "R=QL2H"; // COSH
const char s8[] PROGMEM = "2ER1B=2LH"; // TANH
const char s9[] PROGMEM = "==E1LOLS"; // ASINH
const char s10[] PROGMEM = "==E1BOLS"; // ACOSH
const char s11[] PROGMEM = "==1LI>1LHOS"; // ATANH
const char s12[] PROGMEM = "1AM==EMEMIOp"; // SUM+
const char s13[] PROGMEM = "===EE:07E>I1:6EBR1LQI=E>2HR:3989423E"; // ND
const char s14[] PROGMEM = "=EI==MELO=MH^K"; // R2P
const char s15[] PROGMEM = "I[==E>1LOMEMEIM"; // P2R
const char s16[] PROGMEM = "IM=@H=ME>L@1BHOI"; // STAT
//const char s17[] PROGMEM = "I2H>==EIMIBOMM=K=MIMBMLIM"; // QE
const char s17[] PROGMEM = "qEI@EK@EKrq=EsIMBMBKH===rqtKEB@HI"; // L.R.
const char* const pstable[] PROGMEM = {s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17};
char playbuf[40]; // Holds sii[]

// Function pointer array/table
static void (*dispatch[])(void) = {
  &_numinput, // Normal calculator keys (dispatch 0)     OFFSET: 0
  &_nop, &_ceclx, &_ee, &_enter, &_chs, // 1d:15 2c;13 3e<5 4x=16 5s>9 (no 6f?1)
  &_batt, &_rcl, &_sto, &_sub, // Shiftkeys 0 1 2 3  OFFSET: 6
  &_const, &_cmdkey, &_mult, &_menu, //         4 5 6 7
  &_sum, &_div, &_swap, &_sleep, //             8 9 d c
  &_rotup, &_add, &_rot, &_nop, //              e x s f
  &_sqrt, &_pow, &_inv, // MENU                          OFFSET: 21 //22
  &_exp, &_ln, &_gamma, // Mathematical functions and settings
  &_r2p, &_p2r, &_pv,
  &_nd, &_stat, &_lr,
  &_sin, &_cos, &_tan, // Trigonometrical functions
  &_asin, &_acos, &_atan,
  &_sinh, &_cosh, &_tanh, // Hyperbolical functions
  &_asinh, &_acosh, &_atanh,
  &_setconst, &_setcmdkey, &_contrast, // Settings
  &_rec, &_rec, &_rec, // User definable Menukeys       OFFSET: 48 //49
  &_play, &_play, &_play,
  &_sum1, &_sum2stack, &_shadowsave, &_shadowload1, &_shadowload2// Hidden links
};

// Function pointer array subroutines
static void _add(void) { // ADD +
  stack[0] += stack[1];
  _pullupper();
}
static void _acos(void) { // ACOS
  _playstring(PSACOS);
}
static void _acosh(void) { // ACOSH
  _playstring(PSACOSH);
}
static void _asin(void) { // ASIN
  stack[0] = _exp_sin_asin(stack[0], BITASIN) * RAD;
}
static void _asinh(void) { // ASINH
  _playstring(PSASINH);
}
static void _atan(void) { // ATAN
  _playstring(PSATAN);
}
static void _atanh(void) { // ATANH
  _playstring(PSATANH);
}
static void _batt(void) { // BATT
  _push();
  ADMUX = _BV(MUX3) | _BV(MUX2); // Set voltage bits for ATTINY85
  delayshort(10); // Settle Vref
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC)) ; // Measuring
  byte high = ADCH;
  stack[0] = 1125.3 / ((high << 8) | ADCL); // Calculate Vcc in V - 1125.3 = 1.1 * 1023
}
static void _ce(void) { // CE
  if (isdot) {
    if (stack[0] > TINYNUMBER && decimals > 0) {
      decimals--;
      stack[0] = (long)(stack[0] * _pow10(decimals)) / _pow10(decimals);
    }
    else isdot = false;
  }
  else stack[0] = (long)(stack[0] / 10);
}
static void _ceclx(void) { // CE/CLX
  if (isnewnumber) _clx();
  else _ce();
}
static void _chs(void) { // CHS
  stack[0] = -stack[0];
  isnewnumber = true;
}
static void _clx(void) { // CLX
  stack[0] = 0.0;
}
static void _cmdkey(void) { // CMDKEY
  if (is09) {
    byte tmp = stack[0];
    _pull();
    (*dispatch[EEPROM.read(EECMDKEY + tmp)])();
  }
}
static void _const(void) { // CONST
  if (is09) EEPROM.get(EECONST + (byte)stack[0], stack[0]);
}
static void _contrast(void) { // CONTRAST
  brightness = stack[0];
  dcontrast(brightness);
}
static void _cos(void) { // COS
  _playstring(PSCOS);
}
static void _cosh(void) {  // COSH
  _playstring(PSCOSH);
}
static void _div(void) { // DIV /
  stack[0] = stack[1] / stack[0];
  _pullupper();
}
static void _dot(void) { // DOT .
  if (!isf) {
    _newnumber();
    isdot = true;
  }
}
static void _ee(void) { // EE
  stack[0] = _pow10(stack[0]);
  _mult();
  isnewnumber = true;
}
static void _enter(void) { // ENTER
  _push();
  ispushed = isnewnumber = true;
}
static void _exp(void) {  // EXP
  stack[0] = _exp_sin_asin(stack[0], BITEXP);
}
static void _gamma(void) { // GAMMA
  _playstring(PSGAMMA);
}
static void _inv(void) { // INV
  stack[0] = 1 / stack[0];
}
static void _ln(void) { // LN
  stack[0] = log(stack[0]);
}
static void _lr(void) { // L.R.
  _playstring(PSLR);
}
static void _menu(void) { // MENU
  ismenu = true;
  select = 0;
}
static void _mult(void) { // MULT *
  stack[0] *= stack[1];
  _pullupper();
}
static void _nd(void) { // ND
  _playstring(PSND);
}
static void _newnumber(void) { // NEW number
  if (isnewnumber) { // New number
    //if (ispushed) ispushed = false;
    //else _push();
    if (!ispushed) _push();
    stack[0] = 0.0;
    decimals = 0;
    isdot = isnewnumber = false;
  }
}
static void _nop(void) {} // NOP - no operation
static void _numinput(void) { // NUM Numeric input (0...9)
  _newnumber();
  if (isdot) stack[0] += (key - KEY_14) / _pow10(++decimals); // Append decimal to number
  else { // Append digit to number
    stack[0] *= 10;
    stack[0] += key - KEY_14;
  }
}
static void _p2r(void) { // P2R
  _playstring(PSP2R);
}
static void _recplay(void) { // Prepare variables for REC or PLAY
  recslot = key - KEY_2;
  recptr = EEREC + recslot * MAXREC;
}
static void _play(void) { // PLAY
  _recplay();
  isplay = isnewnumber = true;
}
static void _playstring(byte slot) {  // PLAYSTRING
  restore = slot >= RESTORE2 ? 2 : 1;
  _shadowsave(); // Save stack for salvation
  strcpy_P(playbuf, (char*)pgm_read_word(&(pstable[slot]))); // Copy playstring to buffer
  select = 0;
  isnewnumber = isplaystring = true;
  ispushed = isdot = false;
}
static double _pow(void) { // POW
  _swap();
  _ln();
  _mult();
  _exp();
}
static double _pow10(int8_t e) { // POW10 10^x
  double f = 1.0;
  if (e > 0) while (e--) f *= 10;
  else while (e++) f /= 10;
  return (f);
}
void _pull(void) { // PULL
  memcpy(&stack[0], &stack[1], (STACKSIZE - 2) * sizeof(double));
}
static void _pullupper(void) { // PULLUPPER
  memcpy(&stack[1], &stack[2], (STACKSIZE - 3) * sizeof(double));
}
static void _push(void) { // PUSH
  memmove(&stack[1], &stack[0], (STACKSIZE - 2) * sizeof(double));
}
static void _pv(void) { // PV
  _playstring(PSPV);
}
/*static void _qe(void) { // QE
  _playstring(PSQE);
  }*/
static void _r2p(void) { // R2P
  _playstring(PSR2P);
}
static void _rcl(void) { // RCL
  _push();
  stack[0] = stack[STACKSIZE - 1];
}
static void _rec(void) { // REC
  _recplay();
  isrec = true;
}
static void _rotup(void) { // ROTup
  for (byte i = 0; i < STACKSIZE - 2; i++) _rot();
}
static void _rot(void) { // ROT
  double tmp = stack[0];
  _pull();
  stack[STACKSIZE - 2] = tmp;
}
static void _setcmdkey(void) { // SETCMDKEY
  if (is09) EEPROM.write(EECMDKEY + (byte)stack[0], (byte)stack[1]);
}
static void _setconst(void) { // SETCONST
  if (is09) EEPROM.put(EECONST + (byte)stack[0], stack[1]);
}
static void _shadowsave(void) { // Save stack to shadow buffer (including mem)
  memcpy(shadow, stack, STACKSIZE * sizeof(double));
}
static void _shadowload1(void) { // Load stack from shadow buffer from pos 1
  _shadowload(1);
}
static void _shadowload2(void) { // Load stack from shadow buffer from pos 2
  _shadowload(2);
}
static void _shadowload(byte pos) { // Load higher stack from shadow buffer
  memcpy(&stack[pos], &shadow[pos], (STACKSIZE - pos)*sizeof(double));
}
static void _sin(void) {  // SIN
  stack[0] = _exp_sin_asin(stack[0] / RAD, BITSIN);
}
static void _sinh(void) {  // SINH
  _playstring(PSSINH);
}
static void _sleep(void) { // SLEEP
  EEPROM[EECONTRAST] = brightness; // Save brightness to EEPROM
  savestack();
  sleep();
}
static void _sqrt(void) { // SQRT
  if (stack[0] != 0.0) {
    _ln();
    stack[0] *= 0.5;
    _exp();
  }
}
static void _stat(void) { // STAT
  _sum2stack();
  _playstring(PSSTAT);
}
static void _sto(void) { // STO
  stack[STACKSIZE - 1] = stack[0];
}
static void _sub(void) { // SUB -
  stack[0] = stack[1] - stack[0];
  _pullupper();
}
static void _sum(void) { // SUM
  _playstring(PSSUM);
}
static void _sum1(void) { // SUM addon
  for (byte i = 0; i < STACKSIZE; i++) sum[i] += stack[i];
  _sum2stack(); // Show n
  _rcl();
}
static void _sum2stack(void) { // Copies sum[] to stack[] (including mem)
  memmove(stack, sum, STACKSIZE * sizeof(double));
}
static void _swap(void) { // SWAP
  double tmp = stack[0];
  stack[0] = stack[1];
  stack[1] = tmp;
}
static void _tan(void) { // TAN
  _playstring(PSTAN);
}
static void _tanh(void) { // TANH
  _playstring(PSTANH);
}


// SUBROUTINES

static void savestack(void) { // Save whole stack to EEPROM
  byte *p = (byte *)stack;
  for (byte i = 0; i < STACKSIZE * sizeof(double); i++) EEPROM[EESTACK + i] = *p++;
}
static void loadstack(void) { // Load whole stack from EEMPROM
  byte *p = (byte *)stack;
  for (byte i = 0; i < sizeof(double)*STACKSIZE; i++) *p++ = EEPROM[EESTACK + i];
}

static boolean is09(void) { // Checks, if stack[0] is between 0 and 9
  return (stack[0] >= 0 && stack[0] <= 9);
}

#define CSTEPS 20 // Cordic steps
#define CMULT 1e6 // To convert long to double - 6 digit precision
long const ai[CSTEPS] = { // Cordic A-values (atan(2^-m)) in degrees - 6 digit precision
  45000000,
  26565051,
  14036243,
  7125016,
  3576334,
  1789911,
  895174,
  447614,
  223811,
  111906,
  55953,
  27976,
  13988,
  6994,
  3497,
  1749,
  874,
  437,
  219,
  109
};
//static double _cordic(double f) { // Calculate sin
static long _cordic(long z) { // Calculate sin
  byte m = 0;
  long a = ai[0];
//  long z = f * CMULT;
  long c = 607253, s = 0;
  for (byte i = 1; i < CSTEPS; i++) {
    long ctmp = c;
    if (z >= 0) {
      c -= s >> m;
      s += ctmp >> m;
      z = z - a;
    }
    else {
      c += s >> m;
      s -= ctmp >> m;
      z +=  a;
    }
    a = ai[i];
    m++;
  }
  //return (s / CMULT);
  return (s);
}



static double _exp_sin_asin(double f, byte nr) { // Calculate exp, sin or asin
  //if (nr == BITSIN) return (_cordic(f)*CMULT);
  //if (nr == BITSIN) return sin(f);
  //if (nr == BITASIN) return asin(f);
  //if(nr==BITEXP) return exp(f);
  /*double result =1.0, frac = 1.0; // Start values for exp
  for (int i = 1; frac > TINYNUMBER && i < MAXITERATE; i++) {
    frac *= f / i; // Fraction for exp
    result += frac;
  }
  return (result);//*/
  double result = f, frac = f ; // Start values for sin or asin
  if (nr == BITEXP) result = frac = 1.0; // Start values for exp
  for (int i = 1; _abs(frac) > TINYNUMBER && i < MAXITERATE; i++) {
    int i2 = 2 * i, i2p = 2 * i + 1, i2m = 2 * i - 1, i2m2 = i2m * i2m;
    double ffi2i2p = f * f / (i2 * i2p);
    if (nr == BITEXP) frac *= f / i; // Fraction for exp
    else if (nr == BITSIN) frac *=  -ffi2i2p; // Fraction for sin
    else frac *= ffi2i2p * i2m2; // Fraction for asin
    result += frac;
  }
  return (result);
}

static void printfloat(double f, byte mh, byte y) { // Print float with mantissa height (mh) at line y
  long m; // Mantissa
  int8_t e; // Exponent
  sbuf[0] = CHARSPACE; // * Create sign
  if (f < 0.0) {
    f = - f;
    sbuf[0] = '-';
  }
  e = log(f) / log(10); // * Calculate exponent (without using log10())
  m = (f / _pow10(e - 5)) + 0.5; // * Create mantissa
  if (m > 0 && m < 1e5) // Change (n-1)-digit-mantissa to n digits
    m = (f / _pow10(--e - 5)) + 0.5; // "New" mantissa
  for (byte i = 6; i > 0; i--) { // Print mantissa
    sbuf[i] = _ones(m) + '0';
    m /= 10;
  }
  sbuf[7] = e < 0 ? '-' : CHARSPACE; // * Create exponent
  if (e < 0) e = -e;
  sbuf[8] = e >= 10 ? _tens(e) + '0' : '0';
  sbuf[9] = _ones(e) + '0';
  printcat(sbuf[0], SIZEM, mh , 0, y); // * Print sbuf in float format
  printcat('.', SIZEM, mh, 23, y);
  printcat(sbuf[1], SIZEM, mh, 12 , y);
  //for (byte i = 2; i < 7; i++) printcat(sbuf[i], 2, mh, 12 * i + 9 , y); // With trailing zeros
  byte nonzero = false; // Suppress trailing zeros
  for (byte i = 6; i > 1; i--)
    if (sbuf[i] != '0' || nonzero) {
      nonzero = true;
      printcat(sbuf[i], SIZEM, mh, 12 * i + 8 , y);
    }
  for (byte i = 7; i < 10; i++) printcat(sbuf[i], SIZEM, SIZEM, 12 * i + 10 , 0);
}

static void printscreen(void) { // Print screen due to state
  byte mh = SIZEM; // Mantissa height
  cls();
  printbitshift = 1; // Shift second line one pixel down
  if (isplaystring) printsat("RUN", SIZEM, SIZEM, 0, 2); // Print running message
  else if (ismenu) { // Print MENU above F-keys (789)
    for (byte i = 0; i < FKEYNR; i++) {
      strcpy_P(sbuf, (char*)pgm_read_word(&(cmd[select * FKEYNR + i])));
      printsat(sbuf, SIZEM, SIZEM, 47 * i, 2);
    }
  }
  else {
    mh = SIZEL;
    sbuf[2] = NULL; // Print record and/or shift sign
    sbuf[0] = sbuf[1] = CHARSPACE;
    if (isrec) sbuf[0] = CHARREC;
    if (isf) sbuf[1] = CHARSHIFT;
    printsat(sbuf, SIZEM, SIZEM, 106, 2);
  }
  printbitshift = 0;
  printfloat(stack[0], mh, 0); // Print stack[0]
  display();
}


// *****  S E T U P  A N D  L O O P

void setup() {
  // INIT DISPLAY
  dinit(); // Init display
  drender(); // Render current half of GDDRAM

  // INIT WAKEUP (with pin change interrupt) ... same pin as keyboard!
  pinMode (KPIN, INPUT); // Wakeup pin
  PCMSK  |= bit (KPIN);  // want pin D3 / H2
  GIFR   |= bit (PCIF);  // Clear any outstanding interrupts
  GIMSK  |= bit (PCIE);  // Enable pin change interrupts

  // INIT SYSTEM
  setframerate(FRAMERATE);
  dcontrast(brightness = EEPROM[EECONTRAST]);

  // START
  loadstack(); // Read stack from EEPROM (saved with sleep/OFF)
}


void loop() {
  if (!(nextframe())) return; // Pause render (idle) until it's time for the next frame

  if (isfirstrun) {
    isfirstrun = false;
    key = KEY_DUMMY;
  }
  else {
    key = getkeycode();
    if (key == oldkey) key = NULL; // No key repeat
    else oldkey = key;
  }

  if (key) { // ### Power management
    timestamp = millis();// Keep awake when active
    dcontrast(brightness);
    don();
  }
  byte pot = (millis() - timestamp) / 1000L; // Control power management
  if (pot > POWEROFFTIME) sleep(); // Sleep
  else if (pot > DISPLAYOFFTIME) doff(); // Display off
  else if (pot > DIMTIME) dcontrast(0x00); // Dim display

  if (isplaystring) { // ### Play string
    key = playbuf[select];
    if (key == NULL) { // Stop playstring
      _shadowload(restore); // Restore upper part of stack
      isplaystring = false;
      isnewnumber = true;
      key = KEY_DUMMY;
    }
    else { // Go on for dispatching
      if (key <= KEY_15 && ((select == 0) || (select > 0 && playbuf[select - 1] > KEY_15))) { // New number (0-9,.)
        isnewnumber = true;
        ispushed = false;
      }
      select++;
    }
  }

  else if (isrec || isplay) { // ### Type recorder (else: playstring works inside play)
    int maxptr = EEREC + (recslot + 1) * MAXREC;
    if (isrec) { // Record keys and write to EEPPROM
      if (key && recptr < maxptr) EEPROM[recptr++] = key;
    }
    else { // Read/play key from EEPROM
      if (key == KEY_13) { // Stop execution
        isplay = false;
        key = KEY_DUMMY;
      }
      key = EEPROM[recptr++];
    }
    if (key == KEY_13 || recptr >= maxptr) {
      isplay = isrec = false;
      key = KEY_DUMMY;
    }
  }

  if (key == KEY_1) { // ### SHIFT pressed
    isf = isf ? false : true; // Toggle shift key
    key = KEY_DUMMY;
  }
  else if (key == KEY_15) _dot(); // ### Entering decimals demanded

  if (key) { // ### Execute key
    if (key != KEY_DUMMY) { // Printscreen only
      if (ismenu) { // Select and dispatch MENU
        byte limit = numberofcommands / FKEYNR - 1;
        if (key == KEY_5) { // Cursor up
          if (select > 0) select--;
          else select = limit;
        }
        else if (key == KEY_9) { // Cursor down
          if (select < limit) select++;
          else select = 0;
        }
        else if (key == KEY_13) { // ESC
          ismenu = false;
        }
        else if (key >= KEY_2 && key <= KEY_4) { // F-KEY "7 8 9"
          (*dispatch[22 + select * FKEYNR + key - KEY_2])(); // Dispatch with menu offset
          isnewnumber = true;
          ismenu = false;
        }
      }
      else if (isf) { // Dispatch shifted key
        (*dispatch[key - KEY_14 + 6])();
        isf = ispushed = false;
        isnewnumber = true;
      }
      else { // Dispatch anything else
        if (key <= KEY_4) (*dispatch[0])(); // Dispatch number 0(0...9)
        else (*dispatch[key - KEY_4])(); // Dispatch key (1d: 2c; 3e< 4x= 5s> 6f? + other) due to function table
      }
    }
    printscreen(); // Print screen every keypress (or if key == KEY_DUMMY)
  }

}
