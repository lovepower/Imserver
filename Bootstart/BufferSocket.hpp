/*
 * @Author: power
 * @Date: 2020-02-21 21:29:45
 * @LastEditTime: 2020-02-26 20:54:32
 * @LastEditors: Please set LastEditors
 * @Description: 自定义socket缓存区，异步发送信息，非阻塞IO
 * @FilePath: /server/Bootstart/BufferSocket.hpp
 */
#ifndef __BUFFERSOCKET_H
#define __BUFFERSOCKET_H
#include <string.h>
#include <mutex>
#include "../ThreadPool/ThreadPool.cpp"
#include <errno.h>
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
    char buf[1024];//拷贝
    strcpy(buf,bs->buff);
    memset(bs->buff, 0, sizeof(bs->buff));
    pthread_mutex_unlock(&(bs->mutex));//解锁
    while (1)
    {
        int ret = write(bs->fd,buf,len);//非阻塞模式
        std::cout<<bs->buff<<std::endl;
        if(ret == -1){
           if(errno == EAGAIN){
               continue;//缓存区容量不够再一次操作。
           }
        }else if(ret == 0)
        {
            break;//客户端已断开
        }else
        {
            break;
        }
               
        /* code */
    }
    
}

#endif // !__BUFFERSOCKET_H
