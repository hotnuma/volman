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

#ifndef __TVM_RUN_H__
#define __TVM_RUN_H__

#include <gio/gio.h>

#include <tvm-context.h>

G_BEGIN_DECLS

gboolean tvm_run_command        (TvmContext  *context,
                                 GMount      *mount,
                                 const gchar *command,
                                 GError     **error);
gboolean  tvm_run_burn_software (TvmContext  *context,
                                 GError     **error);
gboolean  tvm_run_cd_player     (TvmContext  *context,
                                 GError     **error);

G_END_DECLS

#endif /* !__TVM_RUN_H__ */
