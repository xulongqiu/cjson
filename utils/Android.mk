LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    ./cJSON/cJSON.c \
	AlarmUtilsJson.cpp
LOCAL_MODULE:= libcjson
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES :=    \
        $(LOCAL_PATH)/../include/ \

include $(BUILD_STATIC_LIBRARY)

