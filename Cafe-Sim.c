#include "stdio.h"
#include "stdlib.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#define MAX 100
#define ANSI_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"
int timer = -1;
int b, k, n;
pthread_mutex_t *bari;
sem_t print_mutex;
int wasted = 0;
typedef struct
{
    char *name;
    int time;
} coffee;
double wait = 0;
int time_from_start()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (timer == -1)
    {
        timer = tv.tv_sec;
        return 0;
    }
    return tv.tv_sec - timer;
}

typedef struct
{
    int cust_no;
    char *coffee_type;
    int t_arr;
    int t_tol;
    int t_curr;
    int t_complete;
    int t_coffee;
} queries;

typedef struct
{
    int b_no;
    int b_time;
    int cust_no;
    char *coffee_type;
    int busy;
} coffee_maker;

typedef struct
{
    queries **cust;
    coffee_maker **barista;
} combination;

typedef struct
{
    queries *cust;
    coffee_maker **barista;
} combination2;

pthread_mutex_t lock;
combination combos;


void *customerArrival(combination2 *args)
{
    queries *cust = args->cust;
    coffee_maker **barista = args->barista;
    while (cust->t_arr > time_from_start())
    {
        sleep(1);
    }
    sem_wait(&print_mutex);
    printf(ANSI_COLOR_WHITE "Customer %d arrived at %d(s)\n" ANSI_COLOR_RESET, cust->cust_no, cust->t_arr);
    printf(ANSI_COLOR_YELLOW "Customer %d orders a %s\n" ANSI_COLOR_RESET, cust->cust_no, cust->coffee_type);
    sem_post(&print_mutex);
    if (cust->cust_no > 1)
    {
        while (combos.cust[cust->cust_no - 2]->t_complete == 0)
        {
        }
    }
    if (cust->cust_no == 1)
    {
        while (!cust->t_complete)
        {
            if (time_from_start() > cust->t_arr + cust->t_tol)
            {
                cust->t_complete = 1;
                sem_wait(&print_mutex);
                printf(ANSI_COLOR_RED "Customer %d leaves without their order at %d (s)\n" ANSI_COLOR_RESET, cust->cust_no, time_from_start());
                sem_post(&print_mutex);
                break;
            }
            sleep(0.001);
            for (int i = 0; i < b; i++)
            {
                if (pthread_mutex_trylock(&bari[i]) == 0)
                {
                    barista[i]->busy = 1;
                    barista[i]->cust_no = cust->cust_no;
                    barista[i]->coffee_type = cust->coffee_type;
                    barista[i]->b_time = time_from_start() + cust->t_tol;
                    cust->t_complete = 1;
                    combos.cust[cust->cust_no - 1]->t_complete = 1;
                    sleep(1);
                    sem_wait(&print_mutex);
                    printf(ANSI_COLOR_CYAN "Barista %d begins preparing the order of customer %d at %d (s)\n" ANSI_COLOR_RESET, i + 1, cust->cust_no, time_from_start());
                    sem_post(&print_mutex);
                    if (cust->t_coffee + time_from_start() <= cust->t_arr + cust->t_tol + 1)
                    {
                        wait += time_from_start() - cust->t_arr;
                        sleep(cust->t_coffee);
                        sem_wait(&print_mutex);
                        printf(ANSI_COLOR_BLUE "Barista %d successfully completes the order of customer %d at %d(s)\n" ANSI_COLOR_RESET, i + 1, cust->cust_no, time_from_start());
                        sem_post(&print_mutex);
                        sem_wait(&print_mutex);
                        printf(ANSI_COLOR_GREEN "Customer %d leaves with their order at %d (s)\n" ANSI_COLOR_RESET, cust->cust_no, time_from_start());
                        sem_post(&print_mutex);
                    }
                    else
                    {
                        wait += cust->t_tol ;
                        int x = time_from_start();
                        if (cust->t_arr + cust->t_tol - time_from_start() + 1 > 0)
                        {
                            sleep(cust->t_arr + cust->t_tol - time_from_start() + 1);
                        }
                        sem_wait(&print_mutex);
                        printf(ANSI_COLOR_RED "Customer %d leaves without their order at %d (s)\n" ANSI_COLOR_RESET, cust->cust_no, time_from_start());
                        wasted++;
                        sem_post(&print_mutex);
                        int diff = cust->t_coffee - (cust->t_arr + cust->t_tol - x + 1);
                        if (diff)
                        {
                            sleep(diff);
                        }
                        sem_wait(&print_mutex);
                        printf(ANSI_COLOR_BLUE "Barista %d successfully completes the order of customer %d at %d(s)\n" ANSI_COLOR_RESET, i + 1, cust->cust_no, time_from_start());
                        sem_post(&print_mutex);
                    }
                    barista[i]->busy = 0;
                    pthread_mutex_unlock(&bari[i]);
                    break;
                }
            }
        }
    }
    else
    {
        while (combos.cust[cust->cust_no - 2]->t_complete == 0)
        {
        }
        if (combos.cust[cust->cust_no - 2]->t_complete >= 1)
        {
            while (!cust->t_complete)
            {
                if (time_from_start() > cust->t_arr + cust->t_tol)
                {
                    sem_wait(&print_mutex);
                    printf(ANSI_COLOR_RED "Customer %d leaves without their order at %d (s)\n" ANSI_COLOR_RESET, cust->cust_no, time_from_start());
                    cust->t_complete = 1;
                    sem_post(&print_mutex);
                    break;
                }
                sleep(0.001);

                for (int i = 0; i < b; i++)
                {
                    if (pthread_mutex_trylock(&bari[i]) == 0)
                    {
                        barista[i]->busy = 1;
                        barista[i]->cust_no = cust->cust_no;
                        barista[i]->coffee_type = cust->coffee_type;
                        barista[i]->b_time = time_from_start() + cust->t_tol;
                        combos.cust[cust->cust_no - 1]->t_complete = 1;
                        cust->t_complete = 1;
                        sleep(1);
                        sem_wait(&print_mutex);
                        printf(ANSI_COLOR_CYAN "Barista %d begins preparing the order of customer %d at %d (s)\n" ANSI_COLOR_RESET, i + 1, cust->cust_no, time_from_start());
                        sem_post(&print_mutex);
                        if (cust->t_coffee + time_from_start() <= cust->t_arr + cust->t_tol + 1)
                        {
                            wait += time_from_start() - cust->t_arr;

                            sleep(cust->t_coffee);
                            sem_wait(&print_mutex);
                            printf(ANSI_COLOR_BLUE "Barista %d successfully completes the order of customer %d at %d(s)\n" ANSI_COLOR_RESET, i + 1, cust->cust_no, time_from_start());
                            sem_post(&print_mutex);
                            sem_wait(&print_mutex);
                            printf(ANSI_COLOR_GREEN "Customer %d leaves with their order at %d (s)\n" ANSI_COLOR_RESET, cust->cust_no, time_from_start());
                            sem_post(&print_mutex);
                        }
                        else
                        {
                            wait += cust->t_tol ;
                            int x = time_from_start();
                            if (cust->t_arr + cust->t_tol - time_from_start() + 1 > 0)
                            {
                                sleep(cust->t_arr + cust->t_tol - time_from_start() + 1);
                            }
                            sem_wait(&print_mutex);
                            printf(ANSI_COLOR_RED "Customer %d leaves without their order at %d (s)\n" ANSI_COLOR_RESET, cust->cust_no, time_from_start());
                            wasted++;
                            sem_post(&print_mutex);
                            int diff = cust->t_coffee - (cust->t_arr + cust->t_tol - x + 1);
                            if (diff)
                            {
                                sleep(diff);
                            }
                            sem_wait(&print_mutex);
                            printf(ANSI_COLOR_BLUE "Barista %d successfully completes the order of customer %d at %d(s)\n" ANSI_COLOR_RESET, i + 1, cust->cust_no, time_from_start());
                            sem_post(&print_mutex);
                        }
                        barista[i]->busy = 0;
                        pthread_mutex_unlock(&bari[i]);
                        break;
                    }
                }
            }
        }
    }
    return NULL;
}

