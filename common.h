#ifndef _COMMON_H
#define _COMMON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define StartFlag 0x40
#define EndFlag 0x23

typedef unsigned char byte;

typedef enum {
    OK = 0,            // 没问题
    LengthError,       // 数据长度不对
    IndexOutofRange,   // 数据超出范围了
    SumError,          // SUM 校验不通过
    StartFlagNotFound, // 起始符没找到
    EndFlagNotFound,   // 终止符没找到
    UnknownError       // 其余未知错误
} ErrorCode;

int *DecodeLength;
int *EncodeLength;

int ParseB2I(byte *input, int len);                // Byte -> Int
float ParseB2F(byte *input, int len);              // Byte -> Float
void ParseI2B(int input, byte *output, int len);   // Int -> Byte
void ParseF2B(float input, byte *output, int len); // Float -> Byte

int ReadLocalAddress();              // EEPROM 读取地址
void WriteLocalAddress(int address); // EEPROM 写入地址

int CheckAddress(int address);                      // 检查 EEPROM 的地址和传入的是否一致
void SendPackage(byte *Package, int PackageLength); // 发送数据包

ErrorCode DecodePackage(byte *input, int len, byte **output, int *address,
                        int *op); // 解码一个 Package，输入 input, len，输出 output, address, op 为解码结果
void EncodePackage(byte *input, int *len, byte **output, int address,
                   int op); // 编码一个 Package，输入 input, address, op，输出output，len为编码结果

#endif //_COMMON_H