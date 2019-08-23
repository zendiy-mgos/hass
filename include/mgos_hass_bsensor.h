/*
 * Copyright (c) 2018 ZenDIY
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Home Assistant | Binary sensors API.
 */

#ifndef MGOS_HASS_BINARY_SENSOR_H
#define MGOS_HASS_BINARY_SENSOR_H

#include <stdbool.h>
#include "mg_ha_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HA_BSENSOR_TYPE "binary_sensor"

/* 
 * Base configuration settings for binary sensors.
 * Note: if you change the struct definition,
 * you must update the macro <MK_HA_BSENSOR_CFG>, and
 * the function mjs_ha_bsensor_cfg_set(...) as well.
 */
#define MK_HA_BSENSOR_CFG(void) {MK_HA_BSTATE_CFG()}
typedef struct ha_bsensor_cfg {
  ha_bstate_cfg_t state_cfg;
} ha_bsensor_cfg_t;

typedef struct ha_bsensor_props {
  ha_bstate_t state_payload;
} ha_bsensor_props_t;

/*
 * Representation of a binary sensor.
 * 
 * Inherits from
 *   - hass/common_h/ha_base_sensor_t
 * Inherited from
 *   - hass-mqtt/bsensor_c/ha_mqtt_bsensor_t
 */
typedef struct ha_bsensor {
  HA_ENTITY_HANDLE handle;
  ha_base_sensor_props_t sens_props;
  /* above inherited from 'ha_base_sensor_t' */
  ha_entity_xstate_history_t state_history;
  ha_bsensor_props_t bsensor_props;
} ha_bsensor_t;

#define GET_HA_BSENSOR_T(h) ((ha_bsensor_t *)(h != NULL ? ((HA_ENTITY_HANDLE_EX)h)->instance : NULL))

/*
 * Handler function type to handle the "state_get"
 * event on binary sensors.
 */
typedef bool (*ha_bsensor_state_get_lambda)(HA_ENTITY_HANDLE handle,
                                            HA_ENTITY_BSTATE state,
                                            void *user_data);

/*
 * Register the "state_get" event handler.
 */
bool mgos_hass_bsensor_on_state_get(HA_ENTITY_HANDLE handle,
                                    ha_bsensor_state_get_lambda handler,
                                    void *user_data);

/*
 * Retrieve the state of a binary sensor.
 */
ha_entity_bstate_t *mgos_hass_bsensor_state_get(HA_ENTITY_HANDLE handle);

/*
 * Helper function that must be invoked
 * during binary sensor entity creation.
 */
bool mg_ha_bsensor_creating(ha_bsensor_t *e,
                            ha_entity_cfg_t *entity_cfg,
                            ha_base_pub_cfg_t *pub_cfg,
                            ha_bsensor_cfg_t *cfg);

/*
 * Helper function that must be invoked
 * during binary sensor closure.
 */
void mg_ha_bsensor_closing(ha_bsensor_t *e);

/*
 * Close and release a binary sensor.
 */
extern void mgos_hass_bsensor_close(HA_ENTITY_HANDLE handle);

/* BEGIN - MJS helper functions */

void mjs_ha_bsensor_cfg_set(ha_bsensor_cfg_t *s,
                            const char *on, const char *off);

/* END - MJS helper functions */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MGOS_HASS_BINARY_SENSOR_H */