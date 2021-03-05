#include<stdio.h>
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
int btn_pressed;

void main()
{	//sema init
	sem_init(&sema, 0, 1);

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	unsigned long * ptr = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
	
	wiringPiSetup();
	//configuring button
	pinMode(27, INPUT); //making push button an input
	pullUpDnControl(27, PUD_DOWN); //pull down resistor initialized
	
	//configuring leds
	pinMode(8, OUTPUT); //red LED pin
	pinMode(9, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(5, OUTPUT);
	
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
		digitalWrite(9, HIGH);		
		//sleep
		delay(3000); //delaying two seconds
		//turn off
		digitalWrite(9, LOW);
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
		digitalWrite(7, HIGH);
		//sleep
		delay(3000); //delaying two seconds
		//turn off
		digitalWrite(7, LOW);
		//release semi
		sem_post(&sema);
		delay(1000);
	}
}

void * thread3()
{
	struct sched_param param;
	param.sched_priority = MY_PRIORITY;
	int ret = sched_setscheduler(0, SCHED_FIFO, &param);
	if(ret == -1)
	{
		printf("Error with scheduler\n");
	}

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	unsigned long * ptr = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
	printf("%x = ptr\n", ptr);
	ptr = ptr + 0x40;
	printf("%x = ptr\n", ptr); 



	while(1)
	{
		//btn_pressed = digitalRead(27);
		btn_pressed = ioread32(ptr);
		printf("btn_pressed = %x\n", btn_pressed); 

		if(btn_pressed)
		{	
			
			//lcok sema
			sem_wait(&sema);
			printf("Thread 3 grabbed semaphile\n");
			//turn on
			digitalWrite(8, HIGH);
			//sleep
			delay(5000); //delaying two seconds
			//turn off
			digitalWrite(8, LOW);
			//release semi
			sem_post(&sema);
			delay(1000);
		}
	}
}
