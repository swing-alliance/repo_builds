QT += widgets
CONFIG += c++17

SOURCES += \
    main.cpp \
    fund_card.cpp \
    $$PWD/../pop_widgets/buy_transaction_widget.cpp \
    $$PWD/../pop_widgets/sell_transaction_widget.cpp

HEADERS += \
    fund_card.h \
    $$PWD/../pop_widgets/buy_transaction_widget.h \
    $$PWD/../pop_widgets/sell_transaction_widget.h

FORMS += \
    fund_card.ui \
    $$PWD/../pop_widgets/buy_transaction_widget.ui \
    $$PWD/../pop_widgets/sell_transaction_widget.ui

RESOURCES += \
    qss.qrc

INCLUDEPATH += $$PWD/../pop_widgets

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