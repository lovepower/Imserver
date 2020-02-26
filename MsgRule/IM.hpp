/*
 * @Author: power
 * @Date: 2020-02-19 16:32:07
 * @LastEditTime: 2020-02-19 17:16:56
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /server/MsgRule/IM.hpp
 */
#ifndef __IM_H_
#define __IM_H_
#include "Rule.hpp"

class IM : public BaseRule
{
private:
    /* data */
    int type;            //消息类型
    std::string content; //消息内容

public:
    IM(/* args */);
    void setType(int type);
    void setContent(std::string content);
    ~IM();
    void process(std::string msg);
};

IM::IM(/* args */)
{
}

IM::~IM()
{
}
void IM::process(std::string msg)
{
    //处理json
    
    
}
void IM::setType(int type)
{
    this->type=type;
}
void IM::setContent(std::string content)
{
    this->content = content;
}
#endif // !__IM_H_