#ifndef MODULE
#define MODULE
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif


#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>

#define GPFSEL_ZERO	0x3F200000
#define GPSET_OFF	0x3F200028
#define GPSET_ON	0x3F20001C

int init_module(void)
{
	// WiringPiSetup
	// Map pointers to specific registers for funcitonality
	unsigned long* select = (unsigned long*) ioremap(GPFSEL_ZERO, 4096);
	unsigned long* on = (unsigned long*) ioremap(GPSET_ON, 4096);

	// pinMode
	// Set the registers for output : LED's
	*select = *select|0x00009240;
	// iowrite32(*select, select);

	// digitalWrite
	// Turn the LED's on
	*on = *on|0x0000003A;
	// iowrite32(*on, on);

	return 0;

} // END init_module

void cleanup_module(void)
{
	// Map to registers
	unsigned long* select = (unsigned long*) ioremap(GPFSEL_ZERO, 4096);
	unsigned long* off = (unsigned long*) ioremap(GPSET_OFF, 4096);

	// Set registers for output
	*select = *select|0x00009240;
	// iowrite32(*select, select);

	// Turn off LED's
	*off = *off|0x0000003A;
	// iowrite32(*off, off);

} // END clearup_module
