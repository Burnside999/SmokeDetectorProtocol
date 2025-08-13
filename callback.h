#ifndef _CALLBACK_H
#define _CALLBACK_H

#include "common.h"

typedef void (*BasicCallback)(byte *, int, int, ErrorCode, void *aim,
                              bool is_float); // param, address, op, err, aim, float

typedef struct {
    bool is_float;
    void *aim;
    BasicCallback callback;
} CallbackHandle; // 回调句柄

CallbackHandle *EncodeCallBack;

void RegisterCallback(int op, BasicCallback Func, void *aim, bool is_float); // 注册一个回调函数

void BasicWrite(byte *param, int address, int op, ErrorCode err, void *aim, bool is_float); // 基础的往 aim 中写入数据
void BasicRead(byte *param, int address, int op, ErrorCode err, void *aim, bool is_float); // 基础的从 aim 中读出数据

// 以下是自定义回调函数

/*
    复位，因为这个函数不需要返回，很特殊，单独处理，只需要传入 address, op 和 err，其余都可以为空。
*/
void Reset(byte *param, int address, int op, ErrorCode err, void *aim, bool is_float);

/*
    写地址，因为这个函数不需要AddressCheck，单独处理一下，只需要传入 param, op, err，其余都可以为空，
*/
void WriteAddress(byte *param, int address, int op, ErrorCode err, void *aim, bool is_float);

/*
    aim 的类型为 byte* 时的情况，适用于多于 4 字节时读出的情况，比如光强，巡检，is_float 可以为空
*/
void BytesRead(byte *param, int address, int op, ErrorCode err, void *aim, bool is_float);

#endif //_CALLBACK_H