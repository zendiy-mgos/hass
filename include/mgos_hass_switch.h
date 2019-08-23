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
 * Home Assistant | Switches API.
 */

#ifndef MGOS_HASS_SWITCH_H
#define MGOS_HASS_SWITCH_H

#include <stdbool.h>
#include "mg_ha_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HA_SWITCH_TYPE "switch"

/* 
 * Base configuration settings for switches.
 * Note: if you change the struct definition,
 * you must update the macro <MK_HA_SWITCH_CFG>, and
 * the function mjs_ha_switch_cfg_set(...) as well.
 */
#define MK_HA_SWITCH_CFG(void) {MK_HA_BSTATE_CFG(), OFF, 0}
typedef struct ha_switch_cfg {
  ha_bstate_cfg_t state_cfg;
  enum ha_toggle_state normal_state;
  int inching_timeout;    
} ha_switch_cfg_t;

typedef struct ha_switch_props {
  enum ha_toggle_state normal_state;
  ha_bstate_t state_payload;
  int inching_timeout;
} ha_switch_props_t;

/*
 * Representation of a switch.
 * 
 * Inherits from
 *   - hass/common_h/ha_base_sensor_t
 *   - hass/common_h/ha_base_actuator_t
 * Inherited from
 *   - hass-mqtt/switch_c/ha_mqtt_switch_t
 */
typedef struct ha_switch {
  HA_ENTITY_HANDLE handle;
  ha_base_sensor_props_t sens_props;
  /* above inherited from 'ha_base_sensor_t' */
  ha_base_actuator_props_t actu_props;
  /* above inherited from 'ha_base_actuator_t' */
  ha_entity_xstate_history_t state_history;
  ha_switch_props_t switch_props;
} ha_switch_t;

#define GET_HA_SWITCH_T(h) ((ha_switch_t *)(h != NULL ? ((HA_ENTITY_HANDLE_EX)h)->instance : NULL))

/*
 * Handler function type to handle the "state_get"
 * event on switchs.
 */
typedef bool (*ha_switch_state_set_lambda)(HA_ENTITY_HANDLE handle,
  enum ha_toggle_state state, void *user_data);

/*
 * Handler function type to handle "state_get" event.
 */
typedef bool (*ha_switch_state_get_lambda)(HA_ENTITY_HANDLE handle,
                                           HA_ENTITY_BSTATE state,
                                           void *user_data);

/*
 * Register the "state_set" event handler.
 */
bool mgos_hass_switch_on_state_set(HA_ENTITY_HANDLE handle,
                                   ha_switch_state_set_lambda handler,
                                   void *user_data);

/*
 * Retrieve the state of a switch.
 */
ha_entity_bstate_t *mgos_hass_switch_state_get(HA_ENTITY_HANDLE handle);

/*
 * Register the "state_set" event handler.
 */
bool mgos_hass_switch_on_state_get(HA_ENTITY_HANDLE handle,
                                   ha_switch_state_get_lambda handler,
                                   void *user_data);

/*
 * Set the state of a switch.
 */
bool mgos_hass_switch_state_set(HA_ENTITY_HANDLE handle,
                                enum ha_toggle_state state);

/*
 * Toggle the state of a switch.
 */
enum ha_toggle_state mgos_hass_switch_state_toggle(HA_ENTITY_HANDLE handle);

/*
 * Helper function that must be invoked
 * during switch entity creation.
 */
bool mg_ha_switch_creating(ha_switch_t *e,
                           ha_entity_cfg_t *entity_cfg,
                           ha_base_pub_cfg_t *pub_cfg,
                           ha_switch_cfg_t *cfg);

/*
 * Helper function that must be invoked
 * during switch closure.
 */
void mg_ha_switch_closing(ha_switch_t *e);

/*
 * Close and release a switch.
 */
extern void mgos_hass_switch_close(HA_ENTITY_HANDLE handle);

/* BEGIN - MJS helper functions */

void mjs_ha_switch_cfg_set(ha_switch_cfg_t *s,
                           const char *on, const char *off,
                           enum ha_toggle_state normal_state,
                           int inching_timeout);

/* END - MJS helper functions */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MGOS_HASS_SWITCH_H */