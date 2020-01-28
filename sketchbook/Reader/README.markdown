# Paper Tape Reader Firmware

This runs on an Arduino Mega 1280/2650

I/O requirements

* 9 analog inputs from PIN diode to represent the 8 data + 1 sprocket channel
* 4 digital outputs to drrive 4 phase stepper
* 8 digital outpouts for 8 LED display of read data
* 4 digital inputs for control buttons
  + start
  + stop
  + step
  + mode

The system supports the following modes: [nn] for LED display
* [80] 8 bit with hex output, 2 digit hex (00..ff) for each character followed by linefeed
* [40] 7 bit ASCII output with NUL and DEL ignored
* [10] 5 bit with hex output, 2 digit hex (00..1f) for each character followed by linefeed
* [11] 5 bit ELLIOTT to ASCII
* [12] 5 bit ELLIOTT to UTF-8 with proper UK currency sign (UTF-8)
* [14] 5 bit ELLIOTT to UTF-8 with less(ASCII), greater(ASCII) and right-arrow(UTF-8)
* [81] 9 bit calibrate mode capture min/current/max of ADC channell for all 9 PIN diodes


Note that opening a comport will cause the Arduino to reset, so mode
must be set on each ne connection.  The 8 bit hex is the default mode.


The anaolog connection is simple a reversed biased PIN diode from +5V
to the analog input with a 10k pulldown resistor. 0.1 inch pith
perfboard aligns with the tape hole spacing so 9 PIN diodes are
mounted below and 9 IR LEDS above. Since the LEDs and diodes are 3mm
they all have to be filed down to 2.54mm to fit.
