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

#include <config.h>
#include <devblock.h>

#include <gtk/gtk.h>

#include <sys/stat.h>
#include <gio/gio.h>
#include <gudev/gudev.h>
#include <context.h>
#include <device.h>
#include <libnotify/notify.h>
#include <tvmnotify.h>

GVolume *tvm_g_volume_monitor_get_volume_for_kind(GVolumeMonitor *monitor,
                                                  const gchar    *kind,
                                                  const gchar    *identifier);

typedef gboolean (*TvmBlockDeviceHandler) (TvmContext *context,
                                           GMount     *mount,
                                           GError     **error);

static void tvm_block_device_mounted(TvmContext *context, GMount *mount,
                                     GError **error)
{
    g_return_if_fail (context != NULL);
    g_return_if_fail (G_IS_MOUNT (mount));
    g_return_if_fail (error == NULL || *error == NULL);

    //gboolean     success = FALSE;
    //guint        n;

    GError      *err = NULL;


    const gchar *summary;
    const gchar *icon;
    const gchar *volume_name;
    gchar       *message;
    gchar       *decoded_name;
    icon = "drive-optical";

    /* distinguish between CDs, DVDs, Blu-rays */
    if (g_udev_device_get_property_as_boolean (context->device, "ID_CDROM_MEDIA_CD"))
    {
        /* generate notification info */
        summary = _("CD mounted");
        message = g_strdup (_("The CD was mounted automatically"));
    }
    else if (g_udev_device_get_property_as_boolean (context->device, "ID_CDROM_MEDIA_DVD"))
    {
        /* generate notification info */
        summary = _("DVD mounted");
        message = g_strdup (_("The DVD was mounted automatically"));
    }
    else if (g_udev_device_get_property_as_boolean (context->device, "ID_CDROM_MEDIA_BD"))
    {
        /* generate notification info */
        summary = _("Blu-ray mounted");
        message = g_strdup (_("The Blu-ray was mounted automatically"));
    }
    else
    {
        /* fetch the volume name */
        volume_name = g_udev_device_get_property (context->device, "ID_FS_LABEL_ENC");
        decoded_name = tvm_notify_decode (volume_name);

        /* generate notification info */
        icon = "drive-removable-media";
        summary = _("Volume mounted");
        if (decoded_name != NULL)
        {
            message = g_strdup_printf (_("The volume \"%s\" was mounted automatically"),
                                       decoded_name);
        }
        else
        {
            message = g_strdup_printf (_("The inserted volume was mounted automatically"));
        }

        g_free (decoded_name);
    }

    /* display the notification */
    tvm_notify (icon, summary, message);

    /* clean up */
    g_free (message);


#if 0
    /* try block device handlers (iPod, cameras etc.) until one succeeds */
    for (n = 0; !success && err == NULL && n < G_N_ELEMENTS (block_device_handlers); ++n)
        success = (block_device_handlers[n]) (context, mount, &err);
#endif

    /* forward errors to the caller */
    if (err != NULL)
        g_propagate_error (error, err);
}

static void tvm_block_device_mount_finish(GVolume *volume, GAsyncResult *result,
                                          TvmContext *context)
{
    GMount *mount;
    GError *error = NULL;

    g_return_if_fail (G_IS_VOLUME (volume));
    g_return_if_fail (G_IS_ASYNC_RESULT (result));
    g_return_if_fail (context != NULL);

    /* finish mounting the volume */
    if (g_volume_mount_finish (volume, result, &error))
    {
        /* get the moint point of the volume */
        mount = g_volume_get_mount (volume);

        if (mount != NULL)
        {
            /* inspect volume contents and perform actions based on them */
            tvm_block_device_mounted (context, mount, &error);

            /* release the mount point */
            g_object_unref (mount);
        }
        else
        {
            /* could not locate the mount point */
            g_set_error (&error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                         _("Unable to locate mount point"));
        }
    }

    /* release the volume */
    g_object_unref (volume);

    /* move error information into the context */
    //if (error != NULL)
    //    g_propagate_error(&context->error, error);

    /* finish processing the device */

    tvm_device_handler_finished (context);
}

