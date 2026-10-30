QT += widgets gui charts
CONFIG += c++17
TEMPLATE = app

# 源文件（必须补充 chartwidget.cpp，否则会导致 undefined reference 错误）
SOURCES += \
    chart_drawer.cpp \
    customchartview.cpp \
    main.cpp \
    chartwidget.cpp

HEADERS += \
    chart_drawer.h \
    chartwidget.h \
    customchartview.h \
    simpleplotwidget.h

FORMS += \
    chart_drawer.ui \
    chartwidget.ui \
    customchartview.ui

# 头文件搜索路径
INCLUDEPATH += .
INCLUDEPATH += $$PWD/../../core/include
INCLUDEPATH += $$PWD/../../model/include
INCLUDEPATH += $$PWD/../../io_medi/include
INCLUDEPATH += $$PWD/../../sqlite3/include

# 基础 CMake 构建目录定义
CMAKE_BUILD_DIR = $$PWD/../../build

# 使用链接器分组选项，解决静态库互相依赖时的顺序报错问题，并兼容可能不带 debug/release 子目录的 CMake 路径
LIBS += -Wl,--start-group \
    -L$$CMAKE_BUILD_DIR/model \
    -L$$CMAKE_BUILD_DIR/model/debug \
    -L$$CMAKE_BUILD_DIR/model/release \
    -lmodel \
    -L$$CMAKE_BUILD_DIR/core \
    -L$$CMAKE_BUILD_DIR/core/debug \
    -L$$CMAKE_BUILD_DIR/core/release \
    -lcore \
    -L$$CMAKE_BUILD_DIR/io_medi \
    -L$$CMAKE_BUILD_DIR/io_medi/debug \
    -L$$CMAKE_BUILD_DIR/io_medi/release \
    -lio_medi \
    -L$$CMAKE_BUILD_DIR/sqlite3 \
    -L$$CMAKE_BUILD_DIR/sqlite3/debug \
    -L$$CMAKE_BUILD_DIR/sqlite3/release \
    -lsqlite3 \
-Wl,--end-group

# Windows 系统库依赖
win32 {
    LIBS += -lws2_32 -lcrypt32
}