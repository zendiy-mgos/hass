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
#include <stdio.h>
#include <string.h>
#include "mgos_sys_config.h"
#include "frozen.h"
#include "mg_ha_entity_state.h"
#include "mg_ha_common.h"

bool mg_ha_bstate_init(ha_bstate_t *state,
                       ha_bstate_cfg_t *cfg) {
  if (state == NULL) return false;
  const char *tmp = NULL;
  tmp = (cfg != NULL ? cfg->on : NULL);
  tmp = (tmp != NULL ? tmp : mgos_sys_config_get_hass_toggle_state_on());
  state->on = HA_SAFE_STRDUP(tmp);
  
  tmp = (cfg != NULL ? cfg->off : NULL);
  tmp = (tmp != NULL ? tmp : mgos_sys_config_get_hass_toggle_state_off());
  state->off = HA_SAFE_STRDUP(tmp);
  
  return (state->off != NULL && state->on != NULL);
}

bool ha_entity_xstate_are_eq(ha_entity_xstate_history_t *h) {
  if (h == NULL) return true;
  if (h->curr.v_type != h->last_pub.v_type) return false;
  switch (h->curr.v_type) {
    case FLOATING_POINT:
      return (h->curr.f_value == h->last_pub.f_value);
    case INTEGER:
    case TOGGLE:
      return (h->curr.d_value == h->last_pub.d_value);
    case STRING:
      if (h->curr.s_value != NULL && h->last_pub.s_value != NULL) {
        return (strcmp(h->curr.s_value, h->last_pub.s_value) == 0);
      }
      return (h->curr.s_value == h->last_pub.s_value); // true if both NULL
  };
  return false;
}

void ha_esa_set(ha_base_entity_state_t *state,
                const char *attribs_fmt, va_list *fmt_args) {
  if (state != NULL) {
    free(state->attribs);
    if (fmt_args != NULL) {
      state->attribs = (attribs_fmt == NULL ? NULL :
        json_vasprintf(attribs_fmt, *fmt_args));
    } else {
      state->attribs = HA_SAFE_STRDUP(attribs_fmt);
    }
  }
}

#define HA_ENTITY_STATE_SET_FUNC_INVOKE(func, state, value, attribs_fmt)            \
  do {                                                                              \
    va_list ap;                                                                     \
    va_start(ap, attribs_fmt);                                                      \
    bool success = func (state, value, attribs_fmt, &ap);                           \
    va_end(ap);                                                                     \
    return success;                                                                 \
  } while (0);                                                                      \

bool ha_entity_dstate_set(HA_ENTITY_XSTATE state,
                          long value,
                          const char *attribs_fmt,
                          va_list *fmt_args) {
  if (state == NULL) return false;
  // set the value
  state->curr.d_value = value;
  state->curr.v_type = INTEGER;
  if (attribs_fmt != NULL) {
    ha_esa_set((ha_base_entity_state_t *)&state->curr,
      attribs_fmt, fmt_args);
  }
  state->changed = !ha_entity_xstate_are_eq(state);
  return true;
}

bool mgos_hass_entity_dstate_set(HA_ENTITY_XSTATE state,
                                 long value,
                                 const char *attribs) {
  return ha_entity_dstate_set(state, value, attribs, NULL);
}

bool mgos_hass_entity_dstate_setf(HA_ENTITY_XSTATE state,
                                  long value,
                                  const char *attribs_fmt, ...) {
  HA_ENTITY_STATE_SET_FUNC_INVOKE(ha_entity_dstate_set,
    state, value, attribs_fmt);
}

bool ha_entity_sstate_set(HA_ENTITY_XSTATE state,
                          const char *value, 
                          const char *attribs_fmt,
                          va_list *fmt_args) {
  if (state == NULL) return false;
  // set the value
  free(state->curr.s_value);
  state->curr.v_type = STRING;
  state->curr.s_value = HA_SAFE_STRDUP(value);
  if (attribs_fmt != NULL) {
    ha_esa_set((ha_base_entity_state_t *)&state->curr,
      attribs_fmt, fmt_args);
  }
  state->changed = !ha_entity_xstate_are_eq(state);
  return true;
}

