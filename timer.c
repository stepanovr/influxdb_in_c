//http://man7.org/linux/man-pages/man2/timerfd_create.2.html
// usage: ./a.out 3 1 100



#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>        /* Definition of uint64_t */

#include "influxdb.h"


#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

static void print_elapsed_time()
{
        static struct timespec start;
        struct timespec curr;
        static int first_call = 1;
        int secs, nsecs;

        if (first_call) {
                first_call = 0;
                if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
                        handle_error("clock_gettime");
        }

        if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1)
                handle_error("clock_gettime");

        secs = curr.tv_sec - start.tv_sec;
        nsecs = curr.tv_nsec - start.tv_nsec;
        if (nsecs < 0) {
                secs--;
                nsecs += 1000000000;
        }
        printf("%d.%03d: ", secs, (nsecs + 500000) / 1000000);
        printf("%llu.%03llu: ", (unsigned long long)curr.tv_sec, (unsigned long long)((curr.tv_nsec + 500000) / 1000000));
}

#if 1
//int main(int argc, char *argv[])

int work(influx_client_t* c)
{
        struct itimerspec new_value;
        int max_exp, fd;
        struct timespec now;
        uint64_t exp, tot_exp;
        ssize_t s;
/*
        if ((argc != 2) && (argc != 4)) {
                fprintf(stderr, "%s init-secs [interval-secs max-exp]\n",
                                argv[0]);
                exit(EXIT_FAILURE);
        }
*/
        if (clock_gettime(CLOCK_REALTIME, &now) == -1)
                handle_error("clock_gettime");

        /* Create a CLOCK_REALTIME absolute timer with initial
           expiration and interval as specified in command line */

//        new_value.it_value.tv_sec = now.tv_sec + atoi(argv[1]);
        new_value.it_value.tv_sec = now.tv_sec + 1;
        new_value.it_value.tv_nsec = now.tv_nsec;
        new_value.it_interval.tv_sec = 1;
/*
        if (argc == 2) {
                new_value.it_interval.tv_sec = 0;
                max_exp = 1;
        } else {
                new_value.it_interval.tv_sec = atoi(argv[2]);
                max_exp = atoi(argv[3]);
        }
*/
        new_value.it_interval.tv_nsec = 0;

        fd = timerfd_create(CLOCK_REALTIME, 0);
        if (fd == -1)
                handle_error("timerfd_create");

        if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
                handle_error("timerfd_settime");

        print_elapsed_time();
        printf("timer started\n");

       while(true){
//        for (tot_exp = 0; tot_exp < max_exp;) {
                s = read(fd, &exp, sizeof(uint64_t));
                if (s != sizeof(uint64_t))
                        handle_error("read");

		post_netif_http(c);

//                tot_exp += exp;
//                print_elapsed_time();
//                printf("read: %llu; total=%llu\n",
//                                (unsigned long long) exp,
//                                (unsigned long long) tot_exp);
        }

        exit(EXIT_SUCCESS);
}
#else
int main(int argc, char *argv[])
{
        struct itimerspec new_value;
        int max_exp, fd;
        struct timespec now;
        uint64_t exp, tot_exp;
        ssize_t s;

        if ((argc != 2) && (argc != 4)) {
                fprintf(stderr, "%s init-secs [interval-secs max-exp]\n",
                                argv[0]);
                exit(EXIT_FAILURE);
        }

        if (clock_gettime(CLOCK_REALTIME, &now) == -1)
                handle_error("clock_gettime");

        /* Create a CLOCK_REALTIME absolute timer with initial
           expiration and interval as specified in command line */

//        new_value.it_value.tv_sec = now.tv_sec + atoi(argv[1]);
        new_value.it_value.tv_sec = now.tv_sec + 1);
        new_value.it_value.tv_nsec = now.tv_nsec;
        new_value.it_interval.tv_sec = 1;
/*
        if (argc == 2) {
                new_value.it_interval.tv_sec = 0;
                max_exp = 1;
        } else {
                new_value.it_interval.tv_sec = atoi(argv[2]);
                max_exp = atoi(argv[3]);
        }
*/
        new_value.it_interval.tv_nsec = 0;

        fd = timerfd_create(CLOCK_REALTIME, 0);
        if (fd == -1)
                handle_error("timerfd_create");

        if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
                handle_error("timerfd_settime");

        print_elapsed_time();
        printf("timer started\n");

        for (tot_exp = 0; tot_exp < max_exp;) {
                s = read(fd, &exp, sizeof(uint64_t));
                if (s != sizeof(uint64_t))
                        handle_error("read");

                tot_exp += exp;
                print_elapsed_time();
                printf("read: %llu; total=%llu\n",
                                (unsigned long long) exp,
                                (unsigned long long) tot_exp);
        }

        exit(EXIT_SUCCESS);
}
#endif



