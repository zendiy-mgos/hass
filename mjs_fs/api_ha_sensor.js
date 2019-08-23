let ha_sensor = {
  _onsg: ffi('bool mgos_hass_sensor_on_state_get(void *, bool(*)(void *, void *, userdata), userdata)'),
  _sget: ffi('void *mgos_hass_sensor_state_get(void *)'),
  _cls: ffi('void mgos_hass_sensor_close(void *)'),

  _proto: {
    close: function() {
      return Hass.SENSOR._cls(this.handle);
    },

    stateGet: function() {
      return Hass.SENSOR._sget(this.handle);
    },

    onStateGet: function(cb, ud) {
      return Hass.SENSOR._onsg(this.handle, cb, ud);
    },
  },
};

Hass.SENSOR = ha_sensor;