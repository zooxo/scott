/*
  TODOs:
  - optimize code
  - recorder
  - sum+
  - reduce font amount
  - HELP shows shift-keys


  ____________________

  - Welcome to SCOTT -
  (Scientific RPN Calculator with OLED display and ATTINY85 microcontroller)
  Version 1.0, (c) 2019 by deetee/zooxo

  SCOTT is free software. You can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software Foundation.

  ____________________

        PREAMBLE
  ____________________

    To see what a simple and cheap microcontroller like the ATTINY85 is able to
    perform was a challenge with the project ScArY. But as ScArY was based on a
    premanufactured display/keyboard-board SCOTT works with simple and cheap
    hardware.

    So beside an ATTINY85 microcontroller SCOTT is based on an OLED display and a
    16 key one wire keypad. As SCOTT is powered by one 3V battery only a good
    power management is essential.

    Two of the five regular I/O-pins of the ATTINY are used to control the display
    (I2C communication) and one (analog) pin is used to handle the one wire keypad.
    So far two pins are available for further developments (i.e. sensors).

    SCOTT uses the internal RAM of the SSD1306 display controller as screenbuffer,
    what saves valuable RAM of the ATTINY. And to speed up the relatively slow
    I2C-communication with the display SCOTT splits the controller RAM. So while
    one half of the RAM is used to diplay its content the other half is filled with
    content via the I2C line. With a RAM swap the new content is displayed
    rapidly (without the effect of "dropping in characters").

    Like in ScArY the functionality of SCOTT outweights comfort and error handling
    which can be seen for instance at the permanent scientific display format or
    a "non interpretable" display after dividing by zero.

    Enjoy!
    deetee
  ____________________


    ATTINY85 PINS:
                              _____
      Analog0/Reset D5 S0  H1|* U  |H8  S7     Vcc
            Analog2 D3 S1  H2|     |H7  S6 D2  SCK/Analog1
            Analog3 D4 S2  H3|     |H6  S5 D1  PWM1/MISO
                GND    S3  H4|_____|H5  S4 D0  PWM0/AREF/SDA/MOSI

      H...Hardware pin
      S...Software pin
      D...Digispark pin


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




    ATTINY85 Microcontroller:
      - 8192 bytes of flash memory (stores program code)
      - 512 bytes of SRAM (holding variables)
      - 512 bytes or EEPROM (stores settings and user data permanently)

    OLED Display:
      - 128x32 pixels
      - I2C communication (occupies two pins of the microcontroller only)
      - Controlled by SSD1306

    Keypad:
      - 16 keys
      - Analog keypad (occupies only one pin of the microcontroller)
      - Needs 7 resistors

    Power Source:
      - 1 x CR2032 battery (3V)

    KEYS:
                 [SHIFT]          7 [MENU]  (F1)    8 [] (F2)      9   [/] (F3)
              EE [SWAP] (UP)      4 [CONST]         5 [CMD]        6   [*]
              +- [ROT]  (DOWN)    1 [RCL]           2 [STO]        3   [-]
              C  [OFF]            0 [CONTR]         . [BATT]     ENTER [+]



    DEC<->HEX
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


    ASCII TABLE
      DEC     |  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
          HEX |  0 1 2 3 4 5 6 7 8 9 a b c d e f
      ------------------------------------------
      032 20  |    ! " # $ % & ' ( ) * + , - . /
      048 30  |  0 1 2 3 4 5 6 7 8 9 : ; < = > ?
      064 40  |  @ A B C D E F G H I J K L M N O
      080 50  |  P Q R S T U V W X Y Z [ \ ] ^ _
      096 60  |  ` a b c d e f g h i j k l m n o
      112 70  |  p q r s t u v w x y z { | } ~

*/


// ***** I N C L U D E S

#include <TinyWireM.h> // I2C wire communication with display
#include <avr/power.h> // Needed for power management
#include <avr/sleep.h> // Needed for sleeping
#include <EEPROM.h> // For saving data to EEPROM


