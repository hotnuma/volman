#if 0

void tvm_device_handler_finished(TvmContext *context)
{
    g_return_if_fail(context != NULL);

    if (context->error != NULL)
    {
        g_list_free(context->handlers);
    }
    else
    {
        if (context->handlers != NULL)
            _device_next_handler(context);
    }
}


GError *err = NULL;

/* try block device handlers (iPod, cameras etc.) until one succeeds */
for (n = 0; !success && err == NULL && n < G_N_ELEMENTS (block_device_handlers); ++n)
    success = (block_device_handlers[n]) (context, mount, &err);

/* forward errors to the caller */
if (err != NULL)
    g_propagate_error (error, err);


// ----------------------------------------------------------------------------

void
tvm_block_device_added (TvmContext *context)
{
#if 0
    GError      *error = NULL;
    gint         response;

    const gchar *media_state;
    gboolean     autoplay;
    guint64      audio_tracks;
    guint64      data_tracks;

    gboolean     is_cdrom;
    is_cdrom = (g_strcmp0 (id_type, "cd") == 0);

    const gchar *id_type;
    id_type = g_udev_device_get_property (context->device, "ID_TYPE");

    if (is_cdrom)
    {
        /* silently ignore CD drives without media */
        if (g_udev_device_get_property_as_boolean (context->device, "ID_CDROM_MEDIA"))
        {
            /* collect CD information */
            media_state = g_udev_device_get_property (context->device,
                          "ID_CDROM_MEDIA_STATE");
            data_tracks =
                g_udev_device_get_property_as_uint64 (context->device,
                        "ID_CDROM_MEDIA_TRACK_COUNT_DATA");
            audio_tracks =
                g_udev_device_get_property_as_uint64 (context->device,
                        "ID_CDROM_MEDIA_TRACK_COUNT_AUDIO");

            /* check if we have a blank CD/DVD here */
            if (g_strcmp0 (media_state, "blank") == 0)
            {
                /* try to run the burn program */
                if (!tvm_run_burn_software (context, &error))
                    g_propagate_error (context->error, error);

                /* finish processing the device */
                tvm_device_handler_finished (context);
            }
            else if (audio_tracks > 0 && data_tracks > 0)
            {
                /* check if both autoplay and automounting of CDs/DVDs is enabled */
                automount = true; /* xfconf_channel_get_bool (context->channel,
                                                     "/automount-media/enabled", FALSE);*/

                autoplay = false; /*xfconf_channel_get_bool (context->channel,
                                                    "/autoplay-audio-cds/enabled", FALSE);*/
                if (automount && autoplay)
                {
                    /* ask what to do with the mixed audio/data disc */
                    response = tvm_prompt (context, "gnome-dev-cdrom-audio",
                                           _("Audio/Data CD"),
                                           _("The CD in the drive contains both music "
                                             "and files"),
                                           _("Would you like to listen to music or "
                                             "browse the files?"),
                                           _("Ig_nore"), GTK_RESPONSE_CANCEL,
                                           _("_Browse Files"), TVM_RESPONSE_BROWSE,
                                           _("_Play CD"), TVM_RESPONSE_PLAY,
                                           NULL);

                    switch (response)
                    {
                    case TVM_RESPONSE_PLAY:
                        goto autoplay_disc;
                        break;

                    case TVM_RESPONSE_BROWSE:
                        goto automount_disc;
                        break;

                    default:
                        /* finish processing the device */
                        tvm_device_handler_finished (context);
                        break;
                    }
                }
                else if (automount)
                {
                    /* just mount the disc automatically */
                    goto automount_disc;
                }
                else if (autoplay)
                {
                    /* just play the disc automatically */
                    goto autoplay_disc;
                }
                else
                {
                    /* finish processing the device */
                    tvm_device_handler_finished (context);
                }
            }
            else if (audio_tracks > 0)
            {
autoplay_disc:
                /* open the audio CD in the favorite CD player */
                tvm_run_cd_player (context, context->error);

                /* finish processing the device */
                tvm_device_handler_finished (context);
            }
            else if (data_tracks > 0)
            {
automount_disc:
                /* check if automounting media is enabled */
                automount = xfconf_channel_get_bool (context->channel,
                                                     "/automount-media/enabled", FALSE);
                if (automount)
                {
                    /* mount the CD/DVD and continue with inspecting its contents */
                    g_timeout_add_seconds(5, tvm_block_device_mount, context);
                }
            }
            else
            {
                /* finish processing the device */
                tvm_device_handler_finished (context);
            }
        }
        else
        {
            /* finish processing the device */
            tvm_device_handler_finished (context);
        }
    }
    else

#endif

    g_return_if_fail (context != NULL);

    gboolean     automount;
    /* collect general device information */
    const gchar *devtype;
    devtype = g_udev_device_get_property (context->device, "DEVTYPE");
    const gchar *id_fs_usage;
    id_fs_usage = g_udev_device_get_property (context->device, "ID_FS_USAGE");

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
        g_set_error (context->error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                     _("Unknown block device type \"%s\""), devtype);

        /* finish processing the device */
        tvm_device_handler_finished (context);
    }
}

