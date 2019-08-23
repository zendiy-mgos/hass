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