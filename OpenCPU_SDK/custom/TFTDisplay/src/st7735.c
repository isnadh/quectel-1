#include "st7735.h"
#include "font.h"
#include "colors.h"
#include "image.h"
#include "common_vars.h"

char madctl;
static u8 buffer[50] = {"\0"};
static u8 last_water_balance_info_buffer[30] = {"\0"};
static u8 last_lastconnectedtime[30] = "6. Unknown";
static char last_latlong[30] = "5. Unknown";
static char Last_service_provider_name[30] = {"4. Unknown"};
static char Last_IMEINumber[20] = {"2. Unknown"};
static char Sensor_Input_String[20] = {"\0"};
static char Last_Sensor_Input_String[20] = {"\0"};
static char Last_CCIDNumber[20] = {"2. Unknown"};
u8 sendonetimelowbal = 0;
u8 sendonetimezerobal = 0;
u8 Balance_in_green = 0;
static u8 LAST_FOTA_FILE_SIZE[25];
static u8 FOTA_FILE_SIZE[25];
extern char Internal_error_code[4];
extern u8 Status_Byte;
extern u8 AMC_Status_Byte;
extern u16 Sensor_Status_Byte;

#define _swap(a, b) {int t; t = a; a = b; b = t;}
extern u32 g_rcvdDataLen;
extern u32 dataLen_here;
extern u8 Fota_Fail_Reason_Code;
extern u16 Sensor_Status_Byte;
int Last_language_index = 0;
int language_index = 0; // 0 == ENGLISH DEFAULT
// 1 == INDONESIAN

#define MESSAGE_BALANCE                           (language_index * MESSAGES_IN_LANGUAGE)
#define MESSAGE_TANK_FULL                         ((language_index * MESSAGES_IN_LANGUAGE)+1)
#define MESSAGE_NO_BALANCE                        ((language_index * MESSAGES_IN_LANGUAGE)+2)
#define MESSAGE_LOW_BALANCE                       ((language_index * MESSAGES_IN_LANGUAGE)+3)
#define MESSAGE_PLEASE_RECHARGE                   ((language_index * MESSAGES_IN_LANGUAGE)+4)
#define MESSAGE_PLEASE_WAIT                       ((language_index * MESSAGES_IN_LANGUAGE)+5)
#define MESSAGE_HEATING_ON                        ((language_index * MESSAGES_IN_LANGUAGE)+6)
#define MESSAGE_HOT_WATER_READY                   ((language_index * MESSAGES_IN_LANGUAGE)+7)

#define LAST_MESSAGE_BALANCE                      (Last_language_index * MESSAGES_IN_LANGUAGE)
#define LAST_MESSAGE_TANK_FULL                    ((Last_language_index * MESSAGES_IN_LANGUAGE)+1)
#define LAST_MESSAGE_NO_BALANCE                   ((Last_language_index * MESSAGES_IN_LANGUAGE)+2)
#define LAST_MESSAGE_LOW_BALANCE                  ((Last_language_index * MESSAGES_IN_LANGUAGE)+3)
#define LAST_MESSAGE_PLEASE_RECHARGE              ((Last_language_index * MESSAGES_IN_LANGUAGE)+4)
#define LAST_MESSAGE_PLEASE_WAIT                  ((Last_language_index * MESSAGES_IN_LANGUAGE)+5)
#define LAST_MESSAGE_HEATING_ON                   ((Last_language_index * MESSAGES_IN_LANGUAGE)+6)
#define LAST_MESSAGE_HOT_WATER_READY              ((Last_language_index * MESSAGES_IN_LANGUAGE)+7)

char LCD_Dislpay_Message[LANGUAGES_SUPPORTED * MESSAGES_IN_LANGUAGE][25] =
{
		"BALANCE", "TANK FULL"   , "NO BALANCE!" , "LOW BALANCE!" ,"PLEASE RECHARGE" ,"PLEASE WAIT" , "HEATING ON"      , "HOT WATER READY",
		"SALDO"  , "TANGKI PENUH", "SALDO HABIS!", "SALDO RENDAH!","HARAP ISI ULANG" ,"HARAP TUNGGU", "PEMANAS MENYALA" , "AIR PANAS SIAP"
};


void write_spi_byte(unsigned char c)
{
	/*
	// using software SPI
	char x;
	for(x=0;x<8;x++)
	{	//clock the byte out
		LCD_SCK = 0;
		LCD_SDO = 0;
		if(c & 0x80)
		LCD_SDO = 1;
		LCD_SCK = 1;
		c <<= 1;
	}
	// using hardware SPI
	// spi2Write(cmd);
	 */
}


void writecommand(unsigned char cmd)
{
	/* LCD_RS = 0;
	LCD_CS = 0;
	write_spi_byte(c);
	LCD_CS = 1;
	 */
	Ql_GPIO_SetLevel(PINNAME_CTS, PINLEVEL_LOW); // D/C = 0
	Ql_SPI_Write(USR_SPI_CHANNAL, (u8 *)&cmd, 1);
}

void writedata(unsigned char data)
{
	/* LCD_RS = 1;
	LCD_CS = 0;
	write_spi_byte(c);
	LCD_CS = 1;
	 */
	Ql_GPIO_SetLevel(PINNAME_CTS, PINLEVEL_HIGH); // D/C = 1
	Ql_SPI_Write(USR_SPI_CHANNAL, (u8 *)&data, 1);

}

void writedata_16bit(int value)
{
	unsigned char buf[2];
	buf[0] = value >> 8;
	buf[1] = value;

	/* LCD_RS = 1;
	LCD_CS = 0;
	write_spi_byte(c);
	LCD_CS = 1;
	 */
	Ql_GPIO_SetLevel(PINNAME_CTS, PINLEVEL_HIGH); // D/C = 1
	Ql_SPI_Write(USR_SPI_CHANNAL, (u8 *)&buf, 2);

}

void setAddrWindow(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1)
{
	writecommand(ST7735_CASET);  // column addr set
	writedata(0x00);
	writedata(x0+0);   // XSTART
	writedata(0x00);
	writedata(x1+0);   // XEND

	writecommand(ST7735_RASET);  // row addr set
	writedata(0x00);
	writedata(y0+0);    // YSTART
	writedata(0x00);
	writedata(y1+0);    // YEND

	writecommand(ST7735_RAMWR);  // write to RAM
}

void ST7735_pushColor(unsigned int color)
{
	// LCD_RS = 1;
	// LCD_CS = 0;
	// write_spi_byte(color >> 8);
	// write_spi_byte(color);
	writedata_16bit(color);
	// LCD_CS = 1;
}

void ST7735_drawPixel(unsigned char x, unsigned char y, unsigned int color)
{
	setAddrWindow(x, y, x, y);

	// LCD_RS = 1;
	// LCD_CS = 0;

	// write_spi_byte(color >> 8);
	// write_spi_byte(color);
	writedata_16bit(color);

	// LCD_CS = 1;
}

