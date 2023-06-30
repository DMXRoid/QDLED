# QDLED
A quick and dirty HTTP-managed LED controller for ESP8266 chips (and prolly others)

# Configuration

In `config.cpp`, there are a couple values you'll need to set for your own usage:

* Wifi SSID and password
* The pin on your chip you want data to come out of

You'll also need to change the light chipset you're using in `lights.cpp` if you aren't using WS2812's.

The light count can be set dynamically if you want to save some cycles on the loops that iterate over the array.  Even without doing that, CPU and power consumption an an ESP-01S are negligible, so
it's probably an over-optimization.

# Build

Use the Ardunio IDE or CLI.
