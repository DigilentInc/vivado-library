While adding this Pmod IP to a microblaze project you will not connect the ui_clk from the mig to 
the ext_spi_clock of the PmodCAN. Instead you will need to use an additional 50 MHZ output 
clock from the clocking wizard to connect to the ext_spi_clock on the PmodCAN IP core.

There are 3 demos LoopBack, RX,TX. The Loopback demo uses the loopback mode.
The rest of the demos use normal mode. The Demos use the PmodCAN to send data using 
the CAN communication protacol. The demos do not use interrupts but rather polling so be
aware of this.