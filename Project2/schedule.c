/* schedule.c
 * This file contains the primary logic for the 
 * scheduler.
 */
#include "schedule.h"
#include "macros.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include <float.h>
#define a 0.5
#define GOODNESS 
//#define PRINT_ON
#define NEWTASKSLICE (NS_TO_JIFFIES(100000000))
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"
/* Local Globals
 * rq - This is a pointer to the runqueue that the scheduler uses.
 * current - A pointer to the current running task.
 */
struct runqueue *rq;
struct task_struct *current;

/* External Globals
 * jiffies - A discrete unit of time used for scheduling.
 *			 There are HZ jiffies in a second, (HZ is 
 *			 declared in macros.h), and is usually
 *			 1 or 10 milliseconds.
 */
extern long long jiffies;
extern struct task_struct *idle;

/*-----------------Initilization/Shutdown Code-------------------*/
/* This code is not used by the scheduler, but by the virtual machine
 * to setup and destroy the scheduler cleanly.
 */
 
 /* initscheduler
  * Sets up and allocates memory for the scheduler, as well
  * as sets initial values. This function should also
  * set the initial effective priority for the "seed" task 
  * and enqueu it in the scheduler.
  * INPUT:
  * newrq - A pointer to an allocated rq to assign to your
  *			local rq.
  * seedTask - A pointer to a task to seed the scheduler and start
  * the simulation.
  */
void initschedule(struct runqueue *newrq, struct task_struct *seedTask){
    #ifdef PRINT_ON
	printf("INITIALIZATION\n");
    #endif
	seedTask->next = seedTask->prev = seedTask;
	newrq->head = seedTask;
	newrq->nr_running++;
}

/* killschedule
 * This function should free any memory that 
 * was allocated when setting up the runqueu.
 * It SHOULD NOT free the runqueue itself.
 */
void killschedule(){
	return;
}
void print_rq() {
	struct task_struct *curr;
	
	printf("Rq: \n");
	curr = rq->head;
	if (curr)
		printf("%p", curr);
	while(curr->next != rq->head) {
		curr = curr->next;
		printf(", %p", curr);
	};
	printf("\n");
}
void myprint(){
	struct task_struct *curr;
	
	if(rq->head->next!=rq->head){
		printf("**********************************************Rq:**********************************************\n");
		for(curr=rq->head->next;;curr=curr->next){
			printf(ANSI_COLOR_RED"Start_time=%lld - current_expected_burst=%f - previous_expected_burst=%f - waiting_time_in_rq=%f - burst=%f\n"ANSI_COLOR_RESET,curr->start_process_time,curr->current_expected_burst,curr->previous_expected_burst,(sched_clock()/1000000-curr->waiting_time_rq),curr->burst);
			if(curr->next==rq->head){break;}
		}
	}
}
/*-------------Scheduler Code Goes Below------------*/
/* This is the beginning of the actual scheduling logic */

/* schedule
 * Gets the next task in the queue
 */
