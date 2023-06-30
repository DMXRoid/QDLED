# QDLED
A quick and dirty HTTP-managed LED controller for ESP8266 chips (and prolly others)

# Configuration

In `config.cpp`, there are a couple values you'll need to set for your own usage:

* Wifi SSID and password
* The pin on your chip you want data to come out of

You'll also need to change the light chipset you're using in `lights.cpp` if you aren't using WS2812's.

# Build

Use the Ardunio IDE or CLI.
