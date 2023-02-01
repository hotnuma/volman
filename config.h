#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#if defined(GETTEXT_PACKAGE)
#include <glib/gi18n-lib.h>
#else
#include <glib/gi18n.h>
#endif

#define APPNAME "volman"
#define APP_DISPLAYNAME "Volman"
#define PACKAGE_NAME APPNAME
#define APPLOCK "/tmp/" APPNAME ".pid"

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#endif // CONFIG_H


