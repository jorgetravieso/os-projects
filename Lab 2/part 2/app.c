// COMPILE LIKE THIS: gcc guido.c -lpthread -lrt -w -o guido

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#define MAXREPORTERS 100
#define MAXCAPACITY 10

void * Speaker();
void * Reporter();
void AnswerStart();
void AnswerDone();
void EnterConferenceRoom(int id);
void LeaveConferenceRoom(int id);
void QuestionStart(int id);
void QuestionDone(int id);

long online = 0;
int reporter_id;


pthread_mutex_t room_mutext;
pthread_cond_t room_condition_cv;




pthread_mutex_t question;
sem_t room_sem;

pthread_mutex_t speak_mutex;
pthread_cond_t speak_condition_cv;


pthread_mutex 





int main(int argc, char* argv[])
{

    long num_reporters;
    long room_capacity;

    if(argc != 3)
    {
        printf("Usage: %s <number_of_reporters> <conference_room_capacity>\n", argv[0]);
        return -1;
    }
    
    if((num_reporters = strtol(argv[1], NULL, 10)) == 0)
    {
        printf("Invalid range for reporters! Please use (0 - %d)\n", MAXREPORTERS);
        return -1;
    }
    
    if((room_capacity = strtol(argv[2], NULL, 10)) == 0)
    {
        printf("Invalid range for room capacity! Please use (0 - %d)\n", MAXCAPACITY);
        return -1;
    }
    
    printf("You entered [%lu] for number of reporters, and [%lu] for max capacity!\n", num_reporters, room_capacity);

    pthread_mutex_init(&speak_mutex, NULL);
    pthread_mutex_init(&question, NULL);
    pthread_cond_init (&speak_condition_cv, NULL);
    sem_init(&room_sem, 0, room_capacity);

    pthread_t reporters_threads[num_reporters];
    pthread_t speaker_thread[1];


    pthread_create(&speaker_thread[0], NULL, Speaker, NULL);    
    int i = 0;
    for(i; i < num_reporters; i++) // create the reporters
        pthread_create(&reporters_threads[i], NULL, Reporter, i);



    for(i = 0 ; i < num_reporters; i++) // make sure all reporters are done
        pthread_join(reporters_threads[i], NULL);


    sem_destroy(&room_sem);
    
    return 1;
}



void * Speaker()
{

    pthread_mutex_lock(&speak_mutex);
    while(1){
        pthread_cond_wait(&speak_condition_cv, &speak_mutex);
        AnswerStart(reporter_id);
        AnswerDone(reporter_id);
    }
    pthread_mutex_unlock(&speak_mutex);
    pthread_exit(NULL);
    
}

void * Reporter(void *p)
{
    int id = (int)p;
    int questions = (id % 4) + 2;

    

    EnterConferenceRoom(id);
    
    int i;
    for(i = 0; i < questions; i++)
    {
        pthread_mutex_lock(&question);
        reporter_id = id;

        QuestionStart(id);
        
        pthread_cond_signal(&speak_condition_cv);
        usleep(100);
        QuestionDone(id);
        pthread_mutex_unlock(&question);
    }
    

    LeaveConferenceRoom(id);
    
}

void AnswerStart(int id)
{
    printf("Speaker starts to answer question for reporter %d\n", id);
}

void AnswerDone(int id)
{
    printf("Speaker is done with answer for reporter %d\n", id);
}

//ok
void EnterConferenceRoom(int id)
{ 
     sem_wait(&room_sem);
     printf("Reporter %d enters the conference room.\n", id);
}

//ok
void LeaveConferenceRoom(int id)
{
    printf("Reporter %d leaves the conference room.\n", id);
    sem_post(&room_sem);
}

void QuestionStart(int id)
{
    printf("Reporter %d asks a question.\n", id);
}

void QuestionDone(int id)  
{
    printf("Reporter %d is satisfied.\n", id);
}

