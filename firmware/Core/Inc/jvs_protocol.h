/**
  ******************************************************************************
  * @file           : jvs_protocol.h
  * @brief          : JVS (Jamma Video Standard) Protocol Implementation
  ******************************************************************************
  * @attention
  *
  * JVS Protocol for arcade cabinets over RS485
  * Compatible with NAOMI, Triforce, Chihiro, Lindbergh, etc.
  *
  * Based on JVS specification 3.0 and inspired by OpenJVS project:
  * https://github.com/OpenJVS/OpenJVS (GPL-3.0 License)
  * See CREDITS.md for full attribution and license information.
  *
  ******************************************************************************
  */

#ifndef __JVS_PROTOCOL_H
#define __JVS_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* JVS Configuration */
#define JVS_MAX_PACKET_SIZE     255
#define JVS_TIMEOUT_MS          200

/* JVS Special Bytes */
#define JVS_SYNC                0xE0
#define JVS_ESCAPE              0xD0
#define JVS_BROADCAST           0xFF
#define JVS_MASTER_ADDR         0x00

/* JVS Commands */
#define JVS_CMD_RESET           0xF0
#define JVS_CMD_RESET_ARG       0xD9
#define JVS_CMD_ASSIGN_ADDR     0xF1
#define JVS_CMD_REQUEST_ID      0x10
#define JVS_CMD_CMD_VER         0x11
#define JVS_CMD_JVS_VER         0x12
#define JVS_CMD_COMM_VER        0x13
#define JVS_CMD_CAPABILITIES    0x14
#define JVS_CMD_CONVEY_ID       0x15
#define JVS_CMD_READ_SWITCHES   0x20
#define JVS_CMD_READ_COINS      0x21
#define JVS_CMD_READ_ANALOG     0x22
#define JVS_CMD_RETRANSMIT      0x2F
#define JVS_CMD_DECREASE_COIN   0x30
#define JVS_CMD_WRITE_COIN      0x35

/* JVS Status */
#define JVS_STATUS_SUCCESS      0x01
#define JVS_STATUS_UNSUPPORTED  0x02
#define JVS_STATUS_CHECKSUM_ERR 0x03
#define JVS_STATUS_OVERFLOW     0x04

/* JVS Report */
#define JVS_REPORT_SUCCESS      0x01
#define JVS_REPORT_PARAM_ERROR  0x02
#define JVS_REPORT_BUSY         0x04

/* JVS Capabilities */
#define JVS_CAP_END             0x00
#define JVS_CAP_PLAYERS         0x01
#define JVS_CAP_COINS           0x02
#define JVS_CAP_ANALOG          0x03
#define JVS_CAP_ROTARY          0x04
#define JVS_CAP_KEYPAD          0x05
#define JVS_CAP_LIGHTGUN        0x06
#define JVS_CAP_GPI             0x07

/* Board Configuration - CUSTOMIZE THIS */
#define JVS_BOARD_NAME          "HIDO Arcade Controller"
#define JVS_CMD_VERSION         0x11    // Command version 1.1
#define JVS_JVS_VERSION         0x30    // JVS version 3.0
#define JVS_COMM_VERSION        0x10    // Comm version 1.0
#define JVS_NUM_PLAYERS         2       // 2 players
#define JVS_BUTTONS_PER_PLAYER  8       // 8 buttons per player (+ start + service)
#define JVS_NUM_COINS           2       // 2 coin slots

/* JVS Sense Line Control (PA2) */
#define JVS_SENSE_PORT          GPIOA
#define JVS_SENSE_PIN           GPIO_PIN_2

/* JVS Packet Structure */
typedef struct {
    uint8_t destination;
    uint8_t length;
    uint8_t data[JVS_MAX_PACKET_SIZE];
} JVS_Packet_t;

/* JVS IO State */
typedef struct {
    uint8_t device_id;
    uint16_t player_switches[JVS_NUM_PLAYERS + 1];  // +1 for system switches
    uint16_t coin_count[JVS_NUM_COINS];
    bool initialized;
} JVS_State_t;

/* Function Prototypes */
void JVS_Init(void);
void JVS_ProcessPackets(void);
void JVS_SetSenseLine(bool state);
void JVS_UpdateInputs(void);

/* Input mapping functions */
void JVS_SetSwitch(uint8_t player, uint8_t button, bool pressed);
void JVS_IncrementCoin(uint8_t slot);

#ifdef __cplusplus
}
#endif

#endif /* __JVS_PROTOCOL_H */
