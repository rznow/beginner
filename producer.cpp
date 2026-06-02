#include <iostream>
#include <mingw.thread.h>
#include <memory>
#include <mingw.mutex.h>
#include <mingw.condition_variable.h>
#include <queue>
using namespace std;

mutex mtx;
condition_variable cv;
queue<int> q;
bool done = false;


class ThreadPool
{
private:
    vector<thread> threads;
    queue<function<void()>> tasks;
    
public:
    ThreadPool(/* args */);
    ~ThreadPool();
};




void Producer()
{
    for(int i = 0;i < 10;i++)
    {
        {
            unique_lock<mutex> l(mtx);
            q.push(i);
        }
        cv.notify_one();
        this_thread::sleep_for(chrono::seconds(1));
    }
    {
        unique_lock<mutex> l(mtx);
        done = true;
    }
    cv.notify_all();
}
void Consumer()
{
    while (1)
    {
        unique_lock<mutex> l(mtx);

        cv.wait(l,[]{return !q.empty()||done;});
        if(done)    break;
        int a = q.front();
        q.pop();
        cout<<"Consumer"<<' '<<a<<endl;
    }
    

}

int main()
{
    thread t1(Producer);
    thread t2(Consumer);

    t1.join();
    t2.join();

    return 0;
}