#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<wiringPi.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#define NUM_THREADS 3

#define MY_PRIORITY 51  // kernel is priority 50

//declarations
void * thread1();
void * thread2();
void * thread3();

int fd;		

sem_t sema; 
volatile unsigned long btn_pressed;
unsigned long ptr;

void main()
{	//sema init
	sem_init(&sema, 0, 1);
	
	wiringPiSetup();
	wiringPiSetupGpio();
	
	//configuring button
	pinMode(16, INPUT); //making push button an input
	pullUpDnControl(16, PUD_DOWN); //pull down resistor initialized
	
	//configuring leds
	pinMode(2, OUTPUT); //red LED pin
	pinMode(3, OUTPUT); //yellow
	pinMode(4, OUTPUT); //green
	
	digitalWrite(3, LOW);
	
	//creating threads
	pthread_t threads[NUM_THREADS];
	
	pthread_create(&threads[0], NULL, thread1, NULL);
	pthread_create(&threads[1], NULL, thread2, NULL);
	pthread_create(&threads[2], NULL, thread3, NULL);
	
	for(int i =0; i < 3; i++)
	{
		pthread_join(threads[i], NULL);
	}
	
	
	sem_destroy(&sema);
	pthread_exit(NULL);
	
}

void * thread1()
{
	struct sched_param param;
	param.sched_priority = MY_PRIORITY;
	int ret = sched_setscheduler(0, SCHED_FIFO, &param);
	if(ret == -1)
	{
		printf("Error with scheduler\n");
	}

	while(1)
	{
		//lcok sema
		sem_wait(&sema);
		printf("Thread 1 grabbed semaphile\n");
		//turn on
		digitalWrite(4, HIGH);		
		//sleep
		delay(3000); //delaying two seconds
		//turn off
		digitalWrite(4, LOW);
		//release semi
		sem_post(&sema);
		delay(1000);
	}
	
}

void * thread2()
{
	struct sched_param param;
	param.sched_priority = MY_PRIORITY;
	int ret = sched_setscheduler(0, SCHED_FIFO, &param);
	if(ret == -1)
	{
		printf("Error with scheduler\n");
	}

	while(1)
	{
		//lcok sema
		sem_wait(&sema);
		printf("Thread 2 grabbed semaphile\n");
		//turn on
		digitalWrite(3, HIGH);
		//sleep
		delay(3000); //delaying two seconds
		//turn off
		digitalWrite(3, LOW);
		//release semi
		sem_post(&sema);
		delay(1000);
	}
}

void * thread3()
{
	struct sched_param param;
	param.sched_priority = MY_PRIORITY + 1; //to be most important priority
	int ret = sched_setscheduler(0, SCHED_FIFO, &param);
	if(ret == -1)
	{
		printf("Error with scheduler\n");
	}

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(fd < 0)
	{
		printf("can't open dev/mem\n");
		exit(1);
	}
	unsigned long * ptr = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
	if(ptr == MAP_FAILED)
	{
		printf("mmap error\n");
		exit(1);
	}
	close(fd);
	ptr = ptr + (0x00000040/4);

	while(1)
	{
		//printf("ptr (mapped) = %08x\n", btn_pressed);

		sem_wait(&sema);		
		btn_pressed = *ptr;

		if(*ptr == 0x00010000)
		{	
			
			//lcok sema	
			printf("Thread 3 grabbed semaphile\n");
			//turn on
			digitalWrite(2, HIGH);
			//sleep
			delay(5000); //delaying two seconds
			//turn off
			digitalWrite(2, LOW);
			//release semi
			*ptr = 0xFFFFFFFF;
			delay(1000);
		}
		sem_post(&sema);
	}
}