int main()
{
    sem_init(&print_mutex, 0, 1);
    scanf("%d %d %d", &b, &k, &n);
    bari = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * b);
    for (int i = 0; i < b; i++)
    {
        pthread_mutex_init(&bari[i], NULL);
    }
    combos.barista = (coffee_maker **)malloc(sizeof(coffee_maker *) * b);
    combos.cust = (queries **)malloc(sizeof(queries *) * n);
    queries *cust;
    coffee types[k];
    combination2 combos2[n];
    for (int i = 0; i < n; i++)
    {
        combos2[i].barista = (coffee_maker **)malloc(sizeof(coffee_maker *) * b);
    }
    coffee_maker *baristas;
    baristas = (coffee_maker *)malloc(sizeof(coffee_maker) * b);
    cust = (queries *)malloc(sizeof(queries) * n);

    for (int i = 0; i < k; i++)
    {
        types[i].name = (char *)malloc(sizeof(char) * MAX);
        scanf("%s %d", types[i].name, &types[i].time);
    }

    for (int i = 0; i < b; i++)
    {
        baristas[i].b_no = i;
        baristas[i].b_time = 0;
        baristas[i].cust_no = 0;
        baristas[i].coffee_type = (char *)malloc(sizeof(char) * MAX);
        baristas[i].busy = 0;
    }
    for (int i = 0; i < n; i++)
    {
        cust[i].coffee_type = (char *)malloc(sizeof(char) * MAX);
        scanf("%d %s", &cust[i].cust_no, cust[i].coffee_type);
        scanf("%d %d", &cust[i].t_arr, &cust[i].t_tol);
        for (int j = 0; j < k; j++)
        {
            if (strcmp(types[j].name, cust[i].coffee_type) == 0)
            {
                cust[i].t_coffee = types[j].time;
                break;
            }
        }
        cust[i].t_complete = 0;
        cust[i].t_curr = 0;
        combos2[i].cust = &cust[i];
        for (int j = 0; j < b; j++)
        {
            combos2[i].barista[j] = &baristas[j];
        }
    }
    for (int i = 0; i < n; i++)
    {
        combos.cust[i] = &cust[i];
    }
    for (int i = 0; i < b; i++)
    {
        combos.barista[i] = &baristas[i];
    }

    pthread_t threads[n];
    pthread_t barista_thread[b];
    for (int i = 0; i < n; i++)
        pthread_create(&threads[i], NULL, (void *)customerArrival, (void *)&combos2[i]);

    for (int i = 0; i < n; ++i)
    {
        pthread_join(threads[i], NULL);
    }
    printf("%d coffee wasted\n", wasted);
    printf("Average Waiting time is %f\n", wait / n);
    return 0;
}