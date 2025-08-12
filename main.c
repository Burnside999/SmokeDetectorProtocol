#include "protocol.h"

/*
    封装一个读指令
*/
void MakeRead(int op, int address, byte **output, int *len) {
    int length = 6;
    *len = length;
    *output = malloc(length * sizeof(byte));
    (*output)[0] = 0x40;
    (*output)[1] = (byte)length;
    (*output)[2] = 0x00;
    (*output)[3] = (byte)op;
    int sum = 0;
    for (int i = 0; i < length - 2; i++) {
        sum += (byte)(*output)[i];
    }
    (*output)[4] = sum & 0xff;
    (*output)[5] = 0x23;
}

/*
    封装一个写指令
*/
void MakeWrite(int op, int address, byte **output, int *len, int param) {
    int length = 6 + DecodeLength[op];
    *len = length;
    *output = malloc(length * sizeof(byte));
    (*output)[0] = 0x40;
    (*output)[1] = (byte)length;
    (*output)[2] = 0x00;
    (*output)[3] = (byte)op;
    ParseI2B(param, (*output) + 4, DecodeLength[op]);
    int sum = 0;
    for (int i = 0; i < length - 2; i++) {
        sum += (byte)(*output)[i];
    }
    (*output)[length - 2] = sum & 0xff;
    (*output)[length - 1] = 0x23;
}

int param4 = 0; // 参数 4
int temp = 18;  // 环境温度

int main() {
    InitProtocol();
    RegisterCallback(EncodeCallBack + 22, BasicWrite, &param4,
                     false); // 写参数 4 的回调
    RegisterCallback(EncodeCallBack + 17, BasicRead, &temp,
                     false); // 读环境温度的回调
    // 模拟测试

    // 测试 1 - 写参数 4
    printf("Original Param4: %d\n", param4);
    byte *Package = NULL;
    int len = 0;
    MakeWrite(22, 0, &Package, &len, 42); // 0 是测试用地址，将 param4 修改为 42
    parse(Package, len);
    printf("After Param4: %d\n", param4); // param4 == 42 即通过测试
    free(Package);
    Package = NULL;

    // 测试 2 - 读环境温度
    MakeRead(17, 0, &Package, &len); // 0 是测试用地址，读取环境温度
    parse(Package, len);             // 回码中输出正确即通过测试，即0x40 0x08 0x00 0x11 0x00
                                     // 0x12 0x6b 0x23
    free(Package);
    Package = NULL;

    return 0;
}