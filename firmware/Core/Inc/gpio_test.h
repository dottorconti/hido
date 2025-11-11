/**
  ******************************************************************************
  * @file           : gpio_test.h
  * @brief          : GPIO Pin Diagnostic Tool Header
  ******************************************************************************
  */

#ifndef __GPIO_TEST_H
#define __GPIO_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Function prototypes */
void GPIO_TestAllPins(void);
void GPIO_ContinuousTest(void);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_TEST_H */
