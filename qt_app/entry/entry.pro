QT += widgets gui charts
CONFIG += c++17
TEMPLATE = app

SOURCES += \
    main.cpp \
    entrywindow.cpp \
    $$PWD/../app_time/timewindow.cpp \
    $$PWD/../funds_pannel/fund_pannel_widget.cpp \
    $$PWD/../funds_pannel/v_pack_pannel.cpp \
    $$PWD/../fund_card/fund_card.cpp \
    $$PWD/../chart_widget/chartwidget.cpp \
    $$PWD/../chart_widget/customchartview.cpp \
    $$PWD/../chart_widget/chart_drawer.cpp \
    $$PWD/../pop_widgets/cal_param_widget.cpp \
    $$PWD/../pop_widgets/sell_transaction_widget.cpp \
    $$PWD/../pop_widgets/buy_transaction_widget.cpp \
    $$PWD/../pop_widgets/addgroup.cpp \
    $$PWD/../pop_widgets/delgroup.cpp \
    $$PWD/../pop_widgets/fundadd_togroup.cpp \
    $$PWD/../pop_widgets/load_fund_group.cpp \
    $$PWD/../pop_widgets/popdialog.cpp

HEADERS += \
    entrywindow.h \
    $$PWD/../app_time/timewindow.h \
    $$PWD/../funds_pannel/fund_pannel_widget.h \
    $$PWD/../funds_pannel/v_pack_pannel.h \
    $$PWD/../fund_card/fund_card.h \
    $$PWD/../chart_widget/chartwidget.h \
    $$PWD/../chart_widget/customchartview.h \
    $$PWD/../chart_widget/simpleplotwidget.h \
    $$PWD/../chart_widget/chart_drawer.h \
    $$PWD/../pop_widgets/cal_param_widget.h \
    $$PWD/../pop_widgets/sell_transaction_widget.h \
    $$PWD/../pop_widgets/buy_transaction_widget.h \
    $$PWD/../pop_widgets/addgroup.h \
    $$PWD/../pop_widgets/delgroup.h \
    $$PWD/../pop_widgets/fundadd_togroup.h \
    $$PWD/../pop_widgets/load_fund_group.h \
    $$PWD/../pop_widgets/popdialog.h

FORMS += \
    entrywindow.ui \
    $$PWD/../app_time/timewindow.ui \
    $$PWD/../funds_pannel/fund_pannel_widget.ui \
    $$PWD/../funds_pannel/v_pack_pannel.ui \
    $$PWD/../fund_card/fund_card.ui \
    $$PWD/../chart_widget/chartwidget.ui \
    $$PWD/../chart_widget/customchartview.ui \
    $$PWD/../chart_widget/chart_drawer.ui \
    $$PWD/../pop_widgets/cal_param_widget.ui \
    $$PWD/../pop_widgets/sell_transaction_widget.ui \
    $$PWD/../pop_widgets/buy_transaction_widget.ui \
    $$PWD/../pop_widgets/addgroup.ui \
    $$PWD/../pop_widgets/delgroup.ui \
    $$PWD/../pop_widgets/fundadd_togroup.ui \
    $$PWD/../pop_widgets/load_fund_group.ui \
    $$PWD/../pop_widgets/popdialog.ui

# 头文件路径
INCLUDEPATH += $$PWD/../app_time
INCLUDEPATH += $$PWD/../funds_pannel
INCLUDEPATH += $$PWD/../fund_card
INCLUDEPATH += $$PWD/../chart_widget
INCLUDEPATH += $$PWD/../pop_widgets

INCLUDEPATH += $$PWD/../../core/include
INCLUDEPATH += $$PWD/../../model/include
INCLUDEPATH += $$PWD/../../net/include
INCLUDEPATH += $$PWD/../../sqlite3/include
INCLUDEPATH += $$PWD/../../virtual/include
INCLUDEPATH += $$PWD/../../io_medi/include

# 核心修改：区分 Debug 和 Release 链接不同的 CMake 构建产物目录
CONFIG(debug, debug|release) {
    # 如果你的 CMake Debug 输出在 build/debug 或 build/Debug
    CMAKE_BUILD_DIR = $$PWD/../../build
    message("Building in DEBUG mode...")
} else {
    # 如果你的 CMake Release 输出在 build/release 或 build/Release
    # 请根据你实际的 CMake Release 输出路径调整（如果是单目录可改成对应的子目录）
    CMAKE_BUILD_DIR = $$PWD/../../build
    message("Building in RELEASE mode...")
}

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
    RC_ICONS = infinite.ico
}

RESOURCES += \
    png.qrc