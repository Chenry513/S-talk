#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <pthread.h>
#include "monRoutines.h"
#include "list.h"


//use these as monitor routines
static pthread_mutex_t montex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sendListEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t listFull = PTHREAD_COND_INITIALIZER;
pthread_cond_t recListEmpty = PTHREAD_COND_INITIALIZER;

int monPrependSend(List* list, char* message)
{
    pthread_mutex_lock(&montex);
    {
        //prepending only fails when list is full
        while(List_prepend(list, message) == -1) {
            pthread_cond_wait(&listFull, &montex);
        }
        pthread_cond_signal(&sendListEmpty);
    }
    pthread_mutex_unlock(&montex);
    return 0;
}




char* monTrimSend(List* list)
{
    char* message;
    pthread_mutex_lock(&montex);
    {
        //must wait for prepend if List is empty
        if(List_count(list) == 0) {
            pthread_cond_wait(&sendListEmpty, &montex);
        }
        message = List_trim(list);
        pthread_cond_signal(&listFull);
    }
    pthread_mutex_unlock(&montex);


    return message;
}


int monPrependRec(List* list, char* message)
{
    pthread_mutex_lock(&montex);
    {
        //prepending only fails when list is full
        while(List_prepend(list, message) == -1) {
            pthread_cond_wait(&listFull, &montex);
            //this call only reached if prepending failed earlier -> need to try to prepend again
        }
        pthread_cond_signal(&recListEmpty);
    }
    pthread_mutex_unlock(&montex);
    return 0;
}




char* monTrimRec(List* list)
{
    char* message;
    pthread_mutex_lock(&montex);
    {
        //must wait for prepend if List is empty
        if(List_count(list) == 0) {
            pthread_cond_wait(&recListEmpty, &montex);
        }
        message = List_trim(list);
        pthread_cond_signal(&listFull);
    }
    pthread_mutex_unlock(&montex);


    return message;
}

