# Home Assistant Mongoose OS base library
## Overview
The base [mongoose-os](https://mongoose-os.com) library for making [Home Assistant](https://www.home-assistant.io) compatible devices.
## Usage
It is not necessary to include this base library into your mongoose-os project. Include instead one of the followings:
 - [hass-mqtt](https://github.com/zendiy-mgos/hass-mqtt) - The mongoose-os library for making Home Assistant compatible MQTT devices
 - [hass-mqtt-discovery](https://github.com/zendiy-mgos/hass-mqtt-discovery) - The mongoose-os library for making MQTT devices dicoverable in Home Assistant
## Configuration

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

|Property|Type|Description|
|--|--|--|
|hass.publish.**on_connect**|boolean|Enable/disable the state publish on (re)connected (default value `true`)|
|hass.publish.**interval**|integer|Interval in milliseconds for publishing state; set to `0` to diasable (default value `0`)|
|hass.toggle_state.**on**|string|The value that represents the ON state (default value `'ON'`)|
|hass.toggle_state.**off**|string|The value that represents the OFF state (default value `'OFF'`)|
