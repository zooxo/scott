# SCOTT
Scientific RPN Calculator with OLED display and ATTINY85

So far I reached my target - and the limit of the flash memory with 8156 (of 8192) bytes.

Still searching ways to optimize and speed up - to add some features and comfort.
Any idea is highly welcome.
So far I got some hints to use CORDIC or use my own double format (struct with mantissa and exponent).

Documentation is poor. Official version 1.0 is still on hold.
---

# SCOTT - Scientific RPN Calculator with OLED display and ATTINY85 microcontroller
  Version 1.0, (c) 2019 by deetee/zooxo

SCOTT is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation.

## PREAMBLE

To examine how much "calculator power" a simple microcontroller like the ATTINY85 is able to perform was already the intention of ScArY (www.github.com/zooxo/scary). Unfortunately ScArY was based on a premanufactured development board (QYF-TM1638) and was restricted to a 7 segment display.

Beside an ATTINY85 microcontroller SCOTT is based on an OLED display and a 16 key one wire keypad. As SCOTT is powered by one 3V battery only a good power management is essential.

Two of the five regular I/O-pins of the ATTINY are used to control the display (I2C communication) and one (analog) pin is used to handle the one wire keypad. So far two pins are available for further developments (i.e. sensors).

SCOTT uses the internal RAM of the SSD1306 display controller as screenbuffer, what saves valuable RAM of the ATTINY. And to speed up the relatively slow I2C-communication with the display SCOTT splits the controller RAM. So while one half of the RAM is used to display its content the other half is filled with content via the I2C line. With a RAM swap the new content is displayed rapidly (without the effect of "dropping in characters").

Like in ScArY the functionality of SCOTT outweights comfort and error handling which can be seen for instance at the permanent scientific display format or a "non interpretable" display after dividing by zero.

Features of SCOTT:
- Scientific RPN calculator (similar to those famous HP calculators)
- Power settings: Set the contrast of the display and save the value permanently to EEPROM, display the recent battery voltage and shift SCOTT into a deep sleep mode.
- Function menu to browse and call every operation of SCOTT comfortable with cursor and function keys ("catalog").
- Basic functions for number input, basic arithmetical operation and memory.
- Low level functions like squareroot, power and inverse.
- High level functions like exponential, logarithmic, trigonometric and hyperbolic.
- User definable keys to store and load 10 physical constants to/from EEPROM permanently.
- User definable keys for fast access of 10 functions/commands.
- Basic financial function (present value).
- Conversion of polar and rectangular coordinates.
- Gaussian normal distribution (CDF, PDF).
- Statistics (Mean value, standard deviation).
- Linear Regression (a, b - where y = a x + b).
- Type recorder (record and play 3 user defined sequences of keypresses).

Enjoy!  
deetee

## THE CALCULATOR

### DISPLAY (permanent SCI notation)

           mantissa         exponent                  mantissa         exponent
       ____|________________|____                ____|________________|____
      |    |                     |              |    |                |    |
      |    |              - e e  |              |  - m . m m m m m  - e e  |
      |  - m . m m m m m    o ^  |              |  M1(7)    M2(8)    M3(9) |
      |__|__________________|_|__|              |__|________|________|_____|
         |                  | |                    |        |        |
         sign               | shift indicator   MENU (function keys 7, 8 and 9)
                            record indicator


### KEYBOARD

      [SHIFT]            7 [(MENU)] (F1)     8 [SUM+] (F2)      9   [/](F3)
      EE [ROT](UP)       4 [CONST]           5 [CMD]            6   [*]
      +- [ROT](DOWN)     1 [RCL]             2 [STO]            3   [-]
      C  [OFF]           0 [BATT]            . [SWAP]         ENTER [+]

### BASIC KEYS
      0 1 2 3 4 5 6 7 8 9 . ... Digits and decimal point
      CHS EE                ... Change sign and enter exponent (actually Y*10^X)
      ENTER                 ... Enter number (push stack)
      C                     ... Clear X, clear entry or escape/stop
      f                     ... Function or shift to choose shifted keys

