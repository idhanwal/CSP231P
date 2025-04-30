/* NAME: Ishan Dhanwal */
/* UCI ID: 88339374*/
/* NAME: Ish Kataria */
/* UCI ID: 18183388 */
/* NAME: Prarthana Majalikar */
/* UCI ID: 33598574 */
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include "primes.h"


// Shared counter for assigning the next number to check
int shared_counter = 1;

// Mutex to protect access to the shared counter
pthread_mutex_t counter_mutex;

// Mutex to protect printing
pthread_mutex_t print_mutex;

// Structure to pass arguments to the thread function
typedef struct {
    int max;
    int verb;
} thread_arg;

/**
 ** Checks if a number is prime.
 ** Returns 1 if prime, 0 otherwise.
 **/
static int is_prime(int n){
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;

    int sqrt_n = (int)sqrt(n);
    for (int i = 3; i <= sqrt_n; i += 2) {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

void* thread_func(void* arg) {
    while (1) {

        int max = ((thread_arg*)arg)->max;
        int verb = ((thread_arg*)arg)->verb;
        int number_to_test;

        // Lock the counter, get a number, and increment it
        pthread_mutex_lock(&counter_mutex);
        if (shared_counter > max) {
            pthread_mutex_unlock(&counter_mutex);
            break;
        }
        number_to_test = shared_counter++;
        pthread_mutex_unlock(&counter_mutex);

        // Test the number for primality
        if (is_prime(number_to_test)) {
            pthread_mutex_lock(&print_mutex);
            if (verb) {
                printf("%d\n", number_to_test);
            }
            pthread_mutex_unlock(&print_mutex);
        }
    }

    return NULL;
}

void primes_st(unsigned int max, unsigned int verb){
    for (unsigned int i = 1; i <= max; i++) {
        if (is_prime(i)) {
            if (verb) {
                printf("%u\n", i);
            }
        }
    }
    return;
}

void primes_mt(unsigned int max, unsigned int threads,	\
	       unsigned int verb){
    thread_arg arg;
    arg.max = max;
    arg.verb = verb;

    pthread_t threads_arr[threads];
    pthread_mutex_init(&counter_mutex, NULL);
    pthread_mutex_init(&print_mutex, NULL);

    // Create threads
    for (int i = 0; i < threads; i++) {
        pthread_create(&threads_arr[i], NULL, thread_func, (void*)&arg);
    }

    // Wait for all threads to finish
    for (int i = 0; i < threads; i++) {
        pthread_join(threads_arr[i], NULL);
    }

    // Cleanup
    pthread_mutex_destroy(&counter_mutex);
    pthread_mutex_destroy(&print_mutex);

    return;
}