void ST7735_fillScreen(unsigned int color)
{
	unsigned char x;
	unsigned char y;

	ST7735_setRotation(0xC0); // Start point Right - top

	setAddrWindow(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);

	// setup for data

	for (x=0; x < SCREEN_WIDTH; x++)
	{
		for (y=0; y < SCREEN_HEIGHT; y++)
		{
			writedata_16bit(color);
		}
	}
}
/*
void ST7735_initB(void)
{
	// LCD_RESET = 1;
	// delay_us(500);
	// LCD_RESET = 0;
	// delay_us(500);
	// LCD_RESET = 1;
	// delay_us(500);

	// LCD_CS = 0;

	writecommand(ST7735_SWRESET); // software reset
	// delay_us(50);
	writecommand(ST7735_SLPOUT);  // out of sleep mode
	// delay_us(500);

	writecommand(ST7735_COLMOD);  // set color mode
	writedata(0x05);        // 16-bit color
	// delay_us(10);

	writecommand(ST7735_FRMCTR1);  // frame rate control
	writedata(0x00);  // fastest refresh
	writedata(0x06);  // 6 lines front porch
	writedata(0x03);  // 3 lines backporch
	// delay_us(10);

	writecommand(ST7735_MADCTL);  // memory access control (directions)
	writedata(0x08);  // row address/col address, bottom to top refresh
	madctl = 0x08;

	writecommand(ST7735_DISSET5);  // display settings #5
	writedata(0x15);  // 1 clock cycle nonoverlap, 2 cycle gate rise, 3 cycle oscil. equalize
	writedata(0x02);  // fix on VTL

	writecommand(ST7735_INVCTR);  // display inversion control
	writedata(0x0);  // line inversion

	writecommand(ST7735_PWCTR1);  // power control
	writedata(0x02);      // GVDD = 4.7V
	writedata(0x70);      // 1.0uA
	// delay_us(10);
	writecommand(ST7735_PWCTR2);  // power control
	writedata(0x05);      // VGH = 14.7V, VGL = -7.35V
	writecommand(ST7735_PWCTR3);  // power control
	writedata(0x01);      // Opamp current small
	writedata(0x02);      // Boost frequency


	writecommand(ST7735_VMCTR1);  // power control
	writedata(0x3C);      // VCOMH = 4V
	writedata(0x38);      // VCOML = -1.1V
	// delay_us(10);

	writecommand(ST7735_PWCTR6);  // power control
	writedata(0x11);
	writedata(0x15);

	writecommand(ST7735_GMCTRP1);
	writedata(0x0f);	//writedata(0x09);
	writedata(0x1a);  //writedata(0x16);
	writedata(0x0f);  //writedata(0x09);
	writedata(0x18);  //writedata(0x20);
	writedata(0x2f);  //writedata(0x21);
	writedata(0x28);  //writedata(0x1B);
	writedata(0x20);  //writedata(0x13);
	writedata(0x22);  //writedata(0x19);
	writedata(0x1f);  //writedata(0x17);
	writedata(0x1b);  //writedata(0x15);
	writedata(0x23);  //writedata(0x1E);
	writedata(0x37);  //writedata(0x2B);
	writedata(0x00);  //writedata(0x04);
	writedata(0x07);  //writedata(0x05);
	writedata(0x02);  //writedata(0x02);
	writedata(0x10);  //writedata(0x0E);
	writecommand(ST7735_GMCTRN1);
	writedata(0x0f);   //writedata(0x0B);
	writedata(0x1b);   //writedata(0x14);
	writedata(0x0f);   //writedata(0x08);
	writedata(0x17);   //writedata(0x1E);
	writedata(0x33);   //writedata(0x22);
	writedata(0x2c);   //writedata(0x1D);
	writedata(0x29);   //writedata(0x18);
	writedata(0x2e);   //writedata(0x1E);
	writedata(0x30);   //writedata(0x1B);
	writedata(0x30);   //writedata(0x1A);
	writedata(0x39);   //writedata(0x24);
	writedata(0x3f);   //writedata(0x2B);
	writedata(0x00);   //writedata(0x06);
	writedata(0x07);   //writedata(0x06);
	writedata(0x03);   //writedata(0x02);
	writedata(0x10);   //writedata(0x0F);
	// delay_us(10);

	writecommand(ST7735_CASET);  // column addr set
	writedata(0x00);
	writedata(0x02);   // XSTART = 2
	writedata(0x00);
	writedata(0x81);   // XEND = 129

	writecommand(ST7735_RASET);  // row addr set
	writedata(0x00);
	writedata(0x02);    // XSTART = 1
	writedata(0x00);
	writedata(0x81);    // XEND = 160

	writecommand(ST7735_NORON);  // normal display on
	// delay_us(10);

	writecommand(ST7735_RAMWR);
	// delay_us(500);

	writecommand(ST7735_DISPON);
	// delay_us(500);

	// LCD_CS = 1;
}
 */
