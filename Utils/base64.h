/*
 * @Author: power
 * @Date: 2020-02-27 19:24:16
 * @LastEditTime: 2020-02-27 19:24:40
 * @LastEditors: Please set LastEditors
 * @Description: Base64加解密算法头文件
 * @FilePath: /server/Utils/base64.h
 */
/*base64.h*/  
#ifndef _BASE64_H  
#define _BASE64_H  
  
#include <stdlib.h>  
#include <string.h>  
  
unsigned char *base64_encode(unsigned char *str);  
  
unsigned char *bae64_decode(unsigned char *code);  
  
#endif  