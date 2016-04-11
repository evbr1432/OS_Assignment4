/*
 * File: pi_fork.c
 * Author: Juan Carlos Herrera
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: 5/11/15
 * Description:
 * 	This file contains a simple program for statistically
 *  calculating pi with fork ability.
 */

/* Local Includes */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>

/* Local Defines */
#define DEFAULT_ITERATIONS 1000000
#define RADIUS (RAND_MAX / 2)

/* Local Functions */
inline double dist(double x0, double y0, double x1, double y1){
    return sqrt(pow((x1-x0),2) + pow((y1-y0),2));
}

inline double zeroDist(double x, double y){
    return dist(0, 0, x, y);
}

int main(int argc, char* argv[]){
    char *c = argv[argc - 1];
    int forks = atoi(c);
    int number, pid;
    int pids[forks];
    for (number=1;number<= forks;number++){
        pid = fork();
        if (pid == 0){
            printf("BREAKING OUT %d\n",number );
            break;
        }
        else
        {
            pids[number-1] = pid;
        }

    }
// Fork Implementation
    if (pid == 0){
        long i;
        long iterations;
        struct sched_param param;
        int policy;
        double x, y;
        double inCircle = 0.0;
        double inSquare = 0.0;
        double pCircle = 0.0;
        double piCalc = 0.0;
    
        // Process program arguments to select iterations and policy
        if(argc < 2){
        iterations = DEFAULT_ITERATIONS;
        }
        // Set default policy if not supplied
        if(argc < 3){
        policy = SCHED_OTHER;
        }
        // Set iterations if supplied 
        if(argc > 1){
        iterations = atol(argv[1]);
        if(iterations < 1){
            fprintf(stderr, "Bad iterations value\n");
            exit(EXIT_FAILURE);
        }
        }
        // Set policy if supplied
        if(argc > 2){
        if(!strcmp(argv[2], "SCHED_OTHER")){
            policy = SCHED_OTHER;
        }
        else if(!strcmp(argv[2], "SCHED_FIFO")){
            policy = SCHED_FIFO;
        }
        else if(!strcmp(argv[2], "SCHED_RR")){
            policy = SCHED_RR;
        }
        else{
            fprintf(stderr, "Unhandeled scheduling policy\n");
            exit(EXIT_FAILURE);
        }
        }
        
        // Set Process to Max Prioty for given Scheduler
        param.sched_priority = sched_get_priority_max(policy);
        
        // Scheduler Policies:
        fprintf(stdout, "Current Scheduling Policy: %d\n", sched_getscheduler(0));
        fprintf(stdout, "Setting Scheduling Policy to: %d\n", policy);
        if(sched_setscheduler(0, policy, &param)){
        perror("Error setting scheduler policy");
        exit(EXIT_FAILURE);
        }
        fprintf(stdout, "New Scheduling Policy: %d\n", sched_getscheduler(0));

        // Calculate Pi using statistical methode across all iterations
        for(i=0; i<iterations; i++){
        x = (random() % (RADIUS * 2)) - RADIUS;
        y = (random() % (RADIUS * 2)) - RADIUS;
        if(zeroDist(x,y) < RADIUS){
            inCircle++;
        }
        inSquare++;
        }
        pCircle = inCircle/inSquare;
        piCalc = pCircle * 4.0;

        fprintf(stdout, "Pi = %f\n", piCalc);
        fflush(stdout);
    }
    else
    {
        int status;
        int i = 0;
        for(i = 0; i < forks ; i++)
        {
            pid = pids[i];
            do{
                pid = waitpid(pid, &status, WNOHANG);
                if (WIFEXITED(status))
                {
                    //Creates too much noise
                    //printf("Child Exited. Status: %d\n", WEXITSTATUS(status));
                }
            } while(pid == 0);
        }
    }
    return 0;
}

