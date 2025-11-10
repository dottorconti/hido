/**
  ******************************************************************************
  * @file           : jvs_protocol.c
  * @brief          : JVS Protocol Implementation
  ******************************************************************************
  * @attention
  *
  * JVS over RS485 with SN65HVD1786D transceiver
  * USART1: PA9 (TX), PA8 (RX), PA10 (Sense Line)
  *
  * This implementation is based on the JVS protocol specification and
  * inspired by the OpenJVS project by Bobby Dilley:
  * https://github.com/OpenJVS/OpenJVS
  *
  * OpenJVS License: GPL-3.0
  * HIDO Project License: MIT (see CREDITS.md for details)
  *
  * The code has been completely rewritten for STM32 embedded systems
  * with hardware-specific optimizations for arcade button scanning.
  *
  ******************************************************************************
  */

#include "jvs_protocol.h"
#include "usart.h"
#include <string.h>

/* External UART handle */
extern UART_HandleTypeDef huart1;

/* JVS State */
static JVS_State_t jvs_state = {0};
static JVS_Packet_t rx_packet = {0};
static JVS_Packet_t tx_packet = {0};

/* Reception buffer */
static uint8_t rx_buffer[JVS_MAX_PACKET_SIZE];
static uint16_t rx_index = 0;
static bool escape_next = false;

/**
  * @brief  Initialize JVS system
  */
void JVS_Init(void)
{
    /* Initialize state */
    memset(&jvs_state, 0, sizeof(JVS_State_t));
    jvs_state.device_id = 0xFF;  // Not assigned yet
    jvs_state.initialized = false;
    
    /* Set sense line to floating (input mode initially) */
    JVS_SetSenseLine(false);
    
    /* Clear buffers */
    memset(&rx_packet, 0, sizeof(JVS_Packet_t));
    memset(&tx_packet, 0, sizeof(JVS_Packet_t));
    rx_index = 0;
    escape_next = false;
}

/**
  * @brief  Control JVS Sense Line
  * @param  state: true = 2.5V (ready), false = floating (waiting)
  */
void JVS_SetSenseLine(bool state)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    if (state) {
        /* Output mode - drive to 2.5V via resistor divider */
        GPIO_InitStruct.Pin = JVS_SENSE_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(JVS_SENSE_PORT, &GPIO_InitStruct);
        HAL_GPIO_WritePin(JVS_SENSE_PORT, JVS_SENSE_PIN, GPIO_PIN_SET);
    } else {
        /* Input mode - floating */
        GPIO_InitStruct.Pin = JVS_SENSE_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(JVS_SENSE_PORT, &GPIO_InitStruct);
    }
}

/**
  * @brief  Calculate checksum
  */
