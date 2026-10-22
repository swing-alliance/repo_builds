QT += widgets gui charts
CONFIG += c++17

# 1. 将模板改为 lib，并指定为静态库
TEMPLATE = lib
CONFIG += staticlib

# 2. 指定生成的库文件名（可选，这里命名为 chart_widget）
TARGET = chart_widget

# 3. 确保包含自己的源码和 UI
SOURCES += \
    chartwidget.cpp

HEADERS += \
    chartwidget.h \
    simpleplotwidget.h

FORMS += \
    chartwidget.ui

INCLUDEPATH += .

# ==================== CMake 库依赖（如果它本身需要的话）====================
CMAKE_BUILD_DIR = $$PWD/../../build
INCLUDEPATH += $$PWD/../../core/include
INCLUDEPATH += $$PWD/../../model/include
INCLUDEPATH += $$PWD/../../io_medi/include

# 如果 chart_widget 自身也需要链接别的库，可以在这里加