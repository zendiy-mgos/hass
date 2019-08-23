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