/*
 * @Author: power
 * @Date: 2020-02-19 17:24:34
 * @LastEditTime: 2020-03-13 22:41:27
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
#include <signal.h>
class Imserver;
/**
 * 基于epoll进行开发
 * 1.主线程接受客户端连接
 * 2.开启2线程处理消息到来
 * 3.实际是开发网关应用，后期消息处理可以使用http服务器进行从而实现分布式集群架构，充分将两者结合起来。0
 */
struct Dispath_Event
{
    /* data */
    int cfd;
    Imserver *server;
    epoll_event epollData;
    
};

void* dispath_client(void *arg);
void idle_check(int sig);//空闲检测
class Imserver : public baseServer
{
private:
    /* data */
    int port;
    struct sockaddr_in serv;
    static int epfd;
    int fd;
    epoll_event ev;
    epoll_event all[2000];
    ThreadPool *dispathPool;//处理客户端线程池
    ThreadPool *sendPool;//处理异步信息发送线程池
    BufferSocket* sbs;
    static std::map<string,int> userMap;//保存用户的连接信息
    static std::map<int,BufferSocket *> heartMap;//维持心跳；
public:
    Imserver(/* args */);
    ~Imserver();
    void initServer() ;
    void process() ;
    void close() ;
    void readConfig();
    void setNoBlock(int fd);
    static void addUser(string id,int fd);
    static int getUser(string id);
    static int removeUser(int fd);
    static void addHeart(int id,BufferSocket *bs);
    static void processHeart();
    static void removeHeart(int id);
    static int getCfd();
    static int timeout;//定义心跳超时间
};
std::map<string,int> Imserver::userMap=std::map<string,int>();
std::map<int,BufferSocket *>Imserver::heartMap = std::map<int,BufferSocket *>();
int Imserver::timeout = 0;
int Imserver::epfd = 0;
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
    //定时器开启
    struct itimerval heart_time = {{timeout,0},{timeout,0}};//定义时间间隔
    setitimer(ITIMER_REAL,&heart_time,nullptr);
    struct sigaction act;
    act.sa_handler = idle_check;
    act.sa_flags =0;
    act.sa_restorer = nullptr;
    sigemptyset(&act.sa_mask);
    sigaction(SIGALRM,&act,nullptr);
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
    ::close(this->fd);
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
    cJSON *item1 = cJSON_GetObjectItem(root,"timeout");
    timeout = item1->valueint;
    port = item->valueint;
    cJSON_Delete(root);
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
    IM im;//实际是模拟分布式http服务器，以后可以进行分布式扩展
    while ((len=recv(event->cfd,buf,sizeof(buf),0))>0)
    {
        /* code */
        //处理数据
        //回送消息
        //使用协议处理器进行处理
        im.init();//初始化
        // std::cout<<"来自客户端的信息:"<<buf<<std::endl;
        std::string str(buf);
        im.process(str);//解析出数据进行处理
        //数据解析完成,开始处理数据
        if(im.getType()== -1)
        {
            continue;
        }
        if (im.getType() == IM_CONNECT)
        {
            /* code */
            //处理客户端首次连接 验证身份（目前先不做）
            Imserver::addUser(im.getSendId(),event->cfd);//将用户加入
            Imserver::addHeart(event->cfd,bs);
            bs->lastSendTime = Time::getInstance()->getLongTiem();
            continue;
            
        }
        if (im.getType() == IM_SEND)
        {
            //转发信息
            //1.构建信息
            bs->lastSendTime = Time::getInstance()->getLongTiem();
            cJSON *root = cJSON_CreateObject();
            cJSON_AddNumberToObject(root,"type",IM_SIGNED);
            cJSON_AddStringToObject(root,"sendId",im.getSendId().c_str());
            cJSON_AddStringToObject(root,"receiveId",im.getReceiveId().c_str());
            cJSON_AddStringToObject(root,"content",im.getContent().c_str());
            char *msg = cJSON_Print(root);
            char sendtomsg[256];
            encode_base64((const unsigned char *)msg,(unsigned char *)sendtomsg);
            cJSON_Delete(root);
            free(msg);
            strcat(sendtomsg,"\n");
            bs->writeBuffer(sendtomsg);           
            continue;
        }
        if (im.getType() == IM_KEEPALIVE)
        {
            /* code */
            //用户心跳检测；
            bs->lastSendTime = Time::getInstance()->getLongTiem();
            continue;
        }
        if (im.getType() == IM_SIGNED)
        {
            /* code */
            //信息接受处理数据库
            bs->lastSendTime = Time::getInstance()->getLongTiem();
            continue;
        }
        
        
        
        
        
        

        
    }
    if(len==0)
    {
        //连接断开
        Imserver::removeUser(event->cfd);
        Imserver::removeHeart(event->cfd);
        epoll_ctl(event->server->getCfd(),EPOLL_CTL_DEL,event->cfd,NULL);
        delete event->epollData.data.ptr;
        close(event->cfd);

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
/**
 * @description: 根据套接字fd
 * @param cfd 
 * @return: 0 成功,-1 失败
 */
int Imserver::removeUser(int cfd)
{
    std::map<string,int>::iterator iter;
    iter = userMap.begin();
    int status = -1;
    while (iter != userMap.end())
    {
        /* code */
        if (iter->second == cfd)
        {
            /* code */
            userMap.erase(iter);//删除保存的客户端信息
            status = 0;
            break;
        }
        iter++;
        
    }
    
    return status;
}
int Imserver::getCfd()
{
    return epfd;
}
void idle_check(int sig)
{
    std::cout <<"来了"<<sig<<std::endl;
    Imserver::processHeart();
    
}
void Imserver::addHeart(int id,BufferSocket *bs)
{
    heartMap.insert(make_pair(id,bs));
}
void Imserver::removeHeart(int id)
{
    std::map<int,BufferSocket*>::iterator iter;
    iter = heartMap.begin();
    while (iter != heartMap.end())
    {
        /* code */
        if (iter->first == id)
        {
            /* code */
            heartMap.erase(iter);//删除保存的客户端信息
            break;
        }
        iter++;
        
    }
}
void Imserver::processHeart()
{
     std::map<int,BufferSocket*>::iterator iter;
    iter = heartMap.begin();
    while (iter != heartMap.end())
    {
        /* code */
        long currTime = Time::getInstance()->getLongTiem();
        if (currTime - (iter->second->lastSendTime) > (long)Imserver::timeout)
        {
            /* code */
            //说明超时了移除客户端连接
            std::cout <<"移除"<<std::endl;
            removeUser(iter->second->fd);
            heartMap.erase(iter);//删除保存的客户端信息
            epoll_ctl(Imserver::getCfd(),EPOLL_CTL_DEL,iter->second->fd,NULL);//下一次重构
            delete iter->second;
            ::close(iter->second->fd);
        }
        iter++;
        
    }
}
#endif // !__IM_SERVER_H_