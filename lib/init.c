#include "conf.h"
#include "log.h"

void __attribute__ ((constructor)) bb_init(void)
{
	init_logging();
}

void __attribute__ ((destructor)) bb_fini(void)
{
	fini_logging();
}
