#include "ex3_q1_given.h"
//--------------- Global Variables, Arrays, Functions ------------------------//

pthread_t producers[N_PROD];
int producerNumbers[N_PROD];
pthread_t consumers[N_CONS];
int consumerNumbers[N_CONS];
struct list_node *list_head = NULL;
struct list_node *list_tail = NULL;
pthread_mutex_t itemCounterMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t randomMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t listMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitProducersCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t waitConsumersCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t counterThresholdCond = PTHREAD_COND_INITIALIZER;
int isProducersFinished = 0; //Boolean variable
int itemsCreated = 0;
int itemsAdded = 0;
int itemsProcessed = 0;

void freeList()
{
    list_node *curr = list_head;
    list_node *next;
    while (curr)
    {
        next = curr->next;
        free(curr->item);
        free(curr);
        curr = next;
    }
}

void mutexAndCondDestroy()
{
    pthread_mutex_destroy(&listMutex);
    pthread_mutex_destroy(&randomMutex);
    pthread_mutex_destroy(&printMutex);
    pthread_mutex_destroy(&itemCounterMutex);
    pthread_cond_destroy(&waitProducersCond);
    pthread_cond_destroy(&waitConsumersCond);
    pthread_cond_destroy(&counterThresholdCond);
}

//--------------- Producers Functions ------------------------//

int getRandomPrimeNumber()
{
    pthread_mutex_lock(&randomMutex);
    int primeNumber = get_random_in_range();
    pthread_mutex_unlock(&randomMutex);
    while (!is_prime(primeNumber))
    {
        pthread_mutex_lock(&randomMutex);
        primeNumber = get_random_in_range();
        pthread_mutex_unlock(&randomMutex);
    }

    return primeNumber;
}

void waitProducers()
{
    pthread_mutex_lock(&itemCounterMutex);
    while (!isProducersFinished)
    {
        pthread_cond_wait(&waitProducersCond, &itemCounterMutex);
    }

    pthread_mutex_unlock(&itemCounterMutex);
}

item *getNewItem()
{
    pthread_mutex_unlock(&itemCounterMutex);
    int firstPrime = getRandomPrimeNumber();
    int secondPrime = getRandomPrimeNumber();
    item *newItem = (item *)malloc(sizeof(item));
    newItem->n1 = firstPrime;
    newItem->n2 = secondPrime;
    newItem->prod = firstPrime * secondPrime;
    newItem->status = (STATUS)NOT_DONE;
    return newItem;
}

void addAndWriteItem(item *newItem, int threadNum)
{
    pthread_mutex_lock(&listMutex);
    add_to_list(newItem);
    pthread_mutex_lock(&printMutex);
    write_adding_item(threadNum, newItem);
    pthread_mutex_unlock(&printMutex);
    pthread_mutex_unlock(&listMutex);
}

void checkConditionRelease()
{
    if (itemsAdded == ITEM_START_CNT)
    {
        pthread_cond_broadcast(&waitConsumersCond);
    }
    
    if (itemsAdded > itemsProcessed)
    {
        pthread_cond_broadcast(&counterThresholdCond);
    }
}

void producerOperation(int threadNum, item *newItem)
{
    itemsCreated++;
    newItem = getNewItem();
    addAndWriteItem(newItem, threadNum);
    pthread_mutex_lock(&itemCounterMutex);
    itemsAdded++;
    checkConditionRelease();
}

void *randAndAddToList(void *ptr)
{
    int threadNum = *((int *)ptr);
    waitProducers();
    item *newItem = NULL;
    pthread_mutex_lock(&itemCounterMutex);
    while (itemsCreated < TOTAL_ITEMS)
    {
        producerOperation(threadNum, newItem);
    }

    pthread_mutex_unlock(&itemCounterMutex);
    pthread_mutex_lock(&printMutex);
    write_producer_is_done(threadNum);
    pthread_mutex_unlock(&printMutex);
    pthread_exit(NULL);
}

