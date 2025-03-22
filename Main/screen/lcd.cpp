#include "lcd.h"
#include "stm32h7xx_hal.h"
#include "config.h"
#include "ltdc.h"

uint32_t L8Clut[256];

extern SPI_HandleTypeDef hspi4;
#define LCD_SPI &hspi4

#define LCD_RST(x) HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, (GPIO_PinState)x)
#define LCD_delay HAL_Delay

static void lcd_bl_on()
{
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);
}

static void LCD_Reset(void)
{
    LCD_RST(0);
    HAL_Delay(200);
    LCD_RST(1);
    HAL_Delay(200);
}

static void SPI_WriteComm(uint16_t cmd)
{
    uint16_t data = 0x000 | cmd;
    HAL_SPI_Transmit(LCD_SPI, (uint8_t *)&data, 1, 0xff);
}
static void SPI_WriteData(uint16_t data)
{
    uint16_t d = 0x100 | data;
    HAL_SPI_Transmit(LCD_SPI, (uint8_t *)&d, 1, 0xff);
}

// ILI9481
static void lcd_init(void)
{
    LCD_delay(20);
    LCD_Reset();

    SPI_WriteComm(0x11); // exit_sleep_mode
    LCD_delay(130);
    SPI_WriteComm(0x13); // Enter_normal_mode
    LCD_delay(120);

    SPI_WriteComm(0x11);
    LCD_delay(20);
    SPI_WriteComm(0x3a); // Set_pixel_format
    SPI_WriteData(0x55); // 16 bits/pixel

    SPI_WriteComm(0xD0); // Power Setting
    SPI_WriteData(0x07);
    SPI_WriteData(0x42);
    SPI_WriteData(0x1B);

    SPI_WriteComm(0xD1); // VCOM Control
    SPI_WriteData(0x00);
    SPI_WriteData(0x14);
    SPI_WriteData(0x1B);

    SPI_WriteComm(0xD2); // Power setting for Normal Mode
    SPI_WriteData(0x01);
    SPI_WriteData(0x12);

    SPI_WriteComm(0xC0); // Panel Driving Setting
    SPI_WriteData(0x00);
    SPI_WriteData(0x3B);
    SPI_WriteData(0x00);
    SPI_WriteData(0x02);
    SPI_WriteData(0x01);

    SPI_WriteComm(0xC5); // Frame rate and Inversion Control
    SPI_WriteData(0x02); // Frame rate : 0x02 85 Hz

    SPI_WriteComm(0xC6); // Interface Control
    SPI_WriteData(0x02); // SDA_EN(D7) = 0, EPL(D1) = 0, DPL(D0) = 0

    SPI_WriteComm(0xC8); // Gamma setting
    SPI_WriteData(0x00);
    SPI_WriteData(0x46);
    SPI_WriteData(0x44);
    SPI_WriteData(0x50);
    SPI_WriteData(0x04);
    SPI_WriteData(0x16);
    SPI_WriteData(0x33);
    SPI_WriteData(0x13);
    SPI_WriteData(0x77);
    SPI_WriteData(0x05);
    SPI_WriteData(0x0F);
    SPI_WriteData(0x00);

    SPI_WriteComm(0x36); // Set Address Mode
    SPI_WriteData(0x48);
    SPI_WriteComm(0x3A);
    SPI_WriteData(0x66);

    LCD_delay(120);
    SPI_WriteComm(0x29); // Set Display On

    //SPI_WriteComm(0x2C); // Write_memory_start

    LCD_delay(10);
    SPI_WriteComm(0x20); // Exit Invert Mode

    SPI_WriteComm(0xB4); // Display and Frame Memory Write Mode
    SPI_WriteData(0x10);
}

