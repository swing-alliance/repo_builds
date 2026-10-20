QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    entrywindow.cpp \
    $$PWD/../app_time/timewindow.cpp \
    test_login.cpp

HEADERS += \
    entrywindow.h \
    $$PWD/../app_time/timewindow.h \
    test_login.h

FORMS += \
    entrywindow.ui \
    $$PWD/../app_time/timewindow.ui \
    test_login.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# ==================== 其他 qt 组件路径 ====================
INCLUDEPATH += $$PWD/../app_time

# ==================== 引入 CMake 编译的 MinGW 静态库与头文件 ====================
CMAKE_BUILD_DIR = $$PWD/../../build

INCLUDEPATH += $$PWD/../../core/include
INCLUDEPATH += $$PWD/../../model/include
INCLUDEPATH += $$PWD/../../net/include
INCLUDEPATH += $$PWD/../../sqlite3/include
INCLUDEPATH += $$PWD/../../virtual/include
INCLUDEPATH += $$PWD/../../io_medi/include

# 使用 --start-group 彻底解决静态库循环依赖或顺序导致的链接报错
# 注意：这里只链接底层的 CMake 静态库，不要加 app_time！
LIBS += -Wl,--start-group \
    -L$$CMAKE_BUILD_DIR/model -lmodel \
    -L$$CMAKE_BUILD_DIR/io_medi -lio_medi \
    -L$$CMAKE_BUILD_DIR/core -lcore \
    -L$$CMAKE_BUILD_DIR/net -lnet \
    -L$$CMAKE_BUILD_DIR/virtual -lvirtual \
    -L$$CMAKE_BUILD_DIR/sqlite3 -lsqlite3 \
-Wl,--end-group

# Windows MinGW 下必须链接的网络与加密库
win32 {
    LIBS += -lws2_32 -lcrypt32
}

RESOURCES += \
    pack.qrc