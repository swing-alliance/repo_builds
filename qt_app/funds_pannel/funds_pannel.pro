QT += widgets
CONFIG += c++17

SOURCES += \
    main.cpp \
    fund_pannel_widget.cpp

HEADERS += \
    fund_pannel_widget.h

FORMS += \
    fund_pannel_widget.ui

# 头文件路径
INCLUDEPATH += A:/fund_qt/qt_app/fund_card

# 链接库（注意加了 /debug）
LIBS += -L$$OUT_PWD/../fund_card/debug -lfund_card

# CMake 库
CMAKE_BUILD_DIR = $$PWD/../../build
INCLUDEPATH += $$PWD/../../core/include
INCLUDEPATH += $$PWD/../../model/include
INCLUDEPATH += $$PWD/../../net/include
INCLUDEPATH += $$PWD/../../sqlite3/include
INCLUDEPATH += $$PWD/../../virtual/include
INCLUDEPATH += $$PWD/../../io_medi/include

LIBS += -Wl,--start-group \
    -L$$CMAKE_BUILD_DIR/model -lmodel \
    -L$$CMAKE_BUILD_DIR/io_medi -lio_medi \
    -L$$CMAKE_BUILD_DIR/core -lcore \
    -L$$CMAKE_BUILD_DIR/net -lnet \
    -L$$CMAKE_BUILD_DIR/virtual -lvirtual \
    -L$$CMAKE_BUILD_DIR/sqlite3 -lsqlite3 \
-Wl,--end-group

win32 {
    LIBS += -lws2_32 -lcrypt32
}