/*
 * @Author: power
 * @Date: 2020-02-19 16:32:07
 * @LastEditTime: 2020-02-26 21:56:20
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /server/MsgRule/IM.hpp
 */
#ifndef __IM_H_
#define __IM_H_
#include "Rule.hpp"
#include "../Bootstart/BufferSocket.hpp"
#define IM_SEND 1 //发送聊天信息
#define IM_KEEPALIVE 2 //保持心跳
#define IM_SUGNED 3 //签收信息
class IM : public BaseRule
{
private:
    /* data */
    int type;            //消息类型
    std::string content; //消息内容
    int sendId;//发送者ID
    int receiveId;//联系人ID
    BufferSocket * bs;

public:
    IM(/* args */);
    void setType(int type);
    void setContent(std::string content);
    ~IM();
    void process(std::string msg);
    void setBufferSocket(BufferSocket *bs);
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
void IM::setBufferSocket(BufferSocket *bs){
    this->bs = bs;
}
#endif // !__IM_H_