static void _on_uevent_debug(GUdevClient *client, const gchar *action, GUdevDevice *device,
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

static gboolean tvm_file_test               (GMount      *mount,
        const gchar *filename,
        GFileTest    test);

static gboolean
tvm_file_test (GMount      *mount,
               const gchar *filename,
               GFileTest    test)
{
    const gchar *name;
    gboolean     result = FALSE;
    GFile       *mount_point;
    gchar       *directory;
    gchar       *path;
    GDir        *dp;

    g_return_val_if_fail (G_IS_MOUNT (mount), FALSE);
    g_return_val_if_fail (filename != NULL && *filename != '\0', FALSE);

    mount_point = g_mount_get_root (mount);
    directory = g_file_get_path (mount_point);
    g_object_unref (mount_point);

    /* try to open the specified directory */
    dp = g_dir_open (directory, 0, NULL);
    if (G_LIKELY (dp != NULL))
    {
        while (!result)
        {
            /* read the next entry */
            name = g_dir_read_name (dp);
            if (G_UNLIKELY (name == NULL))
                break;

            /* check if we have a potential match */
            if (g_ascii_strcasecmp (name, filename) == 0)
            {
                /* check if test condition met */
                path = g_build_filename (directory, name, NULL);
                result = g_file_test (path, test);
                g_free (path);
            }
        }

        /* cleanup */
        g_dir_close (dp);
    }

    g_free (directory);

    return result;
}

static gboolean tvm_block_device_autophoto  (TvmContext  *context,
        GMount      *mount,
        GError     **error);
static gboolean tvm_block_device_autorun    (TvmContext  *context,
        GMount      *mount,
        GError     **error);

static gboolean tvm_block_device_autoipod   (TvmContext  *context,
        GMount      *mount,
        GError     **error);
static gboolean tvm_block_device_autobrowse (TvmContext  *context,
        GMount      *mount,
        GError     **error);

static DeviceBlockHandler block_device_handlers[] =
{
    tvm_block_device_autoipod,
    tvm_block_device_autophoto,
    tvm_block_device_autorun,
    tvm_block_device_autobrowse,
};

static gboolean
tvm_block_device_autobrowse (TvmContext *context,
                             GMount     *mount,
                             GError    **error)
{
    gboolean autobrowse;
    gboolean result = FALSE;
    GError  *err = NULL;

    g_return_val_if_fail (context != NULL, FALSE);
    g_return_val_if_fail (G_IS_MOUNT (mount), FALSE);
    g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

    /* check whether auto-browsing of volumes is enabled */
    autobrowse = xfconf_channel_get_bool (context->channel, "/autobrowse/enabled", FALSE);
    if (autobrowse)
    {
        /* try to open the mount point in the default File Manager */
        result = tvm_run_command (context, mount, "exo-open --launch FileManager %m", &err);
    }

    /* forward errors to the caller */
    if (err != NULL)
        g_propagate_error (error, err);

    return result;
}

static gboolean
tvm_block_device_autoipod (TvmContext *context,
                           GMount     *mount,
                           GError    **error)
{
    gboolean autoipod;
    gboolean is_audio_player = FALSE;
    gboolean is_ipod = FALSE;
    gboolean result = FALSE;
    GFile   *mount_point;
    gchar   *autoipod_command;
    gchar   *autophoto_command;
    gchar   *mount_path;
    gchar   *path_dcim = NULL;
    gint     response = TVM_RESPONSE_NONE;

    g_return_val_if_fail (context != NULL, FALSE);
    g_return_val_if_fail (G_IS_MOUNT (mount), FALSE);
    g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

    autoipod = xfconf_channel_get_bool (context->channel, "/autoipod/enabled", FALSE);
    if (autoipod)
    {
        /* check if we have a portable audio player here */
        is_audio_player = g_udev_device_has_property (context->device,
                          "ID_MEDIA_PLAYER");

        /* check if we have an iPod */
        is_ipod = g_str_has_prefix (g_udev_device_get_property (context->device,
                                    "ID_MODEL"), "iPod");
        if (is_ipod)
        {
            /* determine the mount point path */
            mount_point = g_mount_get_root (mount);
            mount_path = g_file_get_path (mount_point);
            g_object_unref (mount_point);

            /* build dcim path */
            path_dcim = g_build_filename (mount_path, "dcim", NULL);
            g_free (mount_path);

            /* check if the iPod has photos */
            if (!g_file_test (path_dcim, G_FILE_TEST_IS_DIR))
            {
                /* no photos */
                g_free (path_dcim);
                path_dcim = NULL;
            }
        }

        autoipod_command = xfconf_channel_get_string (context->channel,
                           "/autoipod/command", NULL);
        autophoto_command = xfconf_channel_get_string (context->channel,
                            "/autophoto/command", NULL);

        /* check if autophoto command is specified, otherwise we cannot handle the photos
         * on the iPod anyway */
        if (autophoto_command == NULL || *autophoto_command == '\0')
        {
            g_free (path_dcim);
            path_dcim = NULL;
        }

        /* iPods can carry music and photos... */
        if (path_dcim != NULL)
        {
            /* ...so we need to prompt what to do */
            response = tvm_prompt (context, "multimedia-player", _("Photos and Music"),
                                   _("Photos were found on your portable music player"),
                                   _("Would you like to import the photos or manage the "
                                     "music?"),
                                   _("Ig_nore"), GTK_RESPONSE_CANCEL,
                                   _("Import _Photos"), TVM_RESPONSE_PHOTOS,
                                   _("Manage _Music"), TVM_RESPONSE_MUSIC,
                                   NULL);
        }
        else if (is_audio_player || is_ipod)
        {
            response = TVM_RESPONSE_MUSIC;
        }

        /* check what to do */
        if (response == TVM_RESPONSE_MUSIC)
        {
            /* run the preferred application to manage music players */
            result = tvm_run_command (context, mount, autoipod_command, error);
        }
        else if (response == TVM_RESPONSE_PHOTOS)
        {
            /* run the preferred application to manage photos */
            result = tvm_run_command (context, mount, autophoto_command, error);
        }
        else if (path_dcim != NULL)
        {
            /* when the user has decided to ignore photos/music, we don't want
             * to ask him again in autophoto and we don't want to mount the
             * device either... I guess? */
            result = TRUE;
        }

        g_free (autophoto_command);
        g_free (autoipod_command);
        g_free (path_dcim);
    }

    return result;
}

static gboolean
tvm_block_device_autophoto (TvmContext *context,
                            GMount     *mount,
                            GError    **error)
{
    gboolean autophoto;
    gboolean result = FALSE;
    gchar   *autophoto_command;
    gint     response;

    g_return_val_if_fail (context != NULL, FALSE);
    g_return_val_if_fail (G_IS_MOUNT (mount), FALSE);
    g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

    /* check if autophoto support is enabled */
    autophoto = xfconf_channel_get_bool (context->channel, "/autophoto/enabled", FALSE);
    if (autophoto)
    {
        autophoto_command = xfconf_channel_get_string (context->channel,
                            "/autophoto/command", NULL);
        if (autophoto_command != NULL && *autophoto_command != '\0')
        {
            /* check if we have any photos on the volume */
            if (tvm_file_test (mount, "dcim", G_FILE_TEST_IS_DIR))
            {
                /* ask the user to import photos */
                response = tvm_prompt (context, "camera-photo", _("Photo Import"),
                                       _("A photo card has been detected"),
                                       _("There are photos on the card. Would you like to "
                                         "add these photos to your album?"),
                                       _("Ig_nore"), GTK_RESPONSE_CANCEL,
                                       _("Import _Photos"), TVM_RESPONSE_PHOTOS,
                                       NULL);

                if (response == TVM_RESPONSE_PHOTOS)
                {
                    /* run the preferred photo application */
                    result = tvm_run_command (context, mount, autophoto_command, error);
                }
                else
                {
                    /* pretend that we handled the device */
                    result = TRUE;
                }
            }
        }

        g_free (autophoto_command);
    }

    return result;
}

static gboolean
tvm_block_device_autorun (TvmContext *context,
                          GMount     *mount,
                          GError    **error)
{
    struct stat statb_mount_point;
    struct stat statb_autoopen;
    gboolean    autoopen;
    gboolean    autoplay;
    gboolean    autorun;
    gboolean    result = FALSE;
    GError     *err = NULL;
    GFile      *mount_point;
    gchar     **argv;
    gchar      *autoplay_command;
    gchar      *message;
    gchar      *mount_path;
    gchar      *path_autoopen;
    gchar      *wine;
    gchar       line[1024];
    guint       n;
    FILE       *fp;
    gint        response;

    g_return_val_if_fail (context != NULL, FALSE);
    g_return_val_if_fail (G_IS_MOUNT (mount), FALSE);
    g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

    /* check if autoplaying video disks is enabled */
    autoplay = xfconf_channel_get_bool (context->channel,
                                        "/autoplay-video-cds/enabled", FALSE);
    if (autoplay)
    {
        /* check if we have a video disk here */
        if (tvm_file_test (mount, "vcd", G_FILE_TEST_IS_DIR)
                || tvm_file_test (mount, "video_ts", G_FILE_TEST_IS_DIR)
                || tvm_file_test (mount, "bdmv", G_FILE_TEST_IS_DIR))
        {
            /* determine the autoplay command for video disks */
            autoplay_command = xfconf_channel_get_string (context->channel,
                               "/autoplay-video-cds/command",
                               "parole");

            /* try to spawn the preferred video disk player */
            result = tvm_run_command (context, mount, autoplay_command, &err);

            /* free the command string */
            g_free (autoplay_command);

            /* forward errors to the caller */
            if (err != NULL)
                g_propagate_error (error, err);

            /* return success/failure to the caller */
            return result;
        }
    }

    /* check if autorun is enabled */
    autorun = xfconf_channel_get_bool (context->channel, "/autorun/enabled", FALSE);
    if (autorun)
    {
        /* Autostart files according to the Desktop Application Autostart
         * Specification */
        static const gchar *autorun_files[] = { ".autorun", "autorun", "autorun.sh" };
        for (n = 0; n < G_N_ELEMENTS (autorun_files); ++n)
        {
            /* check if one of the autorun files is present and executable */
            if (tvm_file_test (mount, autorun_files[n], G_FILE_TEST_IS_EXECUTABLE)
                    && tvm_file_test (mount, autorun_files[n], G_FILE_TEST_IS_REGULAR))
            {
                /* prompt the user to execute the file */
                message = g_strdup_printf (_("Would you like to allow \"%s\" to run?"),
                                           autorun_files[n]);
                response = tvm_prompt (context, "gnome-fs-executable",
                                       _("Auto-Run Confirmation"),
                                       _("Auto-Run capability detected"), message,
                                       _("Ig_nore"), GTK_RESPONSE_CANCEL,
                                       _("_Allow Auto-Run"), TVM_RESPONSE_AUTORUN,
                                       NULL);
                g_free (message);

                /* check if we should autorun */
                if (response == TVM_RESPONSE_AUTORUN)
                {
                    /* determine the mount point as a string */
                    mount_point = g_mount_get_root (mount);
                    mount_path = g_file_get_path (mount_point);
                    g_object_unref (mount_point);

                    /* prepare argv to launch the autorun file */
                    argv = g_new0 (gchar *, 2);
                    argv[0] = g_build_filename (mount_path, autorun_files[n], NULL);
                    argv[1] = NULL;

                    /* try to launch the autorun file */
                    result = g_spawn_async (mount_path, argv, NULL, 0, NULL, NULL, NULL,
                                            &err);

                    /* free strings */
                    g_strfreev (argv);
                    g_free (mount_path);

                    if (err != NULL)
                        g_propagate_error (error, err);

                    return result;
                }
            }
        }

        /* check if wine is present */
        wine = g_find_program_in_path ("wine");
        if (wine != NULL)
        {
            /* check if we have an autorun.exe file */
            if (tvm_file_test (mount, "autorun.exe", G_FILE_TEST_IS_REGULAR))
            {
                /* prompt the user to execute this file */
                message = g_strdup_printf (_("Would you like to allow \"%s\" to run?"),
                                           "autorun.exe");
                response = tvm_prompt (context, "gnome-fs-executable",
                                       _("Auto-Run Confirmation"),
                                       _("Auto-Run capability detected"), message,
                                       _("Ig_nore"), GTK_RESPONSE_CANCEL,
                                       _("_Allow Auto-Run"), TVM_RESPONSE_AUTORUN,
                                       NULL);
                g_free (message);

                /* check if we should run autogen.exe */
                if (response == TVM_RESPONSE_AUTORUN)
                {
                    /* determine the mount point as a string */
                    mount_point = g_mount_get_root (mount);
                    mount_path = g_file_get_path (mount_point);
                    g_object_unref (mount_point);

                    /* prepare argv to launch the autorun file */
                    argv = g_new0 (gchar *, 3);
                    argv[0] = g_strdup (wine);
                    argv[1] = g_strdup ("autorun.exe");
                    argv[2] = NULL;

                    /* try to launch the autorun file */
                    result = g_spawn_async (mount_path, argv, NULL, 0, NULL, NULL, NULL,
                                            &err);

                    /* free strings */
                    g_strfreev (argv);
                    g_free (mount_path);

                    /* free path to wine */
                    g_free (wine);

                    if (err != NULL)
                        g_propagate_error (error, err);

                    return result;
                }
            }

            /* free path to wine */
            g_free (wine);
        }
    }

    /* check if autoopen support is enabled */
    autoopen = xfconf_channel_get_bool (context->channel, "/autoopen/enabled", FALSE);
    if (autoopen)
    {
        /* "Autoopen Files" (Desktop Application Autostart Specification) */
        static const gchar *autoopen_files[] = { ".autoopen", "autoopen" };
        for (n = 0; n < G_N_ELEMENTS (autoopen_files); ++n)
        {
            /* determine the moint point path */
            mount_point = g_mount_get_root (mount);
            mount_path = g_file_get_path (mount_point);
            g_object_unref (mount_point);

            /* check if one of the autoopen files is present */
            path_autoopen = g_build_filename (mount_path, autoopen_files[n], NULL);
            fp = fopen (path_autoopen, "r");
            g_free (path_autoopen);

            /* check if the file could be opened */
            if (G_UNLIKELY (fp != NULL))
            {
                /* read the first line of the file (MUST NOT be an absolute path) */
                if (fgets (line, sizeof (line), fp) != NULL && !g_path_is_absolute (line))
                {
                    /* determine the absolute path of the file */
                    path_autoopen = g_build_filename (mount_path, g_strstrip (line), NULL);

                    /* the file must exist on exactly this device */
                    if (stat (mount_path, &statb_mount_point) == 0
                            && stat (path_autoopen, &statb_autoopen) == 0
                            && S_ISREG (statb_autoopen.st_mode)
                            && (statb_autoopen.st_mode & 0111) == 0
                            && (statb_mount_point.st_dev == statb_autoopen.st_dev))
                    {
                        /* prompt the user whether to autoopen this file */
                        message = g_strdup_printf (_("Would you like to open \"%s\"?"),
                                                   autoopen_files[n]);
                        response = tvm_prompt (context, "gnome-fs-executable",
                                               _("Auto-Open Confirmation"),
                                               _("Auto-Open capability detected"), message,
                                               _("Ig_nore"), GTK_RESPONSE_CANCEL,
                                               _("_Open"), TVM_RESPONSE_AUTORUN,
                                               NULL);
                        g_free (message);

                        /* check if we should autoopen */
                        if (response == TVM_RESPONSE_AUTORUN)
                        {
                            /* prepare the command to autoopen */
                            argv = g_new (gchar *, 3);
                            argv[0] = g_strdup ("Thunar");
                            argv[1] = path_autoopen;
                            argv[2] = NULL;

                            /* let Thunar open the file */
                            result = g_spawn_async (mount_path, argv, NULL, 0, NULL, NULL,
                                                    NULL, &err);

                            /* cleanup */
                            g_free (path_autoopen);
                            fclose (fp);

                            /* free the mount point path */
                            g_free (mount_path);

                            if (err != NULL)
                                g_propagate_error (error, err);

                            return result;
                        }
                    }

                    /* cleanup */
                    g_free (path_autoopen);
                }

                /* close the file handle */
                fclose (fp);
            }

            /* free the mount point path */
            g_free (mount_path);
        }
    }

    return FALSE;
}

#endif


