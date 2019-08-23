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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "mgos.h"
#include "frozen.h"
#include "mg_ha_utils.h"
#include "mg_ha_common.h"

int ON_INT_PUB_TIMER_ID = MGOS_INVALID_TIMER_ID;

ha_entity_list_item_t HA_REG_ENTITIES_ROOT = {NULL, NULL, NULL};
ha_entity_list_item_t *mg_ha_get_reg_entities_root() {
  return &HA_REG_ENTITIES_ROOT;
}


bool mg_ha_entity_reg(HA_ENTITY_HANDLE handle) {
  if (handle != NULL) {
    ha_entity_list_item_t *root = mg_ha_get_reg_entities_root();

    ha_entity_list_item_t *item = (ha_entity_list_item_t *) calloc(sizeof(ha_entity_list_item_t), 1);
    item->handle = handle;
    item->next = root->next;
    item->prev = root;
    root->next = item;

    if (item->next != NULL) {
        item->next->prev = item;
    }
  }
  return (handle != NULL);
}

bool mg_ha_entity_unreg(HA_ENTITY_HANDLE handle) {
  if (handle != NULL) {
    ha_entity_list_item_t *item = mg_ha_get_reg_entities_root()->next;
    while (item != NULL) {
      if (item->handle == handle) {
        if (item->next != NULL) {
          item->next->prev = item->prev;
        }
        item->prev->next = item->next;
        free(item);
        return true;
      }
      item = item->next;
    }
  }
  return false;
}

void mg_ha_entity_free(HA_ENTITY_HANDLE handle) {
  if (handle == NULL) return;  
  HA_ENTITY_HANDLE_EX h = (HA_ENTITY_HANDLE_EX)handle;
  free(h->device_id);
  free(h->object_id);
  free(h->instance);
  h = memset(h, 0, sizeof(HA_ENTITY_HANDLE));
  free(h);
}

void mg_ha_base_sensor_props_free(ha_base_sensor_props_t props) {
  (void) props;
}

void mg_ha_base_actuator_props_free(ha_base_actuator_props_t props) {
  (void) props;
}

void mg_ha_bstate_free(ha_bstate_t state) {
  free(state.off);
  free(state.on);
}


enum ha_pub_result ha_entity_state_pub_ex(HA_ENTITY_HANDLE handle,
                                          enum mgos_hass_pub_event ev) {
  ha_base_sensor_t *e = GET_HA_BASE_SENSOR_T(handle);
  ha_entity_pub_res_t res = MK_HA_ENTITY_PUB_RES(handle); 
  if (e != NULL && e->sens_props.pub.state_pub != NULL) {
    res.pub_ev = ev;
    res.result = e->sens_props.pub.state_pub(handle);
    if (res.result == FAILED) {
      LOG(LL_ERROR, ("Failed publishing the state of '%s'",
        e->handle->object_id));
    }
    /* Trigger the MGOS_HASS_EV_PUBLISHED event */
    mgos_event_trigger(MGOS_HASS_EV_PUBLISHED, (void *)&res);
  }
  return res.result;
}

enum ha_pub_result mgos_hass_entity_state_pub(HA_ENTITY_HANDLE handle) {
  return ha_entity_state_pub_ex(handle, NONE);
}

static void ha_entity_state_pub_ev(int ev, void *ev_data, void *userdata) { 
  enum mgos_hass_pub_event pev = NONE;  
  switch (ev) {
    case MGOS_HASS_EV_PUB_ON_CONN:
      pev = ON_CONNECT;
      break;
    case MGOS_HASS_EV_PUB_ON_INT:
      pev = ON_INTERVAL;
      break;
  };
  ha_entity_state_pub_ex((HA_ENTITY_HANDLE)userdata, pev);
  (void) ev_data;
}

