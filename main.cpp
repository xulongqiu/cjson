/*************************************************************************
> File: main.c
> Author: eric.xu
> Mail:   eric.xu@libratone.com.cn
> Time:   2017年04月15日 星期六 14时44分57秒
*************************************************************************/

#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<string>
#include<string.h>
#include<iostream>
#include "include/AlarmParas.h"
#include "AlarmUtilsJson.h"

using namespace std;
using namespace android;

int recoverAlarms(const char* path)
{
    FILE* file;
    int size = 0;
    AlarmParaT alarm;
    char buffer[1025] = {0};
    string jsonStr("");


    file = fopen(path, "rb");

    if (!file) {
        printf("[recoverAlarms]:fopen %s fail", path);
        return -1;
    }

    do {
        memset(buffer, 0, 1025);
        size = fread(buffer, 1, 1024, file);
        jsonStr.append(buffer);

    } while (size > 0);

    //cout<< "JsonString:" << jsonStr <<endl;
    AlarmUtilsJson* alarmjson = new AlarmUtilsJson(jsonStr.data());
    int ret = alarmjson->jsonToAlarmsTencent(&alarm);
    printf("recoverAlarms:%d\n", ret);
    delete alarmjson;

    fclose(file);

    return 0;

}

int main(int argc, const char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s filepath\n", argv[0]);
        return 0;
    }

    return recoverAlarms(argv[1]);
}
