/* SUBMIT ONLY THIS FILE */
/* NAME: ....... */
/* UCI ID: .......*/

// only include standard libraries.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simulator.h" // implements

double PI_VAL = 3.14159265358979323846;

void simulate(double *avg_access_time, int avg_access_time_len, int procs, char dist) {
    double EPSILON = 0.02; // Convergence tolerance
    int MAX_CYCLES = 1000000; // Maximum number of cycles (10^6)
    for (int mems = 0; mems < avg_access_time_len; mems++) {
        int p_requests[procs];          // current p_requests of each processor
        int access_count[procs];     // total number of accesses per processor
        int uniform_module_idx[procs]; // only used for normal distribution
        int p_wait_times[procs];        // tracks how long each processor waits for access
        double T_c_S = 0.0, T_c_prev = 0.0;
        int is_correct = 0, cycle = 0, priority_start_idx = 0;

        // Initialize: each processor is assigned its first p_requests
        for (int i = 0; i < procs; ++i) {
            uniform_module_idx[i] = (dist == 'n') ? rand_uniform(mems) : 0;
            p_requests[i] = (dist == 'u') ? rand_uniform(mems)
                                       : rand_normal_wrap(uniform_module_idx[i], 5, mems);
            access_count[i] = 0;
            p_wait_times[i] = 0;
        }

        while (cycle < MAX_CYCLES) {
            int *module_used = calloc(mems, sizeof(int)); // tracks which modules are used 0 or 1
            int first_waitilisted_p = -1; // for circular priority relabeling, tracks first denied processor

            for (int i = 0; i < procs; ++i) {
                int curr_p_idx = (priority_start_idx + i) % procs;
                int req = p_requests[curr_p_idx];

                if (module_used[req] == 0) {
                    module_used[req] = 1;
                    access_count[curr_p_idx]++;
                    p_wait_times[curr_p_idx] = 0;
                    p_requests[curr_p_idx] = (dist == 'u') ? rand_uniform(mems)
                                                : rand_normal_wrap(uniform_module_idx[curr_p_idx], 5, mems);
                } else {
                    p_wait_times[curr_p_idx]++;
                    if (first_waitilisted_p == -1) {
                        first_waitilisted_p = curr_p_idx;
                    }
                }
            }
            free(module_used);

            double T_c_S_sum = 0.0;
            int active_procs = 0;
            for (int i = 0; i < procs; ++i) {
                if (access_count[i] > 0) {
                    double avg_T_pi = ((double)p_wait_times[i] + access_count[i]) / access_count[i];
                    T_c_S_sum += avg_T_pi;
                    active_procs++;
                }
            }

            if (active_procs == procs) {
                T_c_S = T_c_S_sum / procs;
                if (cycle > 0 && fabs(1.0 - T_c_prev / T_c_S) < EPSILON) {
                    is_correct = 1;
                    break;
                }
                T_c_prev = T_c_S;
            }

            cycle++;
            // Use circular relabeling: first processor denied goes first next round
            if (first_waitilisted_p != -1) {
                priority_start_idx = first_waitilisted_p;
            } else {
                priority_start_idx = (priority_start_idx + 1) % procs;
            }
        }

        if (!is_correct) {
            T_c_S = -1.0;
        }

        avg_access_time[mems] = T_c_S;

        free(p_requests);
        free(access_count);
        free(uniform_module_idx);
        free(p_wait_times);
    }
}

int rand_uniform(int max){
    return rand() % max;
}

int rand_normal_wrap(int mean, int dev, int max){
    static double U, V;
    static int phase = 0;
    double Z;
    if(phase == 0){
        U = (rand() + 1.) / (RAND_MAX + 2.);
        V = rand() / (RAND_MAX + 1.);
        Z = sqrt(-2 *log(U)) * sin(2 * PI_VAL * V);
    }else{
        Z = sqrt(-2 * log(U)) * cos(2 * PI_VAL * V);
    }
    phase = 1 - phase;
    double res = dev*Z + mean;

    // round result up or down depending on whether
    // it is even or odd. This compensates some bias.
    int res_int;
    // if even, round up. If odd, round down.
    if ((int)res % 2 == 0)
        res_int = (int)(res+1);
    else
        res_int = (int)(res);

    // wrap result around max
    int res_wrapped = res_int % max;
    // deal with % of a negative number in C
    if(res_wrapped < 0)
        res_wrapped += max;
    return res_wrapped;
}