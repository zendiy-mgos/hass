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


#include <stdlib.h>
#include "mgos.h"
#include "mgos_hass.h"
#include "mg_ha_utils.h"

bool mgos_hass_switch_on_state_get(HA_ENTITY_HANDLE handle,
                                   ha_switch_state_get_lambda handler,
                                   void *user_data) {
  ha_switch_t *e = GET_HA_SWITCH_T(handle);
  if (e != NULL && handler != NULL) {    
    e->sens_props.state_get = handler;
    e->sens_props.state_get_ud = user_data;
    return true;    
  }
  return false;
}

extern bool ha_switch_state_on_pub(HA_ENTITY_HANDLE handle,
                                   ha_entity_bstate_t *state);

enum ha_pub_result ha_switch_state_pub(HA_ENTITY_HANDLE handle) {
  ha_switch_t *e = GET_HA_SWITCH_T(handle);
  if (mgos_hass_switch_state_get(handle) != NULL) 
  {  
    if (e->sens_props.pub.lazy_pub && !e->state_history.changed) {
      LOG(LL_DEBUG, ("Stete publish skipped on '%s' because LAZY_PUB is active.",
        handle->object_id));
      return LAZY; // success, but not published because lazy_pub
    } else if (ha_switch_state_on_pub(handle, (ha_entity_bstate_t *)&e->state_history.curr)) {
      mg_ha_entity_xstate_mark_as_pub(&e->state_history);
      return SUCCESS; // success
    }
  } else {
    LOG(LL_ERROR, ("Unable to publish '%s' state. Invalid <null> state.",
      handle->object_id));
  }
  return FAILED;
}

ha_entity_bstate_t *mgos_hass_switch_state_get(HA_ENTITY_HANDLE handle) {  
  ha_switch_t *e = GET_HA_SWITCH_T(handle);
  if (e != NULL && e->sens_props.state_get != NULL) {
    if (((ha_switch_state_get_lambda)e->sens_props.state_get)(handle,
        &e->state_history, e->sens_props.state_get_ud)){
      return (ha_entity_bstate_t *)&e->state_history.curr;
    }
  }
  return NULL;
}

bool mgos_hass_switch_on_state_set(HA_ENTITY_HANDLE handle,
                                   ha_switch_state_set_lambda handler,
                                   void *user_data) {
  ha_switch_t *e = GET_HA_SWITCH_T(handle);
  if (e != NULL && handler != NULL) {    
    e->actu_props.state_set = handler;
    e->actu_props.state_set_ud = user_data;
    return true;
  }
  return false;
}

static void ha_inching_timer_cb(void *arg) {  
  ha_switch_t *e = GET_HA_SWITCH_T((HA_ENTITY_HANDLE)arg);
  mgos_hass_switch_state_set(e->handle,
    e->switch_props.normal_state);
}

bool mgos_hass_switch_state_set(HA_ENTITY_HANDLE handle,
                                enum ha_toggle_state state) {
  ha_switch_t *e = GET_HA_SWITCH_T(handle);
  if (e != NULL) {
    ha_switch_state_set_lambda state_set = (ha_switch_state_set_lambda)e->actu_props.state_set;
    if (state_set != NULL) {
      if (state_set(handle, state, e->actu_props.state_set_ud)) {
        /* Try to activate the inching-mode */
        if (state != e->switch_props.normal_state &&
            e->switch_props.inching_timeout > 0) {
          mgos_set_timer(e->switch_props.inching_timeout, 0,
            ha_inching_timer_cb, handle);
        }
        /* Try to publish the state */
        ha_switch_state_pub(handle);
      }
      return true;
    }
  }
  return false;
}

enum ha_toggle_state mgos_hass_switch_state_toggle(HA_ENTITY_HANDLE handle) {  
  ha_entity_bstate_t *state = mgos_hass_switch_state_get(handle);
  if (state == NULL) return UNKNOWN;  
  enum ha_toggle_state result = UNKNOWN;
  switch (state->value)
  {
    case ON:
      if (mgos_hass_switch_state_set(handle, OFF)) {
        result = OFF;
      }
      break;
    case OFF:
      if (mgos_hass_switch_state_set(handle, ON)) {
        result = ON;
      }
      break;
    default:
      result = state->value;
      break;
  };
  return result;
}

bool mg_ha_switch_creating(ha_switch_t *e,
                           ha_entity_cfg_t *entity_cfg,
                           ha_base_pub_cfg_t *pub_cfg,
                           ha_switch_cfg_t *cfg) { 
  if (e == NULL) return false;
  
  /* Initialize state_payload */
  if (!mg_ha_bstate_init(&e->switch_props.state_payload,&cfg->state_cfg)) {
    return false;
  }

  /* Set pub_on defualts */
  if (pub_cfg != NULL && pub_cfg->pub_on == DEFAULT) {
    pub_cfg->pub_on = ON_CONNECT;
  }
  
  ha_base_actuator_t *actu = (ha_base_actuator_t *)e;
  if (!mg_ha_base_actuator_creating(HA_SWITCH_TYPE, actu,
        entity_cfg, pub_cfg, ha_switch_state_pub)) {
    return false;
  }

  /* Initialize switch_props */
  e->switch_props.normal_state = cfg->normal_state;
  e->switch_props.inching_timeout = cfg->inching_timeout;

  return true;
}

void mg_ha_switch_closing(ha_switch_t *e) {  
  if (e == NULL) return;
  mg_ha_bstate_free(e->switch_props.state_payload);
  mg_ha_entity_xstate_history_free(e->state_history);
  mg_ha_base_actuator_closing((ha_base_actuator_t *)e);
}

/* BEGIN - MJS helper functions */

void mjs_ha_switch_cfg_set(ha_switch_cfg_t *s,
                           const char *on, const char *off,
                           enum ha_toggle_state normal_state,
                           int inching_timeout) {
  mjs_ha_bstate_cfg_set(&s->state_cfg, on, off);
  s->normal_state = normal_state;
  s->inching_timeout = inching_timeout;
}

/* END - MJS helper functions */

