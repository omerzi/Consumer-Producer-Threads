
#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

#define RAND_LO         100    // low bound for random numbers
#define RAND_HI         300    // hi  bound for random numbers
// #define SEED            time(null)
#define SEED            1001   // constant seed, yield fixed series of random

#define N_PROD          2   // number of producer threads
#define N_CONS          4   // number of consumer threads
#define ITEM_START_CNT  8   // consumer threads start only after so items created
#define TOTAL_ITEMS       20  // total number of messages to create
// print constants
#define ALL_PROD_CREATED "main thread created all producer threads\n"
#define ALL_CONS_CREATED "main thread created all consumer threads\n"
#define PROD_TERMINATED  "all producers terminated\n"
#define CONS_TERMINATED  "all consumers terminated\n"

#define SLEEP_FACTOR    100  // used by my_sleep

// *** type declarations
typedef unsigned uint;
typedef enum STATUS STATUS;
typedef struct item item;
typedef struct list_node list_node;

enum STATUS {NOT_DONE = 0, PROCESSING, DONE};

struct item {
  uint prod;
  uint n1, n2;
  enum STATUS status;
};
struct list_node
{
  item*  item;
  list_node* next;
};

//  Extern for global variables
extern list_node* list_head;
extern list_node* list_tail;


// *********** function prototypes ***********
// function dealing with numbers
int get_random_in_range();
int is_prime(int n);
int set_two_factors(item*);

// function dealing with "items" and the list
void add_to_list (item*);
item* get_undone_from_list();

// function dealing with writing to stdout
void write_adding_item(int thread_num, item*);
void write_getting_item(int thread_num, item*);
void print_list();
void write_producer_is_done(int thread_num);
void write_consumer_is_done(int thread_num);

// other misc. functions
void my_sleep();

