#include "lcd.h"

extern SPI_HandleTypeDef hspi4;
#define LCD_SPI &hspi4

#define LCD_RST(x) HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, (GPIO_PinState)x)
#define LCD_delay HAL_Delay

void lcd_bl_on()
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
void lcd_init(void)
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
/*
    SPI_WriteComm(0x2A); // Set Column Address
    SPI_WriteData(0x00);
    SPI_WriteData(0x00);
    SPI_WriteData(0x01);
    SPI_WriteData(0x3F);

    SPI_WriteComm(0x2B); // Set Page Address
    SPI_WriteData(0x00);
    SPI_WriteData(0x00);
    SPI_WriteData(0x01);
    SPI_WriteData(0xdf);
*/
    LCD_delay(120);
    SPI_WriteComm(0x29); // Set Display On

    //SPI_WriteComm(0x2C); // Write_memory_start

    LCD_delay(10);
    SPI_WriteComm(0x20); // Exit Invert Mode

    SPI_WriteComm(0xB4); // Display and Frame Memory Write Mode
    SPI_WriteData(0x10);
}
