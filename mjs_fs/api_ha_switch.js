/*
 * Copyright (c) 2019 ZenDIY
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

let ha_switch = {
  _onsg: ffi('bool mgos_hass_switch_on_state_get(void *, bool(*)(void *, void *, userdata), userdata)'),
  _onss: ffi('bool mgos_hass_switch_on_state_set(void *, bool(*)(void *, int, userdata), userdata)'),
  _sget: ffi('void *mgos_hass_switch_state_get(void *)'),
  _sset: ffi('bool mgos_hass_switch_state_set(void *, int)'),
  _cls: ffi('void mgos_hass_switch_close(void *)'),

  _proto: {
    close: function() {
      return Hass.SWITCH._cls(this.handle);
    },

    stateGet: function() {
      return Hass.SWITCH._sget(this.handle);
    },

    stateSet: function(v) {
      return Hass.SWITCH._sset(this.handle, v);
    },

    onStateGet: function(cb, ud) {
      return Hass.SWITCH._onsg(this.handle, cb, ud);
    },
    
    onStateSet: function(cb, ud) {      
      return Hass.SWITCH._onss(this.handle, cb, ud);
    },
  },
};

Hass.SWITCH = ha_switch;