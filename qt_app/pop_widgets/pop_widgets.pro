QT += widgets

CONFIG += c++17

SOURCES += \
    buy_transaction_widget.cpp \
    main.cpp \
    cal_param_widget.cpp \
    sell_transaction_widget.cpp

HEADERS += \
    buy_transaction_widget.h \
    cal_param_widget.h \
    sell_transaction_widget.h


FORMS += \
    buy_transaction_widget.ui \
    cal_param_widget.ui \
    sell_transaction_widget.ui\


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CMAKE_BUILD_DIR = $$PWD/../../build
INCLUDEPATH += $$PWD/../../core/include
INCLUDEPATH += $$PWD/../../model/include

LIBS += -Wl,--start-group \
    -L$$CMAKE_BUILD_DIR/model -lmodel \
    -L$$CMAKE_BUILD_DIR/core -lcore \
-Wl,--end-group