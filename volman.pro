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
    context.h \
    devblock.h \
    device.h \
    printlog.h \
    tvmnotify.h

SOURCES = \
    0Temp.c \
    context.c \
    devblock.c \
    device.c \
    main.c \
    printlog.c \
    tvmnotify.c

DISTFILES += \
    License.txt \
    Readme.md \
    install.sh \
    meson.build


