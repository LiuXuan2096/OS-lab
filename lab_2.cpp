#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/wait.h>

using namespace std;

void lab1();
void lab2();

int main()
{

  lab1();  
  lab2();

    return 0;
}

void lab1()
{
  //
  // pid_t是数据类型，实际上是一个整型，通过typedef重新定义了一个名字，用于存储进程id
  pid_t pid, cid;
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
}

void lab2()
{
  pid_t cid;
  string name;
  cid = fork();
  if (cid == 0)
  {
    // child process
    name = "葛诗颖";
    cout << "In child process the person is " << name << endl;
    sleep(3);
  }
  else
  {
    // parent process
    name = "刘轩";
    wait(NULL);
    cout << "In parent process the person is " << name << endl;
  }

}