### FUNCTION KEYS
      + - * /    ... Basic operations
      STO RCL    ... Store number to respectively recall number from memory
      ROT        ... Rotate stack up or down
      SUM+       ... Enter X-data for statistics or X,Y-data for linear regression
      SWAP       ... Swap X and Y register of stack (X<->Y)
      MENU       ... Browse menu
      CONST      ... Recall one of 10 user defined constants
      CMD        ... Execute one (of 10) user defined command keys
      BATT       ... Shows the battery voltage in V
      OFF        ... Deep sleep mode

### MENU
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

## ENTERING NUMBERS
       1 Enter mantissa (with '.' if applicable)
         Press ENTER to push mantissa to stack
       2 To enter power of ten:
         Enter exponent and (if applicable) change sign of exponent with +/-
       3 Press EE to link the mantissa with the exponent
       4 If applicable: Toggle sign of number with +/-

## SPECIALITIES

### PREPARING SCOTT AFTER FLASHING
As SCOTT saves the state (stack and brightness values) when the deep sleep mode (OFF) is entered manually it also loads the state after switching on. But flashing the ATTINY may clear the EEPROM. So the loaded state when switching on the first time after flashing gets undefined values ( ... and a "non interpretable" display).

The following procedure may help to bring SCOTT in a defined state:
        1 Press C (X=0)       ... a regular 0-value is shown (register X)
        2 Press ENTER 3 times ... clear the stack (X=Y=Z=T=0)
        3 Press STO (f-2)     ... clear mem
        4 Press OFF (f-C)     ... save state to EEPROM (activate deep sleep)
        5 Press f             ... wake up from deep sleep mode

### THE MENU CATALOG
To enter the menu catalog press MENU (f-7) and browse the menu items with the UP and DOWN keys (EE and +-). To execute one of the three displayed functions press F1, F2 or F3 (7, 8 or 9).

### WORKING WITH CONSTANTS
SCOTT is capable to save up to 10 constants permanently to the EEPROM memory. See "Appendix - Physical Constants" for some physical constants.

      Example to write PI to slot 4:
        1 3.141593 ENTER ... enter value for PI
        2 Press 4        ... enter desired slot (number 4)
        3 MENU (f-7)     ... enter MENU
        4 Press 8xDOWN   ... goto menu line 9 (alternatively press 3xUP)
        5 Press 7 (F1)   ... set CONST

      Example to load stored constant (from slot 4):
        1 Press 4           ... enter desired slot (number 4)
        2 Press CONST (f-4) ... recall saved constant

### USER DEFINED COMMAND KEY
SCOTT is capable to save 10 user defined function keys for fast access to intrinsic commands. Get the appropriate number for the desired command from the following table:

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

      Example to save the function SIN to slot 4:
        1 Enter 34 ENTER ... enter number for function SIN (34)
        2 Press 4        ... enter desired slot (number 4)
        3 MENU (f-7)     ... enter MENU
        4 Press 8xDOWN   ... goto menu line 9 (alternatively press 3xUP)
        5 Press 8 (F2)   ... set CMD

      Example to execute command stored in slot 4:
        1 Press 4         ... enter desired slot (number 4)
        2 Press CMD (f-5) ... recall saved command

### TYPE RECORDER
SCOTT is able to record 3 user defined sequences of keypresses (up to 146 each) and "replay" them by selecting the appropriate user menu entry. These user defined key sequences or formulas are stored to the EEPROM memory permanently.

      Example to record temperature conversion (Fahrenheit -> Celsius):
        1 MENU (f-7)   ... enter MENU
        2 Press 2xUP   ... goto menu line 9 (record keys)
        3 Press 7 (F1) ... enter recording slot 1 (record indicator is displayed)
        4 32 - 1.8 /   ... convert to celsius
        5 Press C      ... stop recording

      Convert Fahrenheit to Celsius:
        1 50           ... enter value to convert
        2 MENU (f-7)   ... enter MENU
        3 Press UP     ... goto last menu line (play keys)
        4 Press 7 (F1) ... enter playing slot 1
        5 X=10         ... 50 Fahrenheit are 10 Celsius

