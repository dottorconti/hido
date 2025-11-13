/**
  ******************************************************************************
  * @file    flash_config.c
  * @brief   Flash memory configuration storage implementation
  ******************************************************************************
  */

#include "flash_config.h"
#include <string.h>

/* Private variables */
#ifdef USE_KEYBOARD_MODE
static KeyboardConfig_t g_config;
#else
static JoystickConfig_t g_config;
#endif

/* CRC32 lookup table */
static const uint32_t crc32_table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

/**
  * @brief  Calculate CRC32 checksum
  * @param  data: Pointer to data buffer
  * @param  length: Data length in bytes
  * @retval CRC32 checksum
  */
static uint32_t Calculate_CRC32(const uint8_t *data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFF;
    
    for (uint32_t i = 0; i < length; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    
    return ~crc;
}

/**
  * @brief  Load default configuration
  */
void FlashConfig_LoadDefaults(void)
{
    g_config.magic = CONFIG_MAGIC;
    g_config.version = CONFIG_VERSION;
    
#ifdef USE_KEYBOARD_MODE
    /* Default keyboard mapping for Player 1 (WASD + keys) */
    const uint8_t p1_defaults[17] = {
        0x04, 0x16, 0x1A, 0x08,  /* 0-3: A, S, W, D */
        0x14, 0x1B, 0x06, 0x19,  /* 4-7: Q, X, C, V */
        0x1E, 0x1F, 0x20, 0x21,  /* 8-11: 1, 2, 3, 4 */
        0x22, 0x23, 0x24, 0x25,  /* 12-15: 5, 6, 7, 8 */
        0x26                      /* 16: 9 */
    };
    
    /* Default keyboard mapping for Player 2 (Arrow keys + numpad) */
    const uint8_t p2_defaults[17] = {
        0x50, 0x51, 0x52, 0x4F,  /* 0-3: LEFT, DOWN, UP, RIGHT */
        0x59, 0x5A, 0x5B, 0x5C,  /* 4-7: Numpad 1, 2, 3, 4 */
        0x5D, 0x5E, 0x5F, 0x60,  /* 8-11: Numpad 5, 6, 7, 8 */
        0x61, 0x62, 0x63, 0x54,  /* 12-15: Numpad 9, 0, ., / */
        0x55                      /* 16: Numpad * */
    };
    
    const char *p1_names[17] = {"A", "S", "W", "D", "Q", "X", "C", "V", 
                                 "1", "2", "3", "4", "5", "6", "7", "8", "9"};
    const char *p2_names[17] = {"LEFT", "DOWN", "UP", "RIGHT", "Num1", "Num2", 
                                 "Num3", "Num4", "Num5", "Num6", "Num7", "Num8",
                                 "Num9", "Num0", "Num.", "Num/", "Num*"};
    
    for (int i = 0; i < MAX_PINS_PER_PLAYER; i++) {
        g_config.player1[i].silk_pin = i;
        g_config.player1[i].hid_keycode = p1_defaults[i];
        strncpy(g_config.player1[i].key_name, p1_names[i], 15);
        g_config.player1[i].key_name[15] = '\0';
        
        g_config.player2[i].silk_pin = i;
        g_config.player2[i].hid_keycode = p2_defaults[i];
        strncpy(g_config.player2[i].key_name, p2_names[i], 15);
        g_config.player2[i].key_name[15] = '\0';
    }
    
#else /* USE_JOYSTICK_MODE */
    /* Default joystick mapping */
    const uint8_t p1_defaults[17] = {
        JOY_FUNC_AXIS_LEFT, JOY_FUNC_AXIS_DOWN, JOY_FUNC_AXIS_UP, JOY_FUNC_AXIS_RIGHT,
        JOY_FUNC_BUTTON_1, JOY_FUNC_BUTTON_2, JOY_FUNC_BUTTON_3, JOY_FUNC_BUTTON_4,
        JOY_FUNC_BUTTON_5, JOY_FUNC_BUTTON_6, JOY_FUNC_BUTTON_7, JOY_FUNC_BUTTON_8,
        JOY_FUNC_BUTTON_9, JOY_FUNC_BUTTON_10, JOY_FUNC_BUTTON_11, JOY_FUNC_BUTTON_12,
        JOY_FUNC_BUTTON_13
    };
    
    const char *func_names[19] = {
        "Button 1", "Button 2", "Button 3", "Button 4", "Button 5", "Button 6",
        "Button 7", "Button 8", "Button 9", "Button 10", "Button 11", "Button 12",
        "Button 13", "Button 14", "UP", "DOWN", "LEFT", "RIGHT", "Disabled"
    };
    
    for (int i = 0; i < MAX_PINS_PER_PLAYER; i++) {
        /* Player 1 */
        g_config.player1[i].silk_pin = i;
        g_config.player1[i].joy_function = p1_defaults[i];
        strncpy(g_config.player1[i].func_name, func_names[p1_defaults[i]], 15);
        g_config.player1[i].func_name[15] = '\0';
        
        /* Player 2 (same as Player 1) */
        g_config.player2[i].silk_pin = i;
        g_config.player2[i].joy_function = p1_defaults[i];
        strncpy(g_config.player2[i].func_name, func_names[p1_defaults[i]], 15);
        g_config.player2[i].func_name[15] = '\0';
    }
#endif
    
    /* Calculate CRC */
    g_config.crc32 = Calculate_CRC32((uint8_t*)&g_config, sizeof(g_config) - 4);
}

/**
  * @brief  Validate configuration from flash
  * @retval 1 if valid, 0 if invalid
  */
uint8_t FlashConfig_IsValid(void)
{
    /* Check magic number */
    if (g_config.magic != CONFIG_MAGIC) {
        return 0;
    }
    
    /* Check version */
    if (g_config.version != CONFIG_VERSION) {
        return 0;
    }
    
    /* Verify CRC32 */
    uint32_t calculated_crc = Calculate_CRC32((uint8_t*)&g_config, sizeof(g_config) - 4);
    if (calculated_crc != g_config.crc32) {
        return 0;
    }
    
    return 1;
}

/**
  * @brief  Load configuration from flash
  * @retval HAL_OK if successful, HAL_ERROR otherwise
  */
HAL_StatusTypeDef FlashConfig_Load(void)
{
    /* Read configuration from flash */
    uint32_t *flash_ptr = (uint32_t*)CONFIG_FLASH_ADDR;
    uint32_t *config_ptr = (uint32_t*)&g_config;
    uint32_t words = sizeof(g_config) / 4;
    
    for (uint32_t i = 0; i < words; i++) {
        config_ptr[i] = flash_ptr[i];
    }
    
    /* Validate configuration */
    if (!FlashConfig_IsValid()) {
        /* Invalid configuration, load defaults */
        FlashConfig_LoadDefaults();
        return HAL_ERROR;
    }
    
    return HAL_OK;
}

/**
  * @brief  Save configuration to flash
  * @retval HAL_OK if successful, HAL_ERROR otherwise
  */
HAL_StatusTypeDef FlashConfig_Save(void)
{
    HAL_StatusTypeDef status;
    
    /* Update CRC32 */
    g_config.crc32 = Calculate_CRC32((uint8_t*)&g_config, sizeof(g_config) - 4);
    
    /* Unlock flash */
    HAL_FLASH_Unlock();
    
    /* Erase page */
    FLASH_EraseInitTypeDef erase_init;
    uint32_t page_error;
    
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = CONFIG_FLASH_ADDR;
    erase_init.NbPages = 1;
    
    status = HAL_FLASHEx_Erase(&erase_init, &page_error);
    if (status != HAL_OK) {
        HAL_FLASH_Lock();
        return HAL_ERROR;
    }
    
    /* Write configuration */
    uint32_t *config_ptr = (uint32_t*)&g_config;
    uint32_t words = sizeof(g_config) / 4;
    uint32_t flash_addr = CONFIG_FLASH_ADDR;
    
    for (uint32_t i = 0; i < words; i++) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flash_addr, config_ptr[i]);
        if (status != HAL_OK) {
            HAL_FLASH_Lock();
            return HAL_ERROR;
        }
        flash_addr += 4;
    }
    
    /* Lock flash */
    HAL_FLASH_Lock();
    
    return HAL_OK;
}

/**
  * @brief  Reset configuration to defaults and save
  * @retval HAL_OK if successful, HAL_ERROR otherwise
  */
HAL_StatusTypeDef FlashConfig_Reset(void)
{
    FlashConfig_LoadDefaults();
    return FlashConfig_Save();
}

/**
  * @brief  Get pointer to current configuration
  * @retval Pointer to configuration structure
  */
#ifdef USE_KEYBOARD_MODE
KeyboardConfig_t* FlashConfig_Get(void)
#else
JoystickConfig_t* FlashConfig_Get(void)
#endif
{
    return &g_config;
}
