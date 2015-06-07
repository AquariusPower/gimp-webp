/**
 * gimp-webp - WebP Plugin for the GIMP
 * Copyright (C) 2015  Nathan Osman
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

#include <stdio.h>
#include <stdlib.h>
#include <libgimp/gimp.h>

#include "write-webp.h"

/* Utility method for returning an error message from a WebP error code. */
const char * webp_error_string(WebPEncodingError error)
{
    switch(error)
    {
        case VP8_ENC_OK:                            return "no error";
        case VP8_ENC_ERROR_OUT_OF_MEMORY:           return "out of memory";
        case VP8_ENC_ERROR_BITSTREAM_OUT_OF_MEMORY: return "not enough memory to flush bits";
        case VP8_ENC_ERROR_NULL_PARAMETER:          return "NULL parameter";
        case VP8_ENC_ERROR_INVALID_CONFIGURATION:   return "invalid configuration";
        case VP8_ENC_ERROR_BAD_DIMENSION:           return "bad image dimensions";
        case VP8_ENC_ERROR_PARTITION0_OVERFLOW:     return "partition is bigger than 512K";
        case VP8_ENC_ERROR_PARTITION_OVERFLOW:      return "partition is bigger than 16M";
        case VP8_ENC_ERROR_BAD_WRITE:               return "unable to flush bytes";
        case VP8_ENC_ERROR_FILE_TOO_BIG:            return "file is larger than 4GiB";
        case VP8_ENC_ERROR_USER_ABORT:              return "user aborted encoding";
        case VP8_ENC_ERROR_LAST:                    return "list terminator";
        default:                                    return "unknown error";
    }
}

/* Handler for writing the contents to the file. */
int on_write(const uint8_t * data,
             size_t data_size,
             const WebPPicture * picture)
{
    /* Obtain a pointer to our current file and write the contents to it. */
    FILE ** file = (FILE **)picture->custom_ptr;
    return fwrite(data, data_size, 1, *file);
}

/* Handler for displaying encoding process. */
int on_progress(int percent,
                const WebPPicture * picture)
{
    return gimp_progress_update(percent / 100.0);
}

/* TODO: the code below needs more error checking. */

/* Writes the specified drawable to disk. */
int write_webp(const gchar * filename,
               gint drawable_id,
               WebPConfig * config)
{
    gint bpp;
    GimpDrawable * drawable;
    GimpPixelRgn region;
    long int raw_data_size;
    void * raw_data;
    WebPPicture picture;
    FILE * file;
    int stride;
    int status;

    gimp_progress_init_printf("Encoding %s", filename);

    /* Get the number of bytes-per-pixel (BPP). This tells
       us if we are dealing with an alpha channel or not. */
    bpp = gimp_drawable_bpp(drawable_id);

    /* Use the drawable ID to get the drawable and then
       the region which is the entire visible image. */
    drawable = gimp_drawable_get(drawable_id);
    gimp_pixel_rgn_init(&region,
                        drawable,
                        0, 0,
                        drawable->width,
                        drawable->height,
                        FALSE, FALSE);

    /* Prepare the WebPPicture structure for the encoding process later on. */
    WebPPictureInit(&picture);
    picture.use_argb      = config->lossless;
    picture.width         = drawable->width;
    picture.height        = drawable->height;
    picture.writer        = on_write;
    picture.custom_ptr    = &file;
    picture.progress_hook = on_progress;

    /* Calculate the stride. */
    stride = drawable->width * bpp;

    /* Determine exactly how much space we need to allocate for
       the contents of the image. Then allocate that amount. */
    raw_data_size = drawable->width * drawable->height * bpp;
    raw_data = malloc(raw_data_size);

    /* Ensure the allocation was successful. */
    if(!raw_data)
    {
        gimp_drawable_detach(drawable);
        return 0;
    }

    /* Now copy the image data to our array. */
    gimp_pixel_rgn_get_rect(&region,
                            (guchar *)raw_data,
                            0, 0,
                            drawable->width,
                            drawable->height);

    /* Detach the drawable. We have the image data now. */
    gimp_drawable_detach(drawable);

    /* Attempt to open the file we are writing to. */
    file = fopen(filename, "wb");
    if(!file)
    {
        free(raw_data);
        return 0;
    }

    /* Now perform the encoding procedure. */
    if(bpp == 3) status = WebPPictureImportRGB(&picture, raw_data, stride);
    else         status = WebPPictureImportRGBA(&picture, raw_data, stride);

    status = WebPEncode(config, &picture);

    /* Free the picture, raw image data, and close the file. */
    WebPPictureFree(&picture);
    free(raw_data);
    fclose(file);

    return status;
}
