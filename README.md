# MQTT plugins for MADS

This is a project providing Source and Sink plugins for [MADS](https://github.com/MADS-NET/MADS). 

The plugins provides a bridge between a MQTT network and the MADS network. Compilation results in two plugins:

* `mqtt2mads.plugin`: A Source plugin that subscribes to a MQTT topic and publishes the received messages to the MADS network.
* `mads2mqtt.plugin`: A Sink plugin that receives messages from the MADS network and publishes them to a MQTT topic.

*Required MADS version: 1.0.1.*

## Requirements

The system must provide `libmosquitto` and `libmosquittopp` libraries and development files.

## Supported platforms

Currently, the supported platforms are:

* **Linux** 
* **MacOS**


## Installation

Linux and MacOS:

```bash
cmake -Bbuild -DCMAKE_INSTALL_PREFIX="$(mads -p)"
cmake --build build -j4
sudo cmake --install build
```


## INI settings

The plugin supports the following settings in the INI file:

```ini
[mqtt2mads]
broker_host = "localhost" # MQTT broker host
broker_port = 1883        # MQTT broker port
silent = true             # If true, no log messages are printed
QoS = 0                   # MQTT Quality of Service
topic = "#"               # MQTT topic to subscribe to

[mads2mqtt]
broker_host = "localhost" # MQTT broker host
broker_port = 1883        # MQTT broker port
silent = true             # If true, no log messages are printed
QoS = 0                   # MQTT Quality of Service
topic = "mads"            # MQTT topic to publish to
```

All settings are optional; if omitted, the default values are used.


## Executable demo

The plugin comes with an executable demo that can be used to test the plugin. The demo connects to a broker running on localhost, subscribes to the MQTT topic `#` and prints the received messages to the console.

An optional first argument is the address of a different broker (default `localhost`). An optional thirs argument is the broker port (default `1883`)

---