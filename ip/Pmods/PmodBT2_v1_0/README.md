PmodDPG1 IP
==============

Using the IP in Vivado
--------------

A tutorial for using Pmod IP cores in Vivado is available [here](https://reference.digilentinc.com/learn/programmable-logic/tutorials/pmod-ips/start).

When creating a microblaze design, make sure that the axi_uart_lite core connected
to the usb_uart port is configured to use a baud rate of 115200, rather than the
typical 9600. This will ensure that there will be enough down-stream bandwidth to
forward all characters received from the BT2.

An interrupt connection is NOT required for normal operation of this IP core.

Demo Program in Xilinx SDK
--------------

This demo continuously polls the Pmod BT2 and host development board's UART connections
and forwards each character received from each to the other.

To set up the demo you will need to open a serial terminal, such as TeraTerm, to
see the data being printed out. Settings for the terminal may vary depending on
your design. If the 

For Zynq projects, apply the following settings:
- Baud rate: 115200
- Data bits: 8
- Parity:    none
- Stop bits: 1

For MicroBlaze projects, apply the settings according to the AXI Uartlite IP
configurations. These settings can be found by double clicking the AXI Uartlite
IP in the block design and clicking the IP Configuration tab.