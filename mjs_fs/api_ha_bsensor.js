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

let ha_bsensor = {
  _onsg: ffi('bool mgos_hass_bsensor_on_state_get(void *, bool(*)(void *, void *, userdata), userdata)'),
  _sget: ffi('void *mgos_hass_bsensor_state_get(void *)'),
  _cls: ffi('void mgos_hass_bsensor_close(void *)'),

  _proto: {
    close: function() {
      return Hass.BSENSOR._cls(this.handle);
    },

    stateGet: function() {
      return Hass.BSENSOR._sget(this.handle);
    },

    onStateGet: function(cb, ud) {
      return Hass.BSENSOR._onsg(this.handle, cb, ud);
    },
  },
};

Hass.BSENSOR = ha_bsensor;