/*
 * @Author: power
 * @Date: 2020-02-02 14:18:20
 * @LastEditTime: 2020-02-22 13:07:38
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /test/ThreadPool.cpp
 */
#ifndef __POLL_H_CPP
#define __POLL_H_CPP
#include "ThreadPool.h"
using namespace std;
void handler(void *arg)
{
    cout<<(unsigned)pthread_self()<<"任务结束.\n";
    pthread_mutex_unlock((pthread_mutex_t *)arg);
}
void * dispath(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    task *p;
    while (1)
    {
        /* code */
        
        pthread_cleanup_push(handler,(void *)&pool->lock);//注册清理线程函数
        pthread_mutex_lock(&pool->lock);//上锁去访问任务队列
        while(pool->waiting_tasks==0 && !pool->shutdown)
        {
            pthread_cond_wait(&pool->cond,&pool->lock);//条件变量，队列为空进行等待
        }
        if (pool->waiting_tasks ==0 && pool->shutdown)
        {
            /* code */
            //线程池已关闭结束线程。
            pthread_exit(NULL);//线程结束

        }
        //做任务
        p = pool->task_list->next;
        pool->task_list->next = p->next;
        pool->waiting_tasks--;//任务数减一
        pthread_mutex_unlock(&pool->lock);
        pthread_cleanup_pop(0);     
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);//设置不可响应取消线程
        p->do_task(p->arg);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
        // free(p);
        delete p;

    }
    
    pthread_exit(NULL);
}
bool init_pool(ThreadPool *pool,unsigned int threads_number)
{
    //1.初始化互斥锁
    pthread_mutex_init(&pool->lock,NULL);
    //2.初始化条件变量
    pthread_cond_init(&pool->cond,NULL);
    //3.设置线程标志位
    pool->shutdown = false;
    //4.初始化任务队列
    // pool->task_list = (task *)malloc(sizeof(task));
    pool->task_list = new task;
    //5.线程池ID分配空间
    // pool->tids = (pthread_t *)malloc(sizeof(pthread_t) * MAX_ACTIVE_THREADS);
    pool->tids = new pthread_t[sizeof(pthread_t) * MAX_ACTIVE_THREADS];
    if(pool->task_list == NULL || pool->tids == NULL)
    {
        perror("分配空间失败");
        return false;
    }
    pool->task_list->next = NULL;//头结点为空
    //设置最大任务个数
    pool->max_waiting_tasks = MAX_WAITING_TASKS;
    //设置当前等待任务个数
    pool->waiting_tasks = 0;
    //设置当前线程池线程个数
    pool->active_threads = threads_number;
    //创建线程
    for (int i = 0; i < pool->active_threads; i++)
    {
        /* code */
        if(pthread_create(&((pool->tids)[i]),NULL,dispath,(void *)pool)!=0)
        {
            perror("创建线程失败");
            return false;
        }
    }
    return true;
    

}
bool add_task(ThreadPool *pool,void *(*do_task)(void *arg),void *arg)
{
    //为新节点申请内存空间
    // task *new_task = (task *)malloc(sizeof(task));
    task *new_task = new task;
    if(new_task == NULL)
    {
        perror("分配任务空间失败！");
        return false;
    }
    //新节点初始化
    new_task->do_task = do_task;//任务函数
    new_task->arg = arg;//函数参数
    new_task->next = NULL;
    //访问任务队列
    pthread_mutex_lock(&pool->lock);
    if(pool->waiting_tasks>=MAX_WAITING_TASKS)
    {
        pthread_mutex_unlock(&pool->lock);
        cout << "任务队列已满！"<<endl;
        // free(new_task);
        delete new_task;
        return false;
    }
    //添加任务到任务队列中
    task *tmp = pool->task_list;
    while (tmp->next!=NULL)
    {
        /* code */
        tmp = tmp->next;
    }
    tmp->next = new_task;
    pool->waiting_tasks++;
    //解锁任务结束
    pthread_mutex_unlock(&pool->lock);
    pthread_cond_signal(&pool->cond);//唤醒线程
    return true;
    

}
bool destory_pool(ThreadPool *pool)
{
    pool->shutdown = true;//关闭
    int err;
    pthread_cond_broadcast(&pool->cond);//唤醒所有等待的线程，让其主动判断关闭。
    for (int i = 0; i < pool->active_threads; i++)
    {
        /* code */
        err = pthread_join(pool->tids[i],NULL);
        if(err != 0)
        {
            cout<<"错误："<<strerror(err)<<endl;
        }
        else
        {
            cout << (unsigned)pool->tids[i]<<"线程已被关闭"<<endl;
        }
        
    }
    // free(pool->task_list);
    // free(pool->tids);
    // free(pool);
    delete pool->task_list;
    delete pool->tids;
    delete pool;
    return true;
    
}
#endif // !__POLL_H_CPP
