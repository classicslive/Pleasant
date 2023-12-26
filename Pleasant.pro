QT += core gui

DEFINES += \
  QRETRO_INTERNAL=1 \
  CL_LIBRETRO=1 \
  CL_HAVE_EDITOR=1 \
  CL_HAVE_FILESYSTEM=1

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

include(QRetro/QRetro.pri)

QMAKE_LFLAGS += -static
QMAKE_CXXFLAGS += -static-libgcc
QMAKE_CXXFLAGS += -static-libstdc++

SOURCES += \
  Pleasant.cpp \
  cl_frontend.cpp \
  classicslive-integration/cl_action.c \
  classicslive-integration/cl_common.c \
  classicslive-integration/cl_counter.c \
  classicslive-integration/cl_identify.c \
  classicslive-integration/cl_json.c \
  classicslive-integration/cl_main.c \
  classicslive-integration/cl_memory.c \
  classicslive-integration/cl_network.c \
  classicslive-integration/cl_script.c \
  classicslive-integration/cl_search.c \
  classicslive-integration/editor/cle_action_block.cpp \
  classicslive-integration/editor/cle_action_block_bookend.cpp \
  classicslive-integration/editor/cle_action_block_ctrbinary.cpp \
  classicslive-integration/editor/cle_common.cpp \
  classicslive-integration/editor/cle_hex_view.cpp \
  classicslive-integration/editor/cle_main.cpp \
  classicslive-integration/editor/cle_memory_inspector.cpp \
  classicslive-integration/editor/cle_memory_note_submit.cpp \
  classicslive-integration/editor/cle_result_table.cpp \
  classicslive-integration/editor/cle_result_table_normal.cpp \
  classicslive-integration/editor/cle_result_table_pointer.cpp \
  classicslive-integration/editor/cle_script_editor.cpp \
  classicslive-integration/editor/cle_script_editor_block.cpp \
  cls_network_manager.cpp \
  cls_thread.cpp \
  libretro-common/compat/compat_posix_string.c \
  libretro-common/compat/compat_strl.c \
  libretro-common/compat/fopen_utf8.c \
  libretro-common/encodings/encoding_base64.c \
  libretro-common/encodings/encoding_crc32.c \
  libretro-common/encodings/encoding_utf.c \
  libretro-common/file/file_path.c \
  libretro-common/formats/json/jsonsax.c \
  libretro-common/formats/json/jsonsax_full.c \
  libretro-common/formats/libchdr/libchdr_bitstream.c \
  libretro-common/formats/libchdr/libchdr_cdrom.c \
  libretro-common/formats/libchdr/libchdr_chd.c \
  libretro-common/formats/libchdr/libchdr_huffman.c \
  libretro-common/hash/lrc_hash.c \
  libretro-common/streams/chd_stream.c \
  libretro-common/streams/file_stream.c \
  libretro-common/streams/interface_stream.c \
  libretro-common/streams/memory_stream.c \
  libretro-common/string/stdstring.c \
  libretro-common/time/rtime.c \
  libretro-common/utils/md5.c \
  libretro-common/vfs/vfs_implementation.c \
  main.cpp \
  mainwindow.cpp

INCLUDEPATH += \
  $$PWD/classicslive-integration \
  $$PWD/classicslive-integration/editor \
  $$PWD/libretro-common/include

HEADERS += \
  Pleasant.h \
  classicslive-integration/cl_action.h \
  classicslive-integration/cl_common.h \
  classicslive-integration/cl_config.h \
  classicslive-integration/cl_counter.h \
  classicslive-integration/cl_frontend.h \
  classicslive-integration/cl_identify.h \
  classicslive-integration/cl_json.h \
  classicslive-integration/cl_main.h \
  classicslive-integration/cl_memory.h \
  classicslive-integration/cl_network.h \
  classicslive-integration/cl_script.h \
  classicslive-integration/cl_search.h \
  classicslive-integration/cl_types.h \
  classicslive-integration/editor/cle_action_block.h \
  classicslive-integration/editor/cle_action_block_bookend.h \
  classicslive-integration/editor/cle_action_block_ctrbinary.h \
  classicslive-integration/editor/cle_common.h \
  classicslive-integration/editor/cle_hex_view.h \
  classicslive-integration/editor/cle_main.h \
  classicslive-integration/editor/cle_memory_inspector.h \
  classicslive-integration/editor/cle_memory_note_submit.h \
  classicslive-integration/editor/cle_result_table.h \
  classicslive-integration/editor/cle_result_table_normal.h \
  classicslive-integration/editor/cle_result_table_pointer.h \
  classicslive-integration/editor/cle_script_editor.h \
  classicslive-integration/editor/cle_script_editor_block.h \
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
