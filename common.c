/*
    Author: _Burnside
    Description:
   该文件包含了各类杂项函数，比如byte*到int、float的相互转化，EEPROM的操作，编码解码数据包等。
    Date: 2025/08/12
*/

#include "common.h"

int ParseB2I(byte *input, int len) {
    int result = 0;
    for (int i = 0; i < len; i++) {
        result <<= 8;
        result |= input[i];
    }
    return result;
}

float ParseB2F(byte *input, int len) {
    return (float)ParseB2I(input, len); // Not Good
}

void ParseI2B(int input, byte *output, int len) {
    for (int i = 0; i < len; i++) {
        output[i] = (input >> (8 * (len - i - 1))) & 0xff;
    }
}

void ParseF2B(float input, byte *output, int len) {
    for (int i = 0; i < len; i++) {
        output[i] = ((int)input >> (8 * (len - i - 1))) & 0xff;
    }
}

int ReadLocalAddress() {
    // TODO 从 EEPROM 中读出地址
    return 0;
}

void WriteLocalAddress(int address) {
    // TODO 往 EEPROM 中写入地址
}

int CheckAddress(int address) {
    return ReadLocalAddress() == address;
}

void SendPackage(byte *Package, int PackageLength) {
    // TODO 往上位机发命令，目前写成了 printf 的形式，容易调试。
    printf("Send Package: \n");
    for (int i = 0; i < PackageLength; i++) {
        printf("%#x ", Package[i]);
    }
    printf("\n");
}

ErrorCode DecodePackage(byte *input, int len, byte **output, int *address, int *op) {
    if (input == NULL)
        return UnknownError;
    int sum = 0;
    for (int i = 0; i < len - 2; i++) {
        sum += (int)input[i]; // 先算 SUM
    }
    if (input[0] != 0x40)
        return StartFlagNotFound;
    if (input[len - 1] != 0x23)
        return EndFlagNotFound;
    if ((sum & 0xff) != ParseB2I(input + len - 2, 1))
        return SumError;
    int length = ParseB2I(input + 1, 1); // 解析长度
    *address = ParseB2I(input + 2, 1);   // 解析地址
    *op = ParseB2I(input + 3, 1);        // 解析操作码
    int OpLength = DecodeLength[*op];    // 该操作码对应的操作字节数
    if (OpLength != len - 6)
        return LengthError;
    *output = malloc(OpLength * sizeof(byte));
    memcpy(*output, input + 4, OpLength); // 制作 param 数组，等待回调函数处理
    return OK;
}

void EncodePackage(byte *input, int *len, byte **output, int address, int op) {
    if (input == NULL) {
        *len = 0;
        *output = NULL;
    }
    int length = 6 + EncodeLength[op];
    *len = length;
    *output = malloc(length * sizeof(byte));
    (*output)[0] = 0x40;                            // 制作起始码
    (*output)[length - 1] = 0x23;                   // 制作结束码
    ParseI2B(length, (*output) + 1, 1);             // 制作长度
    ParseI2B(address, (*output) + 2, 1);            // 制作地址
    ParseI2B(op, (*output) + 3, 1);                 // 制作操作符
    memcpy((*output) + 4, input, EncodeLength[op]); // 制作操作指令
    int sum = 0;
    for (int i = 0; i < length - 2; i++) {
        sum += (int)(*output)[i]; // 算 SUM
    }
    ParseI2B(sum & 0xff, (*output) + length - 2, 1); // 制作 SUM
}
