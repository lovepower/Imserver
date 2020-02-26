/*
 * @Author: your name
 * @Date: 2020-02-19 17:17:55
 * @LastEditTime: 2020-02-20 19:48:03
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /server/Bootstart/baseServer.hpp
 */
#ifndef __BASESERVER_H_
#define __BASESERVER_H_
class baseServer
{
private:
    /* data */
public:
   /**
    * 初始化服务器
    * 开始处理连接
    * 关闭服务器
    *
    */
   virtual void initServer() = 0;
   virtual void process() = 0;
   virtual void close() = 0;

};


#endif // !__BASESERVER_H_