#include "xparameters.h"
#include "xil_cache.h"

extern void setup();
extern void loop();

void EnableCaches();
void DisableCaches();

int main ()
{
	EnableCaches();
	setup();
	while(1)
	{
		loop();
	}
	return 0;
}


void EnableCaches()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
	Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches()
{
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
	Xil_ICacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
	Xil_DCacheDisable();
#endif
#endif
}
