QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    cal_param_widget.cpp

HEADERS += \
    cal_param_widget.h

FORMS += \
    cal_param_widget.ui


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