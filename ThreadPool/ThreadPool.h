/*
 * @Author: power
 * @Date: 2020-02-02 13:43:24
 * @LastEditTime: 2020-02-22 11:09:34
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /test/ThreadPool.h
 */
#ifndef __ThreadPool_h_/*define*/
#define __ThreadPool_h_/*define*/
/* code */

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include "../Memory/Alloctor.cpp"//内存池
#include <iostream>
#define MAX_WAITING_TASKS 1000//最大任务数
#define MAX_ACTIVE_THREADS 20//最大线程数
/**
 *  任务节点结构体 
 **/
struct task
{
    /* data */
    void* (*do_task)(void *arg);//任务函数指针
    void * arg;//参数
    task *next;//指向下一个任务节点的指针
};
/*
*线程池构建
*/
struct ThreadPool
{
    /* data */
    pthread_mutex_t lock;//互斥锁
    pthread_cond_t cond;//条件变量
    bool shutdown;//true 关闭,false开启
    task *task_list;//任务队列
    pthread_t *tids;//存放线程ID
    unsigned max_waiting_tasks;//最大任务等待数
    unsigned waiting_tasks;//当前等待任务数
    unsigned active_threads;//当前线程池线程的个数

};
/**
 * 初始化线程池
 * pool 线程池
 * threads_number 线程池中维护线程的个数
 **/

bool init_pool(ThreadPool *pool,unsigned int threads_number);
/**
 * 线程调度函数
 **/
void * dispath(void *arg);
/**
 * 任务添加函数
 * pool 线程池
 * do_task 任务函数指针
 **/
bool add_task(ThreadPool *pool,void *(*do_task)(void *arg),void *arg);
/**
 * 销毁线程池
 * pool 线程池
 **/
bool destory_pool(ThreadPool *pool);
#endif ///*define*/