void schedule(){
	struct task_struct *curr,*search_node;
	long long int time;
	double min=DBL_MAX,goodness;
	double min_expected_burst,max_waiting_time_in_rq;
	
	time=sched_clock()/1000000;
    #ifdef PRINT_ON
	printf("In schedule,%lldms\n",time);
    #endif
	//print_rq();
	current->need_reschedule = 0; /* Always make sure to reset that, in case *
								   * we entered the scheduler because current*
								   * had requested so by setting this flag   */
	if (rq->nr_running == 1) {
		context_switch(rq->head);
	}
	else{
		if(current!=rq->head){
			current->waiting_time_rq=sched_clock()/1000000;
		}
		#ifdef PRINT_ON
		myprint();
		#endif
		//Recalculate expected burst value for all processes
		for(search_node=rq->head->next;;search_node=search_node->next){
			search_node->current_expected_burst=(((search_node->previous_expected_burst)*a) + search_node->burst)/(1+a);
			if(search_node->next==rq->head){break;}
		}
		//Find minimum expected burst and maximum waiting in ready queue values
		min_expected_burst=find_expected_burst_min();
        	#ifdef GOODNESS
		max_waiting_time_in_rq=find_waiting_rq_max();
		//Calculate goodness value for all processes in the run queue and find the process with the smallest goodness value
		for(search_node=rq->head->next;;search_node=search_node->next){
			goodness=((1 + search_node->current_expected_burst)*max_waiting_time_in_rq)/(min_expected_burst*(1 + sched_clock()/1000000 - search_node->waiting_time_rq));
			if(goodness < min){
				min=goodness;
				curr=search_node;
			}
			if(search_node->next==rq->head){break;}
		}
		#endif
		#ifndef GOODNESS
		//Find the process with the smallest expected burst value
		for(search_node=rq->head->next;;search_node=search_node->next){
			if(search_node->current_expected_burst < min){
				min=((search_node->current_expected_burst)+1)/min_expected_burst;
				curr=search_node;
			}
			if(search_node->next==rq->head){break;}
		}
		#endif
		if(current!=curr){
			//Save burst value for current process,set previous_expected_burst to current_expected_burst for the current process for future calculations
			current->waiting_time_rq=sched_clock()/1000000;
			current->burst=time - current->start_process_time;
			current->previous_expected_burst=current->current_expected_burst;
			curr->start_process_time=time;
		}
		//Switch to the process with the minimum expected burst value or to the process with minimum goodness value
		context_switch(curr);
	}
}
/* sched_fork
 * Sets up schedule info for a newly forked task
 */
void sched_fork(struct task_struct *p){
	p->time_slice = 100;
}

/* scheduler_tick
 * Updates information and priority
 * for the task that is currently running.
 */
void scheduler_tick(struct task_struct *p){
	schedule();
}

/* wake_up_new_task
 * Prepares information for a task
 * that is waking up for the first time
 * (being created).
 */
void wake_up_new_task(struct task_struct *p){
	
	#ifdef PRINT_ON
	printf("CREATION\n");
	#endif
	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;
	p->previous_expected_burst=0.0;               
	p->start_process_time=0;
	p->burst=0;
	p->current_expected_burst=((p->previous_expected_burst)*a)/(1+a);
	p->waiting_time_rq=sched_clock()/1000000;
	rq->nr_running++;
}
/* activate_task
 * Activates a task that is being woken-up
 * from sleeping.
 */
void activate_task(struct task_struct *p){
	
	#ifdef PRINT_ON
	printf("ACTIVATE\n");
	#endif
	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;
	p->previous_expected_burst=0.0;                 
	p->start_process_time=0;
	p->burst=0;
	p->waiting_time_rq=sched_clock()/1000000;
	p->current_expected_burst=((p->previous_expected_burst)*a)/(1+a);
	rq->nr_running++;
}
/* deactivate_task
 * Removes a running task from the scheduler to
 * put it to sleep.
 */
void deactivate_task(struct task_struct *p){
	#ifdef PRINT_ON
	printf("DEACTIVATE\n");
	#endif
	p->prev->next = p->next;
	p->next->prev = p->prev;
	p->next = p->prev = NULL; /* Make sure to set them to NULL *
							   * next is checked in cpu.c      */
	rq->nr_running--;
}
//This function calculates the minimum expected burst value + 1
double find_expected_burst_min(){
	double min=DBL_MAX;
	struct task_struct *search_node;
	for(search_node=rq->head->next;;search_node=search_node->next){                //find min for goodness
		if((search_node->current_expected_burst +1) < min){
			min=search_node->current_expected_burst +1;
		}
		if(search_node->next==rq->head){break;}
	}
	return min;
}
//This function calculates the maximum waiting in ready queue time value + 1
double find_waiting_rq_max(){
	double max=0.0;
	struct task_struct *search_node;
	for(search_node=rq->head->next;;search_node=search_node->next){                //find max for goodness
		if(((sched_clock()/1000000)-search_node->waiting_time_rq +1) > max){
			max=sched_clock()/1000000-search_node->waiting_time_rq +1;
		}
		if(search_node->next==rq->head){break;}
	}
	return max;
}
