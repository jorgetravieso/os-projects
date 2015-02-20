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

//long online = 0;
int first = 0;
int reporter_id;




sem_t room_sem;

pthread_mutex_t reporter_mutex;
pthread_mutex_t floor_mutex;

pthread_cond_t floor_condition;
pthread_cond_t enter_leave_condition;






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

    pthread_mutex_init(&floor_mutex, NULL);
    pthread_mutex_init(&reporter_mutex, NULL);
    pthread_cond_init (&floor_condition, NULL);
    pthread_cond_init (&enter_leave_condition, NULL);

    sem_init(&room_sem, 0, room_capacity);

    pthread_t reporters_threads[num_reporters];
    pthread_t speaker_thread;


    pthread_create(&speaker_thread, NULL, Speaker, NULL);      
    first = 1;

    int i = 0;
    for(i; i < num_reporters; i++) // create the reporters
        pthread_create(&reporters_threads[i], NULL, Reporter, i);



    for(i = 0 ; i < num_reporters; i++) // make sure all reporters are done
        pthread_join(reporters_threads[i], NULL);


    //sem_destroy(&room_sem);
    
    return 0;
}



void * Speaker()
{

    pthread_mutex_lock(&floor_mutex);
    while(1){
        pthread_cond_wait(&floor_condition, &floor_mutex);
        AnswerStart();
        AnswerDone();
        pthread_cond_signal(&floor_condition);
        
    }
   pthread_mutex_unlock(&floor_mutex);
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
        QuestionStart(id);
        QuestionDone(id);
        if(i != questions - 1)
           usleep(30);                     
       
    }
    LeaveConferenceRoom(id);
    
}

void AnswerStart()
{

    printf("Speaker starts to answer question for reporter %d\n", reporter_id);
}

void AnswerDone()
{
    printf("Speaker is done with answer for reporter %d\n", reporter_id);
    
}

//ok
void EnterConferenceRoom(int id)
{ 
    
 
    
    sem_wait(&room_sem);
    
    pthread_mutex_lock(&floor_mutex);
  
    
    if(!first){
        pthread_cond_wait(&enter_leave_condition, &floor_mutex);
    }
    else{
        first = false;
    }
    
    printf("Reporter %d enters the conference room.\n", id);
    pthread_mutex_unlock(&floor_mutex);
    
    
}

//ok
void LeaveConferenceRoom(int id)
{
    //pthread_cond_wait(&enter_leave_condition, &floor_mutex);
    printf("Reporter %d leaves the conference room.\n", id);
    sem_post(&room_sem);
}

void QuestionStart(int id)
{
    pthread_mutex_lock(&reporter_mutex);
    pthread_mutex_lock(&floor_mutex);
    printf("Reporter %d asks a question.\n", id);
    reporter_id = id;
    pthread_cond_signal(&floor_condition);
    pthread_cond_wait(&floor_condition, &floor_mutex);

}

void QuestionDone(int id)  
{
    
     printf("Reporter %d is satisfied.\n", id);
     pthread_mutex_unlock(&floor_mutex);
     pthread_mutex_unlock(&reporter_mutex);
     pthread_cond_signal(&enter_leave_condition);
}