QT += core widgets gui network testlib
CONFIG += c++17 console warn_on
CONFIG -= app_bundle

TARGET = SystemMonitor
TEMPLATE = app

# Include paths
INCLUDEPATH += src/

SOURCES += \
    src/core/systemutils.cpp \
    src/model/base/basemonitor.cpp \
    src/model/managers/alertmanager.cpp \
    src/model/managers/datamanager.cpp \
    src/model/monitors/cpumonitor.cpp \
    src/model/monitors/memorymonitor.cpp \
    src/view/widgets/circularprogress.cpp \
    src/view/widgets/metriccard.cpp

HEADERS += \
    src/core/constants.h \
    src/core/types.h \
    src/core/systemutils.h \
    src/model/base/basemonitor.h \
    src/model/managers/alertmanager.h \
    src/model/managers/datamanager.h \
    src/model/monitors/cpumonitor.h \
    src/model/monitors/memorymonitor.h \
    src/view/widgets/circularprogress.h \
    src/view/widgets/metriccard.h

# Test configuration
CONFIG(test) {
    TARGET = SystemMonitor_test

    SOURCES += \
        tests/test_main.cpp \
        tests/unit/test_systemutils.cpp \
        tests/unit/test_cpumonitor.cpp

    HEADERS += \
        tests/unit/test_systemutils.h \
        tests/unit/test_cpumonitor.h

} else {
    # Main application
    SOURCES += main.cpp
}

# Register metatypes
DEFINES += QT_DEPRECATED_WARNINGS

# Raspberry Pi specific
contains(QT_ARCH, arm.*) {
    DEFINES += TARGET_RASPBERRY_PI
}
