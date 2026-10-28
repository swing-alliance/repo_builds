QT += widgets

CONFIG += c++17

SOURCES += \
    addgroup.cpp \
    buy_transaction_widget.cpp \
    delgroup.cpp \
    fundadd_togroup.cpp \
    main.cpp \
    cal_param_widget.cpp \
    sell_transaction_widget.cpp

HEADERS += \
    addgroup.h \
    buy_transaction_widget.h \
    cal_param_widget.h \
    delgroup.h \
    fundadd_togroup.h \
    sell_transaction_widget.h


FORMS += \
    addgroup.ui \
    buy_transaction_widget.ui \
    cal_param_widget.ui \
    delgroup.ui \
    fundadd_togroup.ui \
    sell_transaction_widget.ui\


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CMAKE_BUILD_DIR = $$PWD/../../build
INCLUDEPATH += $$PWD/../../core/include
INCLUDEPATH += $$PWD/../../model/include
INCLUDEPATH += $$PWD/../../io_medi/include
INCLUDEPATH += $$PWD/../../sqlite3/include
LIBS += -Wl,--start-group \
    -L$$CMAKE_BUILD_DIR/model -lmodel \
    -L$$CMAKE_BUILD_DIR/core -lcore \
    -L$$CMAKE_BUILD_DIR/io_medi -lio_medi\
    -L$$CMAKE_BUILD_DIR/sqlite3 -lsqlite3\
-Wl,--end-group