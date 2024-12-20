/*
  __  __  ___ _____ _____         _             _       
 |  \/  |/ _ \_   _|_   _|  _ __ | |_   _  __ _(_)_ __  
 | |\/| | | | || |   | |   | '_ \| | | | |/ _` | | '_ \ 
 | |  | | |_| || |   | |   | |_) | | |_| | (_| | | | | |
 |_|  |_|\__\_\|_|   |_|   | .__/|_|\__,_|\__, |_|_| |_|
                           |_|            |___/         
# A Template for Mqtt2madsPlugin, a Source Plugin
# Generated by the command: plugin -t source -d mqtt_plugin mqtt2mads
# Hostname: Fram-IV.local
# Current working directory: /Users/p4010/Develop/MADS_plugins
# Creation date: 2024-08-01T12:22:33.802+0200
# NOTICE: MADS Version 1.0.1
*/
// Mandatory included headers
#include <source.hpp>
#include <nlohmann/json.hpp>
#include <pugg/Kernel.h>
// other includes as needed here
#include <mosquittopp.h>
#include <thread>
#include <csignal>

// Define the name of the plugin
#ifndef PLUGIN_NAME
#define PLUGIN_NAME "mqtt2mads"
#endif
#define NO_ERROR "No Error"

// Load the namespaces
using namespace std;
using json = nlohmann::json;
using namespace mosqpp;



// Plugin class. This shall be the only part that needs to be modified,
// implementing the actual functionality
class Mqtt2madsPlugin : public Source<json>, public mosquittopp {

public:

  // Typically, no need to change this
  string kind() override { return PLUGIN_NAME; }

/*
  __  __  ___ _____ _____            _       _           _ 
 |  \/  |/ _ \_   _|_   _|  _ __ ___| | __ _| |_ ___  __| |
 | |\/| | | | || |   | |   | '__/ _ \ |/ _` | __/ _ \/ _` |
 | |  | | |_| || |   | |   | | |  __/ | (_| | ||  __/ (_| |
 |_|  |_|\__\_\|_|   |_|   |_|  \___|_|\__,_|\__\___|\__,_|
                                                           
*/

  return_type setup() {
    return_type status = return_type::success;
    if (_connected) return status;
    string host = _params["broker_host"];
    string topic = _params["topic"];
    int port = _params["broker_port"];

    lib_init();
    reinitialise("MQTT2MADS-bridge", true);
    connect(host.c_str(), port, 60);
    // subscribe(NULL, topic.c_str(), 0);

    // Connect to MQTT
    _connected = true;
    return status;
  }

  ~Mqtt2madsPlugin() {
    disconnect();
    mosqpp::lib_cleanup();
  }

  void on_connect(int rc) override {
    if (!_params["silent"])
      cerr << "Connected with code " << rc << endl;
    subscribe(NULL, _params["topic"].get<string>().c_str(), 0);
    return;
  }

  void on_subscribe(int mid, int qos_count, const int *granted_qos) override {
    if (!_params["silent"])
      cerr << "Subscribed to " << _params["topic"].get<string>() << " QoS: " << *granted_qos
           << endl;
    return;
  }

  void on_unsubscribe(int mid) override {
    if (!_params["silent"])
      cerr << "Unsubscribed from " << _params["topic"].get<string>() << endl;
    return;
  }

  void on_disconnect(int rc) override {
    if (!_params["silent"])
      cerr << "Disconnected with code " << rc << endl;
    return;
  }

  void on_message(const struct mosquitto_message *message) override {
    _data.clear();
    try {
      _data = json::parse((char *)(message->payload));
      _error = NO_ERROR;
    } catch (json::parse_error &e) {
      _error = e.what();
      _data["error"] = "Error parsing invalid JSON received from MQTT";
      _data["reason"] = _error;
      _data["content"] = (char *)(message->payload);
    }
    _topic = message->topic;
    if (!_params["silent"])
      cerr << "Received message on topic " << _topic << ", status: " 
           << _error << endl;
    return;
  }

/*
  ____  _    _   _  ____ ___ _   _ 
 |  _ \| |  | | | |/ ___|_ _| \ | |
 | |_) | |  | | | | |  _ | ||  \| |
 |  __/| |__| |_| | |_| || || |\  |
 |_|   |_____\___/ \____|___|_| \_|
                                   
*/
  return_type get_output(json &out, std::vector<unsigned char> *blob = nullptr) override {
    if (setup() != return_type::success) {
      return return_type::critical;
    }
    loop();
    if(_data.is_null() || _data.empty()) {
      _data.clear();
      return return_type::retry;
    }
    out.clear();
    out["payload"] = _data;
    out["topic"] = _topic;
    if (!_agent_id.empty()) out["agent_id"] = _agent_id;
    _data.clear();
    this_thread::sleep_for(chrono::microseconds(500));
    if (_error != NO_ERROR) 
      return return_type::error;
    else
      return return_type::success;
  }

  void set_params(void const *params) override {
    Source::set_params(params);
    _params["broker_host"] = "localhost";
    _params["broker_port"] = 1883;
    _params["silent"] = true;
    _params["QoS"] = 0;
    _params["topic"] = "#";
    _params.merge_patch(*(json *)params);
  }

  map<string, string> info() override {
    return {
      {"Broker:", _params["broker_host"].get<string>() + ":" + to_string(_params["broker_port"])},
      {"Topic:", _params["topic"].get<string>()},
      {"QoS: ", to_string(_params["QoS"].get<int>())},
      {"Silent:", _params["silent"] ? "true" : "false"}
    };
  };

private:
  json _data;
  string _topic;
  bool _connected = false;
};


/*
  ____  _             _             _      _
 |  _ \| |_   _  __ _(_)_ __     __| |_ __(_)_   _____ _ __
 | |_) | | | | |/ _` | | '_ \   / _` | '__| \ \ / / _ \ '__|
 |  __/| | |_| | (_| | | | | | | (_| | |  | |\ V /  __/ |
 |_|   |_|\__,_|\__, |_|_| |_|  \__,_|_|  |_| \_/ \___|_|
                |___/
Enable the class as plugin
*/
INSTALL_SOURCE_DRIVER(Mqtt2madsPlugin, json)


/*
                  _
  _ __ ___   __ _(_)_ __
 | '_ ` _ \ / _` | | '_ \
 | | | | | | (_| | | | | |
 |_| |_| |_|\__,_|_|_| |_|

For testing purposes, when directly executing the plugin
*/
static bool running = true;

int main(int argc, char const *argv[]) {
  Mqtt2madsPlugin plugin;
  json output, params;

  params["broker_host"] = "localhost";
  params["broker_port"] = 1883;
  
  if (argc >= 2) {
    params["broker_host"] = argv[1];
  }

  if (argc >= 3) {
    params["broker_port"] = atoi(argv[2]);
  }

  params["topic"] = "#";
  params["silent"] = false;

  // Set parameters
  plugin.set_params(&params);

  signal(SIGINT, [](int s) {
    cout << "Interrupted" << endl;
    running = false;
  });

  // Process data
  while (running) {
    if (plugin.get_output(output) == return_type::success) 
      cout << "MQTT: " << output.dump(2) << endl;
  }
  
  return 0;
}
