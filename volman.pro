TEMPLATE = app
TARGET = volman
CONFIG = c99 link_pkgconfig
DEFINES =
INCLUDEPATH =
PKGCONFIG =

PKGCONFIG += gtk+-3.0
PKGCONFIG += gudev-1.0

HEADERS += \
    config.h \
    tvm-block-device.h \
    tvm-context.h \
    tvm-device.h \
    tvm-gio-extensions.h \
    tvm-notify.h \
    tvm-pango-extensions.h \
    tvm-prompt.h

SOURCES = \
    0Temp.c \
    main.c \
    othermain.c \
    tvm-block-device.c \
    tvm-context.c \
    tvm-device.c \
    tvm-gio-extensions.c \
    tvm-notify.c \
    tvm-pango-extensions.c \
    tvm-prompt.c

DISTFILES += \
    License.txt \
    Readme.md \
    install.sh \
    meson.build


