/*
 * @Author: power
 * @Date: 2020-02-19 16:33:57
 * @LastEditTime: 2020-02-19 16:46:08
 * @LastEditors: Please set LastEditors
 * @Description: 定义底层协议规范，所有的自定义协议都必需继承这个类
 * @FilePath: /server/MsgRule/Rule.hpp
 */
#ifndef __RULE_H
#define __RULE_H
#include <string>
class BaseRule
{
private:
    /* data */
public:
   virtual void process(std::string msg) =0;
   
};


#endif // !__RULE_H