void ST7735_initR(void)
{
	u32 i =0;
	/*
	LCD_RESET = 1;
	delay_us(500);
	LCD_RESET = 0;
	delay_us(500);
	LCD_RESET = 1;
	delay_us(500);

	LCD_CS = 0;
	 */
	for(i=0;i<200000;i++)
		Ql_GPIO_SetLevel(PINNAME_RTS, PINLEVEL_LOW); // reset line set to 0

	for(i=0;i<200000;i++)
		Ql_GPIO_SetLevel(PINNAME_RTS, PINLEVEL_HIGH); // reset line set to 1

	writecommand(ST7735_SWRESET); // software reset
	// delay_us(150);
	writecommand(ST7735_SLPOUT);  // out of sleep mode
	// delay_us(500);

	writecommand(ST7735_COLMOD);  // set color mode
	writedata(0x05);        // 16-bit color
	// delay_us(10);

	writecommand(ST7735_FRMCTR1);  // frame rate control - normal mode
	writedata(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	writedata(0x2C);
	writedata(0x2D);

	writecommand(ST7735_FRMCTR2);  // frame rate control - idle mode
	writedata(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	writedata(0x2C);
	writedata(0x2D);

	writecommand(ST7735_FRMCTR3);  // frame rate control - partial mode
	writedata(0x01); // dot inversion mode
	writedata(0x2C);
	writedata(0x2D);
	writedata(0x01); // line inversion mode
	writedata(0x2C);
	writedata(0x2D);

	writecommand(ST7735_INVCTR);  // display inversion control
	writedata(0x07);  // no inversion

	writecommand(ST7735_PWCTR1);  // power control
	writedata(0xA2);
	writedata(0x02);      // -4.6V
	writedata(0x84);      // AUTO mode

	writecommand(ST7735_PWCTR2);  // power control
	writedata(0xC5);      // VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD

	writecommand(ST7735_PWCTR3);  // power control
	writedata(0x0A);      // Opamp current small
	writedata(0x00);      // Boost frequency

	writecommand(ST7735_PWCTR4);  // power control
	writedata(0x8A);      // BCLK/2, Opamp current small & Medium low
	writedata(0x2A);

	writecommand(ST7735_PWCTR5);  // power control
	writedata(0x8A);
	writedata(0xEE);

	writecommand(ST7735_VMCTR1);  // power control
	writedata(0x0E);

	writecommand(ST7735_INVON);    // invert display


	writecommand(ST7735_MADCTL);  // memory access control (directions)

	// http://www.adafruit.com/forums/viewtopic.php?f=47&p=180341

	// R and B byte are swapped
	// madctl = 0xC8;

	// normal R G B order
	madctl = 0xC0;
	writedata(madctl);  // row address/col address, bottom to top refresh

	writecommand(ST7735_COLMOD);  // set color mode
	writedata(0x05);        // 16-bit color

	writecommand(ST7735_CASET);  // column addr set
	writedata(0x00);
	writedata(0x00);   // XSTART = 0
	writedata(0x00);
	writedata(0x7F);   // XEND = 127

	writecommand(ST7735_RASET);  // row addr set
	writedata(0x00);
	writedata(0x00);    // XSTART = 0
	writedata(0x00);
	writedata(0x9F);    // XEND = 159

	writecommand(ST7735_GMCTRP1);
	writedata(0x0f);
	writedata(0x1a);
	writedata(0x0f);
	writedata(0x18);
	writedata(0x2f);
	writedata(0x28);
	writedata(0x20);
	writedata(0x22);
	writedata(0x1f);
	writedata(0x1b);
	writedata(0x23);
	writedata(0x37);
	writedata(0x00);
	writedata(0x07);
	writedata(0x02);
	writedata(0x10);
	writecommand(ST7735_GMCTRN1);
	writedata(0x0f);
	writedata(0x1b);
	writedata(0x0f);
	writedata(0x17);
	writedata(0x33);
	writedata(0x2c);
	writedata(0x29);
	writedata(0x2e);
	writedata(0x30);
	writedata(0x30);
	writedata(0x39);
	writedata(0x3f);
	writedata(0x00);
	writedata(0x07);
	writedata(0x03);
	writedata(0x10);

	writecommand(ST7735_DISPON);
	// delay_us(100);

	writecommand(ST7735_NORON);  // normal display on
	// delay_us(10);

	// LCD_CS = 1;

}

void ST7735_drawString_align_Center(unsigned char y, char *c, unsigned int color, unsigned char size)
{
	unsigned char string_length;
	unsigned char x;
	unsigned char pixel_strength;
	string_length = strlen(c); // number of letters in the string

	pixel_strength = (string_length * (size * 5))+ ((string_length -1) * size); // number of pixel in width direction of the string

	// Find out the start position of string in x direction to align the string in Center
	x = (160 - pixel_strength)/2;

	while (c[0] != 0)
	{
		ST7735_drawChar(x, y, c[0], color, size);
		x += size*6;
		c++;
		// if (x + 5 >= SCREEN_WIDTH) //Text display is aligned to shorter side of the screen
		if (x + 5 >= SCREEN_HEIGHT) // Text display is aligned to longer side of the screen
		{
			y += 10;
			x = 0;
		}
	}
}


void ST7735_drawString_for_BALANCE_align_Center(unsigned char y, char *c, unsigned int color)
{
	unsigned char string_length;
	unsigned char x;
	unsigned char pixel_strength;
	string_length = strlen(c); // number of letters in the string

	pixel_strength = (string_length * 16)+ ((string_length -1) ); // number of pixel in width direction of the string

	// Find out the start position of string in x direction to align the string in Center
	x = (160 - pixel_strength)/2;
	if(y == 25)
	{
		Font_BigFont(x, y, c, color); // Function for to print "BALANCE" on center of the display
	}
	else
	{
		Font_Grotesk16x32(x, y, c, color); // Function for to print "999.9 L" water balance on center of the display
	}
}



void ST7735_drawString(unsigned char x, unsigned char y, char *c, unsigned int color, unsigned char size)
{
	while (c[0] != 0)
	{
		ST7735_drawChar(x, y, c[0], color, size);
		x += size*6;
		c++;
		// if (x + 5 >= SCREEN_WIDTH) //Text display is aligned to shorter side of the screen
		if (x + 5 >= SCREEN_HEIGHT) // Text display is aligned to longer side of the screen
		{
			y += 10;
			x = 0;
		}
	}
}

void ST7735_drawChar(unsigned char x, unsigned char y, char c, unsigned int color, unsigned char size)
{
	unsigned char i, j;

	unsigned char letter = c < 0x52 ? c - 0x20 : c - 0x52;
	for (i =0; i<5; i++ )
	{
		unsigned char line = c < 0x52 ? Alpha1[letter*5+i] : Alpha2[letter*5+i];

		for (j = 0; j<8; j++)
		{
			if (line & 0x1)
			{
				if (size == 1) // default size
					ST7735_drawPixel(x+i, y+j, color);
				else   // big size
					ST7735_fillRect(x+i*size, y+j*size, size, size, color);
			}
			line >>= 1;
		}
	}
}
// more ready made font are available on http://www.rinkydinkelectronics.com/r_fonts.php this link
// Function for to print "BALANCE" on center of the display
void Font_BigFont(unsigned int x0, unsigned int y0, unsigned char *c, unsigned int color)
{
	int i,j,k,x,y,xx;
	int z = 0;

	unsigned char qm;
	long int ulOffset;
	char  ywbuf[50],temp[2];

	for(i = 0; i<strlen((char*)c);i++)
	{
		int z = 0;
		if(((unsigned char)(*(c+i))) >= 161)
		{
			temp[0] = *(c+i);
			temp[1] = '\0';
			return;
		}
		else
		{
			qm = *(c+i);
			ulOffset = (long int)(qm) * 32;  // calculate pointer to print character (32 bit raw data is available in single character)
			for (j = 0; j < 32; j ++)
			{
				ywbuf[j]=BigFont[(ulOffset-1024)+j]; // copy  character into ywbuf[j] buffer memory (1024 = 32(character per line) *32(difference between 'A' and 'a')
			}
			for(y = 0;y < 16; y++) //16 is vertical line
			{
				for(x=0;x<16;x++) // 16 is horizontal line
				{

					if(x > 7) //print next 8 bit data of 16 bit data
					{
						k=x % 8;
						if(ywbuf[z+1]&(0x80 >> k)) // print ywbuf[z+1] next 8 bit data
						{
							xx=x0+x+i*16;
							//Output_Pixel(xx,y+y0);
							ST7735_drawPixel(xx, y+y0, color);
						}
					}
					else //print first 8 bit data of 16 bit data
					{
						k=x % 8;
						if(ywbuf[z]&(0x80 >> k)) // print ywbuf[z] first 8 bit data
						{
							xx=x0+x+i*16;
							//Output_Pixel(xx,y+y0);
							ST7735_drawPixel(xx, y+y0, color);
						}
					}

				}
				z=z+2; // increment of ywbuf[] buffer memory  by 2 every time because of 16 bit data
			}
		}
	}
}

// Function for to print "999.9 L" water balance on center of the display
void Font_Grotesk16x32(unsigned int x0, unsigned int y0, unsigned char *c, unsigned int color)
{
	int i,j,k,x,y,xx;
	int z = 0;

	unsigned char qm;
	long int ulOffset;
	char  ywbuf[100],temp[2];

	for(i = 0; i<strlen((char*)c);i++)
	{
		int z = 0;
		if(((unsigned char)(*(c+i))) >= 161)
		{
			temp[0] = *(c+i);
			temp[1] = '\0';
			return;
		}
		else
		{
			qm = *(c+i);
			ulOffset = (long int)(qm) * 64;// calculate pointer to print character (64 bit raw data is available in single character)
			for (j = 0; j < 64; j ++)
			{
				ywbuf[j]=Grotesk16x32[(ulOffset-2048)+j];// copy  character into ywbuf[j] buffer memory (2048 = 64(character per line) *32(difference between 'A' and 'a')
			}
			for(y = 0;y < 32; y++) // 32 is vertical line
			{
				for(x=0;x<16;x++)  // 16 is horizontal line
				{

					if(x > 7)  //print next 8 bit data of 16 bit data
					{
						k=x % 8;
						if(ywbuf[z+1]&(0x80 >> k))  // print ywbuf[z+1] next 8 bit data
						{
							xx=x0+x+i*16;
							//Output_Pixel(xx,y+y0);
							ST7735_drawPixel(xx, y+y0, color);
						}
					}
					else  //print first 8 bit dta of 16 bit data
					{
						k=x % 8;
						if(ywbuf[z]&(0x80 >> k)) // print ywbuf[z] first 8 bit data
						{
							xx=x0+x+i*16;
							//Output_Pixel(xx,y+y0);
							ST7735_drawPixel(xx, y+y0, color);
						}
					}

				}
				z=z+2; // increment of ywbuf[] buffer memory  by 2 every time beciuse of 16 bit data
			}
		}
	}
}


void ST7735_drawCircleHelper(signed int x0, signed int y0, signed int r, unsigned char cornername, unsigned int color)
{
	signed int f, ddF_x, ddF_y, x, y;
	f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4)
		{
			ST7735_drawPixel(x0 + x, y0 + y, color);
			ST7735_drawPixel(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2)
		{
			ST7735_drawPixel(x0 + x, y0 - y, color);
			ST7735_drawPixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8)
		{
			ST7735_drawPixel(x0 - y, y0 + x, color);
			ST7735_drawPixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1)
		{
			ST7735_drawPixel(x0 - y, y0 - x, color);
			ST7735_drawPixel(x0 - x, y0 - y, color);
		}
	}
}
void ST7735_fillCircleHelper(signed int x0, signed int y0, signed int r, unsigned char cornername, signed int delta, unsigned int color)
{
	signed int f, ddF_x, ddF_y, x, y;
	f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1)
		{
			ST7735_drawVerticalLine(x0+x, y0-y, 2*y+1+delta, color);
			ST7735_drawVerticalLine(x0+y, y0-x, 2*x+1+delta, color);
		}
		if (cornername & 0x2)
		{
			ST7735_drawVerticalLine(x0-x, y0-y, 2*y+1+delta, color);
			ST7735_drawVerticalLine(x0-y, y0-x, 2*x+1+delta, color);
		}
	}
}
/*
void ST7735_drawRoundRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int r, unsigned int color)
{
	// drawFastHLine(x+r  , y    , w-2*r, color);
	// drawFastHLine(x+r  , y+h-1, w-2*r, color);
	ST7735_drawHorizontalLine(x+r  , y    , w-2*r, color);
	ST7735_drawHorizontalLine(x+r  , y+h-1, w-2*r, color);
	ST7735_drawVerticalLine(x    , y+r  , h-2*r, color);
	ST7735_drawVerticalLine(x+w-1, y+r  , h-2*r, color);
	ST7735_drawCircleHelper(x+r    , y+r    , r, 1, color);
	ST7735_drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
	ST7735_drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
	ST7735_drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

void ST7735_fillRoundRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int r, unsigned int color)
{
	ST7735_fillRect(x+r, y, w-2*r, h, color);
	ST7735_fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
	ST7735_fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}
 */
