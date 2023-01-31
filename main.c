
#include <gudev/gudev.h>
//#include <print.h>

// gcc -Wall -o testcmd main.cpp `pkg-config --cflags --libs glib-2.0 gudev-1.0`

static void _on_uevent(GUdevClient *client, const gchar *action, GUdevDevice *device,
                       void *param);
int main()
{
    static const gchar *subsystems[] = {"block" /*, "input", "usb"*/ , NULL};

    GUdevClient *udev_client = g_udev_client_new(subsystems);

    g_signal_connect(udev_client, "uevent", G_CALLBACK(_on_uevent), NULL);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    g_object_unref(loop);
    g_object_unref(udev_client);

    return EXIT_SUCCESS;
}

static void _on_uevent(GUdevClient *client, const gchar *action, GUdevDevice *device,
                       void *param)
{
    g_return_if_fail(G_UDEV_IS_CLIENT(client));
    g_return_if_fail(action != NULL && *action != '\0');
    g_return_if_fail(G_UDEV_IS_DEVICE(device));

    g_print("action: %s\n", action);
    g_print("--------------------------------------------------------\n");
    const gchar *val;
    gchar valc;

    val = g_udev_device_get_name(device);
    if (val && *val != '\0')
        g_print("name: %s\n", val);

    val = g_udev_device_get_sysfs_path(device);
    if (val && *val != '\0')
        g_print("sysfs: %s\n", val);

    val = g_udev_device_get_subsystem(device);
    if (val && *val != '\0')
        g_print("subsystem: %s\n", val);

    val = g_udev_device_get_devtype(device);
    if (val && *val != '\0')
        g_print("devtype: %s\n", val);

    valc = g_udev_device_get_device_type(device);
    if (valc != '\0')
        g_print("device type: %c\n", valc);

    val = g_udev_device_get_device_file(device);
    if (val && *val != '\0')
        g_print("device file: %s\n", val);

    val = g_udev_device_get_driver(device);
    if (val && *val != '\0')
        g_print("driver: %s\n", val);

    const gchar *const *symlinks = g_udev_device_get_device_file_symlinks(device);
    for (int j = 0; symlinks[j] != NULL; ++j)
    {
        val = symlinks[j];

        if (val && *val != '\0')
            g_print("    symlink: %s\n", val);
    }

    const gchar * const * tags = g_udev_device_get_tags(device);
    for (int j = 0; tags[j] != NULL; ++j)
    {
        val = tags[j];

        if (val && *val != '\0')
            g_print("    tag: %s\n", val);
    }

    g_print("\n\n");
}