### POWER MANAGEMENT
As SCOTT is supplied with battery power a severe power management is essential. After 10 seconds without pressing a key the display brightness will be reduced to a minimum. After 10 more seconds the display will be deactivated. Till now every further keypress will be interpreted as normal keypress for calculations.  
After 10 more seconds without pressing a key SCOTT falls into a deep sleep mode. Only a pin change (pressing any key in the upper part of the keyboard) wakes SCOTT up, but this keypress will not be interpreted as normal keypress for calculations.  
If demanded the user can enter the sleep mode by pressing SHIFT and C. Additionally SCOTT saves the stack and brightness values to the EEPROM. By pressing SHIFT and 0 the battery voltage will be shown. The default brightness of the display can be set by entering a value (0...255) followed CMD from the MENU (SHIFT and 7).  
Running on a single battery (CR2032) SCOTT draws a current of 10 mA (bright display with a lot of "8s"). With a battery capacity of at least 200 mAh SCOTT can calculate approximately 20 hours. After dimming the current falls to 5.5 mA, after deactivating the display 1.1 mA are needed.  
In sleep mode SCOTT consumes less than 0.25 mA. With a battery capacity of at least 200 mAh SCOTT lasts more than a month in sleep mode.

### ANNUITY (PRESENT VALUE)
      Example to calculate the present value of a $1 5 year return with an
      interest rate of 8%:
        1 .08 ENTER 5  ... interest rate and periods
        1 MENU (f-7)   ... enter MENU
        2 Press 2xDOWN ... goto menu line 3
        4 Press 9 (F3) ... enter PV
        2 X=3.99       ... 5 years $1 each are equal to ~$4 invested with 8%

### NORMAL DISTRIBUTION (GAUSS)
                     y
                     ^
                     |
                   1 ------------------------
                     |       +++++++ CDF (Cumulative Distribution Function)
                     |   +
                     | +                     (x)         .
                     |+              CDF = integral(PDF) = 1/(1+exp(-0.07*x^3-1.6*x))
                     +                      (-inf)
                   **+**
                **  +|   **
                  +  |     *         PDF = 1/sqrt(2*PI)*exp(-x*x/2)
             ** +    |      **
      +*+*+*+        |         ***** PDF (Probability Density Function)
      ---------------+------------------------> x

      Example to calculate PDF and CDF at x=0:
        1 0 ND (MENU - 3xDOWN - 7(F1))
        2 PDF=0.3989=1/sqrt(2*PI)
        3 SWAP
        4 CDF=0.5

### STATISTICS, LINEAR REGRESSION
        y
        ^                /
        |  Ypredict(x=1.5)=4
      4 - <----------- /
        |             /
        |            / ^
        |           /  |
        |          /
      3 -        [*] P2=(1|3)
        |        /
        |       /      ^
        |      /       |
        |     /
      2 -   [*] P1=(0.5|2)
        |   /
        |_ /           ^
        | /|           |
        |/ |           |
      1 -  |           |
       /|  |           |
      / |  |           |
     /  |  | Xpredict(y=1.5)=0.25
        |  v           |
    ----+----+----|----+----|-> x
        0       | 1         2
                |
                Xmean=0.75
             |  |
           ->|--|<- Standard Deviation S=0.354

      Example to calculate mean value and standard deviation:
        1 0.5 SUM 1 SUM ... enter X-data (SUM+ ... f-8)
        2 STA (MENU - 3xDOWN - 8(F2))
        3 Xmean=0.75 (mean value)
        4 SWAP
        5 S=0.354 (standard deviation)

      Example to calculate the line of best fit (LR ... linear regression):
        1 2 ENTER 0.5 SUM ... enter YX-data of first node (SUM+ ... f-8)
        2 3 ENTER 1 SUM   ... enter YX-data of second node (SUM+ ... f-8)
        3 LR (MENU - 3xDOWN - 9(F3))
        4 X=2             ... slope of best-fit-line
        5 SWAP
        6 Y=1             ... y-axis-intercept (y = 2x + 1)

