#define _XOPEN_SOURCE 600 // needed for the Barrier

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 

int SharedVariable = 0;  // shared global variable 
pthread_barrier_t barr;  // barrier variable

// function simpleThread that does the threading process
void *SimpleThread(void *threadID) 
{
    int num, val;
    
    for(num = 0; num < 20; num++) 
    {
        if (random() > RAND_MAX / 2)
            usleep(10);
          
        val = SharedVariable;
        printf("*** thread %ld sees value %d\n", (long)threadID, val);
        SharedVariable = val + 1;
        
        // preprocessor commands to do the synchronisation
        #ifdef PTHREAD_SYNC
        // Synchronization point
        int rc = pthread_barrier_wait(&barr);
        if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
        {
            printf("Could not wait on barrier\n");
            exit(-1);
        }
        #endif
           
    }
    val = SharedVariable;
    printf("Thread %ld sees final value %d\n", (long)threadID, val);
   
}


int main (int argc, char *argv[])
{
    long numbThreads;
    
    if ( argc != 2 ) // argc should be 2 for correct execution
    {
        // We print argv[0] assuming it is the program name 
        printf( "Usage: %s <number of threads>\n", argv[0] );
        return -1;
        
    }

    // We assume argv[1] is a valid integer number
    numbThreads = strtol(argv[1], NULL, 10);

    // here we check for integer validation 
    if ( numbThreads == 0 )
    {
        printf( "Error: Please enter a valid integer number\n" );
        return -1;
    }
   
    pthread_t threads[numbThreads];  // array of threads
    int rc;
    long t;
    
    // Barrier initialization
    if(pthread_barrier_init(&barr, NULL, numbThreads))
    {
        printf("Could not create a barrier\n");
        return -1;
    }
    
    for(t = 0; t < numbThreads; t++)
    {
        printf("In main: creating thread %ld\n", t);
        rc = pthread_create(&threads[t], NULL, SimpleThread, (void *)t);
        if (rc) 
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
       
   /* Last thing that main() should do */
   pthread_exit(NULL);

}