// ***** F O N T

#define FONTOFFSET 32
#define FONTWIDTH 5
const byte font [] PROGMEM = { // Edged font
  0x00, 0x00, 0x00, 0x00, 0x00, // sp
  0x00, 0x00, 0x2f, 0x00, 0x00, // !
  0x00, 0x07, 0x00, 0x07, 0x00, // "
  0x14, 0x7f, 0x14, 0x7f, 0x14, // #
  //0x2e, 0x2a, 0x7f, 0x2a, 0x3a, // $
  0x10, 0x7f, 0x01, 0x01, 0x01, // $ squareroot
  0x62, 0x64, 0x08, 0x13, 0x23, // %
  0x36, 0x49, 0x55, 0x22, 0x50, // &
  //0x00, 0x07, 0x03, 0x00, 0x00, // '
  0x00, 0x1b, 0x04, 0x1b, 0x00, // ' raised x
  0x00, 0x1c, 0x22, 0x41, 0x00, // (
  0x00, 0x41, 0x22, 0x1c, 0x00, // )
  0x14, 0x08, 0x3E, 0x08, 0x14, // *
  0x08, 0x08, 0x3E, 0x08, 0x08, // +
  0x00, 0x00, 0xe0, 0x60, 0x00, // ,
  0x08, 0x08, 0x08, 0x08, 0x08, // -
  0x00, 0x60, 0x60, 0x00, 0x00, // .
  0x20, 0x10, 0x08, 0x04, 0x02, // /
  //0x7f, 0x41, 0x49, 0x41, 0x7f, // 0
  0x7f, 0x41, 0x41, 0x41, 0x7f, // 0 no dot
  0x00, 0x00, 0x02, 0x7f, 0x00, // 1
  0x79, 0x49, 0x49, 0x49, 0x4f, // 2
  0x41, 0x49, 0x49, 0x49, 0x7f, // 3
  0x0f, 0x08, 0x08, 0x08, 0x7f, // 4
  0x4f, 0x49, 0x49, 0x49, 0x79, // 5
  0x7f, 0x49, 0x49, 0x49, 0x79, // 6
  0x03, 0x01, 0x01, 0x01, 0x7f, // 7
  0x7f, 0x49, 0x49, 0x49, 0x7f, // 8
  0x4f, 0x49, 0x49, 0x49, 0x7f, // 9
  0x00, 0x36, 0x36, 0x00, 0x00, // :
  0x00, 0x76, 0x36, 0x00, 0x00, // ;
  0x08, 0x14, 0x22, 0x41, 0x00, // <
  0x14, 0x14, 0x14, 0x14, 0x14, // =
  //0x00, 0x41, 0x22, 0x14, 0x08, // >
  0x08, 0x08, 0x3e, 0x1c, 0x08, // > arrow to right
  0x03, 0x01, 0x59, 0x09, 0x0f, // ?
  //0x7b, 0x49, 0x79, 0x41, 0x7f, // @
  0x04, 0xbe, 0xbf, 0xbe, 0x04, // @ shift sign
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
  0x7f, 0x41, 0x41, 0x41, 0x7f, // =
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

static void displayinit(void) { // Run initialization sequence
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
  dsendbyte(0x10 | ((x & 0xf0) >> 4));
  dsendbyte(x & 0x0f);
  dsendstop();
  dx = x;
  dy = y;
}

static void dfill(byte b) { // Fill screen with byte/pattern b
  for (byte i = 0; i < DISPLAY_PAGES; i++) {
    dsetcursor(0, i);
    dsenddatastart();
    for (byte j = 0; j < SCREENWIDTH; j++) dsenddatabyte(b);
    dsendstop();
  }
}


// ***** K E Y B O A R D

// DEFINES
#define KPIN 3  // Pin to read analog keyboard  (H2 = D3)

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
  else return (keys[(byte)(_sqrtcalc(b / 2 - 235) - 1)]); // Approximate keys with quadratic function
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

static byte getkey(void) { // Returns one debounced key - no key repeat
  static byte oldkey = NULL; // Needed for debouncing
  byte key1 = getkeycode(), key2 = NULL;
  if (key1 != oldkey) key2 = getkeycode();
  if (key1 == key2) return (oldkey = key1);
  return (NULL);
}


// *****  S Y S T E M

// DEFINES
#define FRAMERATE 10 // Maximal number of screen refreshes per second (>3)


// VARIABLES
static byte eachframemillis, thisframestart, lastframedurationms; // Framing times
static boolean justrendered; // True if frame was just rendered


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
  //delay((unsigned long) ms);
  long t = millis();
  while (millis() - t < ms) ;
}