### EEPROM usage (512 bytes)

      | CONTRAST | STACK | CMD | CONST | REC1 | REC2 | REC3 | EOREC | EOEE
      0          1       21    31      71     217    363    509     511
      | +1       | +20   | +10 | +40   | +146 | +146 | +146 | +2    |

## HARDWARE

### ATTINY85 PINS
                              _____
      Analog0/Reset D5 S0  H1|* U  |H8  S7     Vcc
            Analog2 D3 S1  H2|     |H7  S6 D2  SCK/Analog1
            Analog3 D4 S2  H3|     |H6  S5 D1  PWM1/MISO
                GND    S3  H4|_____|H5  S4 D0  PWM0/AREF/SDA/MOSI

      H...Hardware pin
      S...Software pin
      D...Digispark pin


### CIRCUIT DIAGRAM
                ____                   _________
               |*   |--Vcc       Vcc--| DISPLAY |
       +---D3--| AT |--SCK-------SCK--| SSD1306 |
       |       |TINY|            GND--| 128x32  |
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

### ATTINY85 Microcontroller
      - 8192 bytes of flash memory (stores program code)
      - 512 bytes of SRAM (holding variables)
      - 512 bytes or EEPROM (stores settings and user data permanently)

### OLED Display
      - 128x32 pixels
      - I2C communication (occupies two pins of the microcontroller only)
      - Controlled by SSD1306

### KEYPAD
      - 16 keys
      - Analog keypad (occupies only one pin of the microcontroller)
      - Needs 7 resistors

### POWER SOURCE
      - 1 x CR2032 battery (3V)

## PLAYSTRING
To save flash memory a lot of functions are calculated with SCOTT itself (and not with libraries). Every command of SCOTT can be dispatched with one character (one byte command - see chapter COMMANDS). So a sequence of commands can be represented and "played" with a sequence of characters in a string.  
After playing a string the result(s) is/are shown in the register(s) X and/or Y. Other parts of the stack (including mem) will be restored from a "shadow stack".

### PLAYSTRING - PSEUDO CODES

####  COS (Cosine)
        COMMANDS (5): 90 + SIN

        Converts    4 mem  -    to:     -
        stack[]     ---------         -----
        (incl.      3  t   -            -        cos(x) = sqrt(1 - sin(x)*sin(x))
        mem)        2  z   -            -
        from:       1  y   -            -
                    0  x   x          cos(x)


#### TAN (Tangent)
        COMMANDS (9): SIN ENTER ENTER * CHS 1 + SQRT /

        Converts    4 mem  -    to:     -
        stack[]     ---------         -----               sin(x)          sin(x)
        (incl.      3  t   -            -        tan(x) = ------ = -----------------------
        mem)        2  z   -            -                 cos(x)   sqrt(1 - sin(x)*sin(x))
        from:       1  y   -            -
                    0  x   x          tan(x)


#### ACOS (Arcus Cosine)
        COMMANDS (4): ASIN CHS 90 +

        Converts    4 mem  -    to:     -
        stack[]     ---------         ------
        (incl.      3  t   -            -        acos(x) = 90 - asin(x)
        mem)        2  z   -            -
        from:       1  y   -            -
                    0  x   x          acos(x)


