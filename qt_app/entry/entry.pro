QT += widgets
CONFIG += c++17

SOURCES += \
    main.cpp \
    entrywindow.cpp \
    $$PWD/../app_time/timewindow.cpp \
    $$PWD/../funds_pannel/fund_pannel_widget.cpp \
    $$PWD/../fund_card/fund_card.cpp \          # 加上这行

HEADERS += \
    entrywindow.h \
    $$PWD/../app_time/timewindow.h \
    $$PWD/../funds_pannel/fund_pannel_widget.h \
    $$PWD/../fund_card/fund_card.h \              # 加上这行

FORMS += \
    entrywindow.ui \
    $$PWD/../app_time/timewindow.ui \
    $$PWD/../funds_pannel/fund_pannel_widget.ui \
    $$PWD/../fund_card/fund_card.ui \              # 加上这行

# 头文件路径
INCLUDEPATH += $$PWD/../app_time
INCLUDEPATH += $$PWD/../funds_pannel
INCLUDEPATH += $$PWD/../fund_card               # 加上这行

# CMake 库保持不变
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

RESOURCES += \
    pack.qrc