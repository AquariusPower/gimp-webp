/**
 * gimp-webp - WebP Plugin for the GIMP
 * Copyright (C) 2016  Nathan Osman & Ben Touchette
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __WEBP_DIALOG_H__
#define __WEBP_DIALOG_H__

#include <gtk/gtk.h>

#include "config.h"
#include "webp-save.h"

GtkResponseType save_dialog(
        WebPSaveParams *params
#ifdef WEBP_0_5
      , gint32 image_ID
      , gint32 nLayers
#endif
);

#endif /* __WEBP_DIALOG_H__ */
