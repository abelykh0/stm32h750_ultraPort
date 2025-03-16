/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32h7xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
  * @brief I2C MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hi2c->Instance==I2C3)
  {
    /* USER CODE BEGIN I2C3_MspInit 0 */

    /* USER CODE END I2C3_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C3;
    PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**I2C3 GPIO Configuration
    PC9     ------> I2C3_SDA
    PA8     ------> I2C3_SCL
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C3_CLK_ENABLE();
    /* USER CODE BEGIN I2C3_MspInit 1 */

    /* USER CODE END I2C3_MspInit 1 */

  }

}

/**
  * @brief I2C MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==I2C3)
  {
    /* USER CODE BEGIN I2C3_MspDeInit 0 */

    /* USER CODE END I2C3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C3_CLK_DISABLE();

    /**I2C3 GPIO Configuration
    PC9     ------> I2C3_SDA
    PA8     ------> I2C3_SCL
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);

    /* USER CODE BEGIN I2C3_MspDeInit 1 */

    /* USER CODE END I2C3_MspDeInit 1 */
  }

}

/**
  * @brief LTDC MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hltdc: LTDC handle pointer
  * @retval None
  */
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hltdc->Instance==LTDC)
  {
    /* USER CODE BEGIN LTDC_MspInit 0 */

    /* USER CODE END LTDC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLL3.PLL3M = 5;
    PeriphClkInitStruct.PLL3.PLL3N = 102;
    PeriphClkInitStruct.PLL3.PLL3P = 2;
    PeriphClkInitStruct.PLL3.PLL3Q = 20;
    PeriphClkInitStruct.PLL3.PLL3R = 50;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_LTDC_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**LTDC GPIO Configuration
    PC0     ------> LTDC_R5
    PA3     ------> LTDC_B5
    PA4     ------> LTDC_VSYNC
    PA5     ------> LTDC_R4
    PA6     ------> LTDC_G2
    PB0     ------> LTDC_R3
    PB1     ------> LTDC_R6
    PE11     ------> LTDC_G3
    PE12     ------> LTDC_B4
    PE13     ------> LTDC_DE
    PE14     ------> LTDC_CLK
    PE15     ------> LTDC_R7
    PB10     ------> LTDC_G4
    PB11     ------> LTDC_G5
    PD10     ------> LTDC_B3
    PC6     ------> LTDC_HSYNC
    PC7     ------> LTDC_G6
    PD3     ------> LTDC_G7
    PB8     ------> LTDC_B6
    PB9     ------> LTDC_B7
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USER CODE BEGIN LTDC_MspInit 1 */

    /* USER CODE END LTDC_MspInit 1 */

  }

}

/**
  * @brief LTDC MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hltdc: LTDC handle pointer
  * @retval None
  */
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* hltdc)
{
  if(hltdc->Instance==LTDC)
  {
    /* USER CODE BEGIN LTDC_MspDeInit 0 */

    /* USER CODE END LTDC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LTDC_CLK_DISABLE();

    /**LTDC GPIO Configuration
    PC0     ------> LTDC_R5
    PA3     ------> LTDC_B5
    PA4     ------> LTDC_VSYNC
    PA5     ------> LTDC_R4
    PA6     ------> LTDC_G2
    PB0     ------> LTDC_R3
    PB1     ------> LTDC_R6
    PE11     ------> LTDC_G3
    PE12     ------> LTDC_B4
    PE13     ------> LTDC_DE
    PE14     ------> LTDC_CLK
    PE15     ------> LTDC_R7
    PB10     ------> LTDC_G4
    PB11     ------> LTDC_G5
    PD10     ------> LTDC_B3
    PC6     ------> LTDC_HSYNC
    PC7     ------> LTDC_G6
    PD3     ------> LTDC_G7
    PB8     ------> LTDC_B6
    PB9     ------> LTDC_B7
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_6|GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_8|GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_10|GPIO_PIN_3);

    /* USER CODE BEGIN LTDC_MspDeInit 1 */

    /* USER CODE END LTDC_MspDeInit 1 */
  }

}

/**
  * @brief QSPI MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hqspi: QSPI handle pointer
  * @retval None
  */
