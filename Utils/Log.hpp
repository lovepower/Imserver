/*
 * @Author: power
 * @Date: 2020-02-19 10:30:25
 * @LastEditTime: 2020-02-21 20:12:54
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /server/Utils/include/Log.hpp
 */
#ifndef __LOG_H_
#define __Log_H_
#include "../Memory/Alloctor.cpp"//内存池
#include "sys/time.h"
#include <iostream>
#include <string>
#include "FileMannger.hpp"
#include "Time.hpp"
#define DEFALUT_FILE "ServerLog.log"
//日志 level级别
#define DEBUG_LEVEL 1
#define INFO_LEVEL 2
#define WARNING_LEVEL 3
#define ERROR_LEVEL 4
class LogMannger
{
private:
    /* data */
public:
    LogMannger(/* args */);
    static LogMannger* getInstance();
    static void clear();
    ~LogMannger();
    void logInfo(int level,int line,char *fileName,char *msg);
private:
    static LogMannger* log;
};
LogMannger* LogMannger::log = nullptr;

LogMannger::LogMannger(/* args */)
{
}

LogMannger::~LogMannger()
{
   
}
LogMannger* LogMannger::getInstance()
{
    if(log==nullptr)
        log = new LogMannger;
    return log;
/**
 * @description: 
 * @param {type} 
 * @return: 
 */
}
void LogMannger::logInfo(int level,int line,char *fileName,char *msg)
{
    std::string levelName[4] = {"debug","info","warning","error"};
    char *stime = new char;
    
    Time::getInstance()->getDateTime(stime);
    if(level<4)
    {
        std::cout<<levelName[level-1]<<":"<<"时间:"<<stime<<"发生在"
        <<fileName<<"第"<<line<<"行"<<"详情:"<<msg<<std::endl;
    }else
    {
        char details[128];
        sprintf(details,"%s:时间:%s:发生在%s文件:第%d行:详情:%s\n",levelName[level-1].data(),stime,fileName,line,msg);
        std::cout<<details;
        //写入文件
        /* code */
        FileMannger::getInstance()->writeFile(DEFALUT_FILE,details);
    }
    delete stime;
}
void LogMannger::clear()
{
    delete log;
    log = nullptr;
}


#endif // !__LOG_H_