#### ATAN (Arcus Tangent)
        COMMANDS (10): ENTER ENTER ENTER * 1 + SQRT INV * ASIN

        Converts    4 mem  -    to:     -
        stack[]     ---------         ------                           x
        (incl.      3  t   -            -        atan(x) = asin( ------------- )
        mem)        2  z   -            -                        sqrt(1 + x*x)
        from:       1  y   -            -
                    0  x   x          atan(x)


#### PV (Present Value)
        COMMANDS (14): CHS SWAP ENTER 1 + SWAP ROT SWAP PWR CHS 1 + ROTup /

        Converts    4 mem  -    to:     -
        stack[]     ---------         -------              1 - (1+i)^-n
        (incl.      3  t   -            -        PV(i,n) = ------------
        mem)        2  z   -            -                       i
        from:       1  y   i            -
                    0  x   n          PV(i,n)


#### GAMMA (due to formula of Nemes)
        COMMANDS (33): 1 + ENTER ENTER ENTER 12 * SWAP 10 * INV - INV +
                     1 EXP / SWAP PWR 2.506628 ROTup SQRT / *


        Converts    4 mem  -    to:    -
        stack[]     ---------         ---                                 x + 1/(12*x - 1/10/x)
        (incl.      3  t   -           -         (x-1)! = sqrt(2*PI/x)* (----------------------)^x
        mem)        2  z   -           -                                            e
        from:       1  y   -           -
                    0  x   x           x!


#### SINH (Hyperbolic Sine)
        COMMANDS (7): EXP ENTER INV CHS + 2 /

        Converts    4 mem  -    to:      -
        stack[]     ---------         -------              exp(x) - exp(-x)
        (incl.      3  t   -             -       sinh(x) = ----------------
        mem)        2  z   -             -                        2
        from:       1  y   -             -
                    0  x   x          sinh(x)


#### COSH (Hyperbolic Cosine)
        COMMANDS (6): EXP ENTER INV + 2 /

        Converts    4 mem  -    to:      -
        stack[]     ---------         -------              exp(x) + exp(-x)
        (incl.      3  t   -             -       cosh(x) = ----------------
        mem)        2  z   -             -                        2
        from:       1  y   -             -
                    0  x   x          cosh(x)


#### TANH (Hyperbolic Tangent)
        COMMANDS (9): 2 * EXP 1 - ENTER 2 + /

        Converts    4 mem  -    to:      -
        stack[]     ---------         -------              exp(x) - exp(-x)
        (incl.      3  t   -             -       tanh(x) = ----------------
        mem)        2  z   -             -                 exp(x) + exp(-x)
        from:       1  y   -             -
                    0  x   x          tanh(x)


#### ASINH (Area Hyperbolic Sine)
        COMMANDS (8): ENTER ENTER * 1 + SQRT + LN

        Converts    4 mem  -    to:      -
        stack[]     ---------         --------
        (incl.      3  t   -             -       asinh(x) = ln(x + sqrt(x*x + 1))
        mem)        2  z   -             -
        from:       1  y   -             -
                    0  x   x          asinh(x)


#### ACOSH (Area Hyperbolic Cosine)
        COMMANDS (8): ENTER ENTER * 1 - SQRT + LN

        Converts    4 mem  -    to:      -
        stack[]     ---------         --------
        (incl.      3  t   -             -       acosh(x) = ln(x + sqrt(x*x - 1))
        mem)        2  z   -             -
        from:       1  y   -             -
                    0  x   x          acosh(x)


#### ATANH (Area Hyperbolic Tangent)
        COMMANDS (11): ENTER ENTER 1 + SWAP CHS 1 + / SQRT LN

        Converts    4 mem  -    to:      -
        stack[]     ---------         --------                      1 + x
        (incl.      3  t   -             -       atanh(x) = ln(sqrt(-----))
        mem)        2  z   -             -                          1 - x
        from:       1  y   -             -
                    0  x   x          atanh(x)


