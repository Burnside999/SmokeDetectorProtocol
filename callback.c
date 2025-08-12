/*
    Author: _Burnside
    Description: 该文件包含了各类回调函数，还有注册回调函数的接口，注册时需要指明：
                 1.回调函数是什么；
                 2. 回调函数的作用地址（读或写）；
                 3.作用的该地址处理时是否要被当作浮点数

                 目前大致分成了5类：
                 1. BasicWrite，基本的往地址中写入值；
                 2. BasicRead，基本的从地址中读出值；
                 3. Reset，软件复位；
                 4. WriteAddress，往 EEPROM 中写入地址；
                 5. BytesRead，读多字节值（需要用户自己封装成 byte* 格式）
    Date: 2025/08/12
*/

#include "callback.h"

// 注册一个回调句柄，用于管理回调函数的上下文信息，因为 ctx 很少，所以分开写了
void RegisterCallback(CallbackHandle *handler, BasicCallback Func, void *aim, bool is_float) {
    handler->callback = Func;
    handler->aim = aim;
    handler->is_float = is_float;
}

// 基本的写入，往地址为 address 的 aim 中写入 param，op 为了控制数据包长度
void BasicWrite(byte *param, int address, int op, ErrorCode err, void *aim, bool is_float) {
    if (!CheckAddress(address)) { // 不是该地址的探测器，忽略这个数据包
        return;
    }
    int length = DecodeLength[op]; // param 长度
    if (!err) {
        if (is_float) { // 分 float 和 int 两种情况写入
            float *ptr = (float *)aim;
            *ptr = ParseB2F(param, DecodeLength[op]);
        } else {
            int *ptr = (int *)aim;
            *ptr = ParseB2I(param, DecodeLength[op]);
        }
    }
    byte *ReturnCode = malloc(EncodeLength[op] * sizeof(byte)); // 制作回码
    byte *Package = NULL;
    int PackageLength = 0;
    ParseI2B(err ? 0 : 1, ReturnCode, 1); // 没有 err 的话回码就是 1，写入成功
    EncodePackage(ReturnCode, &PackageLength, &Package, address,
                  op);                   // 把各类信息编成数据包
    SendPackage(Package, PackageLength); // 发送数据包
    free(ReturnCode);                    // 释放资源
    free(Package);
    ReturnCode = NULL;
    Package = NULL;
}

void BasicRead(byte *param, int address, int op, ErrorCode err, void *aim, bool is_float) {
    if (!CheckAddress(address)) { // 不是该地址的探测器，忽略这个数据包
        return;
    }
    if (err) { // 如果前面解析出现错误了，就不读了
        return;
    }
    int length = DecodeLength[op];
    byte *ReturnCode = malloc(EncodeLength[op] * sizeof(byte)); // 制作回码
    if (!err) {
        if (is_float) { // 分 float 和 int 两种情况读到回码中
            ParseF2B(*(float *)aim, ReturnCode, EncodeLength[op]);
        } else {
            ParseI2B(*(int *)aim, ReturnCode, EncodeLength[op]);
        }
    }
    byte *Package = NULL;
    int PackageLength = 0;
    EncodePackage(ReturnCode, &PackageLength, &Package, address,
                  op);                   // 把各类信息编成数据包
    SendPackage(Package, PackageLength); // 发送数据包
    free(ReturnCode);                    // 释放资源
    free(Package);
    ReturnCode = NULL;
    Package = NULL;
}

void Reset(byte *param, int address, int op, ErrorCode err, void *aim, bool is_float) {
    if (!CheckAddress(address)) { // 不是该地址的探测器，忽略这个数据包
        return;
    }
    if (err) { // 前面解析出现错误了，不能随意复位
        return;
    }
    // TODO 软件复位
}

void WriteAddress(byte *param, int address, int op, ErrorCode err, void *aim, bool is_float) {
    if (err) { // 前面解析出现错误了，不能随意写入地址
        return;
    }
    WriteLocalAddress(ParseB2I(param,
                               DecodeLength[op])); // 往探测器内部写入数据，注意这里没 address 限制
}

void BytesRead(byte *param, int address, int op, ErrorCode err, void *aim, bool is_float) {
    if (!CheckAddress(address)) { // 不是该地址的探测器，忽略这个数据包
        return;
    }
    if (err) { // 如果前面解析出现错误了，就不读了
        return;
    }
    int length = DecodeLength[op];
    byte *ReturnCode = malloc(EncodeLength[op] * sizeof(byte));
    if (!err) {
        memcpy(ReturnCode, (byte *)aim,
               EncodeLength[op] * sizeof(byte)); // 直接把原始数据写入到回码中，原始数据需要用户自己生成
    }
    byte *Package = NULL;
    int PackageLength = 0;
    EncodePackage(ReturnCode, &PackageLength, &Package, address,
                  op);                   // 把各类信息编成数据包
    SendPackage(Package, PackageLength); // 发送数据包
    free(ReturnCode);                    // 释放资源
    free(Package);
    ReturnCode = NULL;
    Package = NULL;
}