/*-
 * Copyright (c) 2010 Jannis Pohlmann <jannis@xfce.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <config.h>

#include <stdlib.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gudev/gudev.h>
#include <tvmnotify.h>
#include <context.h>
#include <device.h>

//#include <libxfce4util/libxfce4util.h>
//#include <xfconf/xfconf.h>

/* variables for command line options */
static gchar   *opt_sysfs_path = NULL;

/* command line options */
static GOptionEntry option_entries[] =
{
    {"device-added", 'a', 0, G_OPTION_ARG_STRING, &opt_sysfs_path,
     N_ ("The sysfs path of the newly added device"), NULL},
    {NULL, 0, 0, 0, NULL, NULL, NULL},
};

/* udev subsystems supported by thunar-volman */
static const gchar *supported_udev_subsystems[] =
{
    "block",
    "input",
    NULL,
};

int main(int argc, char **argv)
{
    /* setup application name */
    g_set_application_name("Volman");

#ifdef G_ENABLE_DEBUG
    /* Do NOT remove this line for now. If something doesn't work, fix your code instead */
    g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);
#endif

    GError *error = NULL;

    if (!gtk_init_with_args(&argc, &argv, NULL,
                            option_entries, NULL /*GETTEXT_PACKAGE*/, &error))
    {
        g_fprintf (stderr, "%s: %s.\n", g_get_prgname (), error->message);
        g_error_free (error);
        return EXIT_FAILURE;
    }

    GMainLoop *loop = NULL;
    gint exit_code = EXIT_SUCCESS;

    if (opt_sysfs_path != NULL)
    {
        /* create an udev client */
        GUdevClient *client = g_udev_client_new(supported_udev_subsystems);

        /* determine the device belonging to the sysfs path */
        GUdevDevice *device = g_udev_client_query_by_sysfs_path(client,
                                                                opt_sysfs_path);

        if (device != NULL)
        {
            /* get a reference on the thunar-volman settings channel */
            //channel = xfconf_channel_get ("thunar-volman");

            /* create a new main loop */
            loop = g_main_loop_new (NULL, FALSE);

            /* allocate a new TvmContext */
            TvmContext    *context = NULL;
            context = tvm_context_new (client, device);

            /* handle the new device in an idle handler */
            g_idle_add (tvm_context_run, context);

            /* release channel and device */
            g_object_unref (device);
        }
        else
        {
            g_set_error (&error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                         _("There is no device with the sysfs path \"%s\""),
                         opt_sysfs_path);
        }

        /* release the udev client */
        g_object_unref (client);
    }
    else
    {
        /* TRANSLATORS: thunar-volman wasn't invoked with either --device-added or
         * --configure */
        g_set_error (&error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                     _("Must specify the sysfs path of new devices with --device-added"));
    }

    /* run the main loop */
    if (loop != NULL)
    {
        g_main_loop_run (loop);
        g_main_loop_unref (loop);
    }

    if (error != NULL)
    {
        g_fprintf (stderr, "%s: %s.\n", g_get_prgname (), error->message);
        g_error_free (error);
        exit_code = EXIT_FAILURE;
    }

    tvm_notify_uninit ();

    /* release the device context */
    if (context != NULL)
        tvm_context_free (context);

    return exit_code;
}
