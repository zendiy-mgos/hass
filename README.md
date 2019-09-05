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
