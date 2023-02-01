#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#if defined(GETTEXT_PACKAGE)
#include <glib/gi18n-lib.h>
#else
#include <glib/gi18n.h>
#endif

#define APP_NAME "volman"
#define APP_DISPLAY_NAME "Volman"
#define PACKAGE_NAME APP_NAME

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#endif // CONFIG_H