bool mgos_hass_entity_sstate_set(HA_ENTITY_XSTATE state,
                                 const char *value, 
                                 const char *attribs) {
  return ha_entity_sstate_set(state, value, attribs, NULL);
}

bool mgos_hass_entity_sstate_setf(HA_ENTITY_XSTATE state,
                                  const char *value,
                                  const char *attribs_fmt, ...) {
  HA_ENTITY_STATE_SET_FUNC_INVOKE(ha_entity_sstate_set,
    state, value, attribs_fmt);
}

bool ha_entity_fstate_set(HA_ENTITY_XSTATE state,
                          double value,
                          const char *attribs_fmt,
                          va_list *fmt_args) {
  if (state == NULL) return false;
  // set the value
  state->curr.f_value = value;
  state->curr.v_type = FLOATING_POINT;
  if (attribs_fmt != NULL) {
    ha_esa_set((ha_base_entity_state_t *)&state->curr,
      attribs_fmt, fmt_args);
  }
  state->changed = !ha_entity_xstate_are_eq(state);
  return true;
}

bool mgos_hass_entity_fstate_set(HA_ENTITY_XSTATE state,
                                 double value,
                                 const char *attribs) {
  return ha_entity_fstate_set(state, value, attribs, NULL);
}

bool mgos_hass_entity_fstate_setf(HA_ENTITY_XSTATE state,
                                 double value,
                                 const char *attribs_fmt, ...) {
  HA_ENTITY_STATE_SET_FUNC_INVOKE(ha_entity_fstate_set,
    state, value, attribs_fmt);
}

bool ha_entity_bstate_set(HA_ENTITY_BSTATE state,
                          enum ha_toggle_state value,
                          const char *attribs_fmt,
                          va_list *fmt_args) {
  if (state == NULL) return false;
  // set the value
  state->curr.d_value = value;
  state->curr.v_type = TOGGLE;
  if (attribs_fmt != NULL) {
    ha_esa_set((ha_base_entity_state_t *)&state->curr,
      attribs_fmt, fmt_args);
  }
  state->changed = !ha_entity_xstate_are_eq(state);
  return true;
}

bool mgos_hass_entity_bstate_set(HA_ENTITY_BSTATE state,
                                 enum ha_toggle_state value,
                                 const char *attribs) {
  return ha_entity_bstate_set(state, value, attribs, NULL);
}

bool mgos_hass_entity_bstate_setf(HA_ENTITY_BSTATE state,
                                  enum ha_toggle_state value,
                                  const char *attribs_fmt, ...) {
  HA_ENTITY_STATE_SET_FUNC_INVOKE(ha_entity_bstate_set,
    state, value, attribs_fmt);
}

bool mg_ha_entity_xstate_mark_as_pub(ha_entity_xstate_history_t *h) {
  if (h == NULL) return false;
  // copy attribs
  free(h->last_pub.attribs);
  h->last_pub.attribs = HA_SAFE_STRDUP(h->curr.attribs);
  // copy float value
  h->last_pub.f_value = h->curr.f_value;
  // copy decimal value
  h->last_pub.d_value = h->curr.d_value;
  // copy string value
  free(h->last_pub.s_value);
  h->last_pub.s_value = HA_SAFE_STRDUP(h->curr.s_value);
  // copy value type
  h->last_pub.v_type = h->curr.v_type;
  // reset the change flag
  h->changed = false;
  return true;
}

void mg_ha_entity_xstate_history_free(ha_entity_xstate_history_t h) {
  free(h.curr.attribs);
  free(h.curr.s_value);
  free(h.last_pub.attribs);
  free(h.last_pub.s_value);
}

/* BEGIN - MJS helper functions */

void mjs_ha_bstate_cfg_set(ha_bstate_cfg_t *s,
                           const char *on, const char *off) {
  s->on = on;
  s->off = off;
}

/* END - MJS helper functions */