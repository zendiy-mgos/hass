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
 * Home Assistant | Common APIs, data types, and structures.
 */

#ifndef MG_HA_COMMON_H
#define MG_HA_COMMON_H

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include "mgos_timers.h"
#include "mgos_event.h"
#include "mg_ha_entity_state.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HA_SAFE_STRDUP(s) (s != NULL ? strdup(s) : NULL)

#define HASS_EVENT_BASE MGOS_EVENT_BASE('M', 'H', 'A')

enum mgos_hass_event {
  MGOS_HASS_EV_PUB_ON_CONN = HASS_EVENT_BASE,
  MGOS_HASS_EV_PUB_ON_INT,
  MGOS_HASS_EV_PUBLISHED,
};

#define HA_ENV_VAR_OBJ_ID "${object_id}"
#define HA_ENV_VAR_DEV_ID "${device_id}"
#define HA_ENV_VAR_STATE_ATTRIBS "${state.attribs}"

enum mgos_hass_pub_event {
  DEFAULT = 0,       /* 000 */
  ON_CONNECT = 1,    /* 001 */
  ON_INTERVAL = 2,   /* 010 */
  ON_ANY = 3,        /* 011 */
  NONE = 128         /* 10000000 */
};

#define HA_NULL_ENTITY_CFG(void) {NULL, NULL}
#define HA_ENTITY_CFG(object_id) {object_id, NULL}
#define HA_DEVICE_ENTITY_CFG(object_id, device_id) {object_id, device_id}

/*
 * Entity configuration
 * 
 * Note: if you change this structure, you must
 * update the function mjs_mk_ha_entity_cfg_t(...)
 * as well.
 */
typedef struct ha_entity_cfg {
  const char *object_id;
  const char *device_id;
} ha_entity_cfg_t;

/*
 * Entity HANDLE
 */
typedef struct ha_entity_handle {
  char *object_id;
  char *device_id;
  const char *entity_type;
} ha_entity_handle_t;

typedef ha_entity_handle_t* HA_ENTITY_HANDLE;

/*
 * Extended entity HANDLE
 */
typedef struct ha_entity_handle_ex {
  char *object_id;
  char *device_id;
  const char *entity_type;
  void *instance;
} ha_entity_handle_ex_t;

typedef ha_entity_handle_ex_t* HA_ENTITY_HANDLE_EX;

/*
 * Entity list item
 */
typedef struct ha_entity_list_item {
  HA_ENTITY_HANDLE handle;
  struct ha_entity_list_item *prev;
  struct ha_entity_list_item *next;
} ha_entity_list_item_t;

enum ha_pub_result {
  FAILED = 0,
  SUCCESS = 1,
  LAZY = 3
};

#define MK_HA_ENTITY_PUB_RES(h) {h, FAILED, NONE}
typedef struct ha_entity_pub_res {
  HA_ENTITY_HANDLE handle;
  enum ha_pub_result result;
  enum mgos_hass_pub_event pub_ev;
} ha_entity_pub_res_t;

/*
 * Handler function type to publish entity's state.
 */
typedef enum ha_pub_result (*ha_entity_state_pub_lambda)(HA_ENTITY_HANDLE handle);

/* 
 * Base configuration settings for publishing.
 * Note: if you change the struct definition,
 * you must update the macro <MK_HA_BASE_PUB_CFG>, and
 * the function mjs_ha_base_pub_cfg_set(...) as well.
 */
#define MK_HA_BASE_PUB_CFG(void) {DEFAULT, 0, true}
typedef struct ha_base_pub_cfg {
  enum mgos_hass_pub_event pub_on;
  int timer_timeout;
  bool lazy_pub;
} ha_base_pub_cfg_t;

typedef struct ha_base_pub_props {
  enum mgos_hass_pub_event pub_on;
  mgos_timer_id timer_id;
  int timer_timeout;
  bool lazy_pub;
  ha_entity_state_pub_lambda state_pub;
} ha_base_pub_props_t;

