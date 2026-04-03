/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: You
 */ 

#include "LCD_GFX.h"
#include "ST7735.h"
#include <stddef.h>

/******************************************************************************
* Local Functions
******************************************************************************/

void SPI_Start_Stream()
{
    clear(LCD_PORT, LCD_TFT_CS);
    set(LCD_PORT, LCD_DC);
}

void SPI_End_Stream()
{
    set(LCD_PORT, LCD_TFT_CS);
}

/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
* @brief		Convert RGB888 value to RGB565 16-bit color data
* @note
*****************************************************************************/
uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
}

/**************************************************************************//**
* @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
* @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
* @note
*****************************************************************************/
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
    if (x < 0 || x >= LCD_WIDTH || y < 0 || y >= LCD_HEIGHT) {
        return;
    }
#if defined(USE_OBJECT_BUFFER)
#elif defined(USE_SPLIT_BUFFER)
    split_buffer_draw(x, y, color);
#elif defined(USE_COMPRESSED_BUFFER)
    compressed_buffer_draw(x, y, color);
#else
    LCD_setAddr(x,y,x,y);
	SPI_ControllerTx_16bit(color);
#endif
}

/**************************************************************************//**
* @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
* @brief		Draw a character starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
    uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
    uint8_t* pixels = ASCII[row];
#if defined(USE_OBJECT_BUFFER)
    object_buffer_push((GraphicsObject){
        .x0 = x,
        .y0 = y,
        .x1 = x+4,
        .y1 = y+7,
        .fg_color = fColor,
        .bg_color = bColor,
        .ellipticalMask = 0,
        .char_bitmap = pixels,
    });
#elif defined(USE_SPLIT_BUFFER) || defined(USE_COMPRESSED_BUFFER)
    int i, j;
	if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
        for(j=0;j<8;j++){
            for(i=0;i<5;i++){
				if (((pixels[i]>>j)&1)==1){
					LCD_drawPixel(x+i, y+j, fColor);
				}
				else {
					LCD_drawPixel(x+i, y+j, bColor);
				}
			}
		}
	}
#else
    LCD_setAddr(x, y, x+4, y+7);
    SPI_Start_Stream();
	int i, j;
	if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
        for(j=0;j<8;j++){
            for(i=0;i<5;i++){
				if (((pixels[i]>>j)&1)==1){
					SPI_ControllerTx_16bit_stream(fColor);
				}
				else {
					SPI_ControllerTx_16bit_stream(bColor);
				}
			}
		}
	}
    SPI_End_Stream();
#endif
}


/******************************************************************************
* LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
* You are free to create and add any additional files, libraries, and/or
*  helper function. All code must be authentically yours.
******************************************************************************/

/**************************************************************************//**
* @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
* @brief		Draw a colored circle of set radius at coordinates
* @note
*****************************************************************************/
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
{
#if defined(USE_OBJECT_BUFFER)
    object_buffer_push((GraphicsObject){
        .x0 = x0-radius,
        .y0 = y0-radius,
        .x1 = x0+radius,
        .y1 = y0+radius,
        .fg_color = color,
        .bg_color = BLACK,
        .ellipticalMask = 1,
        .char_bitmap = NULL,
    });
#else
	int i, j;
    int sq_dist = radius * radius;
    for (i = x0 - radius; i < x0 + radius; i++) {
        int start = -1;
        int end = -1;
        for (j = y0 - radius; j < y0 + radius; j++) {
            if ((i - x0) * (i - x0) + (j - y0) * (j - y0) <= sq_dist) {
                if (start == -1) {
                    start = j;
                } else {
                    end = j;
                }
            }
        }
#if defined(USE_SPLIT_BUFFER) || defined(USE_COMPRESSED_BUFFER)
        for (int y = start; y <= end; y++) {
            LCD_drawPixel(i, y, color);
        }
#else
        LCD_setAddr(i, start, i, end);
        SPI_Start_Stream();
        for (int k = 0; k < end - start + 1; k++) {
            SPI_ControllerTx_16bit_stream(color);
        }
        SPI_End_Stream();
#endif
    }
#endif
}


/**************************************************************************//**
* @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
* @brief		Draw a line from and to a point with a color
* @note
*****************************************************************************/
//void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
//{
//	short dx = x1 - x0;
//    short dy = y1 - y0;
//    short D = 2 * dy - dx;
//    short y = y0;
//    
//    for (int x = x0; x <= x1; x++) {
//        LCD_drawPixel(x, y, c);
//        if (D > 0) {
//            y = y + 1;
//            D = D + (2 * (dy - dx));
//        } else {
//            D = D + 2 * dy;
//        }
//    }
//}

void LCD_drawLine(short x0, short y0, short x1, short y1,uint16_t c)
{
	short dx = (x1 - x0 > 0) ? x1 - x0 : x0 - x1;
    short sx = (x0 < x1) ? 1 : -1;
    short dy = (y1 - y0 < 0) ? y1 - y0 : y0 - y1;
    short sy = (y0 < y1) ? 1 : -1;
    short error = dx + dy;
    
    short e2;
    while (1) {
        LCD_drawPixel(x0, y0, c);
        e2 = 2 * error;
        if (e2 >= dy) {
            if (x0 == x1) break;
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            if (y0 == y1) break;
            error += dx;
            y0 += sy;
        }
    }
}



/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
{
#if defined(USE_OBJECT_BUFFER)
    object_buffer_push((GraphicsObject){
        .x0 = x0,
        .y0 = y0,
        .x1 = x1,
        .y1 = y1,
        .fg_color = color,
        .bg_color = BLACK,
        .ellipticalMask = 0,
        .char_bitmap = NULL,
    });
#elif defined(USE_SPLIT_BUFFER) || defined(USE_COMPRESSED_BUFFER)
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            LCD_drawPixel(x, y, color);
        }
    }
#else
    LCD_setAddr(x0, y0, x1, y1);
    
    SPI_Start_Stream();

    for (int i = 0; i < (x1 - x0 + 1) * (y1 - y0 + 1); i++) {
        SPI_ControllerTx_16bit_stream(color);
    }
    SPI_End_Stream();
#endif
}

/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color) 
{
#if defined(USE_COMPRESSED_BUFFER)
    compressed_buffer_fill(color);
#elif defined(USE_SPLIT_BUFFER)
    split_buffer_fill(color);
#else
    LCD_drawBlock(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, color);
#endif
}

/**************************************************************************//**
* @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
* @brief		Draw a string starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg) {
    while (*str != '\0') {
        LCD_drawChar(x, y, *str, fg, bg);
        str++;
        x += 6;
    }
}