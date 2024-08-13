/*
 * @Author: xia_chao
 * @Date: 2021-06-15 22:43:14
 * @LastEditTime: 2021-06-16 22:07:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /iec61850/src/init.h
 */
#ifndef __IEC_INIT_H__
#define __IEC_INIT_H__

/**
 * @description: 应用模块初始化
 * @param {*}
 * @return {*}
 */
int Init();
int InitLog();
int ShowInfo(int argc, char *argv[]);
void ShowHelp();
void ShowVersion();
int InitMQTT();
int InitIec61850();

#endif // ! __IEC_INIT_H__