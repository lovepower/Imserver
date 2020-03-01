/*
 * @Author: power
 * @Date: 2020-02-27 19:24:16
 * @LastEditTime: 2020-03-01 20:17:54
 * @LastEditors: Please set LastEditors
 * @Description: Base64加解密算法头文件
 * @FilePath: /server/Utils/base64.h
 */
/*base64.h*/  
#ifndef _BASE64_H  
#define _BASE64_H  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define u_char unsigned char
 
int encode_base64(const u_char *src, u_char *dst);
int decode_base64(const u_char *src, u_char *dst);
 
// int main(int argc, char *argv[])
// {
//         u_char src[100] = "rentiansheng";
//         u_char dst[100];
 
//         while(1){
//                 scanf("%s",src);
 
//                 printf("-----------------------------------\n");
//                 encode_base64(src, dst);
 
//                 printf("%s\n%s\n", src, dst);
//                 printf("-----------------------------------\n");
 
//                 decode_base64(dst, src);
//                 printf("%s\n%s\n", dst, src);
//                 printf("-----------------------------------\n");
                
//         }
 
//         return 0;
// }
 
int decode_base64(const u_char *src, u_char *dst)
{
        static char base64char[] ={
                -1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1 , -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1,
                52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
                -1 ,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
                15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63,
                -1 , 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
        };
 
        int i;
        size_t len = strlen((const char *)src);
 
        for(i = 0; i < len; i++){
                if(src[i] == -1){
                        return 1; //unexpected characters
                }
                else if(src[i] == '='){
                        len = i;
                }
        }
 
        if(len % 4 == 1){
                return  2; 
        }
 
        while(len > 3){
                *dst++ = (u_char)(base64char[src[0]]<<2) | (base64char[src[1]]>>4 & 0x3);
                *dst++ = (u_char)(base64char[src[1]]<<4 )|(base64char[src[2]]>>2 & 0xf);
                *dst++ = (u_char)(base64char[src[2]]<<6)|(base64char[src[3]]);
                
                src += 4;
                len -= 4;
        }
 
        if(len){
                if(len > 1){
                        *dst++ = (base64char[src[0]]<<2) | (base64char[src[1]]>>4 & 0x3);
                }
 
                if(len > 2){
                        *dst++ = (base64char[src[1]]<<4 )|(base64char[src[2]]>>2 & 0xf);
                }
 
        }
        *dst = 0;
        return 0;
}
 
 
int encode_base64(const u_char *src, u_char *dst)
{
        
        size_t len = strlen((const char*)src);
        static u_char base64char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
 
        while(len > 2){
                *dst++ = base64char[src[0] >> 2 & 0x3f];
                *dst++ = base64char[(src[0] & 0x3)<<4 | src[1] >> 4 & 0xf];
                *dst++ = base64char[(src[1] & 0xf)<<2 | src[2]>>6 & 0x3];
                *dst++ = base64char[src[2] & 0x3f];
                len -= 3;
                src += 3;
        }
 
        if(len){
                *dst++ = base64char[src[0] >> 2 & 0x3f];
                if(len > 1){
                        *dst++ = base64char[((src[0] & 0x3)<<4) | ((src[1]>> 4) & 0xf) ];
                        *dst++ = base64char[(src[1] & 0xf)<<2];
                }
                else{
                        *dst++ = base64char[(src[0] & 0x3) << 4];
                        *dst++ = '=';
                }
                *dst++ = '=';
        }
        
        *dst = 0;
 
}
#endif