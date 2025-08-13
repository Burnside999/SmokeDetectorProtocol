#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "callback.h"

// Function

/*
    注册操作数、回调函数数组等信息
*/
void InitProtocol();

/*
    解码一个数据包，并产生回码
*/
ErrorCode parse(byte *package, int len);

#endif //_PROTOCOL_H