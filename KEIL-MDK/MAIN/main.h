/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics. 
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <nrfx.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nrf_delay.h"
#include "nrf_gpio.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* 当前硬件版本 */
#define SYS_HW_VERSION						SYS_HW_VERSION_V0_1_0

/* 支持的硬件版本 */
#define SYS_HW_VERSION_V0_1_0				1
#define SYS_HW_VERSION_V0_1_1				2

/* 支持的对象版本 */
#define INCLINOMETER_VERSION				200 	//倾角版本
#define COLLAPSE_VERSION					201 	//崩塌计版本

/* 软件版本 */
#define SYS_SW_MAIN_VERSION					0X00 	//软件主版本号
#define SYS_SW_SUB_VERSION					0X01 	//软件子版本号
#define SYS_SW_MODIFY_VERSION				0X0001 	//软件修改版本号

/* 硬件版本 */
#if (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_0)
#define SYS_HW_MAIN_VERSION					0X00 	//硬件主版本号
#define SYS_HW_SUB_VERSION					0X01 	//硬件子版本号
#define SYS_HW_MODIFY_VERSION				0X0000 	//硬件修改版本号
#elif (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_1)
#define SYS_HW_MAIN_VERSION					0X00
#define SYS_HW_SUB_VERSION					0X01
#define SYS_HW_MODIFY_VERSION				0X0001
#else
#define SYS_HW_MAIN_VERSION					0X00
#define SYS_HW_SUB_VERSION					0X00
#define SYS_HW_MODIFY_VERSION				0X0000
#endif

typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;

typedef enum
{
  SUCCESS = 0,
  ERROR = !SUCCESS
} ErrorStatus;

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#ifndef SOFTDEVICE_PRESENT
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x00000000) /* Base @ of Page 0, 4 Kbytes */ //master boot record(MBR)
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x00001000) /* Base @ of Page 1, 4 Kbytes */ //software device stack start
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x00002000) /* Base @ of Page 2, 4 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x00003000) /* Base @ of Page 3, 4 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x00004000) /* Base @ of Page 4, 4 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x00005000) /* Base @ of Page 5, 4 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x00006000) /* Base @ of Page 6, 4 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x00007000) /* Base @ of Page 7, 4 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x00008000) /* Base @ of Page 8, 4 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x00009000) /* Base @ of Page 9, 4 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x0000A000) /* Base @ of Page 10, 4 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x0000B000) /* Base @ of Page 11, 4 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x0000C000) /* Base @ of Page 12, 4 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x0000D000) /* Base @ of Page 13, 4 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x0000E000) /* Base @ of Page 14, 4 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x0000F000) /* Base @ of Page 15, 4 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x00010000) /* Base @ of Page 16, 4 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x00011000) /* Base @ of Page 17, 4 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x00012000) /* Base @ of Page 18, 4 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x00013000) /* Base @ of Page 19, 4 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x00014000) /* Base @ of Page 20, 4 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x00015000) /* Base @ of Page 21, 4 Kbytes */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x00016000) /* Base @ of Page 22, 4 Kbytes */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x00017000) /* Base @ of Page 23, 4 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x00018000) /* Base @ of Page 24, 4 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x00019000) /* Base @ of Page 25, 4 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x0001A000) /* Base @ of Page 26, 4 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x0001B000) /* Base @ of Page 27, 4 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x0001C000) /* Base @ of Page 28, 4 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x0001D000) /* Base @ of Page 29, 4 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x0001E000) /* Base @ of Page 30, 4 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x0001F000) /* Base @ of Page 31, 4 Kbytes */
#define ADDR_FLASH_PAGE_32    ((uint32_t)0x00020000) /* Base @ of Page 32, 4 Kbytes */
#define ADDR_FLASH_PAGE_33    ((uint32_t)0x00021000) /* Base @ of Page 33, 4 Kbytes */
#define ADDR_FLASH_PAGE_34    ((uint32_t)0x00022000) /* Base @ of Page 34, 4 Kbytes */
#define ADDR_FLASH_PAGE_35    ((uint32_t)0x00023000) /* Base @ of Page 35, 4 Kbytes */
#define ADDR_FLASH_PAGE_36    ((uint32_t)0x00024000) /* Base @ of Page 36, 4 Kbytes */
#define ADDR_FLASH_PAGE_37    ((uint32_t)0x00025000) /* Base @ of Page 37, 4 Kbytes */ //software device stack end
#endif

