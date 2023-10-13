#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main()
{
    // pid_t是数据类型，实际上是一个整型，通过typedef重新定义了一个名字，用于存储进程id
    pid_t pid,cid;
    // getpid()函数返回当前进程的id号
    cout << "Before fork Process id:" << getpid() << endl;
    /*
    fork()函数用于创建一个新的进程，该进程为当前进程的子进程，创建的方法是：将当前进程的内存内容完整拷贝一份到内存的另一个区域，两个进程为父子关系，他们会同时（并发）执行fork()语句后面的所有语句。
    fork()的返回值：
      如果成功创建子进程，对于父子进程fork会返回不同的值，对于父进程它的返回值是子进程的进程id值，对于子进程它的返回值是0.
      如果创建失败，返回值为-1.
    */
    cid = fork();
    cout << "After fork, Process id:" << getpid() << endl;
    pause();

    return 0;
}