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
#include <device.h>

#include <context.h>
#include <devblock.h>
#include "printlog.h"

typedef struct _DeviceHandler DeviceHandler;

static void _device_next_handler(TvmContext *context);

struct _DeviceHandler
{
    const gchar         *subsystem;
    DeviceHandlerFunc   func;
};

static DeviceHandler subsystem_handlers[] =
{
    {"block", device_block_added},
};

void device_added(TvmContext *context)
{
    g_return_if_fail(context != NULL);

#ifdef DEBUG
    g_debug("tvm_device_added:");
    const gchar *const *keys = g_udev_device_get_property_keys(context->device);
    for (n = 0; keys != NULL && keys[n] != NULL; ++n)
        g_debug("    %s = %s", keys[n], g_udev_device_get_property(context->device, keys[n]));
#endif

    const gchar *subsystem = g_udev_device_get_subsystem(context->device);

    // find all subsystem handlers for this subsystem
    for (gint n = G_N_ELEMENTS(subsystem_handlers) - 1; n >= 0; --n)
    {
        if (g_strcmp0(subsystem, subsystem_handlers[n].subsystem) == 0)
        {
            context->handlers = g_list_prepend(context->handlers,
                                               &subsystem_handlers[n]);
        }
    }

    if (!context->handlers)
    {
        printinfo("Device type \"%s\" not supported",
                  g_udev_device_get_property(context->device, "DEVNAME"));
        return;
    }

    int count = g_list_length(context->handlers);

    if (count != 1)
        printinfo("*** handlers count = %d", count);

    _device_next_handler(context);
}

static void _device_next_handler(TvmContext *context)
{
    g_return_if_fail(context != NULL);

    DeviceHandler *handler = context->handlers->data;
    context->handlers = g_list_delete_link(context->handlers, context->handlers);

    handler->func(context);
}

void device_cleanup(TvmContext *context)
{
    g_return_if_fail(context != NULL);

    tvm_context_free(context);
}


