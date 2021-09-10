
TEMPLATE = app
TARGET = ScreenTestKit
VERSION = 1.0.0

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

linux {
    # suppress the default RPATH if you wish
    QMAKE_LFLAGS_RPATH =
    # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

DEFINES += AppVersion=\\\"$${VERSION}\\\"
DEFINES += QT_DEPRECATED_WARNINGS

# Dest directory
DESTDIR = $${PWD}
win32: {
    CONFIG(debug, debug|release) {
        contains(QT_ARCH, i386) {
            DESTDIR = $${PWD}/Debug.Win32
        } else {
            DESTDIR = $${PWD}/Debug.Win64
        }
    } else {
        contains(QT_ARCH, i386) {
            DESTDIR = $${PWD}/Release.Win32
        } else {
            DESTDIR = $${PWD}/Release.Win64
        }
    }

    # Deployment
    DEPLOYMENT_OPTS += --force
    DEPLOYMENT_OPTS += --compiler-runtime
    #DEPLOYMENT_OPTS += --no-translations
    DEPLOYMENT_OPTS += --no-svg
    DEPLOYMENT_OPTS += --no-opengl-sw
    DEPLOYMENT_OPTS += --no-system-d3d-compiler
    DEPLOYMENT_OPTS += --qmldir $$(QTDIR)/qml

    lessThan(QT_MAJOR_VERSION, 6) {
        DEPLOYMENT_OPTS += --no-angle
        DEPLOYMENT_OPTS += --no-webkit2
    }

    CONFIG(release, debug|release) {
        DEPLOYMENT_OPTS += --release
    }

    QMAKE_POST_LINK = $$[QT_INSTALL_BINS]/windeployqt $${DEPLOYMENT_OPTS} $${DESTDIR} $$escape_expand(\\n\\t)
}

# Build Entrys
BUILDDIR = $${DESTDIR}/.tmp/$${TARGET}

CONFIG(debug, debug|release) {
    win32: BUILDDIR = $${BUILDDIR}/win32-debug
    linux: BUILDDIR = $${BUILDDIR}/linux-debug
} else {
    win32: BUILDDIR = $${BUILDDIR}/win32-release
    linux: BUILDDIR = $${BUILDDIR}/linux-release
}

MOC_DIR += $${BUILDDIR}
OBJECTS_DIR += $${BUILDDIR}/obj
UI_DIR += $${BUILDDIR}
RCC_DIR += $${BUILDDIR}

INCLUDEPATH += $${UI_DIR}

win32: {
    contains(QT_ARCH, i386) {
        # Windows x32
        INCLUDEPATH += $$(QTDIR)/../../Tools/OpenSSL/Win_x86/include
    } else {
        # Windows x64
        INCLUDEPATH += $$(QTDIR)/../../Tools/OpenSSL/Win_x64/include
    }
}

# Source , Headers , Rescources
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    testwindow.cpp

HEADERS += \
    mainwindow.hpp \
    testwindow.hpp

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    ScreenTestKit.zh_CN.ts

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    debian/control \
    build_deb.sh \
    ScreenTestKit.desktop

# Update Translations
QMAKE_EXTRA_COMPILERS += lrelease
lrelease.input         = TRANSLATIONS
lrelease.output        = ${QMAKE_FILE_BASE}.qm
lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
lrelease.CONFIG       += no_link target_predeps

win32 {
    RC_FILE = icon.rc
    QMAKE_TARGET_PRODUCT = "ScreenTestKit"
    QMAKE_TARGET_DESCRIPTION = "Utility to test the quality of screens"
    QMAKE_TARGET_COMPANY = "Kylin Soft"
    QMAKE_TARGET_COPYRIGHT = "Copyright 2020-2040. All rights reserved."
}

DEBIAN_VCONTROL.target = deb_vcontrol
DEBIAN_VCONTROL.commands = $$PWD/vControl/build_deb.sh

DEBIAN.target = deb
DEBIAN.commands = $$PWD/build_deb.sh

QMAKE_EXTRA_TARGETS += DEBIAN
