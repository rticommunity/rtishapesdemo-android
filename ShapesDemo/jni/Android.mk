#
# Change the values below to refect your installed and build environment
#
RTI_CONNECT_MICRO_HOME := C:\Rti\rti_connext_micro.2.2.3
PLATFORM_ARCH := armv7leAndroidR8Dgcc4.7

LOCAL_PATH      := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := rti1
LOCAL_SRC_FILES := $(RTI_CONNECT_MICRO_HOME)/lib/$(PLATFORM_ARCH)/librti_me_rhsmzd.a
NDK_DEBUG = 1
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := rti2
LOCAL_SRC_FILES := $(RTI_CONNECT_MICRO_HOME)/lib/$(PLATFORM_ARCH)/librti_me_whsmzd.a
NDK_DEBUG = 1
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := rti3
LOCAL_SRC_FILES := $(RTI_CONNECT_MICRO_HOME)/lib/$(PLATFORM_ARCH)/librti_me_discdpdezd.a
NDK_DEBUG = 1
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := rti4
LOCAL_SRC_FILES := $(RTI_CONNECT_MICRO_HOME)/lib/$(PLATFORM_ARCH)/librti_mezd.a
NDK_DEBUG = 1
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(RTI_CONNECT_MICRO_HOME)\include\rti_me $(RTI_CONNECT_MICRO_HOME)\include
LOCAL_CFLAGS    :=  -g
LOCAL_MODULE    := rti_android_shapesdemo
LOCAL_SRC_FILES := RTIDDSWrapper.c
LOCAL_SRC_FILES += ShapeType.c ShapeTypePlugin.c ShapeTypeSupport.c 

LOCAL_WHOLE_STATIC_LIBRARIES := rti1 rti2 rti3 rti4
LOCAL_LDLIBS += -llog 
NDK_DEBUG = 1

include $(BUILD_SHARED_LIBRARY)
