/*
 * @Author: power
 * @Date: 2020-02-21 21:29:45
 * @LastEditTime: 2020-02-22 19:36:19
 * @LastEditors: Please set LastEditors
 * @Description: 自定义socket缓存区，异步发送信息，非阻塞IO
 * @FilePath: /server/Bootstart/BufferSocket.hpp
 */
#ifndef __BUFFERSOCKET_H
#define __BUFFERSOCKET_H
#include <string.h>
#include <mutex>
#include "../ThreadPool/ThreadPool.cpp"
void* do_task(void *arg);
class BufferSocket
{
private:
    /* data */
    
public:
    int fd;//socket
    char buff[1024];//自定义缓冲区
    ThreadPool* pool;//线程池
    pthread_mutex_t mutex;//互斥锁
    BufferSocket(int fd);
    ~BufferSocket();
    int writeBuffer(char *str);
};

BufferSocket::BufferSocket(int fd):fd(fd)
{
    ::pthread_mutex_init((&this->mutex),NULL);
}

BufferSocket::~BufferSocket()
{
    ::pthread_mutex_destroy(&(this->mutex));
    std::cout<<"我被删除了"<<std::endl;
}
/**
 * @description: 异步写
 * @param {buff} 
 * @return:
 */
int BufferSocket::writeBuffer(char *buff)
{
    
    pthread_mutex_lock(&(this->mutex));
    strcpy((this->buff+strlen(this->buff)),buff);//数据拷贝
    //执行任务
    ::add_task(this->pool,do_task,this);
    pthread_mutex_unlock(&(this->mutex));
    return 0;
}
void* do_task(void *arg)
{
    BufferSocket* bs = (BufferSocket *)arg;
    pthread_mutex_lock(&(bs->mutex));//上锁
    int len = strlen(bs->buff);
    if(len ==0)
    {
        pthread_mutex_unlock(&(bs->mutex));//解锁
        return nullptr;
    }

    int remain = 0;
    char buf[1024];

    while (1)
    {
        int ret = write(bs->fd,bs->buff,len);//非阻塞模式
        std::cout<<bs->buff<<std::endl;
        remain = len - ret;
        if(remain==0)
            break;
        else{
            //缓冲区大小不够不能一次发送
            std::cout<<"ddd"<<std::endl;
            strcpy(bs->buff,(bs->buff+ret));//将剩余拷贝
            len =  strlen(bs->buff);//更新长度
        }
        
        /* code */
    }
    memset(bs->buff, 0, sizeof(bs->buff));
    pthread_mutex_unlock(&(bs->mutex));//解锁
    
}

#endif // !__BUFFERSOCKET_H
