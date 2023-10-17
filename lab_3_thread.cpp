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
/*
采用信号量机制，解决苹果橙子问题：一个能放N（这里N设为3）个水果的盘子，
爸爸只往盘子里放苹果，妈妈只放橙子，女儿只吃盘子里的橙子，儿子只吃苹果
*/
void apple_orange();

int ticketAmount = 2; // Global variable
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //Global lock
sem_t empty;
sem_t full;
int Bank[10] = {0};
int in = 0, out = 0;

sem_t fruits;  // 控制盘子里可放的水果数量
sem_t apple;  // 控制苹果数量
sem_t orange;  // 控制橙子数量
pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;  // 声明互斥量work_mutex
int fruitCount = 0;  // 盘子中水果数量

int main()
{
    // lab1();
    // mutex_lab();
    // semaphore();
    // semaphore2();
    apple_orange();
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

void *procFather(void *arg)
{
    // father线程
    while(1)
    {
        sem_wait(&fruits);  // 占用一个盘子空间，可放水果数减1
        pthread_mutex_lock(&work_mutex);  // 加锁
        cout << "爸爸放入一个苹果！（盘子当前水果总数："<< ++fruitCount << ")" << endl;
        sem_post(&apple);  // 释放一个apple信号，可吃苹果数加1
        pthread_mutex_unlock(&work_mutex);  // 解锁
        sleep(0.1);
    }
}

void *procMother(void *arg)
{
    // mother线程
    while(1)
    {
        sem_wait(&fruits);
        pthread_mutex_lock(&work_mutex);  // 加锁
        cout << "妈妈放入一个i橙子！（盘子当前水果总数：" << ++fruitCount << ")" << endl;
        sem_post(&orange);
        pthread_mutex_unlock(&work_mutex);  // 解锁
        sleep(0.2);
    }
}

void *procSon(void *arg)
{
    // son线程
    while(1)
    {
        sem_wait(&apple);  // 占用一个苹果资源，可吃苹果数减1
        pthread_mutex_lock(&work_mutex);  // 加锁
        cout << "儿子吃了一个苹果！（盘子当前水果总数" << --fruitCount << ")" <<endl;
        sem_post(&fruits);  // 吃了一个苹果，释放一个盘子空间，可放水果加1
        pthread_mutex_unlock(&work_mutex);  // 解锁
        sleep(0.2);
    }
}

void *procDaughter(void *arg)
{
    // daughter线程
    while(1)
    {
        sem_wait(&orange);  
        pthread_mutex_lock(&work_mutex);  // 加锁
        cout << "女儿吃了一个橙子！（盘子当前水果总数" << --fruitCount << endl;
        sem_post(&fruits);
        pthread_mutex_unlock(&work_mutex);  // 解锁
        sleep(0.1);
    }
}

void apple_orange()
{
    // 定义线程
    pthread_t father;
    pthread_t mother;
    pthread_t son;
    pthread_t daughter;

    // 信号量初始化
    sem_init(&fruits, 0, 3);
    sem_init(&apple, 0, 0);
    sem_init(&orange, 0, 0);

    // 创建线程
    pthread_create(&father, nullptr, procFather, nullptr);
    pthread_create(&mother, nullptr, procMother, nullptr);
    pthread_create(&son, nullptr, procSon, nullptr);
    pthread_create(&daughter, nullptr, procDaughter, nullptr);

    sleep(1);
    sem_destroy(&fruits);
    sem_destroy(&apple);
    sem_destroy(&orange); 
}