static void printc(byte c, byte w, byte h) { // Print char c with width and height (1, 2 or 4)
  byte tmpx = dx;
  for (byte k = 0; k < h; k++) { // One byte, two nibbles or 4 pairs of bits
    if (k > 0) { // Manage cursor position, if size >1
      dx = tmpx; // Remember x position
      dy++; // Increment y position/page
      dsetcursor(dx, dy);
    }
    for (byte j = 0; j < FONTWIDTH; j++) {
      byte tmp = pgm_read_byte(&font[FONTWIDTH * (c - FONTOFFSET) + j]); // Fontbyte
      if (h == 2)      tmp = expand4bit((tmp >> (k * 4)) & 0x0f); // Expand 0000abcd
      else if (h == 4) tmp = expand2bit((tmp >> (k * 2)) & 0x03); // Expand 000000ab
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

static void prints(char * s, byte w, byte h) { // Print string with page size 1,2 or 4
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
#define POWEROFFTIME 30 // Time for auto poweroff in s
#define DISPLAYOFFTIME 20 // Time for auto displayoff in s
#define DIMTIME 10 // Time for auto displaydim in s
#define MAXSTRBUF 12 // Maximal length of string buffer sbuf[]
#define STACKSIZE 5 // Size of floatstack
#define TINYNUMBER 1e-7 // Number for rounding to 0
#define RAD 57.29578 // 180/PI
#define MAXITERATE 100 // Maximal number of Taylor series loops to iterate
#define FKEYNR 3 // 3 function keys
#define EECONTRAST 0 // EEPROM address of brightness (1 byte)
#define EESTACK 1 // EEPROM address of stack ((4+1)*4 bytes)
#define EECMDKEY 21 // EEPROM address of command keys (10 bytes)
#define EECONST 31 // EEPROM address of constants (10*4 bytes)
#define KEY_DUMMY 0xff // Needed to enter key-loop and printstring
#define EEREC 71 // EEPROM address Starting EE-address for saving "type recorder"
#define MAXREC 73 // Number of record steps per slot
#define MAXRECSLOT 6 // Maximal slots to record

// Macros
#define _abs(x) ((x<0)?(-x):(x)) // abs()-substitute macro
#define _ones(x) ((x)%10)        // Calculates ones unit
#define _tens(x) (((x)/10)%10)   // Calculates tens unit
#define _huns(x) (((x)/100)%10)  // Calculates hundreds unit
#define _tsds(x) (((x)/1000)%10) // Calculates thousands unit


// VARIABLES
static byte key; // Holds entered key
static char sbuf[MAXSTRBUF]; // Holds string to print
static double stack[STACKSIZE]; // Float stack (XYZT) and memory
static boolean isnewnumber; // True if stack has to be lifted before entering a new number
static boolean ispushed; // True if stack was already pushed by ENTER
static byte decimals; // Number of decimals entered - used for input after decimal dot
static boolean isdot; // True if dot was pressed and decimals will be entered
static boolean isf; // true if f is pressed
static boolean ismenu; // True if menu demanded
static byte select; // Selection number or playstring position
static boolean isplaystring; // True if string should be played
static byte brightness; // Contrast
static boolean isfirstrun; // Allows first run of loop and printscreen without key query
static long timestamp; // Needed for timing of power manangement
/*static int recptr = 0; // Pointer to recording step
  static byte recslot = 0; // Slot number for recording to EEPROM
  static boolean isrec = false, isplay = false; // True, if "Type Recorder" records or plays
*/
/*static double sx = 0.0, sxx = 0.0, sy = 0.0, sxy = 0.0; // Statistical/LR variables
  static byte sn = 0;
*/

// MESSAGES
#define MSGRUN 0
const char m0[] PROGMEM = "RUN"; //
const char* const msg_table[] PROGMEM = {m0,};

// COMMANDS
const char c0[] PROGMEM = "$X";   // Squareroot
const char c1[] PROGMEM = "Y'";   // Raise to the power of
const char c2[] PROGMEM = "1/X";  // Reciprocal
const char c3[] PROGMEM = "EXP";  // Exponential
const char c4[] PROGMEM = "LN";   // Natural logarithm
const char c5[] PROGMEM = "!";    // Gamma function (due to Nemes)
const char c6[] PROGMEM = ">P";   // Rectangular to polar coordinates
const char c7[] PROGMEM = ">R";   // Polar to rectangular coordinates
const char c8[] PROGMEM = "PV";   // Present value (annuity)
const char c9[] PROGMEM = "ND";   // Normal distribution (CDF/PDF)
const char c10[] PROGMEM = "CST"; // Set constant key
const char c11[] PROGMEM = "CMD"; // Set command key
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
const char c24[] PROGMEM = "U3"; //  ... USR recorded functions
const char c25[] PROGMEM = "U4"; //
const char c26[] PROGMEM = "U5"; //
const char c27[] PROGMEM = "U1"; //
const char c28[] PROGMEM = "U2"; //
const char c29[] PROGMEM = "U3"; //
const char* const cmd[] PROGMEM = {
  c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15, c16, c17, c18, c19, c20,
  c21, c22, c23, c24, c25, c26, c27, c28, c29
};
#define numberofcommands (sizeof(cmd)/sizeof(const char *))


// PLAYSTRINGS (Don't play a playstring!)
#define PSCOS 0
#define PSTAN 1
#define PSASIN 2
#define PSACOS 3
#define PSND 4
#define PSR2P 5
#define PSP2R 6
#define PSPV 7
#define PSGAMMA 8
#define PSSINH 9
#define PSCOSH 10
#define PSTANH 11
#define PSASINH 12
#define PSACOSH 13
#define PSATANH 14
const char s0[] PROGMEM = "[=E>1LO"; // COS: SIN ENTER * CHS 1 + SQRT
const char s1[] PROGMEM = "=[=E>1LOK[KH"; // TAN: ENTER COS SWAP SIN SWAP /
const char s2[] PROGMEM = "==E>1LOQE`"; // ASIN: ENTER ENTER * CHS 1 + SQRT INV * ATAN
const char s3[] PROGMEM = "==E>1LOQE`>90L"; // ACOS: ASIN CHS 90 +
const char s4[] PROGMEM = "===EE:07E>K1:6E>LR1LQK=E>2HR:3989423E"; // ND:
// GAUSS-CDF: ENTER ENTER ENTER * * .07 * CHS SWAP 1.6 * CHS + EXP 1 + INV
//            SWAP
//       PDF: ENTER * CHS 2 / EXP 0.3989423 *
const char s5[] PROGMEM = "=MMKM=MKH`MMEMMELO"; // R2P: ENTER ROT ROT SWAP ROT ENTER ROT SWAP / ATAN ROT ROT * ROT ROT * + SQRT
const char s6[] PROGMEM = "K[==E>1LOMEMEKM"; // P2R: SWAP SIN ENTER ENTER * CHS 1 + SQRT ROT * ROT * SWAP ROT
const char s7[] PROGMEM = ">K=1LKMKP>1LMMMH"; // PV: CHS SWAP ENTER 1 + SWAP ROT SWAP PWR CHS 1 + ROT ROT ROT /
const char s8[] PROGMEM = "1L==QR=Q>L2HEK6P810EQLSE2HK>LMMSK:5BEL:9189385LR";
// GAMMA: 1 + ENTER ENTER INV EXP ENTER INV CHS + 2 / * SWAP 6 PWR 810 * INV + LN * 2 /
// SWAP CHS + ROT ROT LN SWAP .5 - * + .9189385 + EXP
const char s9[] PROGMEM = "==RK>RB2H"; // SINH: ENTER ENTER EXP SWAP CHS EXP - 2 /
const char s10[] PROGMEM = "==RK>RL2H"; // COSH: ENTER ENTER EXP SWAP CHS EXP + 2 /
const char s11[] PROGMEM = "2E>R=>1LK1LH"; // TANH: 2 * CHS EXP ENTER CHS 1 + SWAP 1 + /
const char s12[] PROGMEM = "==E1LOLS"; // ASINH: ENTER ENTER * 1 + SQRT + LN
const char s13[] PROGMEM = "==E1BOLS"; // ACOSH: ENTER ENTER * 1 - SQRT + LN
const char s14[] PROGMEM = "==1LK>1LHOS"; // ATANH: ENTER ENTER 1 + SWAP CHS 1 + / SQRT LN
const char* const pstable[] PROGMEM = {s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14};
char playbuf[50]; // Holds sii[]
/* Commands and corresponding ascii characters

     <--------  numbers -------->  DOT CLX EE ENTER CHS CONTR
  c  0  1  2  3  4  5  6  7  8  9  :   ;   <  =     >   ?
  #                                01  02  03 04    05  06
  a  48 49 50 51 52 53 54 55 56 57 58  59  60 61    62  63

     RCL STO SUB n  CMD MULT MENU n  DIV BATT SLEEP SWAP ADD ROT n  SQRT
  c  @   A   B   C  D   E    F    G  H   I    J     K    L   M   N  O
  #  07  08  09  10 11  12   13   14 15  16   17    18   19  20  21 22
  a  64  65  66  67 68  69   70   71 72  73   74    75   76  77  78 79

     POW INV EXP LN GAMMA R2P P2R PV ND SETCONST SETCMD SIN COS TAN ASIN ACOS
  c  P   Q   R   S  T     U   V   W  X  Y        Z      [   \   ]   ^    _
  #  23  24  25  26 27    28  29  30 31 32       33     34  35  36  37   38
  a  80  81  82  83 84    85  86  87 88 89       90     91  92  93  94   95

     ATAN SINH COSH TANH ASINH ACOSH ATANH
  c  `    a    b    c    d     e     f     g h i j k l m n o
  #  39   40   41   42   43    44    45
  a  96   97   98   99   100   101   102

     p q r s t u v w x y z { | } ~
*/


// Function pointer array/table
static void (*dispatch[])(void) = {
  &_numinput, // Normal calculator keys (dispatch 0)     OFFSET: 0
  &_nop, &_ceclx, &_ee, &_enter, &_chs, // 1d:15 2c;13 3e<5 4x=16 5s>9 (no 6f?1)
  &_contrast, &_rcl, &_sto, &_sub, // Shiftkeys 0 1 2 3  OFFSET: 6
  &_const, &_cmdkey, &_mult, &_menu, //         4 5 6 7
  &_nop, &_div, &_batt, &_sleep, //             8 9 d c
  &_swap, &_add, &_rot, &_nop, //               e x s f
  &_sqrt, &_pow, &_inv, // MENU                          OFFSET: 22
  &_exp, &_ln, &_gamma, // Mathematical functions and settings
  &_r2p, &_p2r, &_pv,
  &_nd, &_setconst, &_setcmdkey,
  &_sin, &_cos, &_tan, // Trigonometrical functions
  &_asin, &_acos, &_atan,
  &_sinh, &_cosh, &_tanh, // Hyperbolical functions
  &_asinh, &_acosh, &_atanh,
  &_nop, &_nop, &_nop, // User defineable Menukeys       OFFSET: 46
  &_nop, &_nop, &_nop,
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
  _playstring(PSASIN);
}
static void _asinh(void) { // ASINH
  _playstring(PSASINH);
}
static void _atan(void) { // ATAN
  stack[0] = atan(stack[0]) * RAD;
}
static void _atanh(void) { // ATANH
  _playstring(PSATANH);
}
static void _batt(void) { // BATT
  _push();
  ADMUX = _BV(MUX3) | _BV(MUX2); // Set voltage bits for ATTINY85
  delayshort(5); // Settle Vref
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC)) ; // Measuring
  byte low = ADCL, high = ADCH;
  stack[0] = 1125.3 / ((high << 8) | low); // Calculate Vcc in V - 1125.3 = 1.1 * 1023
}
static void _ce(void) { // CE
  if (isdot) {
    if (stack[0] > TINYNUMBER && decimals > 0)
      stack[0] -= ((long)(stack[0] * _pow10(decimals)) % 10) / _pow10(decimals--);
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
    if (isnewnumber) { // Enable starting new number with .
      if (ispushed) ispushed = false;
      else _push();
      stack[0] = 0.0;
      decimals = 0;
      isnewnumber = false;
    }
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
  ispushed = true;
  isnewnumber = true;
}
static void _exp(void) {  // EXP
  stack[0] = _exp_sin(stack[0], true);
}
static void _gamma(void) { // GAMMA
  _playstring(PSGAMMA);
}
static void _invpos(void) { // INV+
  _ln();
  _chs();
  _exp();
}
static void _inv(void) { // INV
  stack[0] = 1 / stack[0];
  /*_ln();
    _chs();
    _exp();*/
}
static void _ln(void) { // LN
  stack[0] = log(stack[0]);
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
static void _nop(void) {} // NOP - no operation
static void _numinput(void) { // NUM Numeric input (0...9)
  if (isnewnumber) { // New number
    if (ispushed) ispushed = false;
    else _push();
    stack[0] = 0.0;
    decimals = 0;
    isdot = isnewnumber = false;
  }
  if (isdot) stack[0] += (key - KEY_14) / _pow10(++decimals); // Append decimal to number
  else stack[0] = stack[0] * 10 + key - KEY_14; // Append digit to number
}
static void _p2r(void) { // P2R
  _playstring(PSP2R);
}
static void _playstring(byte slot) {  // PLAYSTRING
  strcpy_P(playbuf, (char*)pgm_read_word(&(pstable[slot]))); // Copy playstring to buffer
  select = 0;
  isnewnumber = true;
  ispushed = isdot = false;
  isplaystring = true;
}
static double _pow(void) { // POW
  _swap();
  _ln();
  _mult();
  _exp();
}
static double _pow10(int8_t e) { // POW10 10^x
  boolean ne = (e < 0);
  double f = 1.0;
  if (ne) while (e++)  f *= 10;
  else while (e--)  f *= 10;
  if (ne) f = 1 / f;
  return (f);
}
void _pull(void) { // PULL
  memmove(&stack[0], &stack[1], 3 * sizeof(double));
}
static void _pullupper(void) { // PULLUPPER
  memmove(&stack[1], &stack[2], 2 * sizeof(double));
}
static void _push(void) { // PUSH
  memmove(&stack[1], &stack[0], (STACKSIZE - 2) * sizeof(double));
}
static void _pv(void) { // PV
  _playstring(PSPV);
}
static void _r2p(void) { // R2P
  _playstring(PSR2P);
}
static void _rec(void) { // REC
  /*if (isrec) isrec = isplay = false; // Stop isplay too - if play was recorded
    else {
    recslot = (int)(_abs(stack[0])) - 1; // -1 to convert user's 1234-mind to 0123-slots
    if (recslot < MAXRECSLOT) {
      _pull();
      recptr = recslot * MAXREC;
      isrec = true;
    }
    }*/
}
static void _rcl(void) { // RCL
  _push();
  stack[0] = stack[4];
}
static void _rot(void) { // ROT
  double tmp = stack[0];
  _pull();
  stack[3] = tmp;
}
static void _setcmdkey(void) { // SETCMDKEY
  if (is09) EEPROM.write(EECMDKEY + stack[0], (byte)stack[1]);
}
static void _setconst(void) { // SETCONST
  if (is09) EEPROM.put(EECONST + stack[0], stack[1]);
}
static void _sin(void) {  // SIN
  stack[0] = _exp_sin(stack[0] / RAD, false);
}
static void _sinh(void) {  // SINH
  _playstring(PSSINH);
}
static void _sleep(void) { // SLEEP
  EEPROM.write(EECONTRAST, brightness); // Save brightness to EEPROM
  for (byte i = 0; i < STACKSIZE; i++) // Save stack to EEPROM
    EEPROM.put(EESTACK + i * sizeof(double), stack[i]);
  sleep();
}
static double _sqrtcalc(double f) { // Calculating square root without using sqrt()
  return (_exp_sin(0.5 * log(f), true));
}
static void _sqrt(void) { // SQRT
  stack[0] = _sqrtcalc(stack[0]);
}
static void _sto(void) { // STO
  stack[4] = stack[0];
}
static void _sub(void) { // SUB -
  stack[0] = stack[1] - stack[0];
  _pullupper();
}
/*static void _sum(void) { // SUM
  double f0 = stack[0], f1 = stack[1];
  sx += f0;
  sy += f1;
  sxx += f0 * f0;
  sxy += f0 * f1;
  stack[0] = ++sn;
  }*/
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
static boolean is09(void) { // Checks, if stack[0] is between 0 and 9
  return (stack[0] >= 0 && stack[0] <= 9);
}

static double _exp_sin(double f, boolean isexp) { // Calculate exp or sin with Taylor series
  double result = f;
  double frac = f;
  if (isexp) result = frac = 1.0;
  for (byte i = 1; _abs(frac) > TINYNUMBER && i < MAXITERATE; i++) {
    if (isexp) frac *= f / i; // Fraction for exp
    else frac *=  f  / (-2 * i * (2 * i + 1)) * f; // Fraction for sin
    result += frac;
  }
  return (result);
}

static void printfloat(double f, byte mh, byte y) { // Print float with mantissa height (mh) at line y
  long m; // Mantissa
  int8_t e; // Exponent
  sbuf[0] = ' '; // * Create sign
  if (f < 0.0) {
    f = - f;
    sbuf[0] = '-';
  }
  //e = log10(f); // * Calculate exponent
  e = log(f) / log(10); // * Calculate exponent (without using log10())
  m = (f / _pow10(e - 5)) + .5; // * Create mantissa
  if (m > 0 && m < 1e5) // Change (n-1)-digit-mantissa to n digits
    m = (f / _pow10(--e - 5)) + .5; // "New" mantissa
  for (byte i = 6; i > 0; i--) { // Print mantissa
    sbuf[i] = _ones(m) + '0';
    m /= 10;
  }
  sbuf[7] = e < 0 ? '-' : ' '; // * Create exponent
  if (e < 0) e = -e;
  sbuf[8] = e >= 10 ? _tens(e) + '0' : '0';
  sbuf[9] = _ones(e) + '0';
  printcat(sbuf[0], 2, mh , 0, y); // * Print sbuf in float format
  printcat('.', 2, mh, 23, y);
  printcat(sbuf[1], 2, mh, 12 , y);
  //for (byte i = 2; i < 7; i++) printcat(sbuf[i], 2, mh, 12 * i + 9 , y); // With trailing zeros
  byte nonzero = false; // Suppress trailing zeros
  for (byte i = 6; i > 1; i--)
    if (sbuf[i] != '0' || nonzero) {
      nonzero = true;
      printcat(sbuf[i], 2, mh, 12 * i + 8 , y);
    }
  for (byte i = 7; i < 10; i++) printcat(sbuf[i], 2, 2, 12 * i + 10 , y);
}

static void printscreen(void) { // Print screen due to state
  byte mh = 2; // Mantissa height
  cls();
  if (isplaystring) { // Print running message
    strcpy_P(sbuf, (char*)pgm_read_word(&(msg_table[MSGRUN])));
    printsat(sbuf, 2, 2, 0, 2);
  }
  else if (ismenu) { // Print MENU above F-keys (789)
    for (byte i = 0; i < FKEYNR; i++) {
      strcpy_P(sbuf, (char*)pgm_read_word(&(cmd[select * FKEYNR + i])));
      printsat(sbuf, 2, 2, 47 * i, 2);
    }
  }
  else {
    mh = 4;
    printcat(isf ? '@' : ' ', 2, 2, 117, 2); // Print shift sign or blank
  }
  //if (!ismenu) printcat(isf ? '@' : ' ', 2, 2, 117, 2); // Print shift sign or blank
  printfloat(stack[0], mh, 0); // Print stack[0]

  display();
}


// *****  S E T U P  A N D  L O O P

void reset(void) {
  isnewnumber = isfirstrun = true;
  ispushed = isdot = isf = ismenu = isplaystring = false;
  decimals = select = timestamp = 0;
}

void setup() {
  // INIT DISPLAY
  displayinit(); // Init display
  drender(); // Render current half of GDDRAM

  // INIT KEYBOARD
  pinMode(KPIN, INPUT); // Initialize keyboard pin

  // INIT WAKEUP (with pin change interrupt) ... same pin as keyboard!
  pinMode (KPIN, INPUT); // Wakeup pin
  PCMSK  |= bit (KPIN);  // want pin D3 / H2
  GIFR   |= bit (PCIF);  // Clear any outstanding interrupts
  GIMSK  |= bit (PCIE);  // Enable pin change interrupts

  // SET SYSTEM
  setframerate(FRAMERATE);
  brightness = EEPROM.read(EECONTRAST);
  dcontrast(brightness);

  // START
  reset();
  for (byte i = 0; i < STACKSIZE; i++) // Read stack from EEPROM (saved with sleep/OFF)
    EEPROM.get(EESTACK + i * sizeof(double), stack[i]);
}


void loop() {

  if (!(nextframe())) return; // Pause render (idle) until it's time for the next frame

  if (isfirstrun) {
    isfirstrun = false;
    key = KEY_DUMMY;
  }
  else key = getkey();

  if (key) { // ### Power management
    timestamp = millis();// Keep awake when active
    dcontrast(brightness);
    don();
  }
  byte pot = (millis() - timestamp) / 1000L; // Control power management
  if (pot > POWEROFFTIME) sleep(); // Sleep
  else if (pot > DISPLAYOFFTIME) doff(); // Display off
  else if (pot > DIMTIME) dcontrast(0x00); // Dim display


  /*if (isrec && key != NULL) { // ### Record keys and write to EEPPROM
    if (recptr < (recslot + 1)* MAXREC) EEPROM.write(EEREC + recptr++, key); // Rec key
    }
    else if (isplay) { // ### Plays recorded keys
    }
    /*if (isrec && key != NULL) { // ### Record keys and write to EEPPROM
    if (oldkey == SHIFT) EEPROM.write(EEADDRREC + recptr - 1, oldkey = fcast(key)); // Rec shifted key
    else if (recptr < (recslot + 1)* MAXREC) EEPROM.write(EEADDRREC + recptr++, key); // Rec key
    }
    else if (isplay) { // ### Plays recorded keys
    if (key == CLX) isplay = false; // C stops playing
    else if ( recptr < (recslot + 1)* MAXREC) {
      key = EEPROM.read(EEADDRREC + recptr); // Read key from EEPROM
      if (key == DIGIT8 && oldkey == SHIFT) { // Last record reached
        isplay = false;
        isfpressed = false;
        key = NULL;
      }
      oldkey = key;
      recptr++;
    }
    }*/



  if (isplaystring) { // ### Play string
    key = playbuf[select];
    if (key == NULL) { // Stop playstring
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
