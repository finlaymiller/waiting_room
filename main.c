/*
    - Dalhousie University
    - CSCI 3120 Assignment 3
    - The Doctor on Night Duty
    - Finlay Miller | B00675696

    NOTE: I wrote and tested this program on OSX. I didn't realize that for
    some reason, unnamed semaphores are deprecated. Thus, gcc does throw some
    warnings during compilation, but I'm not sure whether or not it will do so
    on other systems.

    -------- PROCESSES --------
    1 Doctor thread
    X Patient threads
    ^ User determined during startup

    -------- RESOURCES --------
    1 Examination room
    3 Waiting room seats
    Unlimited coffee
*/

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>

// Definitions
#define CHAIR_COUNT 3
#define PAT_COUNT_DEFAULT 5
#define SLEEP_MAX 10

// Function declarations
void *patient_loop(void* patient_id);
void *doctor_loop(void);
int is_waiting(int patient_id);
int isNumber(char number[]);

// Semaphores & MutEx
sem_t sem_pat;
sem_t sem_doc;
pthread_mutex_t exam_room;

// Global variables
int chairs[3];         // number of chairs in waiting room
int number_patients_waiting = 0;    // number of patients waiting
int next_seating_position = 0;      // next chair for patient to sit in
int next_examination_position = 0;  // next patient to be treated
int dr_sleep_flag = 0;              // 0 when doctor is awake | 1 when doctor is sleeping

int main(int argc, char **argv)
{
    /*
        Purpose:    Creates & destroys threads. Initializes variables.
        Input:      Number of patients to simulate
        Output:     0 on successful execution, 1 on failure
    */

	int i;              // global counter
	int patient_num;    // total number of patients

    // get number of patients
	printf("Enter number of patients to simulate:\n> ");
    scanf("%d", &patient_num);

    printf("Running simulation. Use ^C to exit.\n");

    // create array of patient IDs
	int patient_ids[patient_num];

    // threads
	pthread_t patients[patient_num];
	pthread_t doctor;

    // semaphores
	sem_init(&sem_pat, 0, 0);
	sem_init(&sem_doc, 0, 1);

	// create threads
	pthread_mutex_init(&exam_room, NULL);
	pthread_create(&doctor, NULL, doctor_loop, NULL);
	for(i = 0; i < patient_num; i++)
	{
		patient_ids[i] = i + 1;
		pthread_create(&patients[i], NULL, patient_loop, (void*) &patient_ids[i]);
	}

	// join threads
	pthread_join(doctor, NULL);
	for(i = 0; i < patient_num; i++)
	{
		pthread_join(patients[i], NULL);
	}

	return 0;
}

void *doctor_loop(void) 
{ 
    /*
        Purpose:    Simulates working doctor. Doctor will treat patients if
                    there are any in the waiting room. Otherwise he'll sleep
        Input:      None
        Output:     None
    */
	printf("[dr]\tChecking for patients.\n");

	while(1) 
    {
		// if patients are waiting
		if (number_patients_waiting > 0)
        {    
			dr_sleep_flag = 0;                // clear sleep flag
			sem_wait(&sem_pat);               // get next patient
			pthread_mutex_lock(&exam_room);   // lock exam room

			int treatment_time = rand() % SLEEP_MAX + 1;

            // print information
			printf("[dr]\tDoctor treating patient %d for %d seconds. %d patients waiting.\n", 
                chairs[next_examination_position], treatment_time, (number_patients_waiting - 1));

            // free up waiting room            
			chairs[next_examination_position] = 0;
			number_patients_waiting--;
			next_examination_position = (next_examination_position + 1) % CHAIR_COUNT;

            // actually treat patient
			sleep(treatment_time);

            // unlock exam room and free doctor
			pthread_mutex_unlock(&exam_room);
			sem_post(&sem_doc);

		}
		// if no patients are waiting
		else 
        {
            // let doctor sleep
			if (dr_sleep_flag == 0) 
            {
				printf("[dr]\tNo patients waiting. Sleeping.\n");
				dr_sleep_flag = 1;
			}
		}
	}
}

void* patient_loop(void* patient_id) 
{
    /*
        Purpose:    Simulates waiting patient. If there is room in the
                    waiting room the patient will sit and wait. Otherwise
                    they will drink coffee.
        Input:      Thread-based patient ID.
        Output:     None
    */

	int id = *(int*)patient_id; // local patient ID

	while(1) 
    {
		// if patient is already waiting, keep waiting
		if (is_waiting(id)) continue;

		// patient is drinking coffee
		int time = rand() % SLEEP_MAX + 1;
		printf("[pt]\tPatient %d is drinking coffee for %d seconds.\n", id, time);
		sleep(time);

        // make sure examination room is locked
		pthread_mutex_lock(&exam_room);

        // check waiting room for free chair
		if(number_patients_waiting < CHAIR_COUNT)
        {
            // if there is a free chair in the waiting room, sit in it
			chairs[next_seating_position] = id;
			number_patients_waiting++;
			printf("[pt]\tPatient %d takes a seat. %d other patients waiting.\n", id, number_patients_waiting - 1);
			next_seating_position = (next_seating_position + 1) % CHAIR_COUNT;

            // unlock examination room
			pthread_mutex_unlock(&exam_room);

			// wake doctor if sleeping
			sem_post(&sem_pat);
			sem_wait(&sem_doc);
		}
		else 
        {
            // no room in waiting room, drink more coffee
			pthread_mutex_unlock(&exam_room);
			printf("[pt]\tPatient %d will try later.\n", id);
		}
	}
}

int is_waiting(int patient_id) 
{
	/*
        Purpose:    Checks to see whether or not patient is currently in waiting room
        Input:      Thread-based patient ID.
        Output:     0 if patient is not currently waiting, 1 if he is
    */

    int i;

	for (i = 0; i < CHAIR_COUNT; i++) 
    {
		if (chairs[i] == patient_id) return 1;
	}

	return 0;
}
