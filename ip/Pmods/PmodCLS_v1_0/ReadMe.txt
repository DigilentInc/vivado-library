
while adding this pmod IP to a microblaze project you will not connect the ui_clk from the mig to the ext_spi_clock of the PmodCLS. 
Instead you will need to use an additional 50 MHZ output clock from the clocking wizard to connect to the ext_spi_clock on the 
PmodCLS IP core.