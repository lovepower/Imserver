/*
 * @Author: your name
 * @Date: 2020-02-19 14:20:27
 * @LastEditTime: 2020-03-13 19:37:26
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /server/Utils/Time.hpp
 */
#ifndef __Time_h_
#define __Time_h_
#include<time.h>
#include<stdlib.h>
#include <stdio.h>
class Time
{
private:
    /* data */
public:
    Time(/* args */);
    static Time* getInstance();
    static void clear();
    void getDateTime(char * stime);
    long getLongTiem();
    ~Time();
private:
    static Time *stime;
};
Time* Time::stime = nullptr;

Time::Time(/* args */)
{
}

Time::~Time()
{
}
Time* Time::getInstance()
{
    if (stime == nullptr)
        stime = new Time;
    return stime;
    
}
void Time::clear()
{
    delete stime;
    stime = nullptr;
}
void Time::getDateTime(char *stime)
{
    time_t nSeconds;
     struct tm * pTM;
    
    time(&nSeconds);
    pTM = localtime(&nSeconds);

    /* 系统日期和时间,格式: yyyymmddHHMMSS */
    sprintf(stime, "%04d-%02d-%02d %02d:%02d:%02d",
            pTM->tm_year + 1900, pTM->tm_mon + 1, pTM->tm_mday,
            pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

    
}
/**
 * @description: 获取当前时间戳
 * @param {type} 
 * @return: long 当前时间戳
 */
long Time::getLongTiem()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}


#endif // !__Time_h_