#### SUM (Prepare stack/arguments for summarizing)
        COMMANDS (12): 1 STO ROT ENTER ENTER * ROT * ROT SWAP SQRT SUM1

        Converts    4 mem  -    to:    1    and where
        stack[]     ---------         ---   SUM1 adds
        (incl.      3  t   -          x*y   stack[i]
        mem)        2  z   -          x*x   to
        from:       1  y   y           y    sum[i]
                    0  x   x           x


#### ND (Normal Distribution: Probability Density and Cumulative Distribution Function)
        COMMANDS (37): ENTER ENTER ENTER * * .07 * CHS SWAP 1.6 * - EXP 1 + INV
                       SWAP ENTER * CHS 2 / EXP 0.3989423 *

        Converts    4 mem  -    to:    -                  (x)
        stack[]     ---------         ---        CDF = integral(PDF) = 1/(1 + exp(-0.07*x^3 - 1.6*x))
        (incl.      3  t   -           -                 (-inf)
        mem)        2  z   -           -
        from:       1  y   -          CDF        PDF = 1/sqrt(2*PI) * exp(-x*x/2)
                    0  x   x          PDF


#### R2P (Rectangular to Polar Coordinates)
        COMMANDS (14): ENTER * SWAP ENTER ENTER ROT * + SQRT ENTER ROT / ASIN ROTup

        Converts    4 mem  -    to:    -
        stack[]     ---------         ---      r = sqrt(x*x + y*y)
        (incl.      3  t   -           -
        mem)        2  z   -           -                 y
        from:       1  y   y           a       a = atan(---)
                    0  x   x           r                 x


#### P2R (Polar to Rectangular Coordinates)
        COMMANDS (15): SWAP SIN ENTER ENTER * CHS 1 + SQRT ROT * ROT * SWAP ROT

        Converts    4 mem  -    to:    -
        stack[]     ---------         ---      y = r * sin(a)
        (incl.      3  t   -           -
        mem)        2  z   -           -
        from:       1  y   a           y       x = r * cos(a)
                    0  x   r           x


#### STAT (Statistik, Mean Value, Standard Deviation)
        COMMANDS (16): SWAP ROT ENTER RCL / ENTER ROT * CHS + RCL 1 - / SQRT SWAP

        Converts    4 mem  n    to:    -                 XX - X^2 / n
        stack[]     ---------         ---      d = sqrt(--------------)
        (incl.      3  t   XY          -                     n - 1
        mem)        2  z   XX          -
        from:       1  y   Y           d       m = X / n
                    0  x   X           m


#### QE (Quadratic Equation) ... not implemented
        COMMANDS (25): SWAP 2 / CHS ENTER ENTER * SWAP ROT SWAP - SQRT ROT ROT
                       ENTER ROTup ENTER ROT SWAP ROT - ROT + SWAP ROT

        Converts    4 mem  -    to:    -
        stack[]     ---------         ---      y = x*x + p *x + q
        (incl.      3  t   -           -
        mem)        2  z   -           -       x1 = -p/2 + sqrt((p/2)^2 - q)
        from:       1  y   p           x2
                    0  x   q           x1      x2 = -p/2 - sqrt((p/2)^2 - q)


#### LR (Linear Regression)
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


### DEC/HEX-CONVERSION
      HI  LO |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
      --------------------------------------------------------
      0 0000 |  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
      1 0010 | 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
      2 0010 | 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47
      3 0011 | 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63
      4 0100 | 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79
      5 0101 | 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95
      6 0110 | 96 97 98 99  0  1  2  3  4  5  6  7  8  9 10 11
      7 0111 | 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27
      8 1000 | 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43
      9 1001 | 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59
      A 1010 | 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75
      B 1011 | 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91
      C 1100 | 92 93 94 95 96 97 98 99  0  1  2  3  4  5  6  7
      D 1101 |  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
      E 1110 | 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
      F 1111 | 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55