typedef struct ha_base_sensor_props {
  void *state_get;
  void *state_get_ud;
  ha_base_pub_props_t pub;
} ha_base_sensor_props_t;

typedef struct ha_base_actuator_props {  
  void *state_set;
  void *state_set_ud;
} ha_base_actuator_props_t;

/*
 * Representation of a base sensor for all
 * sensors and actuators.
 * 
 * Inherited from:
 *   -  hass/bsensor_h/ha_bsensor_t
 *   -  hass/sensor_h/ha_sensor_t
 *   -  hass/switch_h/ha_switch_t
 */
typedef struct ha_base_sensor {
  HA_ENTITY_HANDLE handle;
  ha_base_sensor_props_t sens_props;
} ha_base_sensor_t;

/*
 * Representation of a base actuators for all
 * actuators.
 * 
 * * Inherits from
 *   - hass/common_h/ha_base_sensor_t
 * Inherited from:
 *   -  hass/switch_h/ha_switch_t
 */
typedef struct ha_base_actuator {
  HA_ENTITY_HANDLE handle;
  ha_base_sensor_props_t sens_props;
  /* above inherited from 'ha_base_sensor_t' */
  ha_base_actuator_props_t actu_props;
} ha_base_actuator_t;

#define GET_HA_BASE_SENSOR_T(h) ((ha_base_sensor_t *)(h != NULL ? ((HA_ENTITY_HANDLE_EX)h)->instance : NULL))

/*
 * Registers the handle
 */
bool mg_ha_entity_reg(HA_ENTITY_HANDLE handle);

/*
 * Unregisters the handle
 */
bool mg_ha_entity_unreg(HA_ENTITY_HANDLE handle);

/*
 * Destroy and release the entity handle
 */
void mg_ha_entity_free(HA_ENTITY_HANDLE handle);

/*
 * Destroy and release the [ha_base_sensor_props_t] instance
 */
void mg_ha_base_sensor_props_free(ha_base_sensor_props_t props);

/*
 * Destroy and release the [ha_base_actuator_props_t] instance
 */
void mg_ha_base_actuator_props_free(ha_base_actuator_props_t props);

/*
 * Destroy and release the [ha_bstate_t] instance
 */
void mg_ha_bstate_free(ha_bstate_t state);


bool mg_ha_base_sensor_creating(const char* sensor_type,
                                ha_base_sensor_t *e,
                                ha_entity_cfg_t *entity_cfg,
                                ha_base_pub_cfg_t *pub_cfg,
                                ha_entity_state_pub_lambda state_pub);

void mg_ha_base_sensor_closing(ha_base_sensor_t *e);

bool mg_ha_base_actuator_creating(const char* sensor_type,
                                  ha_base_actuator_t *e,
                                  ha_entity_cfg_t *entity_cfg,
                                  ha_base_pub_cfg_t *pub_cfg,
                                  ha_entity_state_pub_lambda state_pub);

void mg_ha_base_actuator_closing(ha_base_actuator_t *e);

/*
 * Publish entity's state
 */
enum ha_pub_result mgos_hass_entity_state_pub(HA_ENTITY_HANDLE handle);

/*
 * Set the specified event for publishing
 * entity state.
 */
bool mg_ha_state_pub_event_set(HA_ENTITY_HANDLE handle,
                               enum mgos_hass_pub_event pub_on, ...);

/*
 * Create and start triggers for publishing the state of
 * entities subscribed to events.
 */
bool mg_ha_state_pub_triggers_start();

/*
 * Stop ans clear timers for publishing the state of
 * entities subscribed to events.
 */
bool mg_ha_state_pub_triggers_clear();


/* BEGIN - MJS helper functions */

ha_entity_cfg_t *mjs_ha_entity_cfg_mk(const char *object_id,
                                      const char *device_id);

void mjs_ha_base_pub_cfg_set(ha_base_pub_cfg_t *s, bool lazy_pub,
                             enum mgos_hass_pub_event pub_on,
                             int timer_timeout);

/* END - MJS helper functions */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MG_HA_COMMON_H */