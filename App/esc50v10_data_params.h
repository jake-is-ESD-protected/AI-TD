/**
  ******************************************************************************
  * @file    esc50v10_data_params.h
  * @author  AST Embedded Analytics Research Platform
  * @date    Tue Oct 17 08:39:18 2023
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#ifndef ESC50V10_DATA_PARAMS_H
#define ESC50V10_DATA_PARAMS_H
#pragma once

#include "ai_platform.h"

/*
#define AI_ESC50V10_DATA_WEIGHTS_PARAMS \
  (AI_HANDLE_PTR(&ai_esc50v10_data_weights_params[1]))
*/

#define AI_ESC50V10_DATA_CONFIG               (NULL)


#define AI_ESC50V10_DATA_ACTIVATIONS_SIZES \
  { 29296, }
#define AI_ESC50V10_DATA_ACTIVATIONS_SIZE     (29296)
#define AI_ESC50V10_DATA_ACTIVATIONS_COUNT    (1)
#define AI_ESC50V10_DATA_ACTIVATION_1_SIZE    (29296)



#define AI_ESC50V10_DATA_WEIGHTS_SIZES \
  { 423764, }
#define AI_ESC50V10_DATA_WEIGHTS_SIZE         (423764)
#define AI_ESC50V10_DATA_WEIGHTS_COUNT        (1)
#define AI_ESC50V10_DATA_WEIGHT_1_SIZE        (423764)



#define AI_ESC50V10_DATA_ACTIVATIONS_TABLE_GET() \
  (&g_esc50v10_activations_table[1])

extern ai_handle g_esc50v10_activations_table[1 + 2];



#define AI_ESC50V10_DATA_WEIGHTS_TABLE_GET() \
  (&g_esc50v10_weights_table[1])

extern ai_handle g_esc50v10_weights_table[1 + 2];


#endif    /* ESC50V10_DATA_PARAMS_H */
