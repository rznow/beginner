#include <iostream>
#include <mingw.thread.h>
#include <memory>
#include <mingw.mutex.h>
#include <mingw.condition_variable.h>
#include <queue>
using namespace std;

class ThreadPool
{
private:
    vector<thread> threads;
    queue<function<void()>> tasks;
    mutex mtx;
    condition_variable cv;
    
    bool stop;
public:
    ThreadPool(int numThreads):stop(false)
    {
        for(int i=0;i<numThreads;i++)
        {
            threads.emplace_back([this]{
                while (1)
                {
                    unique_lock<mutex> ul(mtx);
                    cv.wait(ul,[this]{
                        return !tasks.empty()||stop;
                    });
                    if(stop&&tasks.empty()) return;
                    function<void()> task(move(tasks.front()));
                    tasks.pop();
                    ul.unlock(); 
                    task();
                }
                
            });
        }
    }
    ~ThreadPool()
    {
        {   
            unique_lock<mutex> ul(mtx);
            stop = true;
        }

        cv.notify_all();
        for(auto &t:threads)
        {
            t.join();
        }
    }

    template<class F,class... Args>
    void enqueue(F &&f,Args&&... args)
    {
        function<void()> task = bind(forward<F>(f), forward<Args>(args)...);
        {
            unique_lock<mutex> ul(mtx);
            tasks.emplace(move(task));
        }
        cv.notify_one();
    }
};

int main()
{
    ThreadPool pool(4);
    for(int i=0;i < 10;i++)
    {
        pool.enqueue([i]{
            cout<<"task : "<<i<<endl;
            this_thread::sleep_for(chrono::seconds(1));
            cout<<"task : "<<i<<" over!"<<endl;
        });
    }
    return 0;
}

