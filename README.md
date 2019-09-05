Home Assistant Mongoose OS base library
=======================================
# Overview
The base [Mongoose-OS](https://mongoose-os.com) library for making [Home Assistant](https://www.home-assistant.io) compatible devices.
# Usage
It is not necessary to include this base library into your mongoose-os project. Include instead one of the followings:
 - [hass-mqtt](https://github.com/zendiy-mgos/hass-mqtt) - The mongoose-os library for making Home Assistant compatible MQTT devices
 - [hass-mqtt-discovery](https://github.com/zendiy-mgos/hass-mqtt-discovery) - The mongoose-os library for making MQTT devices dicoverable in Home Assistant
# Configuration
The following configuration section is added to the `conf0.json` file in your mongoose-os project.
```json
"hass": {
  "publish": {
    "on_connect": true,
    "interval": 0
  },
  "toggle_state": {
    "on": "ON",
    "off": "OFF"
  }
}
```
|Property|Type|Default|Description|
|--|--|--|--|
|hass.publish.**on_connect**|boolean|`true`|Enable/disable the state publish on (re)connected|
|hass.publish.**interval**|integer|`0`|Interval in milliseconds for publishing state; set to `0` for disabling the timer|
|hass.toggle_state.**on**|string|`'ON'`|The value that represents the ON state|
|hass.toggle_state.**off**|string|`'OFF'`|The value that represents the OFF state|
# C/C++ API reference
## Common data types and API
### HA_ENTITY_HANDLE
```c
typedef struct ha_entity_handle {
  char *object_id;
  char *device_id;
  const char *entity_type;
} ha_entity_handle_t;
```
The entity handle that can be used as parameter in all API.
|Properties||
|object_id|The Home Assistant entity ID.|
|device_id|Optional. The decive ID. if null, the `device.id` config in the `mos.yml` file is used.|
|entity_type|The entity type. It could be `'binary_sensor'`, `'sensor'` or `'switch'`.|
### ha_entity_cfg_t
```c
typedef struct ha_entity_cfg {
  const char *object_id;
  const char *device_id;
} ha_entity_cfg_t;
```
