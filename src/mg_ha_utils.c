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
#include "mgos.h"
#include "mg_ha_utils.h"

int mg_ha_strinstr(const char *str1, const char* str2) {
  int count = 0;
  if (str1 != NULL && str2 != NULL) {
    int string_len = strlen(str2);
    for (const char *p = str1; *p != '\0';) {
      if (strncmp(p, str2, string_len) == 0) {
        ++count;
        p += string_len;
      } else {
        ++p;
      }
    }
  }
  return count;
}

bool mg_ha_sprintf(const char *src, const char *ph,
                   const char* string, char **out) {
  *out = NULL;

  /* count 'ph' occurrences in 'src' */
  int count = mg_ha_strinstr(src, ph);
  if (count == 0) {
    return false;
  }

  int src_len = strlen(src);
  int ph_len = strlen(ph);
  int string_len = (string == NULL ? 0 : strlen(string));

  /* allocate the buffer for the result string */
  int res_len = (src_len + (string_len * count));
  *out = (char *)malloc(res_len + 1);
  strcpy(*out, src);

  /* allocate a temporary buffer */
  char *tmp_buf = (char *)malloc(src_len + 1);
  char *tmp_buf_to_free = tmp_buf;
  char *buf = *out;
      
  for (; *buf != '\0';) {
    if (strncmp(buf, ph, ph_len) == 0) {
      strcpy(tmp_buf, (buf + ph_len));
      strcpy(buf, (string != NULL ? string : ""));
      strcpy((buf + string_len), tmp_buf);
      buf += string_len;
    } else {
      ++buf;
    }
  }

  free(tmp_buf_to_free);
  return true;
}

HA_ENTITY_HANDLE mg_ha_entity_handle_create(const char* entity_type,
                                            ha_entity_cfg_t *cfg,
                                            void *instance) {
  HA_ENTITY_HANDLE_EX h = NULL;
  if (entity_type != NULL && cfg != NULL &&
      cfg->object_id != NULL && instance != NULL) {
    h = (HA_ENTITY_HANDLE_EX)calloc(sizeof(ha_entity_handle_ex_t), 1);

    /* set object_id */
    h->object_id = strdup(cfg->object_id);

    /* set device_id */
    const char *did = (cfg->device_id != NULL ? cfg->device_id :
      mgos_sys_config_get_device_id());
    if (did != NULL) {
      h->device_id = strdup(did);      
    }

    /* set entity_type */
    h->entity_type = entity_type;

    /* set instance */
    h->instance = instance;
  }
  return (HA_ENTITY_HANDLE)h;
}

char *mg_ha_entity_sprintf(HA_ENTITY_HANDLE handle, 
                           const char *str_fmt,
                           const char *default_str) {
  if (handle == NULL || (str_fmt == NULL && default_str == NULL)) {
    return NULL;
  }

  char *et = strdup((str_fmt != NULL ? str_fmt : default_str));
  char *et_tmp = NULL;

  /* replace $object_id placehoder/s */
  if (mg_ha_sprintf(et, HA_ENV_VAR_OBJ_ID, handle->object_id, &et_tmp)) {
    free(et);
    et = et_tmp;
  }

  /* replace $device_id placehoder/s */
  if (mg_ha_sprintf(et, HA_ENV_VAR_DEV_ID, handle->device_id, &et_tmp)) {
    free(et);
    et = et_tmp;
  }

  return et;
}