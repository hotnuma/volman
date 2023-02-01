#include "config.h"

#include "context.h"
#include "tvmnotify.h"
#include "printlog.h"
#include <gtk/gtk.h>
#include <libapp.h>

static void _on_uevent(GUdevClient *client, const gchar *action,
                       GUdevDevice *device, void *param);

static const gchar *_subsystems[] = {"block", NULL};

int main(int argc, char **argv)
{
    if (!app_isfirst(APPLOCK))
    {
        printinfo("Program is already running, abort...");
        return EXIT_FAILURE;
    }

    printinfo("Program started...");

    gtk_init(&argc, &argv);

    g_set_application_name("Volman");

    GUdevClient *client = g_udev_client_new(_subsystems);

    g_signal_connect(client, "uevent", G_CALLBACK(_on_uevent), NULL);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    g_object_unref(loop);
    g_object_unref(client);

    tvm_notify_uninit();

    return EXIT_SUCCESS;
}

static void _on_uevent(GUdevClient *client, const gchar *action,
                       GUdevDevice *device, void *param)
{
    g_return_if_fail(G_UDEV_IS_CLIENT(client));
    g_return_if_fail(action != NULL && *action != '\0');
    g_return_if_fail(G_UDEV_IS_DEVICE(device));

    const gchar *subsystem = g_udev_device_get_subsystem(device);
    g_return_if_fail(strcmp(subsystem, "block") == 0);

    UNUSED(param);

    const gchar *devtype = g_udev_device_get_devtype(device);

    if (strcmp(action, "add") != 0
        || strcmp(devtype, "partition") != 0)
    {
        printinfo("%s : %s", action, devtype);
        return;
    }

    const gchar *devfile = g_udev_device_get_device_file(device);
    printinfo("%s : mount %s", action, devfile);

    TvmContext *context = tvm_context_new(client, device);
    g_idle_add(tvm_context_run, context);
}


