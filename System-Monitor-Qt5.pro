#############################################
# SystemMonitor Qt Project File - PHASE 1
# Target: Raspberry Pi 3B+ with ILI9341 320x240
# Current Phase: Core Foundation (SystemUtils)
# Version: 1.0.0-alpha
#############################################

QT += core testlib
CONFIG += c++17 warn_on

# Disable GUI for Phase 1 (console application only)
QT -= gui widgets

# Application info
TARGET = SystemMonitor
VERSION = 1.0.0
TEMPLATE = app

# Build configuration
CONFIG(debug, debug|release) {
    TARGET = SystemMonitor_debug
    DEFINES += DEBUG_MODE
    QMAKE_CXXFLAGS_DEBUG += -g -O0
}

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    QMAKE_CXXFLAGS_RELEASE += -O2
}

# Test configuration
CONFIG(test) {
    TARGET = SystemMonitor_test
    QT += testlib
    DEFINES += BUILD_TESTS

    message("Building test configuration")

    # Test sources
    SOURCES += \
        tests/test_main.cpp \
        tests/unit/test_systemutils.cpp \
        src/core/systemutils.cpp

    HEADERS += \
        tests/unit/test_systemutils.h \
        src/core/constants.h \
        src/core/types.h \
        src/core/systemutils.h

} else {
    # Normal application build (Phase 1 - minimal)
    message("Building application configuration")

    SOURCES += \
        main.cpp \
        src/core/systemutils.cpp

    HEADERS += \
        src/core/constants.h \
        src/core/types.h \
        src/core/systemutils.h
}

# Include paths
INCLUDEPATH += \
    src/ \
    src/core/

# Compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra

# Raspberry Pi specific optimizations
contains(QT_ARCH, arm.*) {
    message("Building for ARM architecture (Raspberry Pi 3B+)")
    DEFINES += TARGET_ARM TARGET_RASPBERRY_PI
    QMAKE_CXXFLAGS += -mcpu=cortex-a53 -mfpu=neon-fp-armv8

    # Installation path for Pi
    target.path = /opt/systemmonitor
    INSTALLS += target
}

# Build directories
CONFIG(debug, debug|release) {
    DESTDIR = build/debug
    OBJECTS_DIR = build/debug/obj
    MOC_DIR = build/debug/moc
    RCC_DIR = build/debug/rcc
}

CONFIG(release, debug|release) {
    DESTDIR = build/release
    OBJECTS_DIR = build/release/obj
    MOC_DIR = build/release/moc
    RCC_DIR = build/release/rcc
}

# Create build directories
system(mkdir -p build/debug/obj)
system(mkdir -p build/debug/moc)
system(mkdir -p build/debug/rcc)
system(mkdir -p build/release/obj)
system(mkdir -p build/release/moc)
system(mkdir -p build/release/rcc)

# Display configuration
message("=== SystemMonitor Build Configuration ===")
message("Qt version: $$[QT_VERSION]")
message("Target: $$TARGET")
message("Architecture: $$QT_ARCH")
message("Build type: $$CONFIG")
message("Output directory: $$DESTDIR")
message("==========================================")

# Performance optimizations for embedded
DEFINES += QT_NO_FOREACH QT_NO_KEYWORDS

# Phase identification
DEFINES += PHASE_1_CORE_FOUNDATION
