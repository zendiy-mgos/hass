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
 * Home Assistant | Common entity state structures and API.
 */

#ifndef MG_HA_ENTITY_STATE_H
#define MG_HA_ENTITY_STATE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Binary/toggle state configuration settings.
 * Note: if you change the struct definition,
 * you must update the macro <MK_HA_BSTATE_CFG>, and
 * the function mjs_ha_bstate_cfg_set(...) as well.
 */
#define MK_HA_BSTATE_CFG(void) {NULL, NULL}
typedef struct ha_bstate_cfg {
  const char *on;
  const char *off;
} ha_bstate_cfg_t;

typedef struct ha_bstate {
  char *on;
  char *off;
} ha_bstate_t;

enum ha_toggle_state {
  UNKNOWN = 0,  //00000000
  ON = 8,       //00001000
  OFF = 128     //10000000
};

enum ha_xstate_type {
  TOGGLE = 0,
  FLOATING_POINT = 1,
  INTEGER = 2,
  STRING = 4
};

/*
 * The base state of all entities.
 *
 *  Inherited from:
 *    - ha_entity_xstate_t
 *    - ha_entity_bstate_t
 */
typedef struct ha_base_entity_state {
  char *attribs;
} ha_base_entity_state_t;

/*
 * The base state of all entities.
 *
 *  Inherits from:
 *    - ha_base_entity_state
 *  Inherited from:
 *    - ha_entity_xstate_t
 */
typedef struct ha_entity_bstate {
  char *attribs;
  /* above inherited from 'ha_base_entity_state' */
  enum ha_toggle_state value; // as: (int) ha_entity_xstate_t.d_value;
} ha_entity_bstate_t;

/*
 * The state of a gereic entity.
 * 
 * Inherits from:
 *   - ha_base_entity_state
 *   - ha_entity_bstate_t
 */
typedef struct ha_entity_xstate {
  char *attribs;
  /* above inherited from 'ha_base_entity_state' */
  int d_value; // as: (enum ha_toggle_state) ha_entity_bstate_t.value;
  /* above inherited from 'ha_entity_bstate_t' */
  enum ha_xstate_type v_type;
  double f_value;
  char *s_value;
} ha_entity_xstate_t;

/*
 * State history of an entity
 */
typedef struct ha_entity_xstate_history {
  bool changed;
  ha_entity_xstate_t last_pub;
  ha_entity_xstate_t curr;
} ha_entity_xstate_history_t;

typedef ha_entity_xstate_history_t* HA_ENTITY_XSTATE;
typedef ha_entity_xstate_history_t* HA_ENTITY_BSTATE;

/*
 * Initialize an [ha_bstate_t] instance
 */
bool mg_ha_bstate_init(ha_bstate_t *state, ha_bstate_cfg_t *cfg);


bool mgos_hass_entity_dstate_setf(HA_ENTITY_XSTATE state,
                                 long value,
                                 const char *attribs_fmt, ...);

bool mgos_hass_entity_sstate_setf(HA_ENTITY_XSTATE state,
                                 const char *value,
                                 const char *attribs_fmt, ...);

bool mgos_hass_entity_fstate_setf(HA_ENTITY_XSTATE state,
                                 double value,
                                 const char *attribs_fmt, ...);

bool mgos_hass_entity_bstate_setf(HA_ENTITY_BSTATE state,
                                 enum ha_toggle_state value,
                                 const char *attribs_fmt, ...);

bool mg_ha_entity_xstate_mark_as_pub(ha_entity_xstate_history_t *h);

void mg_ha_entity_xstate_history_free(ha_entity_xstate_history_t h);

/* BEGIN - MJS helper functions */

void mjs_ha_bstate_cfg_set(ha_bstate_cfg_t *s,
                           const char *on, const char *off);

/* END - MJS helper functions */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MG_HA_ENTITY_STATE_H */