void LtdcInit()
{
	lcd_init();
	lcd_bl_on();

	LTDC_LayerCfgTypeDef pLayerCfg = {0};
	hltdc.Instance = LTDC;

	LTDC_InitTypeDef* init = &hltdc.Init;
	init->DEPolarity = LTDC_DEPOLARITY_AL;
	init->PCPolarity = LTDC_PCPOLARITY_IIPC;
	init->Backcolor.Blue = 0xFF;
	init->Backcolor.Green = 0;
	init->Backcolor.Red = 0;

	// Horizontal
	init->HSPolarity = VIDEO_MODE_H_POLARITY;
	init->HorizontalSync = VIDEO_MODE_H_SYNC - 1;
	init->AccumulatedHBP = init->HorizontalSync + VIDEO_MODE_H_BACKPORCH;
	init->AccumulatedActiveW = init->AccumulatedHBP + VIDEO_MODE_H_WIDTH;
	init->TotalWidth = init->AccumulatedActiveW + VIDEO_MODE_H_FRONTPORCH;

	// Vertical
	init->VSPolarity = VIDEO_MODE_V_POLARITY;
	init->VerticalSync = VIDEO_MODE_V_SYNC - 1;
	init->AccumulatedVBP = init->VerticalSync + VIDEO_MODE_V_BACKPORCH;
	init->AccumulatedActiveH = init->AccumulatedVBP + VIDEO_MODE_V_HEIGHT;
	init->TotalHeigh = init->AccumulatedActiveH + VIDEO_MODE_V_FRONTPORCH;

	if (HAL_LTDC_Init(&hltdc) != HAL_OK)
	{
		Error_Handler();
	}

	pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_L8;
	pLayerCfg.Alpha = 0xff;
	pLayerCfg.Alpha0 = 0xff;
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;

	uint32_t argb = L8Clut[BORDER_COLOR];
	pLayerCfg.Backcolor.Blue = argb & 0xFF;
	pLayerCfg.Backcolor.Green = (argb >> 8) & 0xFF;
	pLayerCfg.Backcolor.Red = (argb >> 16) & 0xFF;

	pLayerCfg.WindowX0 = (VIDEO_MODE_H_WIDTH - H_SIZE) / 2;
	pLayerCfg.WindowX1 = pLayerCfg.WindowX0 + H_SIZE - 1;
	pLayerCfg.WindowY0 = (VIDEO_MODE_V_HEIGHT - V_SIZE) / 2;
	pLayerCfg.WindowY1 = pLayerCfg.WindowY0 + V_SIZE - 1;
	pLayerCfg.ImageWidth = H_SIZE;
	pLayerCfg.ImageHeight = V_SIZE;
	pLayerCfg.FBStartAdress = (uint32_t)VideoRam;

	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_LTDC_MspInit(&hltdc);

	// Pixel clock
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLL3.PLL3P = 2;
	PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
	PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;

	PeriphClkInitStruct.PLL3.PLL3M = VIDEO_MODE_PLL3M;
	PeriphClkInitStruct.PLL3.PLL3N = VIDEO_MODE_PLL3N;
	PeriphClkInitStruct.PLL3.PLL3R = VIDEO_MODE_PLL3R;
	PeriphClkInitStruct.PLL3.PLL3RGE = VIDEO_MODE_PLL3RGE;

	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_LTDC_ConfigCLUT(&hltdc, L8Clut, 256, LTDC_LAYER_1);
	HAL_LTDC_EnableCLUT(&hltdc, LTDC_LAYER_1);
}

static uint32_t get8bitColor(uint8_t twoBitColor)
{
	switch (twoBitColor)
	{
	case 0x00:
		return 0x00;
	case 0x01:
		return 0x55;
	case 0x02:
		return 0xaa;
	default:
		return 0xff;
	}
}

void PrepareClut()
{
	for (uint32_t i = 0; i < 256; i++)
	{
		// xxBBGGRR > ARGB

		// R5
		uint32_t r = get8bitColor(i & 0x0003);

		// G6
		uint32_t g = get8bitColor((i & 0x000C) >> 2);

		// B5
		uint32_t b = get8bitColor((i & 0x0030) >> 4);

		L8Clut[i] = 0xff000000 | (r << 0x10) | (g << 0x8) | b;
	}
}