static uint8_t JVS_CalcChecksum(uint8_t *data, uint16_t length)
{
    uint8_t sum = 0;
    for (uint16_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return sum;
}

/**
  * @brief  Escape special bytes for transmission
  */
static uint16_t JVS_EscapeData(uint8_t *src, uint16_t src_len, uint8_t *dst)
{
    uint16_t dst_idx = 0;
    
    dst[dst_idx++] = JVS_SYNC;
    
    for (uint16_t i = 0; i < src_len; i++) {
        if (src[i] == JVS_SYNC || src[i] == JVS_ESCAPE) {
            dst[dst_idx++] = JVS_ESCAPE;
            dst[dst_idx++] = src[i] - 1;
        } else {
            dst[dst_idx++] = src[i];
        }
    }
    
    return dst_idx;
}

/**
  * @brief  Send JVS packet
  */
static void JVS_SendPacket(JVS_Packet_t *packet)
{
    uint8_t temp_buffer[JVS_MAX_PACKET_SIZE * 2];
    uint8_t send_buffer[JVS_MAX_PACKET_SIZE * 2];
    uint16_t temp_len = 0;
    
    /* Build raw packet */
    temp_buffer[temp_len++] = packet->destination;
    temp_buffer[temp_len++] = packet->length + 1;  // +1 for checksum
    
    for (uint8_t i = 0; i < packet->length; i++) {
        temp_buffer[temp_len++] = packet->data[i];
    }
    
    /* Calculate and add checksum */
    uint8_t checksum = JVS_CalcChecksum(temp_buffer, temp_len);
    temp_buffer[temp_len++] = checksum;
    
    /* Escape special bytes */
    uint16_t send_len = JVS_EscapeData(temp_buffer, temp_len, send_buffer);
    
    /* Transmit */
    HAL_UART_Transmit(&huart1, send_buffer, send_len, JVS_TIMEOUT_MS);
}

/**
  * @brief  Process received byte
  */
static bool JVS_ProcessByte(uint8_t byte)
{
    static enum { WAIT_SYNC, GET_DEST, GET_LEN, GET_DATA, GET_CHK } state = WAIT_SYNC;
    static uint8_t expected_len = 0;
    static uint8_t checksum = 0;
    
    /* Handle SYNC */
    if (byte == JVS_SYNC && !escape_next) {
        state = GET_DEST;
        rx_index = 0;
        checksum = 0;
        return false;
    }
    
    /* Handle ESCAPE */
    if (byte == JVS_ESCAPE && !escape_next) {
        escape_next = true;
        return false;
    }
    
    /* Unescape byte */
    if (escape_next) {
        byte += 1;
        escape_next = false;
    }
    
    switch (state) {
        case WAIT_SYNC:
            break;
            
        case GET_DEST:
            rx_packet.destination = byte;
            checksum += byte;
            state = GET_LEN;
            break;
            
        case GET_LEN:
            expected_len = byte - 1;  // -1 for checksum
            checksum += byte;
            rx_packet.length = 0;
            state = (expected_len > 0) ? GET_DATA : GET_CHK;
            break;
            
        case GET_DATA:
            rx_packet.data[rx_packet.length++] = byte;
            checksum += byte;
            if (rx_packet.length >= expected_len) {
                state = GET_CHK;
            }
            break;
            
        case GET_CHK:
            if (checksum == byte) {
                state = WAIT_SYNC;
                return true;  // Packet complete!
            }
            state = WAIT_SYNC;  // Checksum error
            break;
    }
    
    return false;
}

/**
  * @brief  Handle JVS command packet
  */
static void JVS_HandlePacket(JVS_Packet_t *rx, JVS_Packet_t *tx)
{
    /* Check if packet is for us */
    if (rx->destination != JVS_BROADCAST && rx->destination != jvs_state.device_id) {
        return;  // Not for us
    }
    
    /* Setup response */
    tx->destination = JVS_MASTER_ADDR;
    tx->length = 0;
    tx->data[tx->length++] = JVS_STATUS_SUCCESS;
    
    uint8_t cmd_idx = 0;
    
    while (cmd_idx < rx->length) {
        uint8_t cmd = rx->data[cmd_idx];
        uint8_t cmd_size = 1;
        
        switch (cmd) {
            case JVS_CMD_RESET:
                /* Reset device */
                jvs_state.device_id = 0xFF;
                jvs_state.initialized = false;
                JVS_SetSenseLine(false);
                cmd_size = 2;
                break;
                
            case JVS_CMD_ASSIGN_ADDR:
                /* Assign device address */
                jvs_state.device_id = rx->data[cmd_idx + 1];
                jvs_state.initialized = true;
                JVS_SetSenseLine(true);
                tx->data[tx->length++] = JVS_REPORT_SUCCESS;
                cmd_size = 2;
                break;
                
            case JVS_CMD_REQUEST_ID:
                /* Send board ID string */
                tx->data[tx->length++] = JVS_REPORT_SUCCESS;
                strcpy((char*)&tx->data[tx->length], JVS_BOARD_NAME);
                tx->length += strlen(JVS_BOARD_NAME) + 1;
                break;
                
            case JVS_CMD_CMD_VER:
                /* Command version */
                tx->data[tx->length++] = JVS_REPORT_SUCCESS;
                tx->data[tx->length++] = JVS_CMD_VERSION;
                break;
                
            case JVS_CMD_JVS_VER:
                /* JVS version */
                tx->data[tx->length++] = JVS_REPORT_SUCCESS;
                tx->data[tx->length++] = JVS_JVS_VERSION;
                break;
                
            case JVS_CMD_COMM_VER:
                /* Communication version */
                tx->data[tx->length++] = JVS_REPORT_SUCCESS;
                tx->data[tx->length++] = JVS_COMM_VERSION;
                break;
                
            case JVS_CMD_CAPABILITIES:
                /* Send capabilities */
                tx->data[tx->length++] = JVS_REPORT_SUCCESS;
                
                /* Players capability */
                tx->data[tx->length++] = JVS_CAP_PLAYERS;
                tx->data[tx->length++] = JVS_NUM_PLAYERS;
                tx->data[tx->length++] = JVS_BUTTONS_PER_PLAYER;
                tx->data[tx->length++] = 0x00;
                
                /* Coin slots capability */
                tx->data[tx->length++] = JVS_CAP_COINS;
                tx->data[tx->length++] = JVS_NUM_COINS;
                tx->data[tx->length++] = 0x00;
                tx->data[tx->length++] = 0x00;
                
                /* End of capabilities */
                tx->data[tx->length++] = JVS_CAP_END;
                break;
                
            case JVS_CMD_READ_SWITCHES:
                /* Read switch inputs */
                tx->data[tx->length++] = JVS_REPORT_SUCCESS;
                
                /* System switches */
                tx->data[tx->length++] = jvs_state.player_switches[0];
                
                /* Player switches */
                uint8_t num_players = rx->data[cmd_idx + 1];
                uint8_t bytes_per_player = rx->data[cmd_idx + 2];
                
                for (uint8_t p = 0; p < num_players; p++) {
                    for (uint8_t b = 0; b < bytes_per_player; b++) {
                        tx->data[tx->length++] = (jvs_state.player_switches[p + 1] >> (8 * (bytes_per_player - 1 - b))) & 0xFF;
                    }
                }
                cmd_size = 3;
                break;
                
            case JVS_CMD_READ_COINS:
                /* Read coin counts */
                tx->data[tx->length++] = JVS_REPORT_SUCCESS;
                
                uint8_t num_coins = rx->data[cmd_idx + 1];
                for (uint8_t c = 0; c < num_coins; c++) {
                    tx->data[tx->length++] = (jvs_state.coin_count[c] >> 8) & 0x3F;  // High 6 bits
                    tx->data[tx->length++] = jvs_state.coin_count[c] & 0xFF;         // Low 8 bits
                }
                cmd_size = 2;
                break;
                
            case JVS_CMD_RETRANSMIT:
                /* Retransmit last packet - already in tx */
                return;
                
            default:
                /* Unsupported command */
                tx->data[0] = JVS_STATUS_UNSUPPORTED;
                tx->length = 1;
                return;
        }
        
        cmd_idx += cmd_size;
    }
}

/**
  * @brief  Update inputs from GPIO to JVS state
  */
void JVS_UpdateInputs(void)
{
    /* Map GPIO pins to JVS switches */
    /* System switches (Test, Service, etc.) */
    jvs_state.player_switches[0] = 0;
    
    // Test button (PB2)
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET) {
        jvs_state.player_switches[0] |= (1 << 7);  // Test
    }
    
    /* Player 1 switches */
    jvs_state.player_switches[1] = 0;
    
    // Start (PA7)
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_RESET) {
        jvs_state.player_switches[1] |= (1 << 7);
    }
    
    // Buttons PB3-PB12
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_RESET) jvs_state.player_switches[1] |= (1 << 0);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_RESET) jvs_state.player_switches[1] |= (1 << 1);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == GPIO_PIN_RESET) jvs_state.player_switches[1] |= (1 << 2);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET) jvs_state.player_switches[1] |= (1 << 3);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11) == GPIO_PIN_RESET) jvs_state.player_switches[1] |= (1 << 4);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET) jvs_state.player_switches[1] |= (1 << 5);
    
    /* Player 2 switches - add your mappings here */
    jvs_state.player_switches[2] = 0;
    
    // You can add more player mappings as needed
}

