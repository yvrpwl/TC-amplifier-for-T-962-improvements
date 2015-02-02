T-962-improvements
==================

Addition hardware for use with Unified Engineering's T-962 Reflow Oven controller improvements.

The new firmware from Mr. Werner Johansson of Unified Engineering make use of new MAX31850 One-Wire Thermocouple amplifier or fall back to the original OP-Amp/ADC design. As of version 3.0, the firmware also allow the use of more common MAX318555 thermocouple amplifier. The MAX318555 is a SPI device, there isn't a free SPI port available on the original controller board. Instead, the I2C bus is used together with NXP's I2C-to-SPI bridge chip, SC18IS602B, to bridge the I2C bus to two MAX31855 chips. NO PATCHES are needed since V.3.0. If you are still on pre v.3.0 firmware, you should upgrade. Please see my blog for more details.

http://theembeddedworkshop.wordpress.com/my-projects-2/arm-projects/improving-the-t-962-reflow-oven/

