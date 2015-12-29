LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dlua_shared

LOCAL_MODULE_FILENAME := libcocos2dlua

LOCAL_SHORT_COMMANDS := true

LOCAL_CPPFLAGS  := -MMD -MP -DENABLE_LOGGING

LOCAL_CFLAGS := -g -Wall -Wextra -ffast-math -Woverloaded-virtual -Wnon-virtual-dtor -Wfloat-equal -D_DEBUG -DDEBUG -D_LIB -DBEHAVIAC_COMPILER_ANDROID_VER=9
ifeq ($(NDK_DEBUG),1)
LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS -Wl -Map=test.map
endif

LOCAL_SRC_FILES := hellolua/main.cpp \
../../Classes/AppDelegate.cpp \
../../Classes/enemy.cpp \
../../Classes/GameOverScene.cpp \
../../Classes/GameScene.cpp \
../../Classes/hero.cpp \
../../Classes/NPC.cpp \
../../Classes/PlaneEnemy.cpp \
../../Classes/PlaneHero.cpp

#anysdk
LOCAL_SRC_FILES += \
../../Classes/anysdkbindings.cpp \
../../Classes/anysdk_manual_bindings.cpp

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/../../Classes/runtime \
$(LOCAL_PATH)/../../Classes \
$(LOCAL_PATH)/../../behaviac/inc \
$(COCOS2DX_ROOT)/external \
$(COCOS2DX_ROOT)/external/protobuf-lite/src \
$(COCOS2DX_ROOT)/quick/lib/quick-src \
$(COCOS2DX_ROOT)/quick/lib/quick-src/extra

#anysdk
LOCAL_C_INCLUDES +=	\
$(LOCAL_PATH)/../protocols/android \
$(LOCAL_PATH)/../protocols/include \

#anysdk
LOCAL_WHOLE_STATIC_LIBRARIES += PluginProtocolStatic

LOCAL_STATIC_LIBRARIES := cocos2d_lua_static
LOCAL_STATIC_LIBRARIES += lua_extensions_static
LOCAL_STATIC_LIBRARIES += extra_static
LOCAL_STATIC_LIBRARIES += cocos_protobuf-lite_static
LOCAL_STATIC_LIBRARIES += behaviac_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,scripting/lua-bindings/proj.android)

$(call import-module, quick-src/lua_extensions)
$(call import-module, quick-src/extra)
$(call import-module, protobuf-lite)

#anysdk
$(call import-module,protocols/android)

$(call import-module,../behaviac) 

