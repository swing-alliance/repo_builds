QT += widgets charts
CONFIG += c++17

SOURCES += \
    main.cpp \
    fund_pannel_widget.cpp \
    $$PWD/../fund_card/fund_card.cpp \
    $$PWD/../chart_widget/chartwidget.cpp \
    $$PWD/../pop_widgets/buy_transaction_widget.cpp \
    $$PWD/../pop_widgets/sell_transaction_widget.cpp \
    $$PWD/../pop_widgets/fundadd_togroup.cpp\
    v_pack_pannel.cpp

HEADERS += \
    fund_pannel_widget.h \
    $$PWD/../fund_card/fund_card.h \
    $$PWD/../chart_widget/chartwidget.h \
    $$PWD/../chart_widget/simpleplotwidget.h \
    $$PWD/../pop_widgets/buy_transaction_widget.h \
    $$PWD/../pop_widgets/sell_transaction_widget.h \
    $$PWD/../pop_widgets/fundadd_togroup.h\
    v_pack_pannel.h

FORMS += \
    fund_pannel_widget.ui \
    $$PWD/../fund_card/fund_card.ui \
    $$PWD/../chart_widget/chartwidget.ui \
    $$PWD/../pop_widgets/buy_transaction_widget.ui \
    $$PWD/../pop_widgets/sell_transaction_widget.ui \
    $$PWD/../pop_widgets/fundadd_togroup.ui\
    v_pack_pannel.ui

# 头文件路径（补上了 fund_card 的路径）
INCLUDEPATH += $$PWD/../fund_card
INCLUDEPATH += $$PWD/../chart_widget
INCLUDEPATH += $$PWD/../pop_widgets

# ==================== CMake 库 ====================
CMAKE_BUILD_DIR = $$PWD/../../build
INCLUDEPATH += \
    $$PWD/../../core/include \
    $$PWD/../../model/include \
    $$PWD/../../net/include \
    $$PWD/../../sqlite3/include \
    $$PWD/../../virtual/include \
    $$PWD/../../io_medi/include

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
    png.qrc