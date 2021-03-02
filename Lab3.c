#include<stdio.h>
#include<WiringPi.h>
#include<pthread.h>

#define NUM_THREADS 3

// u32int_t *ptr = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
//int fd = open("/dev/mem", ORDWR | O_SYNC);

void main()
{
	wiringPiSetup();
	pinMode(16, INPUT); //making push button an input

		
}