static gboolean tvm_block_device_mount (gpointer user_data)
{
    TvmContext      *context = user_data;
    GMountOperation *mount_operation;
    GVolume         *volume;

    /* determine the GVolume corresponding to the udev device */
    volume =
        tvm_g_volume_monitor_get_volume_for_kind (context->monitor,
                G_VOLUME_IDENTIFIER_KIND_UNIX_DEVICE,
                g_udev_device_get_device_file (context->device));

    /* check if we have a volume */
    if (volume != NULL)
    {
        /* check if we can mount the volume */
        if (g_volume_can_mount (volume))
        {
            /* try to mount the volume asynchronously */
            mount_operation = gtk_mount_operation_new (NULL);
            g_volume_mount (volume, G_MOUNT_MOUNT_NONE, mount_operation,
                            NULL, (GAsyncReadyCallback) tvm_block_device_mount_finish, context);
            g_object_unref (mount_operation);
        }
        else
        {
            g_set_error(&context->error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                         _("Unable to mount the device"));

            /* finish processing the device */
            tvm_device_handler_finished(context);
        }
    }
    else
    {
        g_set_error(&context->error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                     _("Could not detect the volume corresponding to the device"));

        /* finish processing the device */
        tvm_device_handler_finished (context);
    }

    return FALSE;
}

GVolume *
tvm_g_volume_monitor_get_volume_for_kind (GVolumeMonitor *monitor,
        const gchar    *kind,
        const gchar    *identifier)
{
    GVolume *volume = NULL;
    GList   *volumes;
    GList   *lp;
    gchar   *value;

    g_return_val_if_fail (G_IS_VOLUME_MONITOR (monitor), NULL);
    g_return_val_if_fail (kind != NULL && *kind != '\0', NULL);
    g_return_val_if_fail (identifier != NULL && *identifier != '\0', NULL);

    volumes = g_volume_monitor_get_volumes (monitor);

    for (lp = volumes; volume == NULL && lp != NULL; lp = lp->next)
    {
        value = g_volume_get_identifier (lp->data, kind);
        if (value != NULL)
        {
            if (g_strcmp0 (value, identifier) == 0)
                volume = g_object_ref (lp->data);

            g_free (value);
        }

        g_object_unref (lp->data);
    }

    g_list_free (volumes);

    return volume;
}

void tvm_block_device_added(TvmContext *context)
{
    g_return_if_fail (context != NULL);

    gboolean     automount;
    /* collect general device information */
    const gchar *devtype = g_udev_device_get_property(context->device, "DEVTYPE");
    const gchar *id_fs_usage = g_udev_device_get_property(context->device, "ID_FS_USAGE");

    /* distinguish device types */
    gboolean     is_partition;
    is_partition = (g_strcmp0 (devtype, "partition") == 0);

    gboolean     is_volume;
    is_volume = (g_strcmp0 (devtype, "disk") == 0)
                && (g_strcmp0 (id_fs_usage, "filesystem") == 0);

    if (is_partition || is_volume)
    {
        /* check if automounting drives is enabled */
        automount = true; /*xfconf_channel_get_bool (context->channel, "/automount-drives/enabled",
                                             FALSE);*/
        if (automount)
        {
            /* mount the partition and continue with inspecting its contents */
            g_timeout_add_seconds(5, tvm_block_device_mount, context);
        }
        else
        {
            /* finish processing the device */
            tvm_device_handler_finished (context);
        }
    }
    else
    {
        /* generate an error for logging */
        g_set_error (&context->error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                     _("Unknown block device type \"%s\""), devtype);

        /* finish processing the device */
        tvm_device_handler_finished (context);
    }
}


