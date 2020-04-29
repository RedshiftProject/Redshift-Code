/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "ff.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
const static nmeaPacket nmea_msg_patron;
nmeaPacket nmea_msg;
ubxPacket ubx_msg;
uint8_t uart_raw[512];
uint8_t good=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//UART RX Complete callback function
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  //HAL_UART_Transmit(&huart1,(uint8_t*)"RX callback\n",12,HAL_MAX_DELAY);
  //If it's the UART port connected to the GPS
  if(huart == &huart2)
  {
    if(uart_raw[0] == 0xB5)
    {
      UBX_parser(huart,uart_raw,&ubx_msg);
      HAL_UART_Receive_IT(&huart2,uart_raw,1);
    }else if(uart_raw[0] == '$')
    {
      //If end of frame has arrived (Frame ends with <LF> - 0x0A)
      if(uart_raw[nmea_msg.counter] == 0x0A)
      {
        good = 1;
        nmea_msg.counter++;
      }else{ //Keep receiving until we get <LF>
        ///TODO: La gestion de packet nmea
        nmea_msg.counter++;
        HAL_UART_Receive_IT(huart, &uart_raw[nmea_msg.counter], 1);    
      }
    }
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  //MX_FATFS_Init();
  MX_CAN_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
    // Activate GPIOC, might be better in gpio.c
    __HAL_RCC_GPIOC_CLK_ENABLE();

    //Create a gpio struct for pin C13 
    GPIO_InitTypeDef GPIO_InitStructure;
 
    GPIO_InitStructure.Pin = GPIO_PIN_13;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC,&GPIO_InitStructure);

    //Array for uart output
    //uartString needs to be predefined as array otherwise (i.e. as pointer) chip crashes 
    //uint8_t uartString[17];

    //I2C Stuff

    //MPU6050 Stuff
    //uint8_t acc_z[2]={0,0};    
    //wakeup[0] Register address for power management of MPU6050
    //wakeup[1] Register value to apply @address wakeup[0] (Sleep->0)
    //uint8_t wakeup[2];
    //wakeup[0]=MPU_6050_PWR_MGMT;
    //wakeup[1]=0x00;
    //MPU6050 : Verify if ready
    //if(HAL_I2C_IsDeviceReady(&hi2c1,MPU_6050_ADD,2,HAL_MAX_DELAY) != HAL_OK)
      //If not ready send message through UART
      //HAL_UART_Transmit(&huart1,(uint8_t *)"Not Okay Ready",14,HAL_MAX_DELAY);
    //MPU6050 : Configure POWER_MGMT register to power it up
    //if(HAL_I2C_Master_Transmit(&hi2c1,MPU_6050_ADD,(uint8_t *)wakeup,2,HAL_MAX_DELAY) != HAL_OK)
      //If not okay send message through UART
      //HAL_UART_Transmit(&huart1,(uint8_t *)"Not Okay Power",14,HAL_MAX_DELAY);

    //MPU 6050 Acceleromter config (2g precision)
    //config[0] Register address for accelerometer precision of MPU6050
    //config[1] Register value to apply @address config[0] (2g precision)
    //uint8_t config[2];
    //config[0]=MPU_6050_ACC_CONF;
    //config[1]=0x00;
    //Send config via i2c
    //if(HAL_I2C_Master_Transmit(&hi2c1,MPU_6050_ADD,(uint8_t *)config,2,HAL_MAX_DELAY) != HAL_OK)
      //If config not okay send message through UART
      //HAL_UART_Transmit(&huart1,(uint8_t *)"Not Okay Conf",14,HAL_MAX_DELAY);
    HAL_Delay(100);
    //FatFs Stuff
    static FATFS g_sFatFs;
    FRESULT fresult;
    FIL file;
    UINT bytes_written;
    
    //GPS STUFF
    //HAL_UART_Receive_IT(&huart2,&uart_raw[0],1);
    HAL_UART_Transmit_DMA(&huart1,"Hello World !\n",15);

    //mount SD card 
    fresult = f_mount(&g_sFatFs, "0", 1);       
    HAL_Delay(50);
    //open file on SD card
    fresult = f_open(&file, "file.txt", FA_OPEN_APPEND | FA_WRITE);
    HAL_Delay(50);
    //write data to the file
    fresult = f_write(&file, "Hello World!\r\n", 14, &bytes_written);
    HAL_Delay(50);
    //close file
    fresult = f_close (&file);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    //GPIO stuff
    //Turn on PINC13 / Reset = ON because of pullup on board
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
    HAL_Delay(500);
    //UART Stuff
    //HAL_UART_Transmit(&huart1,(unsigned char*)"Hello World ! Tx\n",17,100);
    //HAL_UART_Receive(&huart1,uartString,17,2000);
    //HAL_UART_Transmit(&huart1,uartString,17,100);

    //I2C stuff
    /** 
     * Sky note : Master transmit then master receive doesn't work on mpu6050 because of 
     * a stop bit at the end of transmit and/or a non existing register pointer on mpu6050 (maybe ?)
     * Solution : Use HAL_I2C_Mem_Read() to read registers
     */
    //Start with register ACCEL_ZOUT[15:8] (accelerometer on Z axis)
    //if(HAL_I2C_Master_Transmit(&hi2c1,MPU_6050_ADD,(uint8_t *)MPU_6050_ACC_Z_H,1,HAL_MAX_DELAY) != HAL_OK)
    //  HAL_UART_Transmit(&huart1,(uint8_t *)"Not Okay TX",11,HAL_MAX_DELAY);
    //else{
      //Read Z acceleration (2 bytes) register from MPU6050 and put it in acc_z
      //if(HAL_I2C_Mem_Read(&hi2c1,MPU_6050_ADD,MPU_6050_ACC_Z_H,I2C_MEMADD_SIZE_8BIT,acc_z,2,HAL_MAX_DELAY))
      //if(HAL_I2C_Master_Receive(&hi2c1,MPU_6050_ADD,(uint8_t *)acc_z,2,HAL_MAX_DELAY) != HAL_OK)
        //If read fails, send message through UART
        //HAL_UART_Transmit(&huart1,(uint8_t *)"Not Okay RX",11,HAL_MAX_DELAY);
      //else{
        //If it works send z acceleration value through UART
        //HAL_UART_Transmit(&huart1,(uint8_t *)"acc_z: ",7,HAL_MAX_DELAY);
        //HAL_UART_Transmit(&huart1,(uint8_t *)acc_z,2,HAL_MAX_DELAY);
      //}
    //}
    //Send a CR/LF to end line as a delimiter
    //HAL_UART_Transmit(&huart1,(uint8_t *)"\n",1,HAL_MAX_DELAY);
    //Turn off PINC13 / Set = OFF because of pullup on board
    //HAL_UART_Transmit(&huart1,(uint8_t *)"Debug\n",6,HAL_MAX_DELAY);

    //GPS stuff
    if(good)
    {
      good=0;
      #ifdef DEBUG_NEED
      //Output to "debug" serial
      HAL_UART_Transmit_DMA(&huart1,&uart_raw[0],nmea_msg.counter);
      #endif
      ///TODO: La gestion de packet nmea
      NMEA_parser(uart_raw,&nmea_msg);
      nmea_msg = nmea_msg_patron;
      HAL_UART_Receive_IT(&huart2,&uart_raw[0],1);
    }

    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
    HAL_Delay(500);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
