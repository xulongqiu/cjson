#define LOG_NDEBUG 0
#define LOG_TAG "AlarmUtilsJson"

#include "AlarmUtilsJson.h"
#include <string.h>
#include <string>

using namespace std;

namespace android
{
#define ALOGV printf
#define ALOGE printf

AlarmUtilsJson::AlarmUtilsJson(const char* jsonString)
{
    if (jsonString != NULL) {
        mJsonP = cJSON_Parse(jsonString);
    } else {
        ALOGV("AlarmUtilsJson, jsonString=NULL");
    }

    ALOGV("AlarmUtilsJson created\n");
}

AlarmUtilsJson::AlarmUtilsJson()
{
    mJsonP = cJSON_CreateObject();

    ALOGV("AlarmUtilsJson created\n");
}

AlarmUtilsJson::~AlarmUtilsJson(void)
{
    if (mJsonP != NULL) {
        cJSON_Delete(mJsonP);
        mJsonP = NULL;
    }

    ALOGV("AlarmUtilsJson destroyed\n");
}

void AlarmUtilsJson::onFirstRef()
{
    ALOGV("AlarmUtilsJson::onFirstRef(%p)\n", this);
}

void AlarmUtilsJson::jsonRelease(void)
{
    if (mJsonP != NULL) {
        cJSON_Delete(mJsonP);
        mJsonP = NULL;
    }
}

#ifdef SUPPORT_TENCENT_INTERFACE

typedef struct slotNote {
    string ori_txt;
    string txt;
} slotNoteT;

typedef struct slotBasicDt {
    string date;
    int year;
    char mon;
    char day;
    char hour;
    char min;
    char sec;
    char week;
    string ori_txt;
    char peroidOfDay;

} slotBasicDtT;

typedef struct slotInterval {
    slotBasicDtT end;
    slotBasicDtT start;
} slotIntervalT;

typedef enum slotRepeatType {
    repeatNone,
    repeatYear,
    repeatWeek,
    repeatDay,
    repeatHour,
    repeatMin,
    repeatWorkday,
    repeatWeekend

} slotRepeatTypeT;

typedef struct slotRepeat {
    slotIntervalT interval;
    slotRepeatTypeT type;
} slotRepeatT;

typedef enum slotDtType {
    typeNone,
    typeDatatime,
    typeDuration,
    typeRepeatDt

} slotDtTypeT;

typedef struct slotDataTime {
    slotBasicDtT datetime;
    slotIntervalT interval;
    slotRepeatT repeat;
    string ori_txt;
    char type;

} slotDataTimeT;

typedef struct slot {
    slotNoteT note;
    slotDataTime date;
    slotDataTime time;
} slotT;

struct action2Oper {
    uint8_t oper;
    const char* str;
};

struct action2Oper oper_table[ALARM_OPER_MAX] = {
    {.oper = ALARM_ADD, .str = "new"},
    {.oper = ALARM_REMOVE, .str = "delete"},
    {.oper = ALARM_MODIFY, .str = "modify"},
    {.oper = ALARM_CHECK, .str = "check"},
    {.oper = ALARM_CANCEL, .str = "cancel"},
    {.oper = ALARM_TURNON, .str = "on"},
    {.oper = ALARM_TURNOFF, .str = "off"},
};

static int jsonParseBasicDt(cJSON* subJsonP, slotBasicDtT* datetime)
{
    cJSON* obj;

    if (subJsonP == NULL || datetime == NULL) {
        return -1;
    }

    obj = cJSON_GetObjectItem(subJsonP, "date");

    if (obj && strlen(obj->valuestring)) {
        datetime->date.append(obj->valuestring);
    }

    obj = cJSON_GetObjectItem(subJsonP, "year");
    datetime->year = obj ? obj->valueint : 0;
    obj = cJSON_GetObjectItem(subJsonP, "week");
    datetime->week = obj ? obj->valueint : 0;
    obj = cJSON_GetObjectItem(subJsonP, "mon");
    datetime->mon = obj ? obj->valueint : 0;
    obj = cJSON_GetObjectItem(subJsonP, "day");
    datetime->day = obj ? obj->valueint : 0;
    obj = cJSON_GetObjectItem(subJsonP, "hour");
    datetime->hour = obj ? obj->valueint : 0;
    obj = cJSON_GetObjectItem(subJsonP, "min");
    datetime->min = obj ? obj->valueint : 0;
    obj = cJSON_GetObjectItem(subJsonP, "sec");
    datetime->sec = obj ? obj->valueint : 0;
    obj = cJSON_GetObjectItem(subJsonP, "original_text");

    if (obj && strlen(obj->valuestring)) {
        datetime->ori_txt.append(obj->valuestring);
    }

    obj = cJSON_GetObjectItem(subJsonP, "period_of_day");
    datetime->peroidOfDay = obj ? obj->valueint : 0;

    return 0;
}

static int jsonParseSlot(const char* str, slotT* slot, const char* slot_name)
{
    cJSON* jsonP, *subJsonP, *obj, *subInterval;
    int ret = 0;
    slotDataTimeT* dtP = NULL;

    if (str == NULL || slot == NULL || slot_name == NULL) {
        return -1;
    }

    if ((jsonP = cJSON_Parse(str)) == NULL) {
        return -2;
    }

    if (!strcmp(slot_name, "alarm")) {
        obj = cJSON_GetObjectItem(jsonP, "original_text");

        if (obj && strlen(obj->valuestring)) {
            slot->note.ori_txt.append(obj->valuestring);
        }

        obj = cJSON_GetObjectItem(jsonP, "text");

        if (obj && strlen(obj->valuestring)) {
            slot->note.txt.append(obj->valuestring);
        }

        cJSON_Delete(jsonP);
        return 0;
    }

    if (!strcmp(slot_name, "date")) {
        dtP = &slot->date;
    } else if (!strcmp(slot_name, "time")) {
        dtP = &slot->time;
    }

    if (dtP == NULL) {
        ret = -3;
        goto err;
    }

    //datetime
    subJsonP = cJSON_GetObjectItem(jsonP, "datetime");
    ret = jsonParseBasicDt(subJsonP, &dtP->datetime);

    if (ret) {
        goto err;
    }

    //interval
    subInterval = cJSON_GetObjectItem(jsonP, "interval");

    if (subJsonP == NULL) {
        ret = -4;
        goto err;
    }

    obj = cJSON_GetObjectItem(subInterval, "end");
    ret = jsonParseBasicDt(obj, &dtP->interval.end);

    if (ret) {
        goto err;
    }

    obj = cJSON_GetObjectItem(subInterval, "start");
    ret = jsonParseBasicDt(obj, &dtP->interval.start);

    if (ret) {
        goto err;
    }

    //original_text
    obj = cJSON_GetObjectItem(jsonP, "original_text");
    dtP->ori_txt.append(obj ? obj->valuestring : "");

    //repeat
    subJsonP = cJSON_GetObjectItem(jsonP, "repeat");

    if (subJsonP == NULL) {
        ret = -4;
        goto err;
    }

    subInterval = cJSON_GetObjectItem(subJsonP, "interval");

    if (subJsonP == NULL) {
        ret = -4;
        goto err;
    }

    obj = cJSON_GetObjectItem(subInterval, "end");
    ret = jsonParseBasicDt(obj, &dtP->repeat.interval.end);

    if (ret) {
        goto err;
    }

    obj = cJSON_GetObjectItem(subInterval, "start");
    ret = jsonParseBasicDt(obj, &dtP->repeat.interval.start);

    if (ret) {
        goto err;
    }

    obj = cJSON_GetObjectItem(subJsonP, "repeat_datetime_type");
    dtP->repeat.type = (slotRepeatTypeT)(obj ? obj->valueint : 0);

    //type
    obj = cJSON_GetObjectItem(jsonP, "type");
    dtP->type = obj ? obj->valueint : 0;


err:
    cJSON_Delete(jsonP);
    return ret;
}

static int jsonParseSemantic(const char* str, slotT* slot)
{
    cJSON* jsonP, *subJsonP, *obj, *subSlots, *slotItem;
    const char* slotName = NULL;
    int slotCnt = 0, i = 0;
    int ret = 0;

    if (str == NULL || slot == NULL) {
        return -1;
    }

    if ((jsonP = cJSON_Parse(str)) == NULL) {
        return -2;
    }

    subJsonP = cJSON_GetObjectItem(jsonP, "semantic");

    if (subJsonP == NULL) {
        ret = -3;
        goto err;
    }

    subSlots = cJSON_GetObjectItem(subJsonP, "slots");

    if (subSlots == NULL) {
        ret = -3;
        goto err;
    }

    slotCnt = cJSON_GetArraySize(subSlots);

    for (i = 0; i < slotCnt; i++) {
        slotItem = cJSON_GetArrayItem(subSlots, i);

        if (slotItem == NULL) {
            ret = -3;
            goto err;
        }

        obj = cJSON_GetObjectItem(slotItem, "name");
        slotName = obj ? obj->valuestring : NULL;

        obj = cJSON_GetObjectItem(slotItem, "slot_string");

        if (obj == NULL) {
            ret = -4;
            goto err;
        }

        printf("[%s]:%s\n\n", slotName, obj ? obj->valuestring : "none");
        ret = jsonParseSlot(obj->valuestring, slot, slotName);
    }

err:
    cJSON_Delete(jsonP);
    return ret;
}

int AlarmUtilsJson::jsonToAlarmsTencent(AlarmParaT* alarm)
{
    slotT slot;
    cJSON* subJsonP, *obj;
    int i = 0, ret = 0;

    if (mJsonP == NULL) {
        ALOGE("mJsonP is NULL \n");
        return -1;
    }

    memset(&slot, 0, sizeof(slotT));

    subJsonP = cJSON_GetObjectItem(mJsonP, "res");

    if (subJsonP == NULL) {
        return -2;
    }

    subJsonP = cJSON_GetObjectItem(subJsonP, "scene");

    if (subJsonP == NULL) {
        return -2;
    }

    obj = cJSON_GetObjectItem(subJsonP, "action");

    if (obj == NULL) {
        return -2;
    }

    for (i = 0; i < ALARM_OPER_MAX; i++) {
        if (!strcmp(obj->valuestring, oper_table[i].str)) {
            alarm->alarmOperation = oper_table[i].oper;
            break;
        }
    }

    if (i >= ALARM_OPER_MAX) {
        return 0;
    }

    obj = cJSON_GetObjectItem(subJsonP, "scene_data");

    //printf("scene_data:%s\n", obj? obj->valuestring : "none");
    if (obj != NULL) {
        ret = jsonParseSemantic(obj->valuestring, &slot);

        if (ret != 0) {
            return ret;
        }
    } else {
        return -2;
    }

    obj = cJSON_GetObjectItem(subJsonP, "scene_name");
    printf("scene_name:%s\n", obj ? obj->valuestring : "none");


    obj = cJSON_GetObjectItem(subJsonP, "speak");
    printf("speak:%s\n", obj ? obj->valuestring : "none");

    return 1;

}

#endif

int AlarmUtilsJson::jsonToAlarmsLibratone(AlarmParaT* vecAlarm)
{
    cJSON* subJsonP, *subAlarm, *subAlarmTimeInfo, *subAlarmRepeatTimes, *subAlarmIntent, *intentObj, *obj;
    AlarmParaT alarm;
    int alarmCnt = 0, intentCnt = 0, repeatTimesCnt = 0;
    int i = 0, j = 0;
    char* jsonString;

    if (mJsonP == NULL) {
        ALOGE("mJsonP is NULL \n");
        return -1;
    }

    subJsonP = cJSON_GetObjectItem(mJsonP, "vecCalendarInfo");

    if (subJsonP == NULL) {
        return 0;
    }

    alarmCnt = cJSON_GetArraySize(subJsonP);
    jsonString = cJSON_Print(subJsonP);
    ALOGV("vecCalendarInfo=%s\n", jsonString);
    free(jsonString);

    for (i = 0; i < alarmCnt; i++) {
        subAlarm = cJSON_GetArrayItem(subJsonP, i);
        //jsonString = cJSON_Print(subAlarm);
        //ALOGV("alarmPara[%d]=%s\n", i, jsonString);
        //free(jsonString);
        memset(&alarm, 0, sizeof(alarm));
        obj = cJSON_GetObjectItem(subAlarm, "lAlarmID");
        alarm.alarmID = obj ? obj->valuedouble : 0;
        obj = cJSON_GetObjectItem(subAlarm, "iAlarmType");
        alarm.alarmType = obj ? obj->valueint : 0;
        obj = cJSON_GetObjectItem(subAlarm, "eAlarmOperation");
        alarm.alarmOperation = obj ? obj->valueint : 0;
        obj = cJSON_GetObjectItem(subAlarm, "bAlarmOff");
        alarm.alarmOff = obj ? obj->valueint : 0;

        obj = cJSON_GetObjectItem(subAlarm, "sAlarmName");

        if (obj != NULL) {
            strncpy(alarm.alarmName, obj->valuestring, ALARM_NAME_LEN);
        }

        obj = cJSON_GetObjectItem(subAlarm, "sAlarmTimeOri");

        if (obj != NULL) {
            strncpy(alarm.alarmTimeOri, obj->valuestring, ALARM_TIMEORI_LEN);
        }

        obj = cJSON_GetObjectItem(subAlarm, "sAlarmTTS");

        if (obj != NULL) {
            strncpy(alarm.alarmTTS, obj->valuestring, ALARM_TTS_LEN_MAX);
        }

        subAlarmIntent = cJSON_GetObjectItem(subAlarm, "vecAlarmIntent");

        if (subAlarmIntent != NULL) {
            intentCnt = cJSON_GetArraySize(subAlarmIntent);

            if (intentCnt > ALARM_INTENT_CNT_MAX) {
                intentCnt = ALARM_INTENT_CNT_MAX;
            }

            for (j = 0; j < intentCnt; j++) {
                intentObj = cJSON_GetArrayItem(subAlarmIntent, j);
                obj = cJSON_GetObjectItem(intentObj, "sIntentName");

                if (obj != NULL) {
                    strncpy(alarm.alarmIntent[j].name, obj->valuestring, ALARM_INTENT_NAME_LEN);
                }

                obj = cJSON_GetObjectItem(intentObj, "eIntentCode");
                alarm.alarmIntent[j].code = (alarm_intent_code_t)(obj ? obj->valueint : 0);
                obj = cJSON_GetObjectItem(intentObj, "eIntentSubCode");
                alarm.alarmIntent[j].subCode = obj ? obj->valueint : 0;

                obj = cJSON_GetObjectItem(intentObj, "sIntentContent");

                if (obj != NULL) {
                    strncpy(alarm.alarmIntent[j].content, obj->valuestring, ALARM_INTENT_CONTENT_LEN);
                }

                if (alarm.alarmIntent[j].code <= ALARM_INTENT_PLAY
                    && alarm.alarmIntent[j].subCode <= ALARM_PLAY_LOCAL_FILE
                    && strlen(alarm.alarmIntent[j].content) == 0) {
                    strncpy(alarm.alarmIntent[j].content, (char*)ALARM_AUDIO_DEFULAT, ALARM_INTENT_CONTENT_LEN);
                }
            }
        }

        subAlarmTimeInfo = cJSON_GetObjectItem(subAlarm, "stTimeInfo");

        if (subAlarmTimeInfo != NULL) {
            obj = cJSON_GetObjectItem(subAlarmTimeInfo, "lFirstTime");
            alarm.alarmTime.firstTime = obj ? obj->valuedouble * 1000LL : 0;
            obj = cJSON_GetObjectItem(subAlarmTimeInfo, "lInterval");
            alarm.alarmTime.interval = obj ? obj->valueint : 0;

            if (alarm.alarmTime.interval < ALARM_INTERVAL_MIN || alarm.alarmTime.interval > ALARM_INTERVAL_MAX) {
                alarm.alarmTime.interval = ALARM_INTERVAL_DEFAULT;
            }

            alarm.alarmTime.interval *= 1000L;
            obj = cJSON_GetObjectItem(subAlarmTimeInfo, "lRepeatCnt");
            alarm.alarmTime.repeatCnt = obj ? obj->valueint : 0;

            if (alarm.alarmTime.repeatCnt < ALARM_REPEAT_CNT_MIN  || alarm.alarmTime.repeatCnt > ALARM_REPEAT_CNT_MAX) {
                alarm.alarmTime.repeatCnt = ALARM_REPEAT_CNT_DEFAULT;
            }

            obj = cJSON_GetObjectItem(subAlarmTimeInfo, "nRepeatInfo");
            alarm.alarmTime.repeatInfo = obj ? obj->valueint : 0;

            subAlarmRepeatTimes = cJSON_GetObjectItem(subAlarmTimeInfo, "vecRepeatTimes");

            if (subAlarmRepeatTimes != NULL) {
                repeatTimesCnt = cJSON_GetArraySize(subAlarmRepeatTimes);

                if (repeatTimesCnt > ALARM_REPEAT_TIMESTAMP_CNT) {
                    repeatTimesCnt = ALARM_REPEAT_TIMESTAMP_CNT;
                }

                for (j = 0; j < repeatTimesCnt; j++) {
                    alarm.alarmTime.repeatTimes[j] = cJSON_GetArrayItem(subAlarmRepeatTimes, j)->valueint;
                }
            }
        }

        if (alarm.alarmID == 0) {
            alarm.alarmID = ALARM_ID(alarm.alarmTime.firstTime, alarm.alarmTime.repeatInfo);
        }
    }

    return alarmCnt;
}

#ifdef SUPPORT_BAIDU_INTERFACE
int AlarmUtilsJson::jsonToAlarmsBaidu(AlarmParaT* vecAlarm)
{
    return 0;
}
#endif

//"{ \"iRet\": 0, \"sError\": \"\", \"vecCalendarInfo\": [ { \"eAction\": 2, \"sContent\": \"Æð´²\", \"stTimeInfo\": { \"lFirstTime\": 1484697600, \"nRepeatInfo\": 31, \"sTimeOri\": \"°Ëµã\", \"vecRepeatTimes\": [  ]}} ]}"
char* AlarmUtilsJson::alarmsToJsonString(AlarmParaT* vecAlarms)
{
    cJSON* vecCalendarInfo, *alarmItem, *alarmInfo, *vecRepeatTimes, *vecIntent, *intent;
    //char* out;
    int i = 0;
    AlarmParaT* iter = NULL;

    if (mJsonP == NULL) {
        mJsonP = cJSON_CreateObject();
    }

    cJSON_AddNumberToObject(mJsonP, "iRet", 0);
    cJSON_AddStringToObject(mJsonP, "sError", "");

    vecCalendarInfo = cJSON_CreateArray();
    cJSON_AddItemToObject(mJsonP, "vecCalendarInfo", vecCalendarInfo);

    for (iter = vecAlarms; iter != NULL; ++iter) {
        alarmItem = cJSON_CreateObject();
        cJSON_AddNumberToObject(alarmItem, "lAlarmID", iter->alarmID);
        cJSON_AddNumberToObject(alarmItem, "iAlarmType", iter->alarmType);
        cJSON_AddNumberToObject(alarmItem, "eAlarmOperation", iter->alarmOperation);
        cJSON_AddBoolToObject(alarmItem, "bAlarmOff", iter->alarmOff);
        cJSON_AddStringToObject(alarmItem, "sAlarmName", iter->alarmName);
        cJSON_AddStringToObject(alarmItem, "sAlarmTimeOri", iter->alarmTimeOri);
        cJSON_AddStringToObject(alarmItem, "sAlarmTTS", iter->alarmTTS);
        vecIntent = cJSON_CreateArray();
        cJSON_AddItemToObject(alarmItem, "vecAlarmIntent", vecIntent);

        for (i = 0; i < ALARM_INTENT_CNT_MAX; i++) {
            if (iter->alarmIntent[i].name[0] == '\0') {
                break;
            }

            intent = cJSON_CreateObject();
            cJSON_AddStringToObject(intent, "sIntentName", iter->alarmIntent[i].name);
            cJSON_AddNumberToObject(intent, "eIntentCode", iter->alarmIntent[i].code);
            cJSON_AddNumberToObject(intent, "eIntentSubCode", iter->alarmIntent[i].subCode);
            cJSON_AddStringToObject(intent, "sIntentContent", iter->alarmIntent[i].content);
            cJSON_AddItemToArray(vecIntent, intent);
        }

        alarmInfo = cJSON_CreateObject();
        cJSON_AddItemToObject(alarmItem, "stTimeInfo", alarmInfo);
        cJSON_AddNumberToObject(alarmInfo, "lFirstTime", iter->alarmTime.firstTime);
        cJSON_AddNumberToObject(alarmInfo, "lInterval", iter->alarmTime.interval);
        cJSON_AddNumberToObject(alarmInfo, "lRepeatCnt", iter->alarmTime.repeatCnt);
        cJSON_AddNumberToObject(alarmInfo, "nRepeatInfo", iter->alarmTime.repeatInfo);

        for (i = 0; i < ALARM_REPEAT_TIMESTAMP_CNT; i++) {
            if (iter->alarmTime.repeatTimes[i] == 0) {
                break;
            }
        }

        vecRepeatTimes = cJSON_CreateIntArray((int*)iter->alarmTime.repeatTimes, i);
        cJSON_AddItemToObject(alarmInfo, "vecRepeatTimes", vecRepeatTimes);
        cJSON_AddItemToArray(vecCalendarInfo, alarmItem);
    }

    return cJSON_Print(mJsonP);
}

}
