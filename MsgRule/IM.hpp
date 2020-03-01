/*
 * @Author: power
 * @Date: 2020-02-19 16:32:07
 * @LastEditTime: 2020-03-01 20:47:04
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /server/MsgRule/IM.hpp
 */
#ifndef __IM_H_
#define __IM_H_
#include "Rule.hpp"
#include "../Bootstart/BufferSocket.hpp"
#include "../Utils/base64.h"
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
    std::string last_str;
public:
    void setType(int type);
    void setContent(std::string content);
    void process(std::string msg);
    void setBufferSocket(BufferSocket *bs);
};
void IM::process(std::string msg)
{
    //处理json
    //解析数据
    if(last_str.length()!=0)
    {
        //上次粘包遗留的数据拼接
        last_str.append(msg);
        msg = last_str;
        last_str.clear();//清空数据
    }
    int rindex = 0;
    int lindex = 0;
    while (true)
    {
        /* code */
        rindex = msg.find('\n',lindex);
        if (rindex != msg.npos)
        {
            /* code */
            string data(msg,lindex,rindex-lindex);
            //base64解码
            unsigned char dist[256];//目标解析出的字符串
            //json结构体解码开始判断类型
            decode_base64((const unsigned char *)data.c_str(),dist);
            //json解密
            
            //类型 发送人 接收人 信息 解析出来服务器作为中转，内容进行解密
            //获取解密后的数据
            if (msg.length() == rindex+1)
                break;
            lindex= rindex+1;
            
        }else
        {
            //此时出现了粘包现象将剩余的包进行保存
            last_str = std::string(msg,lindex,-1);
            break;
        }
        
        
    
     
    // bs->writeBuffer((char *)msg.data());
    
}
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