load('api_events.js');
load('api_math.js');
load('api_sys.js');

/* Polyfill function for Sys.strdup(..) */
if (!Sys.strdup) {
  Sys._strdup = ffi('void *strdup(char *)');
  Sys.strdup = function(s) {
    return (s ? this._strdup(s) : null);
  };
}

/* Polyfill function for Sys.parseInt(..) */
if (!Sys.parseInt) {
  Sys.parseInt = function(v) {
    return JSON.parse(JSON.stringify(Math.round(v)));
  };
}

let Hass = {
  _esp: ffi('int mgos_hass_entity_state_pub(void *)'),
  
  _edss: ffi('bool mgos_hass_entity_dstate_set(void *, int, char *)'),
  _esss: ffi('bool mgos_hass_entity_sstate_set(void *, char *, char *)'),
  _efss: ffi('bool mgos_hass_entity_fstate_set(void *, double, char *)'),
  _ebss: ffi('bool mgos_hass_entity_bstate_set(void *, int, char *)'),

  _mecfg: ffi('void *mjs_ha_entity_cfg_mk(void *, void *)'),
  
  mkEntityCfg: function(object_id, device_id) {
    return this._mecfg(object_id || null, device_id || null);
  },

  pubEvent: {
    DEFAULT: 0,
    ON_CONNECT: 1,
    ON_INTERVAL: 2,
    ON_ANY: 3,
    NONE: 128
  },

  pubResult: {
    FAILED: 0,
    SUCCESS: 1,
    LAZY: 3
  },

  toggleState: {
    UNKNOWN: 0,
    ON: 8,
    OFF: 128
  },
  
  entityStatePub: function(handle) {
    return this._esp(handle);
  },

  entityXStateSet: function(es, v, a) {
    if (typeof(v) === 'string') {
      return this._esss(es, v, a || null);
    }
    if (typeof(v) === 'boolean') {
      return this._ebss(es,
        (v ? this.toggleState.ON : this.toggleState.OFF), a || null);
    }
    if (typeof(v) === 'number') {
      if (v === Sys.parseInt(v)) {
        return this._edss(es, v, a || null);
      } else {
        return this._efss(es, v, a || null);
      }
    }    
    return false;
  },

  entityToggleStateSet: function(es, v, a) {
    if (typeof(v) === 'boolean') {
      v = (v ? this.toggleState.ON : this.toggleState.OFF);
    }
    return this._ebss(es, v, a || null);
  },

  // ## **`Hass.EVENT_GRP`**
  // Possible events are:
  // - `Hass.EV_PUB_ON_CONN`
  // - `Hass.EV_PUB_ON_INT`
  // - `Hass.EV_PUBLISHED`
  EVENT_GRP: Event.baseNumber("MHA"),
};

Hass.EV_PUB_ON_CONN = Hass.EVENT_GRP + 0;
Hass.EV_PUB_ON_INT  = Hass.EVENT_GRP + 1;
Hass.EV_PUBLISHED   = Hass.EVENT_GRP + 2;

load('api_ha_bsensor.js');
load('api_ha_sensor.js');
load('api_ha_switch.js');