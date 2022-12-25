# Raspberry PicoW DS18B20 code

A quick and dirty DS18B20 logger to MQTT.

This code is mostly on GitHub just so I can give an example of working minimal
code and build process, without needing a particular IDE. The Pico W LwIP
library is still a work in process and the socket support does not cleanly
compile, so for now this code does not do DNS resolution.

## Building

To compile, make sure ``PICO_SDK_PATH`` is set in your environment, if you don't
have a copy then just ``git clone https://github.com/raspberrypi/pico-sdk``
and ``export PICO_SDK_PATH=<new directory>``. You will also need to grab a copy
of the appropriate onewire library and add it to the modules directory within
the picow-DS18B20 source directory:

    picow-DS18B20 % mkdir -p modules
    modules % cd modules
    modules % git clone https://github.com/adamboardman/pico-onewire
    modules % cd ..
    picow-DS18B20 %

Then copy settings_example.h to settings.h, edit and run
the following commands to compile:

    picow-DS18B20 % mkdir build
    picow-DS18B20 % cd build
    build % cmake ..
    build % make

You can then copy the compiled picow_DS18B20.uf2 to your Pico. Some basic output
is sent to the serial port, which you can connect to with a terminal emulator.

## Hardware

This was built for use with waterproof sensors, such as [these](https://www.amazon.co.uk/gp/product/B00CHEZ250/),
which I use for monitoring an Aquarium via Telegraf/InfluxDB but any DS18b20
sensor will work. The red and black wires of the sensor should be connected
to the 3v3 and GND pins on the PicoW respectively, and the yellow signal wire
to a GPIO pi with a pull up resistor of abount 4k7ohms to 3.3v. The example
settings header file is set to use pin 22, labelled GP22 on the Pico itself.
There are [lots](https://how2electronics.com/interfacing-ds18b20-sensor-with-raspberry-pi-pico/)
[of](http://www.pibits.net/code/raspberry-pi-pico-and-ds18b20-thermometer-using-micropython.php)
[examples](https://microcontrollerslab.com/raspberry-pi-pico-ds18b20-thingspeak/)
online of how to do the eletronics part, but they mostly use Python and I
prefer C for embedded work.