#ifndef __ALARMPARAS_H__
#define __ALARMPARAS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/types.h>

namespace android
{
#define ALARM_AUDIO_SILENCE NULL
#define AlARM_AUDIO_TEST "/data/data/test.wav"
#define ALARM_AUDIO_DEFULAT "/etc/alarmaudios/Libratone_Master_Alarm_4_Bird-A_32Khz-16bit.wav"
#define ALARM_NAME_LEN 63
#define ALARM_INTENT_NAME_LEN 63
#define ALARM_INTENT_CONTENT_LEN 127
#define ALARM_TIMEORI_LEN 63
#define ALARM_REPEAT_TIMESTAMP_CNT 25
#define ALARM_INTENT_CNT_MAX 25
#define ALARM_TTS_LEN_MAX  128


/*alarm repeat interval*/
#define ALARM_INTERVAL_DEFAULT 600 //seconds
#define ALARM_INTERVAL_MIN 60 //seconds
#define ALARM_INTERVAL_MAX 3600 //seconds

/*alarm repeat cnt*/
#define ALARM_REPEAT_CNT_DEFAULT 5 //seconds
#define ALARM_REPEAT_CNT_MIN 1 //seconds
#define ALARM_REPEAT_CNT_MAX 20 //seconds

//#define ALARM_ID(pid, firstTime, repeatInfo) (((pid & 0xffff)<<40) | ((firstTime&0xffffffff)<<8) | (repeatInfo & 0xff))
#define ALARM_ID(firstTime, repeatInfo) (((firstTime/1000)<<16) | (repeatInfo & 0xffff))

enum alarm_operation {
    ALARM_ADD = 0,
    ALARM_REMOVE,
    ALARM_MODIFY,
    ALARM_CHECK,
    ALARM_CANCEL,
    ALARM_TURNON,
    ALARM_TURNOFF,

    ALARM_OPER_MAX,
};

typedef enum alarm_resp {
    ALARM_RESP_HAPPEN = 0,
    ALARM_LIST_CHANGED,
} alarm_resp_t;

enum alarm_repeat_bitmap {
    ALARM_REPEATE_MONDAY    =   1 << 0,   // 每周一都闹钟
    ALARM_REPEATE_TUESDAY   =   1 << 1,
    ALARM_REPEATE_WEDNESDAY =   1 << 2,
    ALARM_REPEATE_THURSDAY  =   1 << 3,
    ALARM_REPEATE_FRIDAY    =   1 << 4,
    ALARM_REPEATE_SATURDAY  =   1 << 5,
    ALARM_REPEATE_SUNDAY    =   1 << 6,
    ALARM_REPEATE_MONTH     =   1 << 7,   // 从第一次闹钟时间开始，按照阳历按月重复
    ALARM_REPEATE_YEAR      =   1 << 8,    // 从第一次闹钟时间开始，按照阳历按年重复
    ALARM_REPEATE_HOUR     = 1 << 9,
    ALARM_REPEATE_MIN    = 1 << 10,
};

//alarm action.
typedef enum alarm_intent_code {
    ALARM_INTENT_NONE,
    ALARM_INTENT_PLAY,  //play alarm, radio, music list
    ALARM_INTENT_CTRL,  //control light/tv.
} alarm_intent_code_t;

enum alarm_play_code {
    ALARM_PLAY_NONE,
    ALARM_PLAY_LOCAL_FILE,
    ALARM_PLAY_NETRADIO,
    ALARM_PLAY_DB_FM,
    ALARM_PLAY_BAIDU_MUSIC,
};

enum alarm_ctrl_code {
    ALARM_CTRL_LIGHT,
    ALARM_CTRL_TV
};

typedef struct AlarmIntent {
    char      name[ALARM_INTENT_NAME_LEN + 1];
    alarm_intent_code_t  code;
    uint16_t subCode;
    char  content[ALARM_INTENT_CONTENT_LEN];
} AlarmIntentT;

typedef struct AlarmTime {
    uint64_t firstTime; //milliseconds
    uint64_t interval;
    uint64_t repeatCnt;
    uint16_t repeatInfo;
    uint64_t repeatTimes[ALARM_REPEAT_TIMESTAMP_CNT];
} AlarmTimeT;

typedef struct AlarmPara {
    uint64_t alarmID;
    int alarmType;
    int alarmOperation;
    bool alarmOff;
    char alarmName[ALARM_NAME_LEN + 1];
    char alarmTimeOri[ALARM_TIMEORI_LEN + 1];
    char alarmTTS[ALARM_TTS_LEN_MAX];
    AlarmTimeT alarmTime;
    AlarmIntentT alarmIntent[ALARM_INTENT_CNT_MAX];
} AlarmParaT;


enum alarm_ret_code {
    ret_alarm_ok = 0,
    ret_alarm_service_died,
    ret_alarm_paras_null,
    ret_alarm_add_existed,
    ret_alarm_time_para_invalid,
    ret_alarm_modify_not_found,
    ret_alarm_remove_not_found,
    ret_alarm_turnon_not_found,
    ret_alarm_turnoff_not_found,
    ret_alarm_set_rtc_error,
    ret_alarm_check_no_alarm,
    ret_alarm_max
};
};

#endif //__ALARMPARAS_H__