/**
  * @brief  Set individual switch state (alternative to UpdateInputs)
  */
void JVS_SetSwitch(uint8_t player, uint8_t button, bool pressed)
{
    if (player > JVS_NUM_PLAYERS) return;
    
    if (pressed) {
        jvs_state.player_switches[player] |= (1 << button);
    } else {
        jvs_state.player_switches[player] &= ~(1 << button);
    }
}

/**
  * @brief  Increment coin counter
  */
void JVS_IncrementCoin(uint8_t slot)
{
    if (slot < JVS_NUM_COINS) {
        if (jvs_state.coin_count[slot] < 16383) {  // Max coin count
            jvs_state.coin_count[slot]++;
        }
    }
}

/**
  * @brief  Process incoming JVS packets
  * Call this frequently in main loop
  */
void JVS_ProcessPackets(void)
{
    uint8_t byte;
    
    /* Check for received data */
    if (HAL_UART_Receive(&huart1, &byte, 1, 1) == HAL_OK) {
        /* Process byte */
        if (JVS_ProcessByte(byte)) {
            /* Complete packet received */
            JVS_HandlePacket(&rx_packet, &tx_packet);
            JVS_SendPacket(&tx_packet);
        }
    }
    
    /* Update inputs from GPIO */
    JVS_UpdateInputs();
}