#define ADDR_FLASH_PAGE_38    ((uint32_t)0x00026000) /* Base @ of Page 38, 4 Kbytes */ //APP start
#define ADDR_FLASH_PAGE_39    ((uint32_t)0x00027000) /* Base @ of Page 39, 4 Kbytes */
#define ADDR_FLASH_PAGE_40    ((uint32_t)0x00028000) /* Base @ of Page 40, 4 Kbytes */
#define ADDR_FLASH_PAGE_41    ((uint32_t)0x00029000) /* Base @ of Page 41, 4 Kbytes */
#define ADDR_FLASH_PAGE_42    ((uint32_t)0x0002A000) /* Base @ of Page 42, 4 Kbytes */
#define ADDR_FLASH_PAGE_43    ((uint32_t)0x0002B000) /* Base @ of Page 43, 4 Kbytes */
#define ADDR_FLASH_PAGE_44    ((uint32_t)0x0002C000) /* Base @ of Page 44, 4 Kbytes */
#define ADDR_FLASH_PAGE_45    ((uint32_t)0x0002D000) /* Base @ of Page 45, 4 Kbytes */
#define ADDR_FLASH_PAGE_46    ((uint32_t)0x0002E000) /* Base @ of Page 46, 4 Kbytes */
#define ADDR_FLASH_PAGE_47    ((uint32_t)0x0002F000) /* Base @ of Page 47, 4 Kbytes */
#define ADDR_FLASH_PAGE_48    ((uint32_t)0x00030000) /* Base @ of Page 48, 4 Kbytes */
#define ADDR_FLASH_PAGE_49    ((uint32_t)0x00031000) /* Base @ of Page 49, 4 Kbytes */
#define ADDR_FLASH_PAGE_50    ((uint32_t)0x00032000) /* Base @ of Page 50, 4 Kbytes */
#define ADDR_FLASH_PAGE_51    ((uint32_t)0x00033000) /* Base @ of Page 51, 4 Kbytes */
#define ADDR_FLASH_PAGE_52    ((uint32_t)0x00034000) /* Base @ of Page 52, 4 Kbytes */
#define ADDR_FLASH_PAGE_53    ((uint32_t)0x00035000) /* Base @ of Page 53, 4 Kbytes */
#define ADDR_FLASH_PAGE_54    ((uint32_t)0x00036000) /* Base @ of Page 54, 4 Kbytes */
#define ADDR_FLASH_PAGE_55    ((uint32_t)0x00037000) /* Base @ of Page 55, 4 Kbytes */
#define ADDR_FLASH_PAGE_56    ((uint32_t)0x00038000) /* Base @ of Page 56, 4 Kbytes */
#define ADDR_FLASH_PAGE_57    ((uint32_t)0x00039000) /* Base @ of Page 57, 4 Kbytes */
#define ADDR_FLASH_PAGE_58    ((uint32_t)0x0003A000) /* Base @ of Page 58, 4 Kbytes */
#define ADDR_FLASH_PAGE_59    ((uint32_t)0x0003B000) /* Base @ of Page 59, 4 Kbytes */
#define ADDR_FLASH_PAGE_60    ((uint32_t)0x0003C000) /* Base @ of Page 60, 4 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0003D000) /* Base @ of Page 61, 4 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0003E000) /* Base @ of Page 62, 4 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0003F000) /* Base @ of Page 63, 4 Kbytes */
#define ADDR_FLASH_PAGE_64    ((uint32_t)0x00040000) /* Base @ of Page 64, 4 Kbytes */
#define ADDR_FLASH_PAGE_65    ((uint32_t)0x00041000) /* Base @ of Page 65, 4 Kbytes */
#define ADDR_FLASH_PAGE_66    ((uint32_t)0x00042000) /* Base @ of Page 66, 4 Kbytes */
#define ADDR_FLASH_PAGE_67    ((uint32_t)0x00043000) /* Base @ of Page 67, 4 Kbytes */
#define ADDR_FLASH_PAGE_68    ((uint32_t)0x00044000) /* Base @ of Page 68, 4 Kbytes */
#define ADDR_FLASH_PAGE_69    ((uint32_t)0x00045000) /* Base @ of Page 69, 4 Kbytes */
#define ADDR_FLASH_PAGE_70    ((uint32_t)0x00046000) /* Base @ of Page 70, 4 Kbytes */
#define ADDR_FLASH_PAGE_71    ((uint32_t)0x00047000) /* Base @ of Page 71, 4 Kbytes */
#define ADDR_FLASH_PAGE_72    ((uint32_t)0x00048000) /* Base @ of Page 72, 4 Kbytes */ //APP end
#define ADDR_FLASH_PAGE_73    ((uint32_t)0x00049000) /* Base @ of Page 73, 4 Kbytes */ //APP data start(system param storage sector)
#define ADDR_FLASH_PAGE_74    ((uint32_t)0x0004A000) /* Base @ of Page 74, 4 Kbytes */ //battery data storage sector
#define ADDR_FLASH_PAGE_75    ((uint32_t)0x0004B000) /* Base @ of Page 75, 4 Kbytes */
#define ADDR_FLASH_PAGE_76    ((uint32_t)0x0004C000) /* Base @ of Page 76, 4 Kbytes */
#define ADDR_FLASH_PAGE_77    ((uint32_t)0x0004D000) /* Base @ of Page 77, 4 Kbytes */
#define ADDR_FLASH_PAGE_78    ((uint32_t)0x0004E000) /* Base @ of Page 78, 4 Kbytes */
#define ADDR_FLASH_PAGE_79    ((uint32_t)0x0004F000) /* Base @ of Page 79, 4 Kbytes */
#define ADDR_FLASH_PAGE_80    ((uint32_t)0x00050000) /* Base @ of Page 80, 4 Kbytes */
#define ADDR_FLASH_PAGE_81    ((uint32_t)0x00051000) /* Base @ of Page 81, 4 Kbytes */
#define ADDR_FLASH_PAGE_82    ((uint32_t)0x00052000) /* Base @ of Page 82, 4 Kbytes */ //APP data end
#define ADDR_FLASH_PAGE_83    ((uint32_t)0x00053000) /* Base @ of Page 83, 4 Kbytes */ //APP update backup data start
#define ADDR_FLASH_PAGE_84    ((uint32_t)0x00054000) /* Base @ of Page 84, 4 Kbytes */
#define ADDR_FLASH_PAGE_85    ((uint32_t)0x00055000) /* Base @ of Page 85, 4 Kbytes */
#define ADDR_FLASH_PAGE_86    ((uint32_t)0x00056000) /* Base @ of Page 86, 4 Kbytes */
#define ADDR_FLASH_PAGE_87    ((uint32_t)0x00057000) /* Base @ of Page 87, 4 Kbytes */
#define ADDR_FLASH_PAGE_88    ((uint32_t)0x00058000) /* Base @ of Page 88, 4 Kbytes */
#define ADDR_FLASH_PAGE_89    ((uint32_t)0x00059000) /* Base @ of Page 89, 4 Kbytes */
#define ADDR_FLASH_PAGE_90    ((uint32_t)0x0005A000) /* Base @ of Page 90, 4 Kbytes */
#define ADDR_FLASH_PAGE_91    ((uint32_t)0x0005B000) /* Base @ of Page 91, 4 Kbytes */
#define ADDR_FLASH_PAGE_92    ((uint32_t)0x0005C000) /* Base @ of Page 92, 4 Kbytes */
#define ADDR_FLASH_PAGE_93    ((uint32_t)0x0005D000) /* Base @ of Page 93, 4 Kbytes */
#define ADDR_FLASH_PAGE_94    ((uint32_t)0x0005E000) /* Base @ of Page 94, 4 Kbytes */
#define ADDR_FLASH_PAGE_95    ((uint32_t)0x0005F000) /* Base @ of Page 95, 4 Kbytes */
#define ADDR_FLASH_PAGE_96    ((uint32_t)0x00060000) /* Base @ of Page 96, 4 Kbytes */
#define ADDR_FLASH_PAGE_97    ((uint32_t)0x00061000) /* Base @ of Page 97, 4 Kbytes */
#define ADDR_FLASH_PAGE_98    ((uint32_t)0x00062000) /* Base @ of Page 98, 4 Kbytes */
#define ADDR_FLASH_PAGE_99    ((uint32_t)0x00063000) /* Base @ of Page 99, 4 Kbytes */
#define ADDR_FLASH_PAGE_100   ((uint32_t)0x00064000) /* Base @ of Page 100, 4 Kbytes */
#define ADDR_FLASH_PAGE_101   ((uint32_t)0x00065000) /* Base @ of Page 101, 4 Kbytes */
#define ADDR_FLASH_PAGE_102   ((uint32_t)0x00066000) /* Base @ of Page 102, 4 Kbytes */
#define ADDR_FLASH_PAGE_103   ((uint32_t)0x00067000) /* Base @ of Page 103, 4 Kbytes */
#define ADDR_FLASH_PAGE_104   ((uint32_t)0x00068000) /* Base @ of Page 104, 4 Kbytes */
#define ADDR_FLASH_PAGE_105   ((uint32_t)0x00069000) /* Base @ of Page 105, 4 Kbytes */
#define ADDR_FLASH_PAGE_106   ((uint32_t)0x0006A000) /* Base @ of Page 106, 4 Kbytes */
#define ADDR_FLASH_PAGE_107   ((uint32_t)0x0006B000) /* Base @ of Page 107, 4 Kbytes */
#define ADDR_FLASH_PAGE_108   ((uint32_t)0x0006C000) /* Base @ of Page 108, 4 Kbytes */
#define ADDR_FLASH_PAGE_109   ((uint32_t)0x0006D000) /* Base @ of Page 109, 4 Kbytes */
#define ADDR_FLASH_PAGE_110   ((uint32_t)0x0006E000) /* Base @ of Page 110, 4 Kbytes */
#define ADDR_FLASH_PAGE_111   ((uint32_t)0x0006F000) /* Base @ of Page 111, 4 Kbytes */
#define ADDR_FLASH_PAGE_112   ((uint32_t)0x00070000) /* Base @ of Page 112, 4 Kbytes */
#define ADDR_FLASH_PAGE_113   ((uint32_t)0x00071000) /* Base @ of Page 113, 4 Kbytes */
#define ADDR_FLASH_PAGE_114   ((uint32_t)0x00072000) /* Base @ of Page 114, 4 Kbytes */
#define ADDR_FLASH_PAGE_115   ((uint32_t)0x00073000) /* Base @ of Page 115, 4 Kbytes */
#define ADDR_FLASH_PAGE_116   ((uint32_t)0x00074000) /* Base @ of Page 116, 4 Kbytes */
#define ADDR_FLASH_PAGE_117   ((uint32_t)0x00075000) /* Base @ of Page 117, 4 Kbytes */ //APP update backup data end