// fill a circle
void ST7735_fillCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned int color) {
	int f = 1 - r;
	int ddF_x = 1;
	int ddF_y = -2 * r;
	int x = 0;
	int y = r;

	ST7735_drawVerticalLine(x0, y0-r, 2*r+1, color);

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7735_drawVerticalLine(x0+x, y0-y, 2*y+1, color);
		ST7735_drawVerticalLine(x0-x, y0-y, 2*y+1, color);
		ST7735_drawVerticalLine(x0+y, y0-x, 2*x+1, color);
		ST7735_drawVerticalLine(x0-y, y0-x, 2*x+1, color);
	}
}

// draw a circle outline
void ST7735_drawCircle(unsigned char x0, unsigned char y0, unsigned char r, unsigned int color) {
	int f = 1 - r;
	int ddF_x = 1;
	int ddF_y = -2 * r;
	int x = 0;
	int y = r;

	ST7735_drawPixel(x0, y0+r, color);
	ST7735_drawPixel(x0, y0-r, color);
	ST7735_drawPixel(x0+r, y0, color);
	ST7735_drawPixel(x0-r, y0, color);

	while (x<y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7735_drawPixel(x0 + x, y0 + y, color);
		ST7735_drawPixel(x0 - x, y0 + y, color);
		ST7735_drawPixel(x0 + x, y0 - y, color);
		ST7735_drawPixel(x0 - x, y0 - y, color);

		ST7735_drawPixel(x0 + y, y0 + x, color);
		ST7735_drawPixel(x0 - y, y0 + x, color);
		ST7735_drawPixel(x0 + y, y0 - x, color);
		ST7735_drawPixel(x0 - y, y0 - x, color);

	}
}

unsigned char ST7735_getRotation()
{
	return madctl;
}

void ST7735_setRotation(unsigned char m)
{
	madctl = m;
	writecommand(ST7735_MADCTL);  // memory access control (directions)
	writedata(madctl);  // row address/col address, bottom to top refresh
}

// draw a rectangle
void ST7735_drawRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h,unsigned int color)
{
	// smarter version
	ST7735_drawHorizontalLine(x, y, w, color);
	ST7735_drawHorizontalLine(x, y+h-1, w, color);
	ST7735_drawVerticalLine(x, y, h, color);
	ST7735_drawVerticalLine(x+w-1, y, h, color);
}

void ST7735_fillRect(unsigned char x, unsigned char y, unsigned char w, unsigned char h,unsigned int color)
{
	// smarter version

	setAddrWindow(x, y, x+w-1, y+h-1);

	// setup for data
	// LCD_RS = 1;
	// LCD_CS = 0;

	for (x=0; x < w; x++)
	{
		for (y=0; y < h; y++)
		{
			// write_spi_byte(colorB);
			// write_spi_byte(color);
			// writedata(colorB);
			writedata_16bit(color);
		}
	}
	// LCD_CS = 1;
}

void ST7735_drawFastLine(unsigned char x, unsigned char y, unsigned char length,unsigned int color, unsigned char rotflag)
{
	if (rotflag)
	{
		setAddrWindow(x, y, x, y+length-1);
	}
	else
	{
		setAddrWindow(x, y, x+length, y-1);
	}
	// setup for data
	// LCD_RS = 1;
	// LCD_CS = 0;

	while (length--)
	{
		// write_spi_byte(colorB);
		// write_spi_byte(color);
		// writedata(colorB);
		writedata_16bit(color);
	}
	// LCD_CS = 1;
}

void ST7735_drawVerticalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color)
{
	if (x >= SCREEN_WIDTH)
		return;
	if (y+length >= SCREEN_HEIGHT)
		length = SCREEN_HEIGHT-y-1;

	ST7735_drawFastLine(x,y,length,color,1);
}

void ST7735_drawHorizontalLine(unsigned char x, unsigned char y, unsigned char length, unsigned int color)
{
	if (y >= SCREEN_HEIGHT)
		return;
	if (x+length >= SCREEN_WIDTH)
		length = SCREEN_WIDTH-x-1;

	ST7735_drawFastLine(x,y,length,color,0);
}


int abs(int val)
{
	return (val > 0) ? val : -val;
}

// bresenham's algorithm - thx wikpedia
void ST7735_drawLine(int x0, int y0, int x1, int y1,unsigned int color)
{
	unsigned int steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1)
	{
		swap(x0, x1);
		swap(y0, y1);
	}

	unsigned int dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int err = dx / 2;
	int ystep;

	if (y0 < y1)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}

	for (; x0<=x1; x0++)
	{
		if (steep)
		{
			ST7735_drawPixel(y0, x0, color);
		}
		else
		{
			ST7735_drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}
}
// Load an image on the LCD screen.
//The images can appear on screen in three styles
// 0 -> converge to center of image. 1 -> Horizontal swipe, 1 -> 2 horizontal swipes in opposite direction
void loadBitmapToLCD(char image_number, char style)
{
	unsigned int Pixel_Data_Read = 0;
	unsigned char start_X = 0;
	unsigned char start_Y = 0;
	unsigned char curX = 0;
	unsigned char curY = 0;
	unsigned char bitmapWidth = 128;
	unsigned char bitmapHeight = 160;
	unsigned char coloumn = curY;
	unsigned char row = curX;
	int *image_data;

	switch(image_number)
	{
	case WELCOME_LOGO://Stripped welcome logo.. Background is BLACK..
		start_X = 40;
		start_Y = 42;
		bitmapWidth = 53;
		bitmapHeight = 73;

		image_data = (int *)Welcome_Logo_Image_data;
		break;
	}

	switch(style)
	{
	case 0:
		// Plane Horizontal swipe in one directions to fill the image on screen
		for (Pixel_Data_Read = 0; Pixel_Data_Read<(bitmapWidth * bitmapHeight);Pixel_Data_Read++)
		{
			ST7735_drawPixel(curX+start_X, (bitmapHeight-curY+start_Y-1), (image_data[Pixel_Data_Read]));
			curX++;

			if (curX == bitmapWidth)
			{
				curY++;
				curX = 0;
			}
		}
		break;
	case 1:
		// Plane two Horizontal swipes in opposite directions to fill the image
		for (Pixel_Data_Read = 0; Pixel_Data_Read<((bitmapWidth * bitmapHeight)/2);Pixel_Data_Read++)
		{
			ST7735_drawPixel(curX+start_X, curY+start_Y, image_data[Pixel_Data_Read]);
			ST7735_drawPixel(curX+start_X, bitmapHeight-curY+start_Y-1, image_data[((bitmapHeight-curY-1) * bitmapWidth)+ curX]);
			curX++;

			if (curX == bitmapWidth)
			{
				curY++;
				curX = 0;
			}
		}
		break;
	case 2:

		for (coloumn = 0; coloumn<bitmapHeight/2;)
		{
			for(row = curX; row<(bitmapWidth-curX); row++)
			{
				// if(image_data[(coloumn * bitmapWidth) + row] !=BACKGROUND_COLOUR)// skip those pixels with background colour to speed up the entire image paint
				ST7735_drawPixel(row+start_X, coloumn+start_Y, image_data[(coloumn * bitmapWidth) + row]);
				// if(image_data[((bitmapHeight-coloumn-1) * bitmapWidth)+ row] !=BACKGROUND_COLOUR)
				ST7735_drawPixel(row+start_X, bitmapHeight-coloumn+start_Y-1, image_data[((bitmapHeight-coloumn-1) * bitmapWidth)+ row]);
			}
			curY++;
			row = curX;

			for(coloumn = curY; coloumn<(bitmapHeight-curY); coloumn++)
			{
				// if(image_data[(coloumn * bitmapWidth) + row] !=BACKGROUND_COLOUR)
				ST7735_drawPixel(row+start_X, coloumn+start_Y, image_data[(coloumn * bitmapWidth) + row]);
				// if(image_data[(coloumn * bitmapWidth) + bitmapWidth-row-1] !=BACKGROUND_COLOUR)// skip those pixels with background colour to speed up the entire image paint
				ST7735_drawPixel(bitmapWidth-row+start_X-1, coloumn+start_Y, image_data[(coloumn * bitmapWidth) + bitmapWidth-row-1]);
			}
			curX++;
			coloumn = curY;
		}
		break;
	}

}