void HAL_QSPI_MspInit(QSPI_HandleTypeDef* hqspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hqspi->Instance==QUADSPI)
  {
    /* USER CODE BEGIN QUADSPI_MspInit 0 */

    /* USER CODE END QUADSPI_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_QSPI;
    PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_D1HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**QUADSPI GPIO Configuration
    PB2     ------> QUADSPI_CLK
    PE7     ------> QUADSPI_BK2_IO0
    PE8     ------> QUADSPI_BK2_IO1
    PE9     ------> QUADSPI_BK2_IO2
    PE10     ------> QUADSPI_BK2_IO3
    PC11     ------> QUADSPI_BK2_NCS
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* USER CODE BEGIN QUADSPI_MspInit 1 */

    /* USER CODE END QUADSPI_MspInit 1 */

  }

}

/**
  * @brief QSPI MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hqspi: QSPI handle pointer
  * @retval None
  */
void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* hqspi)
{
  if(hqspi->Instance==QUADSPI)
  {
    /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

    /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();

    /**QUADSPI GPIO Configuration
    PB2     ------> QUADSPI_CLK
    PE7     ------> QUADSPI_BK2_IO0
    PE8     ------> QUADSPI_BK2_IO1
    PE9     ------> QUADSPI_BK2_IO2
    PE10     ------> QUADSPI_BK2_IO3
    PC11     ------> QUADSPI_BK2_NCS
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_11);

    /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

    /* USER CODE END QUADSPI_MspDeInit 1 */
  }

}

/**
  * @brief SD MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hsd: SD handle pointer
  * @retval None
  */
void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hsd->Instance==SDMMC1)
  {
    /* USER CODE BEGIN SDMMC1_MspInit 0 */

    /* USER CODE END SDMMC1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDMMC;
    PeriphClkInitStruct.PLL2.PLL2M = 5;
    PeriphClkInitStruct.PLL2.PLL2N = 96;
    PeriphClkInitStruct.PLL2.PLL2P = 2;
    PeriphClkInitStruct.PLL2.PLL2Q = 2;
    PeriphClkInitStruct.PLL2.PLL2R = 10;
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
    PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_SDMMC1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDMMC1 GPIO Configuration
    PC8     ------> SDMMC1_D0
    PC12     ------> SDMMC1_CK
    PD2     ------> SDMMC1_CMD
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* SDMMC1 interrupt Init */
    HAL_NVIC_SetPriority(SDMMC1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
    /* USER CODE BEGIN SDMMC1_MspInit 1 */

    /* USER CODE END SDMMC1_MspInit 1 */

  }

}

/**
  * @brief SD MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hsd: SD handle pointer
  * @retval None
  */
void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{
  if(hsd->Instance==SDMMC1)
  {
    /* USER CODE BEGIN SDMMC1_MspDeInit 0 */

    /* USER CODE END SDMMC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDMMC1_CLK_DISABLE();

    /**SDMMC1 GPIO Configuration
    PC8     ------> SDMMC1_D0
    PC12     ------> SDMMC1_CK
    PD2     ------> SDMMC1_CMD
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDMMC1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(SDMMC1_IRQn);
    /* USER CODE BEGIN SDMMC1_MspDeInit 1 */

    /* USER CODE END SDMMC1_MspDeInit 1 */
  }

}

/**
  * @brief SPI MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(hspi->Instance==SPI4)
  {
    /* USER CODE BEGIN SPI4_MspInit 0 */

    /* USER CODE END SPI4_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI4;
    PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_SPI4_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**SPI4 GPIO Configuration
    PE2     ------> SPI4_SCK
    PE4     ------> SPI4_NSS
    PE6     ------> SPI4_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* USER CODE BEGIN SPI4_MspInit 1 */

    /* USER CODE END SPI4_MspInit 1 */

  }

}

/**
  * @brief SPI MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI4)
  {
    /* USER CODE BEGIN SPI4_MspDeInit 0 */

    /* USER CODE END SPI4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI4_CLK_DISABLE();

    /**SPI4 GPIO Configuration
    PE2     ------> SPI4_SCK
    PE4     ------> SPI4_NSS
    PE6     ------> SPI4_MOSI
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_6);

    /* USER CODE BEGIN SPI4_MspDeInit 1 */

    /* USER CODE END SPI4_MspDeInit 1 */
  }

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
