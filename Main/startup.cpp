#include "stm32h7xx_hal.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include <stdio.h>

#include "w25qxx_qspi.h"
#include "fatfs.h"

// EEPROM AT24C02 2K
#define EEPROM_ADDRESS 0xA0
extern I2C_HandleTypeDef hi2c3;

static void MapFlash();
static int EEPROM_WriteBytes(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t TxBufferSize);
static int EEPROM_ReadBytes(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t* pData, uint16_t RxBufferSize);

extern "C" void initialize()
{
}

extern "C" void setup()
{
	HAL_GPIO_WritePin(ON_GPIO_Port, ON_Pin, GPIO_PIN_SET);

	uint8_t buffer[0x800];


	// Write to EEPROM
	for (uint8_t i = 1; i <= 0x20; i++)
	{
		buffer[i] = 0xff;
	}
	EEPROM_WriteBytes(&hi2c3, EEPROM_ADDRESS, 0, (uint8_t*)&buffer, 0x20);


	// Read from EEPROM
	EEPROM_ReadBytes(&hi2c3, EEPROM_ADDRESS, 0, (uint8_t*)&buffer, 0x800);

	// QSPI Flash 8MB (W25Q128JVSIM_TR)
	MapFlash();

	// Read from QSPI Flash
	uint8_t data[16];
	memcpy(data, (uint8_t*)QSPI_BASE, 16);

	HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);
/*
 * dump external flash
	if (f_mount(&SDFatFS, (TCHAR*)u"0:/", 1) == FR_OK)
	{
		FIL dumpFile;
		FRESULT res = f_open(&dumpFile, "extflash.bin", FA_CREATE_ALWAYS | FA_WRITE);
	    if(res != FR_OK) {
	        return;
	    }

	    uint8_t* from = (uint8_t*)QSPI_BASE;
	    uint32_t pos = 0;
	    UINT bytesWritten;
	    while (pos < 0x800000)
	    {
	    	res = f_write(&dumpFile, from, 4096, &bytesWritten);
			if(res != FR_OK || bytesWritten != 4096)
			{
				return;
			}

			from += 4096;
			pos += 4096;
	    }

		res = f_close(&dumpFile);
		if(res != FR_OK)
		{
			return;
		}

		f_mount(nullptr, nullptr, 1);
	}
*/
}

extern "C" void loop()
{
	HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
	HAL_Delay (1000);
}

static void MapFlash()
{
	w25qxx_Init();
	w25qxx_EnterQPI();
	w25qxx_Startup(w25qxx_NormalMode); // w25qxx_DTRMode
}

static int EEPROM_ReadBytes(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint16_t MemAddress, uint8_t* pData, uint16_t RxBufferSize)
{
	int TimeOut;
	/*
	 * program just get the DevAddress of the Slave (not master) and for the next step
	 * You know that the most of the EEprom address start with 0xA0
	 * get the MemAddress for the location you want to write data on it
	 * get the Data buffer so it can write Data on this location
	 */
	//Note that this function works properly to 31bytes

	while((RxBufferSize - 16) > 0)
	{
		//if your data is more than 16 bytes,you are here
		TimeOut = 0;
		 while(HAL_I2C_Mem_Read(hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_8BIT, pData, (uint16_t)16, 1000) != HAL_OK && TimeOut < 10)
		 {
			TimeOut++;
		 }

		 RxBufferSize -= 16;
		 pData += 16;
		 MemAddress += 16;
	}
//			//remaining data
	TimeOut = 0;
	while(HAL_I2C_Mem_Read(hi2c, (uint16_t)DevAddress, (uint16_t)MemAddress, I2C_MEMADD_SIZE_8BIT, pData, (uint16_t)RxBufferSize, 1000) != HAL_OK && TimeOut < 10)
	{
		TimeOut++;
	}

	return 1;
}

static int EEPROM_WriteBytes(I2C_HandleTypeDef *hi2c,uint16_t DevAddress,uint16_t MemAddress, uint8_t *pData,uint16_t TxBufferSize)
{
	/*
	 * program just get the DevAddress of the Slave (not master) and for the next step
	 * You know that the most of the EEprom address start with 0xA0
	 * give MemAddress for the location you want to write to
	 * give Data buffer so it can write Data on this location
	 */
	//Note that this function works properly to 31 bytes
	if(MemAddress+TxBufferSize > 16)
	{
		//Write to 16bytes
		while(HAL_I2C_Mem_Write(hi2c,(uint16_t)DevAddress,(uint16_t)MemAddress,I2C_MEMADD_SIZE_8BIT,pData,(uint16_t)16-MemAddress,1000)!= HAL_OK);
		//write remaining bytes
		*pData = *pData + (16-MemAddress);
		while(HAL_I2C_Mem_Write(hi2c,(uint16_t)DevAddress,(uint16_t)16,I2C_MEMADD_SIZE_8BIT,pData,(uint16_t)((MemAddress+TxBufferSize)-16),1000)!= HAL_OK);

	}
	else
	{
			while( (TxBufferSize-16)>0 )
			{
				//if your data is more than 16 bytes,you are here
				 while(HAL_I2C_Mem_Write(hi2c,(uint16_t)DevAddress,(uint16_t)MemAddress,I2C_MEMADD_SIZE_8BIT,pData,(uint16_t)16,1000)!= HAL_OK);
				 TxBufferSize-=16;
				 pData+=16;
				 MemAddress+=16;
			}
			//remaining data
			while(HAL_I2C_Mem_Write(hi2c,(uint16_t)DevAddress,(uint16_t)MemAddress,I2C_MEMADD_SIZE_8BIT,pData,(uint16_t)TxBufferSize,1000)!= HAL_OK);
	}
	return 1;
}