### ASCII TABLE
      DEC     |  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
          HEX |  0 1 2 3 4 5 6 7 8 9 a b c d e f
      ------------------------------------------
      032 20  |    ! " # $ % & ' ( ) * + , - . /
      048 30  |  0 1 2 3 4 5 6 7 8 9 : ; < = > ?
      064 40  |  @ A B C D E F G H I J K L M N O
      080 50  |  P Q R S T U V W X Y Z [ \ ] ^ _
      096 60  |  ` a b c d e f g h i j k l m n o
      112 70  |  p q r s t u v w x y z { | } ~

### PHYSICAL CONSTANTS
      Constant      ID  Name
      ---recommended:-------------------------------------------
      3.141593      PI  Number PI
      0.01745329    RpD Radians per Degrees
      ---from WP34S-Manual:-------------------------------------
      365.2425      A   Gregorian year
      5.291772E-11  Ao  Bohr radius
      384.4E6       Am  Semi-major axis of the Moon's orbit
      1.495979E11   Ae  Semi-major axis of the Earth's orbit
      2.997942E8    c   Speed of light
      3.741772E-16  C1  First radiation constant
      0.01438777    C2  Second radiation constant
      1.602177E-19  e   Electron charge
      96485.34      F   Faraday constant
      2.502908      Fa  Feigenbaum's alpha
      4.669202      Fd  Feigenbaum's delta
      9.80665       g   Standard earth accelleration
      6.674083E-11  G   Newtonian constant of gravitation
      7.748092E-5   Go  Conductance quantum
      0.9159656     Gc  Catalan's constant
      -2.002232     Ge  Lande's electron g-factor
      6.626069E-34  h   Planck constant
      1.38065E-23   k   Boltzmann constant
      4.835979E14   Kj  Josephson constant
      1.616199      lP  Planck length
      9.109383E-31  me  Electron mass
      7.349E22      mM  Mass of the Moon
      1.674927E-27  mn  Neutron mass
      1.672622E-27  mp  Proton mass
      2.17651E-8    mP  Planck mass
      1.660539E-27  mu  Atomic mass unit
      1.492417E-10  Muc Energy equivalent of atomic mass unit
      1.883541E-28  mm  Muon mass
      1.9891E30     mS  Mass of the Sun
      5.9736E24     mE  Mass of the Earth
      6.022141E23   NA  Avogadro's number
      101325        po  Standard atmospheric pressure
      1.875546E-18  qP  Planck charge
      8.314472      R   Molar gas constant
      2.81794E-15   re  Electron radius
      25812.81      RK  Von Klitzing constant
      1.73753E6     RM  Mean radius of the Moon
      1.097373E7    Ryd Rydberg constant
      6.96E8        rS  Mean radius of the Sun
      6.37101E6     rE  Mean radius of the Earth
      273.15        To  Standard temperature
      5.39106E-44   tP  Planck time
      1.416833E-32  TP- Planck temperature
      0.02241397    Vm  Molar volume of an ideal gas
      376.7303      Zo  Impedance of vacuum
      7.297353E-3   _A  Fine structure constant
      0.5772157     _GE Euler-Mascheroni constant
      2.675222E8    _GP Proton gyromagnetic ratio
      8.854188E-12  _Eo Electric constant or vacuum permitivity
      2.42631E-12   _Le Compton wavelength of the electron
      1.319591E-15  _Ln Compton wavelength of the neutron
      1.32141E-15   _Lp Compton wavelength of the proton
      1.256673E-6   _mo Magnetic constant or vacuum permeability
      9.274009E-24  _mB Bohr's magneton
      -9.284764E-24 _me Electron magnetic moment
      -9.662364E-27 _mn Neutron magnetic moment
      1.410607E-26  _mp Proton magnetic moment
      5.050783E-27  _mu Nuclear magneton
      -4.490448E-26 _mm Muon magnetic moment
      5.670373E-8   _SB Stefan-Boltzmann constant
      1.618034      _P  Golden ratio
      2.067834E-15  _Po Magnetic flux quantum
