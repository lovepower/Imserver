/*
 * @Author: power
 * @Date: 2020-01-28 10:31:28
 * @LastEditTime: 2020-02-22 11:05:09
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /test/Alloctor.h
 */
#ifndef _ALLOCTOR_H_ //解决头文件重复引入
#define _ALLOCTOR_H_ //宏定义头
#include <sys/types.h>
/*
 *重载全局运算符
*/
void* operator new(size_t size);
void operator delete(void* p);
void* operator new[](size_t size);
void operator delete[](void* p);
void* mem_alloc(size_t size);
void mem_free(void* p);
#endif // _ALLOCTOR_H_