/*-
 * Copyright (c) 2010 Jannis Pohlmann <>
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

#ifndef __TVM_DEVICE_H__
#define __TVM_DEVICE_H__

#include <glib.h>

#include <tvm-context.h>

G_BEGIN_DECLS

/* callback function type for device handlers */
typedef void (*TvmDeviceHandlerCallback) (TvmContext *context);

/* function type for device handlers */
typedef void (*TvmDeviceHandlerFunc) (TvmContext *context);



void tvm_device_added            (TvmContext *context);
void tvm_device_handler_finished (TvmContext *context);

G_END_DECLS

#endif /* !__TVM_DEVICE_H__ */
