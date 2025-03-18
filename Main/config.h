#ifndef __CONFIG_H
#define __CONFIG_H

// Visible screen area (without borders)
#define H_SIZE 320
#define V_SIZE 480
#define TEXT_COLUMNS (H_SIZE / 8)
#define TEXT_ROWS (V_SIZE / 8)
#define BORDER_COLOR 0x30 // ..BBGGRR
extern uint8_t VideoRam[H_SIZE * V_SIZE];

// Pixel clock 10.2 MHz // 11.875 MHz
#define VIDEO_MODE_PLL3M 5
#define VIDEO_MODE_PLL3N 100
#define VIDEO_MODE_PLL3R 50
#define VIDEO_MODE_PLL3RGE RCC_PLL3VCIRANGE_2

// Horizontal 320 pixels
#define VIDEO_MODE_H_SYNC 2
#define VIDEO_MODE_H_BACKPORCH 3
#define VIDEO_MODE_H_WIDTH 320
#define VIDEO_MODE_H_FRONTPORCH 15
#define VIDEO_MODE_H_POLARITY LTDC_HSPOLARITY_AL

// Vertical 480 lines
#define VIDEO_MODE_V_SYNC 2
#define VIDEO_MODE_V_BACKPORCH 3
#define VIDEO_MODE_V_HEIGHT 480
#define VIDEO_MODE_V_FRONTPORCH 15
#define VIDEO_MODE_V_POLARITY LTDC_VSPOLARITY_AL

#endif
