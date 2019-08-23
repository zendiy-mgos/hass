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
 * Home Assistant | Utilities and helpers.
 */

#ifndef MG_HA_UTILS_H
#define MG_HA_UTILS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include "mg_ha_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Count recurrences of str2 in str1
 */
int mg_ha_strinstr(const char *str1, const char* str2);

/*
 * TODO: comment
 */
bool mg_ha_sprintf(const char *src, const char *ph,
                   const char* string, char **out);

/*
 * Create and initialize an entity handle
 */
HA_ENTITY_HANDLE mg_ha_entity_handle_create(const char* entity_type,
                                                   ha_entity_cfg_t *cfg,
                                                   void *instance);

/*
 * TODO: comment
 */
char *mg_ha_entity_sprintf(HA_ENTITY_HANDLE handle, 
                           const char *str_fmt,
                           const char *default_str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MG_HA_UTILS_H */