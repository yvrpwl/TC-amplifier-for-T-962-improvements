T-962-improvements
==================

Addition libraries for use with Unified Engineering's T-962 Reflow Oven controller improvements.

The new firmware from Unified Engineering make use of new MAX31850 One-Wire Thermocouple amplifier or fall back to the original OP-Amp/ADC design. This libraries allow the use of more common MAX318555 thermocouple amplifier. The MAX318555 is a SPI device, there isn't a free SPI port available on the original controller board. Instead, the I2C bus is used together with NXP's I2C-to-SPI bridge chip, SC18IS602B, to bridge the I2C bus to two MAX31855 chips. Patches are needed to the original main.c file for v.011 firmware and reflow.c for v.0.20 firmware.
