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
 * Home Assistant | Sensors API.
 */

#ifndef MGOS_HASS_SENSOR_H
#define MGOS_HASS_SENSOR_H

#include <stdbool.h>
#include "mg_ha_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HA_SENSOR_TYPE "sensor"

/*
 * Representation of a sensor.
 * 
 * Inherits from
 *   - hass/common_h/ha_base_sensor_t
 * Inherited from
 *   - hass-mqtt/sensor_c/ha_mqtt_sensor_t
 */
typedef struct ha_sensor {
  HA_ENTITY_HANDLE handle;
  ha_base_sensor_props_t sens_props;
  /* above inherited from 'ha_base_sensor_t' */
  ha_entity_xstate_history_t state_history;
} ha_sensor_t;

#define GET_HA_SENSOR_T(h) ((ha_sensor_t *)(h != NULL ? ((HA_ENTITY_HANDLE_EX)h)->instance : NULL))

/*
 * Handler function type to handle the "state_get"
 * event on sensors.
 */
typedef bool (*ha_sensor_state_get_lambda)(HA_ENTITY_HANDLE handle,
                                           HA_ENTITY_XSTATE state,
                                           void *user_data);

/*
 * Register the "state_get" event handler.
 */
bool mgos_hass_sensor_on_state_get(HA_ENTITY_HANDLE handle,
                                   ha_sensor_state_get_lambda handler,
                                   void *user_data);

/*
 * Retrieve the state of a sensor.
 */
ha_entity_xstate_t *mgos_hass_sensor_state_get(HA_ENTITY_HANDLE handle);

/*
 * Helper function that must be invoked
 * during sensor creation.
 */
bool mg_ha_sensor_creating(ha_sensor_t *e,
                           ha_entity_cfg_t *entity_cfg,
                           ha_base_pub_cfg_t *pub_cfg);

/*
 * Helper function that must be invoked
 * during sensor closure.
 */
void mg_ha_sensor_closing(ha_sensor_t *e);

/*
 * Close and release a sensor.
 */
extern void mgos_hass_sensor_close(HA_ENTITY_HANDLE handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MGOS_HASS_SENSOR_H */