bool mg_ha_state_pub_event_set(HA_ENTITY_HANDLE handle,
                               enum mgos_hass_pub_event pub_on, ...) {
  LOG(LL_DEBUG, ("Subscribing to event %d", pub_on));
  ha_base_sensor_t *e = GET_HA_BASE_SENSOR_T(handle);
  if (e == NULL) return false;

  ha_base_pub_props_t *pp = &e->sens_props.pub;
  if (pp->state_pub == NULL) return false;

  /* If the entity is already subscribed... */
  if (pub_on != NONE && (pp->pub_on & pub_on) == pub_on) {
    LOG(LL_DEBUG, ("Entity '%s' is already subscribed to event %d",
      handle->object_id, pub_on));
    return true;
  }

  bool success = false;
  va_list args;
  va_start(args, pub_on);

  if (pub_on == ON_INTERVAL) {
    /* Subscribe to MGOS_HASS_EV_PUB_ON_INT event */
    pp->timer_timeout = va_arg(args, int);
    if (pp->timer_timeout <= 0) {
      success = mgos_event_add_handler(MGOS_HASS_EV_PUB_ON_INT,
        ha_entity_state_pub_ev, handle);
    } else {
      success = true;
    }
  } else if (pub_on == ON_CONNECT) {
    /* Subscribe to MGOS_HASS_EV_PUB_ON_CONN event */
    success = mgos_event_add_handler(MGOS_HASS_EV_PUB_ON_CONN,
      ha_entity_state_pub_ev, handle);
  } /*else if (pub_on == ON_?) {
    Add here event registration code
  } */

  va_end(args);
 
  if (!success) {
    LOG(LL_ERROR, ("Entity '%s' failed subscribing to event %d",
      handle->object_id, pub_on));
  } else {
    pp->pub_on = pp->pub_on | pub_on;
    LOG(LL_DEBUG, ("Entity '%s' subscribed to event %d",
      handle->object_id, pub_on));
  }

  return success;
}

bool ha_state_pub_events_clear(HA_ENTITY_HANDLE handle) { 
  ha_base_sensor_t *e = GET_HA_BASE_SENSOR_T(handle);
  if (e == NULL) return false;
  ha_base_pub_props_t *pp = &e->sens_props.pub;
  if (pp->state_pub == NULL) return false;
  
  /* Remove the MGOS_HASS_EV_PUB_ON_INT event subscription */
  if ((pp->pub_on & ON_INTERVAL) == ON_INTERVAL) {    
    pp->pub_on = (pp->pub_on ^ ON_INTERVAL);
    if (pp->timer_timeout <= 0) {
      mgos_event_remove_handler(MGOS_HASS_EV_PUB_ON_INT,
        ha_entity_state_pub_ev, handle);
    }
  }

  /* Remove the MGOS_HASS_EV_PUB_ON_CONN event subscription */
  if ((pp->pub_on & ON_CONNECT) == ON_CONNECT) {
    mgos_event_remove_handler(MGOS_HASS_EV_PUB_ON_CONN,
      ha_entity_state_pub_ev, handle);
    pp->pub_on = (pp->pub_on ^ ON_CONNECT);
  }

  return true;
}

static void ha_publish_timer_cb(void *arg) {  
  (void) arg;
  LOG(LL_DEBUG, ("Triggering event %d", MGOS_HASS_EV_PUB_ON_INT));
  mgos_event_trigger(MGOS_HASS_EV_PUB_ON_INT, NULL);
}

static void ha_entity_publish_timer_cb(void *userdata) {
  mgos_hass_entity_state_pub((HA_ENTITY_HANDLE)userdata);
}

bool mg_ha_state_pub_triggers_start() {
  /* Trigger publish ON_CONNECT */
  if (mgos_sys_config_get_hass_publish_on_connect()) {
    LOG(LL_DEBUG, ("Triggering event %d", MGOS_HASS_EV_PUB_ON_CONN));
    mgos_event_trigger(MGOS_HASS_EV_PUB_ON_CONN, NULL);
  }
  
  /* Start the defualt timer */
  int interval = mgos_sys_config_get_hass_publish_interval();
  if (interval > 0) {
    ON_INT_PUB_TIMER_ID = mgos_set_timer(interval, MGOS_TIMER_REPEAT,
      ha_publish_timer_cb, NULL);
  }

  /* Start dedicated timers */
  ha_entity_list_item_t *item = mg_ha_get_reg_entities_root()->next;
  while (item != NULL) {
    ha_base_sensor_t *e = GET_HA_BASE_SENSOR_T(item->handle);
    if (e != NULL && e->sens_props.pub.timer_timeout > 0 &&
        ON_INTERVAL == (e->sens_props.pub.pub_on & ON_INTERVAL)) {
      e->sens_props.pub.timer_id = mgos_set_timer(e->sens_props.pub.timer_timeout,
        MGOS_TIMER_REPEAT, ha_entity_publish_timer_cb, item->handle);
    } else {
      e->sens_props.pub.timer_id = MGOS_INVALID_TIMER_ID;
    }
    item = item->next;
  }

  return true;
}

