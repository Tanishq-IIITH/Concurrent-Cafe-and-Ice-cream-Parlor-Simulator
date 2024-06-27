#include "stdio.h"
#include "stdlib.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <sys/time.h>
#define MAX 100
#define ANSI_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"
int n, k, f, t;

int max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

typedef struct
{
    int machine_id;
    int tm_start;
    int tm_end;
    int busy;
} machine;

typedef struct
{
    char *name;
    int t_f;
} icecream;

typedef struct
{
    char *name;
    int q_t;
} topping;

typedef struct
{
    int customer_id;
    int t_arr;
    int number_of_icecreams;
    char **ic_crm;
    char ***tops;
    int *top_size;
    int *ice_time;
    int *complete;
} queries;

machine *machines;
icecream *icecreams;
topping *toppings;
queries *cust;
pthread_t *customer;
pthread_t *machine_thread;
pthread_mutex_t **queue_lock;
pthread_mutex_t print;
pthread_mutex_t lock;
int timer = -1;

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

struct Queue
{
    int front, rear, size;
    unsigned capacity;
    queries **array;
};

struct Queue *createQueue(unsigned capacity)
{
    struct Queue *queue = (struct Queue *)malloc(
        sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;

    queue->rear = capacity - 1;
    queue->array = (queries **)malloc(
        queue->capacity * sizeof(queries *));
    return queue;
}

int isFull(struct Queue *queue)
{
    return (queue->size == queue->capacity);
}

int isEmpty(struct Queue *queue)
{
    return (queue->size == 0);
}

void enqueue(struct Queue *queue, queries *item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    // printf("%d enqueued to queue\n", item);
}

queries *dequeue(struct Queue *queue)
{
    if (isEmpty(queue))
        return NULL;
    queries *item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

void remov(struct Queue *queue, int index)
{
    if (isEmpty(queue))
        return;
    index -= queue->front;
    for (int i = index; i < queue->size - 1; i++)
    {
        int current = (queue->front + i) % queue->capacity;
        int next = (current + 1) % queue->capacity;
        queue->array[current] = queue->array[next];
    }

    // Adjust front and rear pointers and size
    queue->rear = (queue->rear - 1 + queue->capacity) % queue->capacity;
    queue->size--;
}

queries *front(struct Queue *queue)
{
    if (isEmpty(queue))
        return NULL;
    return queue->array[queue->front];
}

queries *rear(struct Queue *queue)
{
    if (isEmpty(queue))
        return NULL;
    return queue->array[queue->rear];
}
struct Queue *queue;
int ex = 0;
int t_max = 0;
void *serve_customer(queries *args)
{
    queries *cust = (queries *)args;
    int id = cust->customer_id;
    while (time_from_start() < cust->t_arr)
        sleep(1);
    usleep(1000 * id);
    if (isFull(queue))
    {
        pthread_mutex_lock(&print);
        printf("The customer %d sees that the parlour is full, and goes away.\n", id);
        pthread_mutex_unlock(&print);
        return NULL;
    }
    pthread_mutex_lock(&print);
    printf("Customer %d enters at %d second(s)\n", id, time_from_start());
    printf(ANSI_COLOR_YELLOW "Customer %d orders %d icecreams\n" ANSI_COLOR_RESET, id, cust->number_of_icecreams);
    pthread_mutex_unlock(&print);
    for (int i = 0; i < cust->number_of_icecreams; i++)
    {
        pthread_mutex_lock(&print);
        printf(ANSI_COLOR_YELLOW "Ice cream  %d: %s" ANSI_COLOR_RESET, i + 1, cust->ic_crm[i]);
        pthread_mutex_unlock(&print);
        for (int j = 0; j < cust->top_size[i]; j++)
        {
            pthread_mutex_lock(&print);
            printf(ANSI_COLOR_YELLOW " %s" ANSI_COLOR_RESET, cust->tops[i][j]);
            pthread_mutex_unlock(&print);
        }
        pthread_mutex_lock(&print);
        printf(ANSI_COLOR_YELLOW "\n" ANSI_COLOR_RESET);
        pthread_mutex_unlock(&print);
    }
    
    pthread_mutex_lock(&lock);
    int arr[t];
    for (int i = 0; i < t; i++)
        arr[i] = 0;
    for (int i = 0; i < cust->number_of_icecreams; i++)
    {
        for (int j = 0; j < t; j++)
        {
            for (int k = 0; k < cust->top_size[i]; k++)
            {
                if (strcmp(cust->tops[i][k], toppings[j].name) == 0)
                {
                    arr[j]++;
                }
            }
        }
    }
    for (int i = 0; i < t; i++)
    {
        if (arr[i] > toppings[i].q_t)
        {
            printf("%d\n", arr[i]);
            pthread_mutex_lock(&print);
            printf(ANSI_COLOR_RED "Customer %d leaves the parlour at %d second(s) with an unfulfilled order due to topping issue\n" ANSI_COLOR_RESET, id, time_from_start());
            pthread_mutex_unlock(&print);
            pthread_mutex_unlock(&lock);
            return NULL;
        }
    }
    pthread_mutex_unlock(&lock);
    pthread_mutex_lock(&lock);
    enqueue(queue, cust);
    pthread_mutex_unlock(&lock);
    sleep(1);
    while (1)
    {
        if (time_from_start() == t_max + 1)
        {
            pthread_mutex_lock(&print);
            printf(ANSI_COLOR_RED "Customer %d was not serviced due to unavailability of machines\n" ANSI_COLOR_RESET, cust->customer_id);
            pthread_mutex_unlock(&print);
            break;
        }
        int f = 1;
        for (int i = 0; i < cust->number_of_icecreams; i++)
        {
            if (cust->complete[i] == 2)
                continue;
            else if (cust->complete[i] == 0 || cust->complete[i] == 1)
                f = 0;
            else if (cust->complete[i] == -1)
            {
                pthread_mutex_lock(&print);
                printf(ANSI_COLOR_RED "Customer %d leaves the parlour at %d second(s) with an unfulfilled order\n" ANSI_COLOR_RESET, id, time_from_start());
                pthread_mutex_unlock(&print);
                return NULL;
            }
        }
        if (f == 0)
        {
        }
        if (f == 1)
        {
            pthread_mutex_lock(&print);
            printf(ANSI_COLOR_GREEN "Customer %d has collected their order(s) and left at %d second(s)\n" ANSI_COLOR_RESET, cust->customer_id, time_from_start());
            pthread_mutex_unlock(&print);
            break;
        }
    }
    return NULL;
}

void *serve_machine(machine *args)
{
    machine *m = (machine *)args;
    int id = m->machine_id;
    while (time_from_start() < m->tm_start)
        sleep(1);
    pthread_mutex_lock(&print);
    printf("\033[38;5;208m");
    printf("Machine %d has started working at %d second(s)\n", id, time_from_start());
    printf("\033[0m");
    pthread_mutex_unlock(&print);
    while (1)
    {
        if (time_from_start() >= m->tm_end)
            break;
        if (isEmpty(queue))
            continue;
        pthread_mutex_lock(&lock);
        for (int i = queue->front; i <= queue->rear; i++)
        {
            queries *cust = queue->array[i];
            int ct = 0;
            if (cust == NULL)
                break;
            if (cust->t_arr >= time_from_start())
                break;
            for (int it = 0; it < cust->number_of_icecreams; it++)
            {
                if (cust->complete[it] == 0)
                {
                    int f = 1;
                    for (int j = 0; j < t; j++)
                    {
                        for (int k = 0; k < cust->top_size[it]; k++)
                        {
                            if (strcmp(cust->tops[it][k], toppings[j].name) == 0)
                            {
                                if (toppings[j].q_t == 0)
                                {
                                    f = 0;
                                    break;
                                }
                            }
                        }
                    }
                    if (f == 0)
                    {
                        remov(queue, i);
                        cust->complete[it] = -1;
                    }
                    else
                    {
                        if (m->tm_end < time_from_start() + cust->ice_time[it])
                            break;
                        int f2 = 1;
                        for (int mac = 0; mac < id - 1; mac++)
                        {
                            if (time_from_start() + cust->ice_time[it] <= machines[mac].tm_end && machines[mac].busy == 0)
                            {
                                f2 = 0;
                                break;
                            }
                        }
                        if (f2 == 0)
                            break;
                        for (int j = 0; j < t; j++)
                        {
                            for (int k = 0; k < cust->top_size[it]; k++)
                            {
                                if (strcmp(cust->tops[it][k], toppings[j].name) == 0)
                                    toppings[j].q_t--;
                            }
                        }
                        pthread_mutex_lock(&print);
                        printf(ANSI_COLOR_CYAN "Machine %d starts preparing ice cream %d of customer %d at %d second(s)\n" ANSI_COLOR_RESET, id, it + 1, cust->customer_id, time_from_start());
                        pthread_mutex_unlock(&print);
                        m->busy = 1;
                        cust->complete[it] = 1;
                        pthread_mutex_unlock(&lock);
                        sleep(cust->ice_time[it]);
                        pthread_mutex_lock(&lock);
                        m->busy = 0;
                        pthread_mutex_lock(&print);
                        printf(ANSI_COLOR_BLUE "Machine %d completes preparing ice cream %d of customer %d at %d seconds(s)\n" ANSI_COLOR_RESET, id, it + 1, cust->customer_id, time_from_start());
                        pthread_mutex_unlock(&print);
                        cust->complete[it] = 2;
                    }
                }
                else if (cust->complete[it] == 2)
                    ct++;
            }
            if (ct == cust->number_of_icecreams)
                remov(queue, i);
        }
        pthread_mutex_unlock(&lock);
    }
    pthread_mutex_lock(&print);
    printf("\033[38;5;208m");
    printf("Machine %d has stopped working at %d second(s)\n", id, time_from_start());
    printf("\033[0m");
    pthread_mutex_unlock(&print);
    return NULL;
}

int main()
{
    scanf("%d %d %d %d", &n, &k, &f, &t);
    machines = (machine *)malloc(n * sizeof(machine));
    queue_lock = (pthread_mutex_t **)malloc(sizeof(pthread_mutex_t *) * (k + 1));
    queue = createQueue(k);
    for (int i = 0; i < n; i++)
    {
        machines[i].machine_id = i + 1;
        scanf("%d %d", &machines[i].tm_start, &machines[i].tm_end);
        t_max = max(t_max, machines[i].tm_end);
        machines[i].busy = 0;
    }
    icecreams = (icecream *)malloc(f * sizeof(icecream));
    for (int i = 0; i < f; i++)
    {
        icecreams[i].name = (char *)malloc(100 * sizeof(char));
        scanf("%s %d", icecreams[i].name, &icecreams[i].t_f);
    }
    toppings = (topping *)malloc(t * sizeof(topping));
    for (int i = 0; i < t; i++)
    {
        toppings[i].name = (char *)malloc(100 * sizeof(char));
        scanf("%s %d", toppings[i].name, &toppings[i].q_t);
        if (toppings[i].q_t == -1)
            toppings[i].q_t = INT_MAX;
    }
    cust = (queries *)malloc(sizeof(queries));
    int i = -1;
    int mx_ice = 0;
    while (1)
    {
        char inp[100];
        fgets(inp, sizeof(inp), stdin);
        if (inp[0] == '\n')
        {
            fgets(inp, sizeof(inp), stdin);
            if (inp[0] == '\n')
                break;
        }
        i++;
        queries *cust2 = (queries *)realloc(cust, sizeof(queries) * (i + 1));
        cust = cust2;
        char *tok = strtok(inp, " ");
        cust[i].customer_id = atoi(tok);
        tok = strtok(NULL, " ");
        cust[i].t_arr = atoi(tok);
        tok = strtok(NULL, " ");
        cust[i].number_of_icecreams = atoi(tok);
        mx_ice = max(mx_ice, cust[i].number_of_icecreams);
        cust[i].ic_crm = (char **)malloc(sizeof(char *) * cust[i].number_of_icecreams);
        cust[i].complete = (int *)malloc(sizeof(int) * cust[i].number_of_icecreams);
        cust[i].ice_time = (int *)malloc(sizeof(int) * cust[i].number_of_icecreams);
        for (int j = 0; i < cust[j].number_of_icecreams; j++)
            cust[i].complete[j] = 0;
        for (int j = 0; j < cust[i].number_of_icecreams; j++)
            cust[i].ic_crm[j] = (char *)malloc(sizeof(char) * 100);
        cust[i].tops = (char ***)malloc(sizeof(char **) * (cust[i].number_of_icecreams));
        cust[i].top_size = (int *)malloc(sizeof(int) * (cust[i].number_of_icecreams));
        for (int j = 0; j < cust[i].number_of_icecreams; j++)
        {
            char input[100], input2[100];
            fgets(input, 100, stdin);
            strcpy(input2, input);
            char *token = strtok(input, " ");
            int ct = 0;
            while (token != NULL)
            {
                token = strtok(NULL, " ");
                ct++;
            }
            cust[i].tops[j] = (char **)malloc(sizeof(char *) * (ct - 1));
            char *token2 = strtok(input2, " ");
            strcpy(cust[i].ic_crm[j], token2);
            token2 = strtok(NULL, " ");
            for (int it = 0; it < ct - 1; it++)
            {
                cust[i].tops[j][it] = (char *)malloc(sizeof(char *) * 100);
                strcpy(cust[i].tops[j][it], token2);
                if (it == ct - 2)
                    cust[i].tops[j][it][strlen(cust[i].tops[j][it]) - 1] = '\0';
                token2 = strtok(NULL, " ");
            }
            cust[i].top_size[j] = ct - 1;
        }
        for (int k = 0; k < cust[i].number_of_icecreams; k++)
        {
            for (int j = 0; j < f; j++)
            {
                if (strcmp(cust[i].ic_crm[k], icecreams[j].name) == 0)
                {
                    cust[i].ice_time[k] = icecreams[j].t_f;
                    break;
                }
            }
        }
    }
    for (int i = 0; i < k + 1; i++)
    {
        queue_lock[i] = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * mx_ice);
        for (int j = 0; j < mx_ice; j++)
            pthread_mutex_init(&queue_lock[i][j], NULL);
    }
    pthread_mutex_init(&lock, NULL);
    int sz = i + 1;
    pthread_mutex_init(&print, NULL);
    i = 0;
    customer = (pthread_t *)malloc(sizeof(pthread_t) * sz);
    machine_thread = (pthread_t *)malloc(sizeof(pthread_t) * n);
    for (int i = 0; i < sz; i++)
        pthread_create(&customer[i], NULL, (void *)serve_customer, (void *)&cust[i]);
    for (int i = 0; i < n; i++)
        pthread_create(&machine_thread[i], NULL, (void *)serve_machine, (void *)&machines[i]);
    for (int i = 0; i < sz; i++)
        pthread_join(customer[i], NULL);
    for (int i = 0; i < n; i++)
        pthread_join(machine_thread[i], NULL);
    printf("Parlour Closed\n");
    return 0;
}