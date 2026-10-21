QT += widgets
CONFIG += c++17

SOURCES += \
    main.cpp \
    fund_card.cpp

HEADERS += \
    fund_card.h

FORMS += \
    fund_card.ui

RESOURCES += \
    qss.qrc

# ==================== CMake 库 ====================
CMAKE_BUILD_DIR = $$PWD/../../build
INCLUDEPATH += $$PWD/../../core/include
INCLUDEPATH += $$PWD/../../model/include

LIBS += -Wl,--start-group \
    -L$$CMAKE_BUILD_DIR/model -lmodel \
    -L$$CMAKE_BUILD_DIR/core -lcore \
-Wl,--end-group

win32 {
    LIBS += -lws2_32 -lcrypt32
}