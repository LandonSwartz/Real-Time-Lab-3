/* ece4220lab1_isr.c
 * ECE4220/7220
 * Author: Luis Alberto Rivera
 
 Basic steps needed to configure GPIO interrupt and attach a handler.
 Check chapter 6 of the BCM2837 ARM Peripherals manual for details about
 the GPIO registers, how to configure, set, clear, etc.
 
 Note: this code is not functional. It shows some of the actual code that you need,
 but some parts are only descriptions, hints or recommendations to help you
 implement your module.
 
 You can compile your module using the same Makefile provided. Just add
 obj-m += YourModuleName.o
 */

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Landon Swartz");

/* Declare your pointers for mapping the necessary GPIO registers.
   You need to map:
   
   - Pin Event detect status register(s)
   - Rising Edge detect register(s) (either synchronous or asynchronous should work)
   - Function selection register(s)
   - Pin Pull-up/pull-down configuration registers
   
   Important: remember that the GPIO base register address is 0x3F200000, not the
   one shown in the BCM2835 ARM Peripherals manual.
*/

int mydev_id;	// variable needed to identify the handler
unsigned long memory_size = 4096; //memory of virtual regs

// Interrupt handler function. Tha name "button_isr" can be different.
// You may use printk statements for debugging purposes. You may want to remove
// them in your final code.
static irqreturn_t button_isr(int irq, void *dev_id)
{
	unsigned long *gpclr0;
	unsigned long *gpset0;
	unsigned long *gpeds0;
	gpset0 = (unsigned long*)ioremap(0x3F20001C, 1024);
	gpclr0 = (unsigned long*)ioremap(0x3F200028, 4096);
	gpeds0 = (unsigned long*)ioremap(0x3F200040, 1024);
	

	// In general, you want to disable the interrupt while handling it.
	disable_irq_nosync(79);

	// This same handler will be called regardless of what button was pushed,
	// assuming that they were properly configured.
	// How can you determine which button was the one actually pushed?
		
	// DO STUFF (whatever you need to do, based on the button that was pushed)
	
	if(ioread32(gpeds0) == 0x00100000)		//Button 5
	{
		iowrite32(0x0000003C,gpclr0); 		//Turns off all of the LEDs
		printk("Button 5");
	}
	else if(ioread32(gpeds0) == 0x00080000)		//Button 4
	{
		iowrite32(0x00000020,gpset0); 		//Turns on Blue
		printk("Button 4");
	}
	else if(ioread32(gpeds0) == 0x00040000)		//Button 3
	{
		iowrite32(0x00000010,gpset0); 		//Turns on Green
		printk("Button 3");
	}
	else if(ioread32(gpeds0) == 0x00020000)		//Button 2
	{
		iowrite32(0x00000008,gpset0); 		//Turns on Yellow
		printk("Button 2");
	}
	else if(ioread32(gpeds0) == 0x00010000)		//Button 1
	{
		iowrite32(0x00000004,gpset0); 		//Turns on Red
		printk("Button 1");
	}

	// IMPORTANT: Clear the Event Detect status register before leaving.
	iowrite32(0xFFFFFFFF, gpeds0); //clearing every bit of registers with the 0xFFFFFFFF command
	
	//clearing maps
	iounmap(gpclr0);
	iounmap(gpset0);
	iounmap(gpeds0);
	
	printk("Interrupt handled\n");	
	enable_irq(79);		// re-enable interrupt
	
    return IRQ_HANDLED;
}

