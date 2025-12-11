QT += core gui

DEFINES += \
  QRETRO_INTERNAL=1 \
  CL_LIBRETRO=1 \
  CL_HAVE_EDITOR=1 \
  CL_HAVE_FILESYSTEM=1 \
  CL_HAVE_SSL=0

CONFIG += QRETRO_CONFIG_NO_MIDI

include(classicslive-integration/classicslive-integration.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

include(QRetro/QRetro.pri)

win32 {
  QMAKE_LFLAGS += -static
  QMAKE_CXXFLAGS += -static-libgcc
  QMAKE_CXXFLAGS += -static-libstdc++
}

SOURCES += \
  Pleasant.cpp \
  cl_frontend.cpp \
  cls_network_manager.cpp \
  cls_thread.cpp \
  main.cpp \
  mainwindow.cpp

INCLUDEPATH += \
  $$PWD/classicslive-integration \
  $$PWD/classicslive-integration/editor \
  $$PWD/libretro-common/include

HEADERS += \
  Pleasant.h \
  cls_network_manager.h \
  cls_thread.h \
  mainwindow.h

TRANSLATIONS += \
  Pleasant_en_US.ts \
  Pleasant_ja_JP.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

ANDROID_ABIS = x86_64
