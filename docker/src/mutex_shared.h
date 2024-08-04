
#ifndef MUTEX_SHARED_H
#define MUTEX_SHARED_H

#include <pthread.h>

extern pthread_mutex_t mutex; 
void init_sharedmutex();
#endif