void Refresh_Antenna_Symbol(void)
{
	if(FLAG.SIMCARD_REGISTERED)//Draw antenna signal strength only if SIM Card is Present and detected
	{
		ST7735_setRotation(0x0); // Start point Left - top

		if(RssiLevel >=21 && RssiLevel <=31)
		{
			ST7735_fillRect(111, 156, 15, 2, colWhite);
			ST7735_fillRect(111, 152, 12, 2, colWhite);
			ST7735_fillRect(111, 148,  9, 2, colWhite);
			ST7735_fillRect(111, 144,  6, 2, colWhite);
		}
		else if(RssiLevel >=16 && RssiLevel <=20)
		{
			ST7735_fillRect(111, 156, 15, 2, colGray);
			ST7735_fillRect(111, 152, 12, 2, colWhite);
			ST7735_fillRect(111, 148,  9, 2, colWhite);
			ST7735_fillRect(111, 144,  6, 2, colWhite);
		}
		else if(RssiLevel >=11 && RssiLevel <=15)
		{
			ST7735_fillRect(111, 156, 15, 2, colGray);
			ST7735_fillRect(111, 152, 12, 2, colGray);
			ST7735_fillRect(111, 148,  9, 2, colWhite);
			ST7735_fillRect(111, 144,  6, 2, colWhite);
		}
		else if(RssiLevel >=6 && RssiLevel <=10)
		{
			ST7735_fillRect(111, 156, 15, 2, colGray);
			ST7735_fillRect(111, 152, 12, 2, colGray);
			ST7735_fillRect(111, 148,  9, 2, colGray);
			ST7735_fillRect(111, 144,  6, 2, colWhite);
		}
		else
		{
			ST7735_fillRect(111, 156, 15, 2, colGray);
			ST7735_fillRect(111, 152, 12, 2, colGray);
			ST7735_fillRect(111, 148,  9, 2, colGray);
			ST7735_fillRect(111, 144,  6, 2, colGray);
		}

		//If in Diagnostic print RSSI value just below the antenna symbol
		if(FLAG.DIGNOSTIC_MODE == TRUE)
		{
			ST7735_setRotation(0x60);
			Ql_sprintf((char *)buffer,"%d",previous_rssival);
			ST7735_drawString(145, 20, (char *)buffer, colBlack, 1);  //Service provider name details
			Ql_sprintf((char *)buffer,"%d",RssiLevel);
			ST7735_drawString(145, 20, (char *)buffer, colWhite, 1);  //Service provider name details
		}
		//assign present value to prev variable
		previous_rssival = RssiLevel;
	}
}

void Refresh_Cloud_Connectivity_Symbol(char cloud_connetcivity_status)
{
	ST7735_setRotation(0x0); // Start point Left - top
	if(cloud_connetcivity_status == 2)
	{
		ST7735_fillCircle(116, 125, 5, colLawngreen);
		if(RssiLevel < 6)
		{
			RssiLevel = 6;// Show at least one active bar when connection is success with rssi value lower than 6
			Refresh_Antenna_Symbol();
		}
	}
	else if(cloud_connetcivity_status == 1) // DOT will become RED when DPS session is complete but IoT Hub session is not success
	{
		ST7735_fillCircle(116, 125, 5, colOrangeRed);
	}
	else if(cloud_connetcivity_status == 0) // The DOT will remain in GRAY colour till DPS session is complete
	{
		ST7735_fillCircle(116, 125, 5, colGray);
		if(RssiLevel < 6)
		{
			RssiLevel = 6;// Show at least one active bar when connection is success with rssi value lower than 6
			Refresh_Antenna_Symbol();
		}
	}

}

void Refresh_Communication_inprogress_Symbol(unsigned int colour)
{
	if(FLAG.WATER_SCREEN_INITIALIZED == TRUE) // Only if water balance screen is initialized
	{
		ST7735_setRotation(0x0); // Start point Left - top
		ST7735_drawCircle(116, 125, 7, colour);
		FLAG.CLOUD_COMMUNICATION_IN_PROGRESS = TRUE;//This will clean up the symbol / indication after 1 second.
	}
}


void Init_display(void)
{
	ST7735_initR();
	ST7735_fillScreen(colBlack);
	loadBitmapToLCD(WELCOME_LOGO,2);
}


void Fill_Screen_Color(u32 color)
{
	ST7735_fillScreen(color);
}

void Display_Water_Balance_on_LCD(bool Schedule_Data_Publish)
{
	ST7735_setRotation(0x60);
	ST7735_drawString_for_BALANCE_align_Center(45, water_balance_buffer_last, colBlack);  // Start point Right - top // Erase last message string
	if(water_balance<20)
	{
		ST7735_drawString_align_Center(80, LCD_Dislpay_Message[MESSAGE_TANK_FULL], colBlack, 1);  // Start point Right - top
		ST7735_drawString_for_BALANCE_align_Center(45, water_balance_buffer, colOrangeRed);  // Start point Right - top
		if(water_balance > 0 && water_balance < 20)
		{
			if(!sendonetimelowbal)
			{
				// Display Low Balance message
				ST7735_setRotation(0x60);
				ST7735_drawString_align_Center(90, LCD_Dislpay_Message[MESSAGE_NO_BALANCE], colBlack, 1);  // Start point Right - top
				ST7735_drawString_align_Center(90, LCD_Dislpay_Message[MESSAGE_LOW_BALANCE], colOrangeRed, 1);  // Start point Right - top
				ST7735_drawString_align_Center(105, LCD_Dislpay_Message[MESSAGE_PLEASE_RECHARGE], colOrangeRed, 1);  // Start point Right - top

				if(Schedule_Data_Publish)
				{
					// Send message to cloud
					PUBLISH_RESPONSE_REQ_source |= SCHEDULE_PUBLISH_BAL_LOW;
				}
				sendonetimelowbal = 1;
			}
			sendonetimezerobal = 0;
		}
		else if(0 == water_balance )
		{
			if(!sendonetimezerobal)
			{
				// Display No Balance message
				ST7735_setRotation(0x60);
				ST7735_drawString_align_Center(90, LCD_Dislpay_Message[MESSAGE_LOW_BALANCE], colBlack, 1);  // Start point Right - top
				ST7735_drawString_align_Center(90, LCD_Dislpay_Message[MESSAGE_NO_BALANCE], colOrangeRed, 1);  // Start point Right - top
				ST7735_drawString_align_Center(105, LCD_Dislpay_Message[MESSAGE_PLEASE_RECHARGE], colOrangeRed, 1);  // Start point Right - top

				if(Schedule_Data_Publish)
				{
					// Send message to cloud
					PUBLISH_RESPONSE_REQ_source |= SCHEDULE_PUBLISH_BAL_ZERO;
				}
				sendonetimezerobal = 1;
			}
			sendonetimelowbal = 0;
		}
		Balance_in_green =0;
	}
	else
	{
		if(FLAG.TANK_STATUS_IS_FULL)
			ST7735_drawString_align_Center(80, LCD_Dislpay_Message[MESSAGE_TANK_FULL], colLawngreen, 1);  //Erase Tank Full message
		ST7735_drawString_for_BALANCE_align_Center(45, water_balance_buffer, colLawngreen);  // Start point Right - top
		if(!Balance_in_green)
		{
			// clear water balance alarm messages
			ST7735_setRotation(0x60);
			ST7735_drawString_align_Center(90, LCD_Dislpay_Message[MESSAGE_NO_BALANCE], colBlack, 1);  // Start point Right - top
			ST7735_drawString_align_Center(90, LCD_Dislpay_Message[MESSAGE_LOW_BALANCE], colBlack, 1);  // Start point Right - top
			ST7735_drawString_align_Center(105, LCD_Dislpay_Message[MESSAGE_PLEASE_RECHARGE], colBlack, 1);  // Start point Right - top

			Balance_in_green =1;
		}
		if(Schedule_Data_Publish)
		{
			// Send message to cloud ..
			PUBLISH_RESPONSE_REQ_source |= SCHEDULE_PUBLISH_WATER_BALANCE;
		}
		sendonetimelowbal = 0;
		sendonetimezerobal = 0;
	}
	strcpy(water_balance_buffer_last,water_balance_buffer);
}

