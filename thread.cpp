#include <iostream>
#include <mingw.thread.h>
#include <memory>
#include <mingw.mutex.h>
#include <mingw.condition_variable.h>
#include <mingw.future.h>
using namespace std;

void func(string s) {
    cout << s << endl;
}

void func(int &a) {
    a++;
}
mutex mtx;
timed_mutex tmtx;
void addi(int &a)
{
    
    for(int i=0;i < 100000;i++)  
    {
        // mtx.lock();
        // lock_guard<mutex> lg(mtx,adopt_lock);//智能锁
        unique_lock<mutex> ul(mtx,defer_lock);
        ul.lock();
        a++;
        // mtx.unlock();
    }
    
}

class T
{
private:
    // static T *instance;
    static once_flag once; 
    T(){};
    ~T(){};
public:
    // static T& getinstance()
    // {
    //     call_once(once,[]{
    //         instance = new T();
    //     });
    //     return *instance;
    // }
    static T& getinstance()
    {
        static T instance;
        return instance;
    }

};

// T* T::instance = nullptr;
once_flag T::once;



int main() {
    
    // 示例1：传递字符串（按值）
    thread t1([](){func("Hello World!");});
    t1.join();

    // 示例2：传递引用
    int a = 0;
    thread t2([&](){func(a);});  // 必须用 std::ref
    t2.join();
    cout << "a = " << a << endl;  // a = 1

    thread t3(addi,ref(a));
    thread t4(addi,ref(a));
    t3.join();
    t4.join();
    cout << "a = " << a << endl;

    future<int> fr = async(launch::async,[]{return 1;});

    int res = fr.get();
    cout<<res<<endl;
    return 0;
}