### Clock constraints ###
create_generated_clock -source [get_ports RGB_PixelClk] -multiply_by 5 [get_ports SerialClk]