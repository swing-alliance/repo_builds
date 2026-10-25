QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    timewindow.cpp

HEADERS += \
    timewindow.h

FORMS += \
    timewindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
# ==================== 引入 CMake 编译的 MinGW 静态库与头文件 界 ====================

CMAKE_BUILD_DIR = $$PWD/../../build

INCLUDEPATH += $$PWD/../../core/include
INCLUDEPATH += $$PWD/../../model/include
# INCLUDEPATH += $$PWD/../../net/include
# INCLUDEPATH += $$PWD/../../sqlite3/include
INCLUDEPATH += $$PWD/../../virtual/include
# INCLUDEPATH += $$PWD/../../io_medi/include

# 使用 --start-group 彻底解决静态库循环依赖或顺序导致的链接报错
LIBS += -Wl,--start-group \
    -L$$CMAKE_BUILD_DIR/model -lmodel \
    # -L$$CMAKE_BUILD_DIR/io_medi -lio_medi \
    -L$$CMAKE_BUILD_DIR/core -lcore \
    # -L$$CMAKE_BUILD_DIR/net -lnet \
    -L$$CMAKE_BUILD_DIR/virtual -lvirtual \
    # -L$$CMAKE_BUILD_DIR/sqlite3 -lsqlite3 \
-Wl,--end-group

# Windows MinGW 下必须链接的网络与加密库
win32 {
    LIBS += -lws2_32 -lcrypt32
}
#==============================界===============================
message("=== 调试路径 ===")
message("CMAKE_BUILD_DIR 设置为: $$CMAKE_BUILD_DIR")
message("Model 库查找路径: $$CMAKE_BUILD_DIR/model")