#ifndef SOFTDEVICE_PRESENT
#define ADDR_FLASH_PAGE_118   ((uint32_t)0x00076000) /* Base @ of Page 118, 4 Kbytes */	//udpate:boot sector start
#define ADDR_FLASH_PAGE_119   ((uint32_t)0x00077000) /* Base @ of Page 119, 4 Kbytes */
#define ADDR_FLASH_PAGE_120   ((uint32_t)0x00078000) /* Base @ of Page 120, 4 Kbytes */
#define ADDR_FLASH_PAGE_121   ((uint32_t)0x00079000) /* Base @ of Page 121, 4 Kbytes */
#define ADDR_FLASH_PAGE_122   ((uint32_t)0x0007A000) /* Base @ of Page 122, 4 Kbytes */
#define ADDR_FLASH_PAGE_123   ((uint32_t)0x0007B000) /* Base @ of Page 123, 4 Kbytes */
#define ADDR_FLASH_PAGE_124   ((uint32_t)0x0007C000) /* Base @ of Page 124, 4 Kbytes */
#define ADDR_FLASH_PAGE_125   ((uint32_t)0x0007D000) /* Base @ of Page 125, 4 Kbytes */ //udpate:boot sector end
#define ADDR_FLASH_PAGE_126   ((uint32_t)0x0007E000) /* Base @ of Page 126, 4 Kbytes */	//udpate:MBR parameter storage sector
#define ADDR_FLASH_PAGE_127   ((uint32_t)0x0007F000) /* Base @ of Page 127, 4 Kbytes */	//udpate:bootloader settings sector
#endif

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