int init_module()
{
	//pointers of registers
	unsigned long *gpfsel0;
	unsigned long *gpfsel1;
	unsigned long *gpfsel2;
	unsigned long *gpset0;
	unsigned long *gpclr0;
	unsigned long *gparen0;
	unsigned long *gppud;
	unsigned long *gppudclk0;
	unsigned long *gpeds0;

	int dummy = 0;

	//mapping registers
	gpclr0 = (unsigned long*)ioremap(0x3F200028, 4096);
	gpfsel0 = (unsigned long*)ioremap(0x3F200000, 1024);
	gpfsel1 = (unsigned long*)ioremap(0x3F200004, 1024);
	gpfsel2 = (unsigned long*)ioremap(0x3F200008, 1024);
	gpset0 = (unsigned long*)ioremap(0x3F20001C, 1024);
	gpclr0 = (unsigned long*)ioremap(0x3F200028, 4096);
	gparen0 = (unsigned long*)ioremap(0x3F20007C, 1024);
	gppud = (unsigned long*)ioremap(0x3F200094, 1024);
	gppudclk0 = (unsigned long*)ioremap(0x3F200098, 1024);
	gpeds0 = (unsigned long*)ioremap(0x3F200040, 1024);
	

	//Step One
	//=========
	// Map GPIO registers
	// Remember to map the base address (beginning of a memory page)
	// Then you can offset to the addresses of the other registers

	// Don't forget to configure all ports connected to the push buttons as inputs.

	iowrite32(0x00009240,gpfsel0);
	iowrite32(0x00000000,gpfsel1);
	iowrite32(0x00000000,gpfsel2);
	
	//Step Two
	//=========
	// You need to configure the pull-downs for all those ports. There is
	// a specific sequence that needs to be followed to configure those pull-downs.
	// The sequence is described on page 101 of the BCM2837-ARM-Peripherals manual.
	// You can use  udelay(100);  for those 150 cycles mentioned in the manual.
	// It's not exactly 150 cycles, but it gives the necessary delay.
	// WiringPi makes it a lot simpler in user space, but it's good to know
	// how to do this "by hand".

	iowrite32(0x00000001,gppud);
	udelay(100);
	iowrite32(0x001F0000,gppudclk0);
	udelay(100);
	iowrite32(0x00000000,gppud);
	iowrite32(0x00000000,gppudclk0);
	
		
	//Step 3
	//=========
	// Enable (Async) Rising Edge detection for all 5 GPIO ports.

	iowrite32(0x001F0000,gparen0);
			
	// Request the interrupt / attach handler (line 79, doesn't match the manual...)
	// The third argument string can be different (you give the name)
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);
	
	//unmap memories
	iounmap(gpfsel0);
	iounmap(gpfsel1);
	iounmap(gpfsel2);
	iounmap(gpset0);
	iounmap(gpclr0);
	iounmap(gparen0);
	iounmap(gppud);
	iounmap(gppudclk0);
	iounmap(gpeds0);

	printk("Button Detection enabled.\n");
	return 0;
}

// Cleanup - undo whatever init_module did
void cleanup_module()
{
	//doing same init with pointers and mapping
	unsigned long *gpfsel0;
	unsigned long *gpfsel1;
	unsigned long *gpfsel2;
	unsigned long *gpset0;
	unsigned long *gpclr0;
	unsigned long *gparen0;
	unsigned long *gppud;
	unsigned long *gppudclk0;
	unsigned long *gpeds0;

	gpfsel0 = (unsigned long*)ioremap(0x3F200000, 1024);
	gpfsel1 = (unsigned long*)ioremap(0x3F200004, 1024);
	gpfsel2 = (unsigned long*)ioremap(0x3F200008, 1024);
	gpset0 = (unsigned long*)ioremap(0x3F20001C, 1024);
	gpclr0 = (unsigned long*)ioremap(0x3F200028, 4096);
	gparen0 = (unsigned long*)ioremap(0x3F20007C, 1024);
	gppud = (unsigned long*)ioremap(0x3F200094, 1024);
	gppudclk0 = (unsigned long*)ioremap(0x3F200098, 1024);
	gpeds0 = (unsigned long*)ioremap(0x3F200040, 1024);
	
	// Good idea to clear the Event Detect status register here, just in case.
	iowrite32(0xFFFFFFFF, gpeds0);
		
	// Disable (Async) Rising Edge detection for all 5 GPIO ports.
	iowrite32(0x00000000,gparen0);
	
	// Remove the interrupt handler; you need to provide the same identifier
    	free_irq(79, &mydev_id);
    	
    	iowrite32(0x3C, gpclr0);
    	
    	//unmap memories
	iounmap(gpfsel0);
	iounmap(gpfsel1);
	iounmap(gpfsel2);
	iounmap(gpset0);
	iounmap(gpclr0);
	iounmap(gparen0);
	iounmap(gppud);
	iounmap(gppudclk0);
	iounmap(gpeds0);
	
	printk("Button Detection disabled.\n");
}
