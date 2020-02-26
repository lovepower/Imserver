/*
 * @Author: your name
 * @Date: 2020-02-19 14:58:54
 * @LastEditTime: 2020-02-20 22:29:49
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /server/Utils/FileMannger.hpp
 */
#ifndef __FILEMANNGER_h_
#define __FILEMANNGER_h_
#include "../Memory/Alloctor.cpp"//内存池
#include <stdlib.h>
#include "string.h"
class FileMannger
{
private:
    /* data */
public:
    FileMannger(/* args */);
    static FileMannger* getInstance();
    void writeFile(char* fileName,char* msg);
    int readFile(char* fileName,char *str,int length);
    static void clear();
    ~FileMannger();
private:
    static FileMannger * fileMannger;
};
FileMannger* FileMannger::fileMannger = nullptr;

FileMannger::FileMannger(/* args */)
{
    
}

FileMannger::~FileMannger()
{
}
FileMannger* FileMannger::getInstance()
{
    if (fileMannger == nullptr)
    {
        /* code */
        fileMannger = new FileMannger;
    }
    return fileMannger;
    
}
void FileMannger::writeFile(char* fileName,char* msg)
{
    FILE *fp;
    fp = fopen(fileName,"a+");
    if (fp==nullptr)
        return;
    fwrite(msg,sizeof(char),strlen(msg),fp);
    fclose(fp);
    

}
void FileMannger::clear()
{
    delete fileMannger;
}
int FileMannger::readFile(char *fileName,char* str,int length)
{
    FILE *fp;
    fp = fopen(fileName,"r");
    if(fp==nullptr)
    {
        std::cout<<"失败"<<std::endl;
        return -1;
    }
    fread(str,length,1,fp);
    fclose(fp);
    return 0;
    
}

#endif // 