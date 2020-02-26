/*
 * @Author: your name
 * @Date: 2020-02-19 14:36:08
 * @LastEditTime: 2020-02-22 19:43:35
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings 
 * @FilePath: /server/ServerBoot.cpp
 */ 
#include "./Bootstart/Imserver.hpp"
int main(int argc, char const *argv[])
{
 
   baseServer *server = new Imserver;  
   server->initServer();
   server->process(); 
   sleep(10);
   server->close();
}
