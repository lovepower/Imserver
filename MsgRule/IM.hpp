/*
 * @Author: power
 * @Date: 2020-02-19 16:32:07
 * @LastEditTime: 2020-03-14 21:14:02
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /server/MsgRule/IM.hpp
 */
#ifndef __IM_H_
#define __IM_H_
#include "Rule.hpp"
#include "../Bootstart/BufferSocket.hpp"
#include "../Utils/base64.h"
#include "../Utils/cJSON.c"
#define IM_SEND 1 //发送聊天信息
#define IM_KEEPALIVE 2 //保持心跳
#define IM_SIGNED 3 //签收信息
#define IM_CONNECT 4 //连接信息
class IM : public BaseRule
{
private:
    /* data */
    int type;
    std::string content; //消息内容
    std::string sendId;//发送者ID
    std::string receiveId;//联系人ID
    std::string last_str;
    bool isNext;
    bool isFirst;
public:
    void process(std::string msg);
    void praseJson(char* dist);
    void init();
    void parse(std::string msg);
    int getType();
    string getSendId();
    string getReceiveId();
    string getContent();
    bool getIsNext();
    bool getIsFirst();
};
bool IM::getIsNext()
{
    return isNext;
}
bool IM::getIsFirst()
{
    return isFirst;
}
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
    parse(msg);//处理消息

}
void IM::parse(std::string msg)
{
    type = -1;
    isNext = false;
    isFirst = false;
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
            //json解密 type sendId receiveId content
            praseJson((char *)dist);
            
            //类型 发送人 接收人 信息 解析出来服务器作为中转，内容进行解密
            //获取解密后的数据
            if (msg.length() == rindex+1)
                break;
            else{
                isNext = true;
                lindex= rindex+1;//还有剩余的
                last_str = std::string(msg,lindex,-1);//将剩余的保存结束这次调度
                break;
            }
           
            
        }else
        {
            //此时出现了粘包现象将剩余的包进行保存
            last_str = std::string(msg,lindex,-1);
            break;
        }
        
        
    
     
    // bs->writeBuffer((char *)msg.data());
    
}
}
void IM::praseJson(char* dist)
{
     cJSON* root = cJSON_Parse((const char*)dist);
            cJSON* typeItem = cJSON_GetObjectItem(root,"type");
            if(typeItem)
                type = typeItem->valueint;
            cJSON* sendIdItem = cJSON_GetObjectItem(root,"sendId");
            if (sendIdItem)
                sendId = sendIdItem->valuestring;
            cJSON* receiveItem = cJSON_GetObjectItem(root,"receiveId");
            if(receiveItem)
                receiveId = receiveItem->valuestring;
            cJSON* contentItem = cJSON_GetObjectItem(root,"content");
            if(contentItem)
                content = contentItem->valuestring;
            cJSON_Delete(root);
}
void IM::init()
{
    type = -1;
    isFirst = true;
}
int IM::getType()
{
    return type;
}
string IM::getSendId()
{
    return sendId;
}
string IM::getReceiveId()
{
    return receiveId;
}
string IM::getContent()
{
    return content;
}
#endif // !__IM_H_