/*
 * @Author: power
 * @Date: 2020-02-19 17:24:34
 * @LastEditTime: 2020-03-01 21:18:09
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /server/Bootstart/Imserver.hpp
 */
#ifndef __IM_SERVER_H_
#define __IM_SERVER_H_
#include "baseServer.hpp"
#include<iostream>
#include<unistd.h>
#include <stdlib.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <string>
#include "../Utils/Log.hpp"
#include "../Utils/cJSON.c"
#include "BufferSocket.hpp"
#include "../MsgRule/IM.hpp"
#include <map>
class Imserver;
/**
 * 基于epoll进行开发
 * 1.主线程接受客户端连接
 * 2.开启2线程处理消息到来
 */
struct Dispath_Event
{
    /* data */
    int cfd;
    Imserver *server;
    epoll_event epollData;
    
};

void* dispath_client(void *arg);
class Imserver : public baseServer
{
private:
    /* data */
    int port;
    struct sockaddr_in serv;
    int epfd;
    int fd;
    epoll_event ev;
    epoll_event all[2000];
    ThreadPool *dispathPool;//处理客户端线程池
    ThreadPool *sendPool;//处理异步信息发送线程池
    BufferSocket* sbs;
    std::map<string,int> userMap;//保存用户的连接信息
public:
    Imserver(/* args */);
    ~Imserver();
    void initServer() ;
    void process() ;
    void close() ;
    void readConfig();
    void setNoBlock(int fd);
    void addUser(string id,int fd);
    int getUser(string id);
};

Imserver::Imserver(/* args */)
{
    //初始化操作
    memset(&serv,0,sizeof(serv));

}

Imserver::~Imserver()
{
}
/**
 *基于libevent的tcp连接 
 */
void Imserver::initServer()
{
    LogMannger::getInstance()->logInfo(INFO_LEVEL,__LINE__,__FILE__,"初始化服务器!");
    dispathPool = new ThreadPool;
    sendPool = new ThreadPool;
    init_pool(dispathPool,5);
    init_pool(sendPool,2);
    readConfig();
    memset(&serv,0,sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    fd = socket(AF_INET,SOCK_STREAM,0);
    int ops = 1;
    setsockopt(fd,SOL_SOCKET,SO_REUSEPORT,&ops,sizeof(ops));
    sbs = new BufferSocket(fd);
    bind(fd,(sockaddr *)&serv,sizeof(serv));
    listen(fd,5);
    LogMannger::getInstance()->logInfo(INFO_LEVEL,__LINE__,__FILE__,"初始化服务器完成!");
    
}
void Imserver::process()
{
    LogMannger::getInstance()->logInfo(INFO_LEVEL,__LINE__,__FILE__,"开始启动服务器!");
    epfd = epoll_create(2000);
    ev.events = EPOLLIN;
    ev.data.ptr = (void *)sbs;   
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
    sockaddr_in client_addr;
    socklen_t cli_len = sizeof(client_addr);
    BufferSocket* bs = nullptr;
    Dispath_Event* event = nullptr;
    
    while (1)
    {
        LogMannger::getInstance()->logInfo(INFO_LEVEL,__LINE__,__FILE__,"进入epoll!");
        int ret = epoll_wait(epfd,all,2000,-1);//监听客户端连接
        for (int i = 0; i < ret; i++)
        {
            /* code */
            // int zfd = (BufferSocket *)(all[i].data.ptr)->fd;
            int zfd = ((BufferSocket *)(all[i].data.ptr))->fd;
            if (this->fd == zfd)
            {
                /* code */
                int cfd = accept(fd,(sockaddr*)&client_addr,&cli_len);
                if(cfd == -1)
                {
                    LogMannger::getInstance()->logInfo(ERROR_LEVEL,__LINE__,__FILE__,"接受连接失败");
                    continue;              
                }
                setNoBlock(cfd);
                //挂载新的节点
                struct epoll_event client;
                bs = new BufferSocket(cfd);
                bs->pool = sendPool;
                client.events = EPOLLIN | EPOLLET;
                client.data.ptr = (void *)bs;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&client);

            }else{
                //客户端信息到来 才有线程池
                event = new Dispath_Event;
                event->cfd = zfd;
                event->server = this;
                event->epollData = all[i];
                add_task(dispathPool,dispath_client,(void *)event); //进行任务调度               
            }
            
        }
        
        

       //
        
    }

    
    LogMannger::getInstance()->logInfo(INFO_LEVEL,__LINE__,__FILE__,"启动服务器完成!");
}
void Imserver::close()
{
    LogMannger::getInstance()->logInfo(INFO_LEVEL,__LINE__,__FILE__,"关闭启动服务器!");
    destory_pool(dispathPool);
    destory_pool(sendPool);
    delete sbs;
}
/**
 * @description: 读取配置文件
 * @param {type} 
 * @return: 
 */
void Imserver::readConfig()
{
    char* configFile = "Config/Server.json";
    char configStr[1000] = {0};
    if(FileMannger::getInstance()->readFile(configFile,configStr,1000)==-1){
        LogMannger::getInstance()->logInfo(WARNING_LEVEL,__LINE__,__FILE__,"读取文件失败");
        exit(0);
        return;
    }
    cJSON *root = cJSON_Parse(configStr);
    cJSON *item = cJSON_GetObjectItem(root,"port");
    port = item->valueint;
}
void Imserver::setNoBlock(int fd)
{
     int flags;
     if(flags = fcntl(fd, F_GETFL, 0) < 0)
    {
        
        LogMannger::getInstance()->logInfo(ERROR_LEVEL,__LINE__,__FILE__,"获取文件描述失败!");
    }
    flags |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flags) < 0)
    {
        LogMannger::getInstance()->logInfo(ERROR_LEVEL,__LINE__,__FILE__,"设置文件描述失败!");
    }
}
/**
 * @description: 处理来自客户端的消息
 * @param {type} 
 * @return: 
 */
void* dispath_client(void *arg)
{
    Dispath_Event * event = (Dispath_Event *)arg;
    if (!event->epollData.events & EPOLLIN)
    {
        /* code */
        delete event;
        return nullptr;
    }
    char buf[256] ={0};
    int len;
    BufferSocket* bs = (BufferSocket *)(event->epollData.data.ptr);
    IM im;
    im.setBufferSocket(bs);
    while ((len=recv(event->cfd,buf,sizeof(buf),0))>0)
    {
        /* code */
        //处理数据
        //回送消息
        //使用协议处理器进行处理
        std::cout<<"来自客户端的信息:"<<buf<<std::endl;
        char sendtomsg[256] = "111";
        std::string str(buf);
        im.process(str);//解析出数据进行处理
        bs->writeBuffer(sendtomsg);
        
        

        
    }
    if(len==0)
    {
        //连接断开
        delete event->epollData.data.ptr;
    }else if (len ==-1)
    {
        /* code */
        if(errno == EAGAIN)
        {
            //缓冲区为空
        }else
        {
            /* code */
            LogMannger::getInstance()->logInfo(ERROR_LEVEL,__LINE__,__FILE__,"发生了接受信息的错误！");
        }
        
    }
    delete event;//事件执行完毕清除事件
    
    
    
}
void Imserver::addUser(string id,int fd)
{
    userMap.insert(make_pair(id,fd));
}
int Imserver::getUser(string id)
{
    std::map<string,int>::iterator iter;
    iter = userMap.find(id);
    if(iter != userMap.end())
        return iter->second;
    return -1;
}


#endif // !__IM_SERVER_H_