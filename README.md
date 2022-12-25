# Raspberry PicoW DS18B20 code

A quick and dirty DS18B20 logger to MQTT. To compile, make sure PICO_SDK_PATH
is set in your environment, copy settings_example.h to settings.h, edit and run
the following commands to compile:

    $ mkdir -p modules
    $ cd modules
    $ git checkout https://github.com/adamboardman/pico-onewire
    $ cd ..
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

You can then copy the compiled picow_DS18B20.uf2 to your Pico.

This code is mostly on GitHub just so I can give an example of working minimal
code to others. The Pico W LwIP library is still a work in process and the
socket support does not cleanly compile, so for now this code does not do DNS
resolution. 