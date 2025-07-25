QT += core widgets gui network
CONFIG += c++17 console warn_on
CONFIG -= app_bundle

TARGET = SystemMonitor
TEMPLATE = app

# Phase 1: Core Foundation
SOURCES += \
    src/core/systemutils.cpp

HEADERS += \
    src/core/constants.h \
    src/core/types.h \
    src/core/systemutils.h

# Phase 2: Monitoring Classes
SOURCES += \
    src/model/base/basemonitor.cpp \
    #src/model/monitors/cpumonitor.cpp \
    #src/model/monitors/memorymonitor.cpp \
    #src/model/managers/alertmanager.cpp \
    #src/model/managers/datamanager.cpp

HEADERS += \
    src/model/base/basemonitor.h \
    #src/model/monitors/cpumonitor.h \
    #src/model/monitors/memorymonitor.h \
    #src/model/managers/alertmanager.h \
    #src/model/managers/datamanager.h

# Main application
SOURCES += main.cpp

# Register metatypes
DEFINES += QT_DEPRECATED_WARNINGS
