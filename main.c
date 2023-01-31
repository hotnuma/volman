#include "config.h"

#include "context.h"
#include "tvmnotify.h"
//#include <gudev/gudev.h>

static void _on_uevent(GUdevClient *client, const gchar *action,
                       GUdevDevice *device, void *param);
int main()
{
    g_set_application_name("Volman");

    static const gchar *subsystems[] = {"block", NULL};

    GUdevClient *client = g_udev_client_new(subsystems);

    g_signal_connect(client, "uevent", G_CALLBACK(_on_uevent), NULL);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    g_object_unref(loop);
    g_object_unref(client);

    tvm_notify_uninit ();

    return EXIT_SUCCESS;
}

static void _on_uevent(GUdevClient *client, const gchar *action,
                       GUdevDevice *device, void *param)
{
    g_return_if_fail(G_UDEV_IS_CLIENT(client));
    g_return_if_fail(action != NULL && *action != '\0');
    g_return_if_fail(G_UDEV_IS_DEVICE(device));

    UNUSED(param);

    const gchar *subsystem = g_udev_device_get_subsystem(device);
    const gchar *devtype = g_udev_device_get_devtype(device);
    const gchar *syspath = g_udev_device_get_sysfs_path(device);

    if (strcmp(action, "add") != 0
        || strcmp(subsystem, "block") != 0
        || strcmp(devtype, "partition") != 0
        || syspath == NULL
        || *syspath == '\0')
        return;

    g_print("add %s\n", syspath);

    TvmContext *context = tvm_context_new(client, device);

    g_idle_add(tvm_context_run, context);
}


