#include<stdio.h>
#include<wiringPi.h>
#include<pthread.h>
#include<semaphore.h>

#define NUM_THREADS 3

#define MY_PRIORITY 51  // kernel is priority 50

//declarations
void * thread1();
void * thread2();
void * thread3();

//u32int_t *ptr = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
//int fd = open("/dev/mem", ORDWR | O_SYNC);

sem_t sema; 
int btn_pressed;

void main()
{	//sema init
	sem_init(&sema, 0, 1);
	
	wiringPiSetup();
	//configuring button
	pinMode(16, INPUT); //making push button an input
	pullUpDnControl(16, PUD_DOWN); //pull down resistor initialized
	
	//configuring leds
	pinMode(2, OUTPUT); //LED pin
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
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
		digitalWrite(2, HIGH);		
		//sleep
		delay(2); //delaying two seconds
		//turn off
		digitalWrite(2, LOW);
		//release semi
		sem_post(&sema);
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
		delay(2); //delaying two seconds
		//turn off
		digitalWrite(3, LOW);
		//release semi
		sem_post(&sema);
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

	while(1)
	{
		if(btn_pressed)
		{
			//lcok sema
			sem_wait(&sema);
			printf("Thread 3 grabbed semaphile\n");
			//turn on
			digitalWrite(3, HIGH);
			//sleep
			delay(2); //delaying two seconds
			//turn off
			digitalWrite(3, LOW);
			//release semi
			sem_post(&sema);
		}
	}
}