void Init_Water_Balance_Screen()
{
	Fill_Screen_Color(colBlack);
	FLAG.WATER_SCREEN_INITIALIZED = TRUE;//Set flag to avoid unwanted message to appear on Unilever logo
	// Display "BALANCE" as a word
	ST7735_setRotation(0x60);
	//ST7735_drawLine(0 , 20, 160, 20,colDarkSlateGray);
	ST7735_drawString_for_BALANCE_align_Center(25, LCD_Dislpay_Message[MESSAGE_BALANCE], colWhite);  // Start point Right - top

	//Draw Gallon Symbol
	//ST7735_setRotation(0x0); // Start point Left - top
	//ST7735_fillRoundRect(60, 125, 25, 16, 3, colAqua);
	//ST7735_fillRect(85, 130, 6, 6, colAqua);

	//Finally show water balance string
	Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_STRING_VALUE);
}

void display_please_wait(void)
{
	ST7735_setRotation(0x60);
	ST7735_drawString_align_Center(105, LCD_Dislpay_Message[MESSAGE_PLEASE_WAIT], colWhite, 1);  // Start point Right - top
}
void display_FW_Version_Info()
{
	Ql_sprintf((char *)buffer,"PUREIT VERSION %d.%d",FW_Version_Major_COMM,FW_Version_Minor_COMM);//Display version information
	ST7735_setRotation(0x60);
	ST7735_drawString_align_Center(105, LCD_Dislpay_Message[MESSAGE_PLEASE_WAIT], colBlack, 1);  // Start point Right - top
	ST7735_drawString_align_Center(105, (char *)buffer, colWhite, 1);  // Start point Right - top
}
void repaint_water_balance_screen(void)
{
	if(FLAG.DIGNOSTIC_MODE == FALSE)
	{
		// Display "BALANCE" as a word
		ST7735_setRotation(0x60);
		ST7735_drawString_for_BALANCE_align_Center(25, LCD_Dislpay_Message[LAST_MESSAGE_BALANCE], colBlack);  //Erase last message
		ST7735_drawString_for_BALANCE_align_Center(25, LCD_Dislpay_Message[MESSAGE_BALANCE], colWhite);  // repaint in new language

		//Now update the water balance and associated messages.
		sendonetimelowbal = 0;
		sendonetimezerobal = 0;
		Balance_in_green = 0;

		//Erase last water balance related warnings
		ST7735_setRotation(0x60);
		ST7735_drawString_align_Center(90, LCD_Dislpay_Message[LAST_MESSAGE_NO_BALANCE], colBlack, 1);
		ST7735_drawString_align_Center(90, LCD_Dislpay_Message[LAST_MESSAGE_LOW_BALANCE], colBlack, 1);
		ST7735_drawString_align_Center(105, LCD_Dislpay_Message[LAST_MESSAGE_PLEASE_RECHARGE], colBlack, 1);
		ST7735_drawString_align_Center(80, LCD_Dislpay_Message[LAST_MESSAGE_TANK_FULL], colBlack, 1);  //Erase Tank Full message
		Display_Water_Balance_on_LCD(0); // Just to repaint water balance related warings if any without publish to cloud

		//Erase last Heater related messages
		ST7735_drawString(5, 120, LCD_Dislpay_Message[LAST_MESSAGE_HOT_WATER_READY], colBlack, 1);
		ST7735_drawString(5, 120, LCD_Dislpay_Message[LAST_MESSAGE_HEATING_ON], colBlack, 1); //Erase last Tank Full message// Start point Right - top

		IPC_CMD_REQ_source |= SCHEDULE_IPC_GET_HEATER_STATUS_DATA_CMD; // Just to refresh the hear status
	}
}
void display_on_tftlcd(u32 dparam)
{
	u8 i =0;

	if(FLAG.DIGNOSTIC_MODE == TRUE) // Belwo are the messages grouped together for diagnostic screen)
	{
		switch(dparam)
		{
		case TFT_STATE_DRAW_DIAG_INIT:
			ST7735_setRotation(0x60);
			ST7735_drawString(  0, 25, (char *)deviceidentitydata, colWhite, 1);  // Control Module Identity String
			ST7735_drawString(  0, 45, control_module_identity_number, colWhite, 1);  //Communication Module identity string
			ST7735_drawString(  0, 55, Last_service_provider_name, colBlack, 1);  //Erase earlier message.. mostly it is "Unknown"
			ST7735_drawString(  0, 55, service_provider_name, colWhite, 1);  //Service provider name details
			strcpy(Last_service_provider_name,service_provider_name);
			ST7735_drawString(  0, 65, last_latlong, colBlack, 1);  //Last gps information clear
			Ql_sprintf((char*)buffer,"5. %s %s",latitude_[0]=='\0'? "Unknown" : latitude_,longitude_[0]=='\0'? "Unknown" : longitude_);
			ST7735_drawString(  0, 65, (char *)buffer, colWhite, 1);  //GPS information
			strcpy(last_latlong,(char *)buffer);
			break;
			//Recoverable faults..
		case TFT_STATE_DRAW_STRING_E1:
			ST7735_setRotation(0x60);
			ST7735_drawString(5, 120, "E1", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_CLEAR_STRING_E1:
			ST7735_setRotation(0x60);
			ST7735_drawString(5, 120, "E1", colBlack, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_E5:
			ST7735_setRotation(0x60);
			ST7735_drawString(25, 120, "E5", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_CLEAR_STRING_E5:
			ST7735_setRotation(0x60);
			ST7735_drawString(25, 120, "E5", colBlack, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_E15:
			ST7735_setRotation(0x60);
			ST7735_drawString(45, 120, "E15", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_CLEAR_STRING_E15:
			ST7735_setRotation(0x60);
			ST7735_drawString(45, 120, "E15", colBlack, 1);  // Start point Right - top
			break;
			// Now persistent faults to display
		case TFT_STATE_DRAW_STRING_E2:
			ST7735_setRotation(0x60);
			ST7735_drawString(105, 120, "E2", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_E3:
			ST7735_setRotation(0x60);
			ST7735_drawString(105, 120, "E3", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_E4:
			ST7735_setRotation(0x60);
			ST7735_drawString(105, 120, "E4", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_E6:
			ST7735_setRotation(0x60);
			ST7735_drawString(105, 120, "E6", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_E9:
			ST7735_setRotation(0x60);
			ST7735_drawString(105, 120, "E9", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_E10:
			ST7735_setRotation(0x60);
			ST7735_drawString(105, 120, "E10", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_E11:
			ST7735_setRotation(0x60);
			ST7735_drawString(105, 120, "E11", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_E12:
			ST7735_setRotation(0x60);
			ST7735_drawString(105, 120, "E12", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_IMEI:
			ST7735_setRotation(0x60);
			ST7735_drawString(  0, 35, Last_CCIDNumber, colBlack, 1);  //Erase earlier message.. mostly it is "Unknown"
			ST7735_drawString(  0, 35, IMEINumber, colWhite, 1);  //SIM details
			strcpy(Last_IMEINumber,IMEINumber);
			break;
		case TFT_STATE_DRAW_CCID:
			ST7735_setRotation(0x60);
			ST7735_drawString(  0, 35, Last_IMEINumber, colBlack, 1);  //Erase earlier message.. mostly it is "Unknown"
			ST7735_drawString(  0, 35, CCIDNumber, colWhite, 1);  //SIM details
			strcpy(Last_CCIDNumber,CCIDNumber);
			break;
		case TFT_STATE_DRAW_DIAG_LAST_CONNECT_TIME:
			ST7735_setRotation(0x60);
			ST7735_drawString(  0, 75, (char *)last_lastconnectedtime, colBlack, 1);  // Start point Right - top // Erase last message string
			Ql_sprintf((char *)buffer,"6. %s",last_connectedtime[0] == '\0'? "Unknown" : last_connectedtime);  //for diagnostic
			ST7735_drawString(  0, 75, (char *)buffer, colWhite, 1);
			strcpy((char *)last_lastconnectedtime,(char *)buffer);
			break;
		case TFT_STATE_DRAW_DIAG_RECHARGE_INFO:
			ST7735_setRotation(0x60);
			ST7735_drawString(0, 85, (char *)last_water_balance_info_buffer, colBlack, 1);  // Start point Right - top // Erase last message string
			Ql_sprintf((char *)buffer,"7. %s  %s",(char)cmdrcbal[0] == '\0'? "Unknown" : cmdrcbal,water_balance_buffer);
			ST7735_drawString(  0, 85, (char *)buffer, colWhite, 1);  //Display recharge and current balance
			strcpy((char *)last_water_balance_info_buffer,(char *)buffer);
			break;
		case TFT_STATE_DRAW_SENSOR_INPUT:
			ST7735_setRotation(0x60);
			Ql_sprintf((char *)buffer,"8. %s",Last_Sensor_Input_String);
			ST7735_drawString(0, 95,(char *)buffer, colBlack, 1);  // Start point Right - top // Erase last message string
			for(i=0; i <16 ; i++)
			{
				Sensor_Input_String[i] = (((Sensor_Status_Byte >> i) & 0x1 ) ? 'X' : '-');
			}
			Sensor_Input_String[16] = '\0';
			Ql_sprintf((char *)buffer,"8. %s",Sensor_Input_String);
			ST7735_drawString(0, 95,(char *)buffer, colWhite, 1);  // Start point Right - top
			strcpy(Last_Sensor_Input_String,Sensor_Input_String);
			break;
		case TFT_STATE_DRAW_RO_PUMP_CURRENT:
			ST7735_setRotation(0x60);
			Ql_sprintf((char *)buffer,"%d",previous_ro_pump_current);
			ST7735_drawString(132, 85,(char *)buffer, colBlack, 1);  // Start point Right - top // Erase last message string
			Ql_sprintf((char *)buffer,"%d",ro_pump_current_in_mA);
			ST7735_drawString(132, 85,(char *)buffer, colWhite, 1);  // Start point Right - top
			previous_ro_pump_current = ro_pump_current_in_mA;
			break;
		case TFT_STATE_DRAW_TDS_SENSOR_IN_TEMPRATURE:
			ST7735_setRotation(0x60);
			Ql_sprintf((char *)buffer,"9. %d",previous_TDS_Sensor_In_Temp_value);
			ST7735_drawString(0, 105,(char *)buffer, colBlack, 1);  // Start point Right - top // Erase last message string
			Ql_sprintf((char *)buffer,"9. %d",TDS_Sensor_In_Temp_value);
			ST7735_drawString(0, 105,(char *)buffer, colWhite, 1);  // Start point Right - top
			previous_TDS_Sensor_In_Temp_value = TDS_Sensor_In_Temp_value;
			break;
		case TFT_STATE_DRAW_TDS_IN_SENSOR_VALUE_IN_PPM:
			ST7735_setRotation(0x60);
			Ql_sprintf((char *)buffer,"%d",previous_TDS_Sensor_In_value);
			ST7735_drawString(42, 105,(char *)buffer, colBlack, 1);  // Start point Right - top // Erase last message string
			Ql_sprintf((char *)buffer,"%d",TDS_Sensor_In_value);
			ST7735_drawString(42, 105,(char *)buffer, colWhite, 1);  // Start point Right - top
			previous_TDS_Sensor_In_value = TDS_Sensor_In_value;
			break;
		case TFT_STATE_DRAW_TDS_SENSOR_OUT_TEMPRATURE:
			ST7735_setRotation(0x60);
			Ql_sprintf((char *)buffer,"%d",previous_TDS_Sensor_Out_Temp_value);
			ST7735_drawString(72, 105,(char *)buffer, colBlack, 1);  // Start point Right - top // Erase last message string
			Ql_sprintf((char *)buffer,"%d",TDS_Sensor_Out_Temp_value);
			ST7735_drawString(72, 105,(char *)buffer, colWhite, 1);  // Start point Right - top
			previous_TDS_Sensor_Out_Temp_value = TDS_Sensor_Out_Temp_value;
			break;
		case TFT_STATE_DRAW_TDS_OUT_SENSOR_VALUE_IN_PPM:
			ST7735_setRotation(0x60);
			Ql_sprintf((char *)buffer,"%d",previous_TDS_Sensor_OUT_value);
			ST7735_drawString(96, 105,(char *)buffer, colBlack, 1);  // Start point Right - top // Erase last message string
			Ql_sprintf((char *)buffer,"%d",TDS_Sensor_OUT_value);
			ST7735_drawString(96, 105,(char *)buffer, colWhite, 1);  // Start point Right - top
			previous_TDS_Sensor_OUT_value = TDS_Sensor_OUT_value;
			break;
		case TFT_STATE_DRAW_TOTAL_ON_TIME:
			ST7735_setRotation(0x60);
			Ql_sprintf((char *)buffer,"%d",Previous_Purification_ON_Counter);
			ST7735_drawString(126, 105,(char *)buffer, colBlack, 1);  // Start point Right - top // Erase last message string
			Ql_sprintf((char *)buffer,"%d",Purification_ON_Counter);
			ST7735_drawString(126, 105,(char *)buffer, colWhite, 1);  // Start point Right - top
			Previous_Purification_ON_Counter = Purification_ON_Counter;
			break;
		case TFT_STATE_DRAW_AMC_TIMER:
			ST7735_setRotation(0x60);
			Ql_sprintf((char *)buffer,"%d",previous_amc_timer);
			ST7735_drawString(138, 75,(char *)buffer, colBlack, 1);  // Start point Right - top // Erase last message string
			Ql_sprintf((char *)buffer,"%d",amc_timer);
			if(Status_Byte & AMC_MODE_ON) // AMC mode is ON
			{
				if(AMC_Status_Byte & AMC_PHASE_1_START)
					ST7735_drawString(138, 75,(char *)buffer, colYellow, 1);  // Start point Right - top
				else if(AMC_Status_Byte & AMC_PROCESS_BUILDING_PRESSURE)
					ST7735_drawString(138, 75,(char *)buffer, colOrangeRed, 1);  // Start point Right - top
				else if(AMC_Status_Byte & AMC_MEMBRANE_CLEANING)
					ST7735_drawString(138, 75,(char *)buffer, colLightGreen, 1);  // Start point Right - top
				else
					ST7735_drawString(138, 75,(char *)buffer, colWhite, 1);  // Start point Right - top
			}
			else // normal purification mode is ON
				ST7735_drawString(138, 75,(char *)buffer, colWhite, 1);  // Start point Right - top
			previous_amc_timer = amc_timer;
			break;
		case TFT_STATE_DRAW_FLOW_RATE:
			ST7735_setRotation(0x60);
			Ql_sprintf((char *)buffer,"%d",previous_flowrate);
			ST7735_drawString(132, 95, (char *)buffer, colBlack, 1);
			Ql_sprintf((char *)buffer,"%d",water_flow_rate_ml_minute);
			if( water_flow_rate_ml_minute >= 80)
			{
				ST7735_drawString(132, 95, (char *)buffer, colAqua, 1);  //Flow rate in mL per minute
			}
			else if( water_flow_rate_ml_minute >=50  && water_flow_rate_ml_minute < 80)
			{
				ST7735_drawString(132, 95, (char *)buffer, colLawngreen, 1);  //Flow rate in mL per minute
			}
			else if( water_flow_rate_ml_minute < 50)
			{
				ST7735_drawString(132, 95, (char *)buffer, colOrangeRed, 1);  //Flow rate in mL per minute
			}
			previous_flowrate = water_flow_rate_ml_minute;
			break;
		case TFT_STATE_DRAW_STRING_HOTG:
			ST7735_setRotation(0x60);
			ST7735_drawString(70, 0, "HOT", colLawngreen, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_HOTR:
			ST7735_setRotation(0x60);
			ST7735_drawString(70, 0, "HOT", colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_CLEAR_STRING_HOT:
			ST7735_setRotation(0x60);
			ST7735_drawString(70, 0, "HOT", colBlack, 1);  // Start point Right - top
			break;
		}
	}
	else //Messages on Main water balance screen
	{
		switch (dparam)
		{
		case TFT_STATE_DRAW_STRING_VALUE:
			Display_Water_Balance_on_LCD(1); //Just to repaint water balance and related warings if any with publish to cloud
			break;
		case TFT_STATE_DRAW_STRING_TANK_FULL:
			ST7735_setRotation(0x60);
			ST7735_drawString_align_Center(80, LCD_Dislpay_Message[MESSAGE_TANK_FULL], colLawngreen, 1);  // Start point Right - top
			break;
		case TFT_STATE_CLEAR_STRING_TANK_FULL:
			ST7735_setRotation(0x60);
			ST7735_drawString_align_Center(80, LCD_Dislpay_Message[MESSAGE_TANK_FULL], colBlack, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_HOTG:
			ST7735_setRotation(0x60);
			ST7735_drawString(5, 120, LCD_Dislpay_Message[MESSAGE_HEATING_ON], colBlack, 1);  // Start point Right - top
			ST7735_drawString(5, 120, LCD_Dislpay_Message[MESSAGE_HOT_WATER_READY], colLawngreen, 1);  // Start point Right - top
			break;
		case TFT_STATE_DRAW_STRING_HOTR:
			ST7735_setRotation(0x60);
			ST7735_drawString(5, 120, LCD_Dislpay_Message[MESSAGE_HOT_WATER_READY], colBlack, 1);  // Start point Right - top
			ST7735_drawString(5, 120, LCD_Dislpay_Message[MESSAGE_HEATING_ON], colOrangeRed, 1);  // Start point Right - top
			break;
		case TFT_STATE_CLEAR_STRING_HOT:
			ST7735_setRotation(0x60);
			ST7735_drawString(5, 120, LCD_Dislpay_Message[MESSAGE_HOT_WATER_READY], colBlack, 1);  // Start point Right - top
			ST7735_drawString(5, 120, LCD_Dislpay_Message[MESSAGE_HEATING_ON], colBlack, 1);  // Start point Right - top
			break;
		}
	}

	//Common Messages on Main water balance screen and Diagnostic screen
	switch (dparam)
	{
	case TFT_STATE_DRAW_INIT_SCREEN:
		Init_Water_Balance_Screen();
		break;
	case TFT_STATE_DRAW_ANTENNA_SYMBOL:
		Refresh_Antenna_Symbol();
		break;
	case TFT_STATE_DRAW_CLOUD_CON_ON_SYMBOL:
		Refresh_Cloud_Connectivity_Symbol(2);
		break;
	case TFT_STATE_DRAW_CLOUD_CON_OFF_SYMBOL:
		if(FLAG.AUTOPROVISIONING_COMPLETE == TRUE) // Only if Azure DPS is complete.
			Refresh_Cloud_Connectivity_Symbol(1);
		break;
	case TFT_STATE_DRAW_CLOUD_COM_INPROGRESS_ON_SYMBOL:
		Refresh_Cloud_Connectivity_Symbol(2); //Just to make sure there is no conectivity activity displayed with conectivity symbol as inactive
		Refresh_Communication_inprogress_Symbol(colLawngreen);
		break;
	case TFT_STATE_DRAW_CLOUD_COM_INPROGRESS_OFF_SYMBOL:
		Refresh_Communication_inprogress_Symbol(colBlack);
		FLAG.CLOUD_COMMUNICATION_IN_PROGRESS = FALSE;
		break;
	case TFT_STATE_DRAW_STRING_INTERNAL_ERROR_CODE:
		if(FLAG.INTERNAL_ERROR_SET == FALSE) // Do not overwrite internal faults..
		{
			ST7735_setRotation(0x60);
			ST7735_drawString(135, 120, Internal_error_code, colOrangeRed, 1);  // Start point Right - top
			FLAG.INTERNAL_ERROR_SET = TRUE;
		}
		break;
	case TFT_STATE_CLEAR_SCREEN:
		ST7735_setRotation(0x0);
		ST7735_fillScreen(colBlack);
		break;
	case TFT_STATE_DRAW_SIM_NOT_PRESENT:
		ST7735_setRotation(0x0);
		ST7735_fillRect(110, 145, 15, 6, colWhiteSmoke);
		ST7735_fillRect(110, 151, 14, 1, colWhiteSmoke);
		ST7735_fillRect(110, 152, 13, 1, colWhiteSmoke);
		ST7735_fillRect(110, 153, 12, 1, colWhiteSmoke);
		ST7735_fillRect(110, 154, 11, 1, colWhiteSmoke);
		ST7735_drawString(  115, 146, "X", colOrangeRed, 1);  //set x to indicate sim not present
		break;
	case TFT_STATE_DRAW_STRING_LOCKED:
		ST7735_setRotation(0x60);
		ST7735_drawString(0, 0, "LOCKED", colOrangeRed, 1);  // Start point Right - top
		break;
	case TFT_STATE_CLEAR_STRING_LOCKED:
		ST7735_setRotation(0x60);
		ST7735_drawString(0, 0, "LOCKED", colBlack, 1);  // Start point Right - top
		break;
	case TFT_STATE_FOTA_UPDATE:
		ST7735_fillScreen(colBlack);
		ST7735_setRotation(0x60);
		ST7735_drawString_for_BALANCE_align_Center(25, "UPDATING...", colWhite);  // Start point Right - top
		break;
	case TFT_STATE_FOTA_FAIL:
		ST7735_setRotation(0x60);
		Ql_sprintf((char *)buffer,"FAIL %d",Fota_Fail_Reason_Code);
		ST7735_drawString(40,65, (char *)buffer, colOrangeRed, 2);  // Start point Right - top
		break;
	case TFT_STATE_FOTA_SUCCESS:
		ST7735_setRotation(0x60);
		Ql_sprintf((char *)buffer,"SUCCESS");
		ST7735_drawString_for_BALANCE_align_Center(40, (char *)buffer, colLawngreen);  // Start point Right - top
		break;
	case TFT_FOTA_FILE_DOWNLOAD_SIZE:
		ST7735_setRotation(0x60);
		ST7735_drawString(10,90, "FILE SIZE", colWhite, 1);
		ST7735_drawString(80, 90,(char *)LAST_FOTA_FILE_SIZE, colBlack, 1);  // Start point Right - top // Erase last message string
		Ql_sprintf((char *)&FOTA_FILE_SIZE," %d",g_rcvdDataLen + dataLen_here);
		ST7735_drawString(80, 90,(char *)FOTA_FILE_SIZE, colWhite, 1);  // Start point Right - top
		strcpy((char *)LAST_FOTA_FILE_SIZE,(char *)FOTA_FILE_SIZE);
		break;
	}
}

