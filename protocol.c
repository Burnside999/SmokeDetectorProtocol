/*
    Author: _Burnside
    Description: 该文件包含了各类接口，比如通信协议初始化、处理数据的接口。
    Date: 2025/08/12
*/

#include "protocol.h"

// Interface
void InitProtocol() {
    // DecodeLength
    int MaxOpLength = 38;
    DecodeLength = malloc((MaxOpLength + 1) * sizeof(int));
    memset(DecodeLength, 0, (MaxOpLength + 1) * sizeof(int));
    DecodeLength[3] = 1;  // 写地址
    DecodeLength[19] = 3; // 写参数 1
    DecodeLength[20] = 3; // 写参数 2
    DecodeLength[21] = 3; // 写参数 3
    DecodeLength[22] = 2; // 写参数 4
    DecodeLength[23] = 2; // 写参数 5
    DecodeLength[24] = 2; // 写参数 6
    DecodeLength[25] = 2; // 写参数 7
    DecodeLength[26] = 2; // 写参数 8
    DecodeLength[27] = 2; // 写参数 9
    DecodeLength[32] = 2; // 读报警状态
    DecodeLength[38] = 2; // 写入特定参数（如P1）

    // EncodeLength
    EncodeLength = malloc((MaxOpLength + 1) * sizeof(int));
    memset(EncodeLength, 0, (MaxOpLength + 1) * sizeof(int));
    EncodeLength[1] = 7;  // 巡检，1字节状态 + 6字节模拟量
    EncodeLength[3] = 1;  // 写地址
    EncodeLength[4] = 1;  // 读地址
    EncodeLength[10] = 2; // 读消光信号强度
    EncodeLength[11] = 2; // 读甲烷吸收强度
    EncodeLength[12] = 6; // 读散射信号强度
    EncodeLength[13] = 2; // 读消光基线强度
    EncodeLength[14] = 6; // 读散射基线强度
    EncodeLength[15] = 2; // 读吸收峰位置
    EncodeLength[16] = 2; // 读散射消光比
    EncodeLength[17] = 2; // 读环境温度
    EncodeLength[18] = 2; // 读TEC温度
    EncodeLength[19] = 1; // 写参数1
    EncodeLength[20] = 1; // 写参数2
    EncodeLength[21] = 1; // 写参数3
    EncodeLength[22] = 1; // 写参数4
    EncodeLength[23] = 1; // 写参数5
    EncodeLength[24] = 1; // 写参数6
    EncodeLength[25] = 1; // 写参数7
    EncodeLength[26] = 1; // 写参数8
    EncodeLength[27] = 1; // 写参数9
    EncodeLength[28] = 2; // 读甲烷浓度
    EncodeLength[29] = 2; // 读烟雾浓度
    EncodeLength[30] = 2; // 读干扰源浓度
    EncodeLength[31] = 2; // 读干扰源状态
    EncodeLength[32] = 2; // 读报警状态
    EncodeLength[38] = 1; // 写入特定参数（如P1）
    EncodeLength[39] = 1; // 设置甲烷报警阈值
    EncodeLength[40] = 1; // 设置烟雾报警阈值
    EncodeLength[41] = 1; // 设置干扰源报警灵敏度
    
    // EncodeCallBack 建议在 main 中设置
    EncodeCallBack = malloc((MaxOpLength + 1) * sizeof(CallbackHandle));
    for (int i = 0; i < MaxOpLength + 1; i++) {
        EncodeCallBack[i].callback = NULL;
        EncodeCallBack[i].aim = NULL;
    }
}

ErrorCode parse(byte *package, int len) {
    byte *param = NULL;
    int address = 0;
    int op = 0;
    ErrorCode result = OK;
    result = DecodePackage(package, len, &param, &address,
                           &op); // 解码数据包，得到 param, address 和 op
    if (EncodeCallBack[op].callback == NULL || EncodeCallBack[op].aim == NULL)
        return UnknownError; // 为什么会没定义回调函数？
    EncodeCallBack[op].callback(param, address, op, result, EncodeCallBack[op].aim,
                                EncodeCallBack[op].is_float); // 通过回调句柄执行回调函数
    if (param != NULL) {                                      // 释放资源
        free(param);
        param = NULL;
    }
    return result;
}