/*-
 * Copyright (c) 2007-2008 Benedikt Meurer <benny@xfce.org>
 * Copyright (c) 2010      Jannis Pohlmann <jannis@xfce.org>
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

#include "config.h"
#include "devblock.h"

#include "device.h"
#include "tvmnotify.h"
#include "printlog.h"

#include <gtk/gtk.h>

static gboolean _devblock_mount(gpointer user_data);
static void _devblock_mount_finish(GVolume *volume, GAsyncResult *result,
                                   TvmContext *context);
static void _devblock_notify(TvmContext *context, GMount *mount, GError **error);

GVolume* _volume_monitor_get_volume_for_kind(GVolumeMonitor *monitor,
                                             const gchar    *kind,
                                             const gchar    *identifier);

typedef gboolean (*DeviceBlockHandler) (TvmContext *context,
                                        GMount     *mount,
                                        GError     **error);

void device_block_added(TvmContext *context)
{
    g_return_if_fail(context != NULL);

    // get device types
    const gchar *devtype = g_udev_device_get_property(context->device,
                                                      "DEVTYPE");
    const gchar *id_fs_usage = g_udev_device_get_property(context->device,
                                                          "ID_FS_USAGE");

    gboolean is_partition = (g_strcmp0(devtype, "partition") == 0);

    gboolean is_volume = (g_strcmp0(devtype, "disk") == 0)
                          && (g_strcmp0(id_fs_usage, "filesystem") == 0);

    if (!is_partition && !is_volume)
    {
        printinfo("Unknown block device type \"%s\"", devtype);
        device_cleanup(context);
        return;
    }

    guint delay = 5;

    if (is_partition)
        delay = 2;

    g_timeout_add_seconds(delay, _devblock_mount, context);
}

static gboolean _devblock_mount(gpointer user_data)
{
    TvmContext *context = user_data;

    /* determine the GVolume corresponding to the udev device */
    GVolume *volume = _volume_monitor_get_volume_for_kind(
                                context->monitor,
                                G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE,
                                g_udev_device_get_device_file(context->device));

    /* check if we have a volume */
    if (!volume)
    {
        printinfo("Could not detect the volume corresponding to the device");
        device_cleanup(context);
        return false;
    }

    /* check if we can mount the volume */
    if (!g_volume_can_mount(volume))
    {
        printinfo("Unable to mount the device");
        device_cleanup(context);
        return false;
    }

    /* try to mount the volume asynchronously */
    GMountOperation *mount_operation = gtk_mount_operation_new(NULL);

    g_volume_mount(volume,
                   G_MOUNT_MOUNT_NONE,
                   mount_operation,
                   NULL,
                   (GAsyncReadyCallback) _devblock_mount_finish,
                   context);

    g_object_unref(mount_operation);

    return false;
}

GVolume* _volume_monitor_get_volume_for_kind(GVolumeMonitor *monitor,
                                             const gchar    *kind,
                                             const gchar    *identifier)
{
    g_return_val_if_fail(G_IS_VOLUME_MONITOR (monitor), NULL);
    g_return_val_if_fail(kind != NULL && *kind != '\0', NULL);
    g_return_val_if_fail(identifier != NULL && *identifier != '\0', NULL);

    GList *volumes = g_volume_monitor_get_volumes(monitor);

    GVolume *volume = NULL;

    for (GList *lp = volumes; volume == NULL && lp != NULL; lp = lp->next)
    {
        gchar *value = g_volume_get_identifier(lp->data, kind);

        if (value != NULL)
        {
            if (g_strcmp0(value, identifier) == 0)
                volume = g_object_ref(lp->data);

            g_free(value);
        }

        g_object_unref(lp->data);
    }

    g_list_free(volumes);

    return volume;
}

static void _devblock_mount_finish(GVolume *volume, GAsyncResult *result,
                                   TvmContext *context)
{
    g_return_if_fail(G_IS_VOLUME(volume));
    g_return_if_fail(G_IS_ASYNC_RESULT(result));
    g_return_if_fail(context != NULL);

    //GError *error = NULL;

    /* finish mounting the volume */
    if (g_volume_mount_finish(volume, result, NULL /*&error*/))
    {
        /* get the moint point of the volume */
        GMount *mount = g_volume_get_mount(volume);

        if (mount != NULL)
        {
            _devblock_notify(context, mount, NULL /*&error*/);

            g_object_unref(mount);
        }
        else
        {
            /* could not locate the mount point */
            //g_set_error(&error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
            //            _("Unable to locate mount point"));

            printinfo("Unable to locate mount point");
        }
    }

    g_object_unref(volume);

    device_cleanup(context);
}

static void _devblock_notify(TvmContext *context, GMount *mount, GError **error)
{
    g_return_if_fail(context != NULL);
    g_return_if_fail(G_IS_MOUNT (mount));
    g_return_if_fail(error == NULL || *error == NULL);

    const gchar *icon = "drive-optical";

    const gchar *summary;
    gchar *message;

    // CD
    if (g_udev_device_get_property_as_boolean(context->device, "ID_CDROM_MEDIA_CD"))
    {
        summary = _("CD mounted");
        message = g_strdup(_("The CD was mounted automatically"));
    }

    // DVD
    else if (g_udev_device_get_property_as_boolean(context->device, "ID_CDROM_MEDIA_DVD"))
    {
        summary = _("DVD mounted");
        message = g_strdup(_("The DVD was mounted automatically"));
    }

    // Blu-ray
    else if (g_udev_device_get_property_as_boolean(context->device, "ID_CDROM_MEDIA_BD"))
    {
        summary = _("Blu-ray mounted");
        message = g_strdup(_("The Blu-ray was mounted automatically"));
    }
    else
    {
        /* fetch the volume name */
        const gchar *volume_name = g_udev_device_get_property(context->device,
                                                              "ID_FS_LABEL_ENC");
        gchar *decoded_name = tvm_notify_decode(volume_name);

        /* generate notification info */
        icon = "drive-removable-media";
        summary = _("Volume mounted");
        if (decoded_name != NULL)
        {
            message = g_strdup_printf(_("The volume \"%s\" was mounted automatically"),
                                      decoded_name);
        }
        else
        {
            message = g_strdup_printf(_("The inserted volume was mounted automatically"));
        }

        g_free(decoded_name);
    }

    // show the notification
    tvm_notify(icon, summary, message);

    g_free(message);
}


