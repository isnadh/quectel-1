// Interface with the ST7735 1.8" SPI LCD Module
// Code adapted from https://github.com/adafruit/Adafruit-ST7735-Library

/*
1.8" Serial SPI TFT LCD Module (ST7735 Controller) with SD Card Socket Pinout:

1. GND
2. VCC
3. NC
4. NC
5. NC
6. LCD RESET
7. LCD A0 (R/S)
8. LCD SDA
9. LCD SCK
10. LCD CS
11. SD SCK
12. SD MISO
13. SD MOSI
14. SD CS
15. LED+
16. LED-
*/

#define swap(a, b) { int t = a; a = b; b = t; }


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160


#define ST7735_NOP 0x0
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN  0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON  0x12
#define ST7735_NORON  0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36


#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1


// extra connections besides SPI
#define LCD_RESET  LATAbits.LATA4   // reset
#define LCD_RS     LATAbits.LATA0   // data or command
#define LCD_CS     LATAbits.LATA2   // chip select
#define LCD_SCK    LATBbits.LATB7   // SPI Clock, for bit-bang SPI only
#define	LCD_SDO    LATBbits.LATB6   // SPI Data, for bit-bang SPI only

void setAddrWindow(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1);
void ST7735_pushColor(unsigned int color); // for what?
void ST7735_drawPixel(unsigned char x, unsigned char y, unsigned int color);
void ST7735_fillScreen(unsigned int color);
//void ST7735_initB(void); // not used
void ST7735_initR(void); // initialize a ST7735 chip
void ST7735_drawString(unsigned char x, unsigned char y, char *c, unsigned int color, unsigned char size);
void ST7735_drawChar(unsigned char x, unsigned char y, char c, unsigned int color, unsigned char size);
void ST7735_fillCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned int color);
void ST7735_drawCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned int color);
void ST7735_setRotation(unsigned char m);
void ST7735_drawRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned int color);
void ST7735_fillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned int color);
void ST7735_drawVerticalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color);
void ST7735_drawHorizontalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color);
void ST7735_drawFastLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color, unsigned char rotflag);
void ST7735_drawLine(int x0, int y0, int x1, int y1, unsigned int color);
unsigned char ST7735_getRotation(void);

void loadBitmapToLCD(char image_number, char style);

#define BACKGROUND_COLOUR            0x0000 // Black
#define WELCOME_LOGO                 0x0001 // Welcome screen with Pureit logo





