QT += widgets charts
CONFIG += c++17
SOURCES += \
    main.cpp \
    fund_pannel_widget.cpp \
    $$PWD/../fund_card/fund_card.cpp \
    $$PWD/../chart_widget/chartwidget.cpp

HEADERS += \
    fund_pannel_widget.h \
    $$PWD/../fund_card/fund_card.h \
    $$PWD/../chart_widget/chartwidget.h\
    $$PWD/../chart_widget/simpleplotwidget.h\

FORMS += \
    fund_pannel_widget.ui \
    $$PWD/../fund_card/fund_card.ui \
    $$PWD/../chart_widget/chartwidget.ui

# 头文件路径
INCLUDEPATH += $$PWD/../fund_card
INCLUDEPATH += $$PWD/../chart_widget

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
