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

bool mgos_hass_bsensor_on_state_get(HA_ENTITY_HANDLE handle,
                                    ha_bsensor_state_get_lambda handler,
                                    void *user_data) {
  ha_bsensor_t *e = GET_HA_BSENSOR_T(handle);
  if (e != NULL &&  handler != NULL) {
    e->sens_props.state_get = handler;
    e->sens_props.state_get_ud = user_data;
    return true;    
  }
  return false;
}

extern bool ha_bsensor_state_on_pub(HA_ENTITY_HANDLE handle,
                                    ha_entity_bstate_t *state);

enum ha_pub_result ha_bsensor_state_pub(HA_ENTITY_HANDLE handle) {
  ha_bsensor_t *e = GET_HA_BSENSOR_T(handle);
  if (mgos_hass_bsensor_state_get(handle) != NULL) {
    if (e->sens_props.pub.lazy_pub && !e->state_history.changed) {
      LOG(LL_DEBUG, ("Stete publish skipped on '%s' because LAZY_PUB is active.",
        handle->object_id));
      return LAZY; // success, but not published because lazy_pub
    } else if (ha_bsensor_state_on_pub(handle, (ha_entity_bstate_t *)&e->state_history.curr)) {
      mg_ha_entity_xstate_mark_as_pub(&e->state_history);
      return SUCCESS;
    }    
  } else {
    LOG(LL_ERROR, ("Unable to publish '%s' state. Invalid <null> state.",
      handle->object_id));
  }
  return FAILED;
}

ha_entity_bstate_t *mgos_hass_bsensor_state_get(HA_ENTITY_HANDLE handle) {
  ha_bsensor_t *e = GET_HA_BSENSOR_T(handle);
  if (e != NULL && e->sens_props.state_get != NULL) {
    if (((ha_bsensor_state_get_lambda)e->sens_props.state_get)(handle,
        &e->state_history, e->sens_props.state_get_ud)){
      return (ha_entity_bstate_t *)&e->state_history.curr;
    }    
  }
  return NULL;
}

bool mg_ha_bsensor_creating(ha_bsensor_t *e,
                            ha_entity_cfg_t *entity_cfg,
                            ha_base_pub_cfg_t *pub_cfg,
                            ha_bsensor_cfg_t *cfg) {
  if (e == NULL) return false;

  /* Initialize state_payload */
  if (!mg_ha_bstate_init(&e->bsensor_props.state_payload,
        &cfg->state_cfg)) {
    return false;
  }

  /* Set pub_on defualts */
  if (pub_cfg != NULL && pub_cfg->pub_on == DEFAULT) {
    pub_cfg->pub_on = (ON_CONNECT | ON_INTERVAL);
  }

  ha_base_sensor_t *sens = (ha_base_sensor_t *)e;
  if (!mg_ha_base_sensor_creating(HA_BSENSOR_TYPE, sens,
        entity_cfg, pub_cfg, ha_bsensor_state_pub)) {  
    return false;
  }

  return true;
}

void mg_ha_bsensor_closing(ha_bsensor_t *e) {
  if (e == NULL) return;
  mg_ha_bstate_free(e->bsensor_props.state_payload);
  mg_ha_entity_xstate_history_free(e->state_history);
  mg_ha_base_sensor_closing((ha_base_sensor_t *)e);
}

/* BEGIN - MJS helper functions */

void mjs_ha_bsensor_cfg_set(ha_bsensor_cfg_t *s,
                            const char *on, const char *off) {
  mjs_ha_bstate_cfg_set(&s->state_cfg, on, off);
}

/* END - MJS helper functions */