bool mg_ha_state_pub_triggers_clear() {
  /* Clear the defualt timer for publishing states */
  if (ON_INT_PUB_TIMER_ID != MGOS_INVALID_TIMER_ID) {
    mgos_clear_timer(ON_INT_PUB_TIMER_ID);
    ON_INT_PUB_TIMER_ID = MGOS_INVALID_TIMER_ID;
  }

  /* Clear dedicated timers */
  ha_entity_list_item_t *item = mg_ha_get_reg_entities_root()->next;
  while (item != NULL) {
    ha_base_sensor_t *e = GET_HA_BASE_SENSOR_T(item->handle);
    if (e != NULL && ON_INTERVAL == (e->sens_props.pub.pub_on & ON_INTERVAL) &&
        e->sens_props.pub.timer_id != MGOS_INVALID_TIMER_ID) {
      mgos_clear_timer(e->sens_props.pub.timer_id);
      e->sens_props.pub.timer_id = MGOS_INVALID_TIMER_ID;
    }
    item = item->next;
  }

  return true;
}

bool mg_ha_base_sensor_creating(const char* sensor_type,
                                ha_base_sensor_t *e,
                                ha_entity_cfg_t *entity_cfg,
                                ha_base_pub_cfg_t *pub_cfg,
                                ha_entity_state_pub_lambda state_pub) {
  e->handle = mg_ha_entity_handle_create(sensor_type, entity_cfg, e);
  /* Set the state_pub handler */
  e->sens_props.pub.state_pub = state_pub;
  /* Set the lazy_pub flag */
  e->sens_props.pub.lazy_pub = pub_cfg->lazy_pub;
  
  /* Set publishing event/s */
  if (pub_cfg != NULL) {
    if ((pub_cfg->pub_on & ON_CONNECT) == ON_CONNECT) {
      mg_ha_state_pub_event_set(e->handle, ON_CONNECT);
    }
    if ((pub_cfg->pub_on & ON_INTERVAL) == ON_INTERVAL) {
      mg_ha_state_pub_event_set(e->handle, ON_INTERVAL,
        pub_cfg->timer_timeout);
    }
  }

  return (e->handle != NULL);
}

void mg_ha_base_sensor_closing(ha_base_sensor_t *e) {
  if (e == NULL) return;
  mg_ha_entity_unreg(e->handle);
  ha_state_pub_events_clear(e->handle);
  mg_ha_base_sensor_props_free(e->sens_props);
  mg_ha_entity_free(e->handle);
}

bool mg_ha_base_actuator_creating(const char* sensor_type,
                                  ha_base_actuator_t *e,
                                  ha_entity_cfg_t *entity_cfg,
                                  ha_base_pub_cfg_t *pub_cfg,
                                  ha_entity_state_pub_lambda state_pub) {
  return mg_ha_base_sensor_creating(sensor_type,
    (ha_base_sensor_t *)e, entity_cfg, pub_cfg, state_pub);
}

void mg_ha_base_actuator_closing(ha_base_actuator_t *e) {
  if (e == NULL) return;
  mg_ha_base_actuator_props_free(e->actu_props);
  mg_ha_base_sensor_closing((ha_base_sensor_t *)e);
}


/* BEGIN - MJS helper functions */

ha_entity_cfg_t *mjs_ha_entity_cfg_mk(const char *object_id,
                                      const char *device_id) {
  ha_entity_cfg_t *s = (ha_entity_cfg_t*)calloc(sizeof(ha_entity_cfg_t), 1);
  s->object_id = object_id;
  s->device_id = device_id;  
  return s;
}

void mjs_ha_base_pub_cfg_set(ha_base_pub_cfg_t *s, bool lazy_pub,
                             enum mgos_hass_pub_event pub_on,
                             int timer_timeout) {
  s->lazy_pub = lazy_pub;
  s->pub_on = pub_on;
  s->timer_timeout = timer_timeout;
}

/* END - MJS helper functions */

