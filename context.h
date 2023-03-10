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

#ifndef __TVM_CONTEXT_H__
#define __TVM_CONTEXT_H__

#include <gudev/gudev.h>
#include <gio/gio.h>

typedef struct _TvmContext TvmContext;

struct _TvmContext
{
    GVolumeMonitor *monitor;
    GUdevClient    *client;
    GUdevDevice    *device;
    GList          *handlers;
};

TvmContext* tvm_context_new(GUdevClient *client, GUdevDevice *device)
                            G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
void        tvm_context_free(TvmContext *context);
gboolean    tvm_context_run(gpointer user_data);

#endif // __TVM_CONTEXT_H__


