#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <semaphore.h>

using namespace std;

void* threadFunc(void* arg)
{
    // thread function
    cout << "I am a new thread" << endl;
}

void lab1();
void mutex_lab();
void semaphore();
void semaphore2();

int ticketAmount = 2; // Global variable
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //Global lock
sem_t empty;
sem_t full;
int Bank[10] = {0};
int in = 0, out = 0;

int main()
{
    // lab1();
    // mutex_lab();
    // semaphore();
    semaphore2();
    return 0;
}

void lab1()
{
    pthread_t tid;
    pthread_create(&tid, NULL, threadFunc, NULL);
    pthread_join(tid, NULL);
    cout << "In main  thread" << endl;
}

void* ticketAgent(void* arg)
{
    pthread_mutex_lock(&lock);
    int t = ticketAmount;
    if (t > 0)
    {
        cout << "One ticket sold!" << endl;
        --t;
    }
    else
    {
        cout << "Ticket sold out!" << endl;
    }
    ticketAmount = t;
    pthread_mutex_unlock(&lock);
    pthread_exit(0);
}

void mutex_lab()
{
    pthread_t ticketAgent_tid[2];

    for (int i = 0; i < 2; ++i)
    {
        pthread_create(ticketAgent_tid+i, nullptr, ticketAgent, nullptr);
    }

    for (int i = 0; i < 2; ++i)
    {
        pthread_join(ticketAgent_tid[i], nullptr);
    }

    cout << "The left ticket is " << ticketAmount << endl;

}

void* producerThd(void* arg)
{
    for (int i = 0; i < 10; ++i)
    {
        cout << "**Producing one item.**" << endl;
        sem_wait(&empty);
        cout << "**Putting item to warehouse.**" << endl;
        sem_post(&full);
    }
    pthread_exit(0);
}

void* consumerThd(void* arg)
{
    for (int i = 0; i < 10; ++i)
    {
        sem_wait(&full);
        cout << "##Getting item from warehouse.##" << endl;
        sem_post(&empty);
        cout << "##Consuming the item.##" << endl;
    }
    pthread_exit(0);
}

void semaphore()
{
    pthread_t produce_tid, consumer_tid;

    sem_init(&empty, 0, 1);
    sem_init(&full, 0, 0);

    pthread_create(&produce_tid, nullptr, producerThd, nullptr);
    pthread_create(&consumer_tid, nullptr, consumerThd, nullptr);

    pthread_join(produce_tid, nullptr);
    pthread_join(consumer_tid, nullptr);

    sem_destroy(&empty);
    sem_destroy(&full);
}

void printBand()
{
    // 输出缓冲区全部数值
    cout << "Bank:";
    for (int i = 0; i < 10; ++i)
    {
        cout << "[" << Bank[i] << "]";
        if (i == 9)
        {
            cout << endl;
        }
    }
}

void* produceThd2(void* arg)
{
    for (int i = 0; i < 20; i++)
    {
        sem_wait(&empty);

        pthread_mutex_lock(&lock);  // 临界区开始
        Bank[in] = 1;
        in = (in + 1) % 10;
        printBand();
        sleep(0.1);
        pthread_mutex_unlock(&lock);  // 临界区结束

        sem_post(&full);
    }
    pthread_exit(0);
}

void* consumerThd2(void* arg)
{
    for (int i = 0; i < 20; ++i)
    {
        sem_wait(&full);

        pthread_mutex_lock(&lock);  // 临界区开始
        Bank[out] = 0;
        out = (out + 1) % 10;
        printBand();
        sleep(1);
        pthread_mutex_unlock(&lock);  // 临界区结束
        
        sem_post(&empty);
    }
    pthread_exit(0);
}

void semaphore2()
{
    pthread_t producer_tid, consumer_tid;

    sem_init(&empty, 0, 10);
    sem_init(&full, 0, 0);

    pthread_create(&producer_tid, nullptr, produceThd2, nullptr);
    pthread_create(&consumer_tid, nullptr, consumerThd2, nullptr);

    pthread_join(producer_tid, nullptr);
    pthread_join(consumer_tid, nullptr);

    sem_destroy(&empty);
    sem_destroy(&full);
}