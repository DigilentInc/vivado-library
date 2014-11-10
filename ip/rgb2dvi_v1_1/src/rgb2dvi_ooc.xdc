create_clock -period 6.060 [get_ports RGB_PixelClk]
create_generated_clock -source [get_ports RGB_PixelClk] -multiply_by 5 [get_ports SerialClk]