//--------------- Consumers Functions ------------------------//

void waitConsumersStart()
{
    pthread_mutex_lock(&itemCounterMutex);
    while (itemsAdded < ITEM_START_CNT)
    {
        pthread_cond_wait(&waitConsumersCond, &itemCounterMutex);
    }

    pthread_mutex_unlock(&itemCounterMutex);
}

void finishConsumer(int threadNumber)
{
    pthread_mutex_lock(&printMutex);
    write_consumer_is_done(threadNumber);
    pthread_mutex_unlock(&printMutex);
    pthread_exit(NULL);
}

void checkProcessed(int threadNumber)
{
    while (itemsProcessed == itemsAdded)
    {
        pthread_cond_wait(&counterThresholdCond, &itemCounterMutex);
        if (itemsProcessed == TOTAL_ITEMS)
        {
            pthread_mutex_unlock(&itemCounterMutex);
            finishConsumer(threadNumber);
        }
    }

    itemsProcessed++;
}

void processItem(int threadNumber, item *itemPoped)
{
    pthread_mutex_lock(&listMutex);
    itemPoped = get_undone_from_list();
    pthread_mutex_lock(&printMutex);
    write_getting_item(threadNumber, itemPoped);
    pthread_mutex_unlock(&printMutex);
    pthread_mutex_unlock(&listMutex);
    itemPoped->status = DONE;
    set_two_factors(itemPoped);
}

void consumerOperation(int threadNumber, item *itemPoped)
{
    checkProcessed(threadNumber);
    pthread_mutex_unlock(&itemCounterMutex);
    processItem(threadNumber, itemPoped);
    pthread_mutex_lock(&itemCounterMutex);
}

void *getFromList(void *ptr)
{
    waitConsumersStart();
    int threadNumber = *((int *)ptr);
    item *itemPoped = NULL;
    pthread_mutex_lock(&itemCounterMutex);
    while (itemsProcessed < TOTAL_ITEMS)
    {
        consumerOperation(threadNumber, itemPoped);
    }

    pthread_mutex_unlock(&itemCounterMutex);
    finishConsumer(threadNumber);
    pthread_exit(NULL);
}

//--------------- Main Thread Functions ------------------------//

void createProducersThreads()
{
    for (int i = 0; i < N_PROD; i++)
    {
        producerNumbers[i] = i + 1;
        pthread_create(&producers[i], NULL, randAndAddToList, (void *)&producerNumbers[i]);
    }

    isProducersFinished = 1;
    pthread_mutex_lock(&printMutex);
    printf(ALL_PROD_CREATED);
    pthread_mutex_unlock(&printMutex);
}

void createConsumersThreads()
{
    for (int i = 0; i < N_CONS; i++)
    {
        consumerNumbers[i] = i + 1;
        pthread_create(&consumers[i], NULL, getFromList, (void *)&consumerNumbers[i]);
    }

    pthread_mutex_lock(&printMutex);
    printf(ALL_CONS_CREATED);
    pthread_mutex_unlock(&printMutex);
}

void waitThreads()
{
    for (int i = 0; i < N_PROD; i++)
    {
        pthread_join(producers[i], NULL);
    }

    pthread_mutex_lock(&printMutex);
    printf(PROD_TERMINATED);
    pthread_mutex_unlock(&printMutex);
    for (int i = 0; i < N_CONS; i++)
    {
        pthread_join(consumers[i], NULL);
    }

    pthread_mutex_lock(&printMutex);
    printf(CONS_TERMINATED);
    pthread_mutex_unlock(&printMutex);
}

void createThreads()
{
    createProducersThreads();
    createConsumersThreads();
    pthread_cond_broadcast(&waitProducersCond);
}

//--------------- Main ------------------------//

int main()
{
    createThreads();
    waitThreads();
    print_list();
    freeList();
    mutexAndCondDestroy();
}