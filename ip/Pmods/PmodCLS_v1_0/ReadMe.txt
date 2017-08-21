While adding this Pmod IP to a microblaze project you will not connect the ui_clk from the mig to the ext_spi_clock of the PmodCLS. 
Instead you will need to use an additional 50 MHZ output clock from the clocking wizard to connect to the ext_spi_clock on the 
PmodCLS IP core.
This demo sends data through spi to the PmodCLS. The demo alters the first string slighly so you can see that the data is changing 
between the different prints to the PmodCLS. Depending on the Rev of the PmodCLS the jumper configuration will be different. 
Make sure to refer to the Reference Manual for the correct jumper configuration.