/*
 * Copyright (C) 2011, Hewlett-Packard Development Company, L.P.
 *   Author: Sebastian Dröge <sebastian.droege@collabora.co.uk>, Collabora Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>

#include "gstomxh264dec.h"

GST_DEBUG_CATEGORY_STATIC (gst_omx_h264_dec_debug_category);
#define GST_CAT_DEFAULT gst_omx_h264_dec_debug_category
#ifdef __LINUX_MEDIA_NAS__
#define GST_OMX_H264_OUTPUT_WIDTH_MAX 4096
#define GST_OMX_H264_OUTPUT_WIDTH_MIN 240
#define GST_OMX_H264_OUTPUT_HEIGHT_MAX 2160
#define GST_OMX_H264_OUTPUT_HEIGHT_MIN 135
#define GST_OMX_H264_OUTPUT_TURBOMODE_DISABLE 0
#define GST_OMX_H264_OUTPUT_TURBOMODE_ENABLE 1
#endif

/* prototypes */
static gboolean gst_omx_h264_dec_is_format_change (GstOMXVideoDec * dec,
    GstOMXPort * port, GstVideoCodecState * state);
static gboolean gst_omx_h264_dec_set_format (GstOMXVideoDec * dec,
    GstOMXPort * port, GstVideoCodecState * state);
#ifdef __LINUX_MEDIA_NAS__
static void gst_omx_h264_dec_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_omx_h264_dec_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void gst_omx_h264_dec_set_scaling (GstOMXVideoDec * self);
static void gst_omx_h264_dec_set_turboMode (GstOMXVideoDec * dec);
#endif

enum
{
  PROP_0,
#ifdef __LINUX_MEDIA_NAS__
  PROP_WIDTH,
  PROP_HEIGHT,
  PROP_FPS,
  PROP_TURBOMODE,
  PROP_AUTORESIZE
#endif
};

/* class initialization */

#define DEBUG_INIT \
  GST_DEBUG_CATEGORY_INIT (gst_omx_h264_dec_debug_category, "omxh264dec", 0, \
      "debug category for gst-omx video decoder base class");

G_DEFINE_TYPE_WITH_CODE (GstOMXH264Dec, gst_omx_h264_dec,
    GST_TYPE_OMX_VIDEO_DEC, DEBUG_INIT);

static void
gst_omx_h264_dec_class_init (GstOMXH264DecClass * klass)
{
  GstOMXVideoDecClass *videodec_class = GST_OMX_VIDEO_DEC_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  videodec_class->is_format_change =
      GST_DEBUG_FUNCPTR (gst_omx_h264_dec_is_format_change);
  videodec_class->set_format = GST_DEBUG_FUNCPTR (gst_omx_h264_dec_set_format);

  videodec_class->cdata.default_sink_template_caps = "video/x-h264, "
      "parsed=(boolean) true, "
      "alignment=(string) au, "
      "stream-format=(string) byte-stream, "
      "width=(int) [1,MAX], " "height=(int) [1,MAX]";

  gst_element_class_set_static_metadata (element_class,
      "OpenMAX H.264 Video Decoder",
      "Codec/Decoder/Video",
      "Decode H.264 video streams",
      "Sebastian Dröge <sebastian.droege@collabora.co.uk>");

#ifdef __LINUX_MEDIA_NAS__
  gobject_class->set_property = gst_omx_h264_dec_set_property;
  gobject_class->get_property = gst_omx_h264_dec_get_property;

  g_object_class_install_property (gobject_class, PROP_WIDTH,
      g_param_spec_uint ("width", "width",
          "video width of decode output port",
          GST_OMX_H264_OUTPUT_WIDTH_MIN, GST_OMX_H264_OUTPUT_WIDTH_MAX,
          GST_OMX_H264_OUTPUT_WIDTH_MIN,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_HEIGHT,
      g_param_spec_uint ("height", "height",
          "video height of decode output port",
          GST_OMX_H264_OUTPUT_HEIGHT_MIN, GST_OMX_H264_OUTPUT_HEIGHT_MAX,
          GST_OMX_H264_OUTPUT_HEIGHT_MIN,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_FPS,
      g_param_spec_uint ("fps", "fps",
          "video fps of decode output port",
          GST_OMX_OUTPUT_FPS_MIN, GST_OMX_OUTPUT_FPS_MAX,
          GST_OMX_OUTPUT_FPS_MIN, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_TURBOMODE,
      g_param_spec_uint ("turboMode", "turboMode",
          "Speedup decode performance. Suggest enabling on 4k2k case",
          GST_OMX_H264_OUTPUT_TURBOMODE_DISABLE,
          GST_OMX_H264_OUTPUT_TURBOMODE_ENABLE,
          GST_OMX_H264_OUTPUT_TURBOMODE_DISABLE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

    g_object_class_install_property (gobject_class, PROP_AUTORESIZE,
      g_param_spec_string ("autoResize", "autoResize",
          "video output resolution: FHD, HD, SD, NONE",
          GST_OMX_OUTPUT_AUTORISIZE_DAFAULT,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
#endif

  gst_omx_set_default_role (&videodec_class->cdata, "video_decoder.avc");
}

static void
gst_omx_h264_dec_init (GstOMXH264Dec * self)
{
#ifdef __LINUX_MEDIA_NAS__
  self->width = GST_OMX_OUTPUT_WIDTH_INVALID;
  self->height = GST_OMX_OUTPUT_HEIGHT_INVALID;
  self->fps = GST_OMX_OUTPUT_FPS_INVALID;
  self->turboMode = GST_OMX_H264_OUTPUT_TURBOMODE_DISABLE;
  strcpy(self->autoResize, GST_OMX_OUTPUT_AUTORISIZE_DAFAULT);
#endif
}

static gboolean
gst_omx_h264_dec_is_format_change (GstOMXVideoDec * dec,
    GstOMXPort * port, GstVideoCodecState * state)
{
  GstCaps *old_caps = NULL;
  GstCaps *new_caps = state->caps;
  GstStructure *old_structure, *new_structure;
  const gchar *old_profile, *old_level, *new_profile, *new_level;

  if (dec->input_state) {
    old_caps = dec->input_state->caps;
  }

  if (!old_caps) {
    return FALSE;
  }

  old_structure = gst_caps_get_structure (old_caps, 0);
  new_structure = gst_caps_get_structure (new_caps, 0);
  old_profile = gst_structure_get_string (old_structure, "profile");
  old_level = gst_structure_get_string (old_structure, "level");
  new_profile = gst_structure_get_string (new_structure, "profile");
  new_level = gst_structure_get_string (new_structure, "level");

  if (g_strcmp0 (old_profile, new_profile) != 0
      || g_strcmp0 (old_level, new_level) != 0) {
    return TRUE;
  }

  return FALSE;
}

static gboolean
gst_omx_h264_dec_set_format (GstOMXVideoDec * dec, GstOMXPort * port,
    GstVideoCodecState * state)
{
  gboolean ret;
  OMX_PARAM_PORTDEFINITIONTYPE port_def;
#ifdef __LINUX_MEDIA_NAS__
  GstOMXH264Dec *self = GST_OMX_H264_DEC (dec);

  if(self->fps != 0){
    state->info.fps_n = self->fps * state->info.fps_d;
  }

#endif

  gst_omx_port_get_port_definition (port, &port_def);

  port_def.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
  ret = gst_omx_port_update_port_definition (port, &port_def) == OMX_ErrorNone;

#ifdef __LINUX_MEDIA_NAS__
  gst_omx_dec_calculate_output_resolution(self->autoResize, &self->width, &self->height);
  gst_omx_h264_dec_set_scaling (dec);
  gst_omx_h264_dec_set_turboMode (dec);
#endif

  return ret;
}

#ifdef __LINUX_MEDIA_NAS__
static void
gst_omx_h264_dec_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstOMXH264Dec *self = GST_OMX_H264_DEC (object);

  GST_LOG_OBJECT (self, "[%s] set_property <<<<<, property id:%u\n",
      __FUNCTION__, prop_id);

  switch (prop_id) {
    case PROP_WIDTH:
    {
      guint32 guVal = g_value_get_uint (value);

      if (guVal >= GST_OMX_H264_OUTPUT_WIDTH_MIN
          && guVal <= GST_OMX_H264_OUTPUT_WIDTH_MAX) {
        self->width = guVal;
      } else {
        GST_FIXME_OBJECT (self, "[%s] Not in range\n", __FUNCTION__);
      }

      break;
    }
    case PROP_HEIGHT:
    {
      guint32 guVal = g_value_get_uint (value);

      if (guVal >= GST_OMX_H264_OUTPUT_HEIGHT_MIN
          && guVal <= GST_OMX_H264_OUTPUT_HEIGHT_MAX) {
        self->height = guVal;
      } else {
        GST_FIXME_OBJECT (self, "[%s] Not in range\n", __FUNCTION__);
      }

      break;
    }
    case PROP_FPS:
    {
      guint32 guVal = g_value_get_uint (value);

      if (guVal >= GST_OMX_OUTPUT_FPS_MIN && guVal <= GST_OMX_OUTPUT_FPS_MAX) {
        self->fps = guVal;
      } else {
        GST_FIXME_OBJECT (self, "[%s] Not in range\n", __FUNCTION__);
      }

      break;
    }
    case PROP_TURBOMODE:
    {
      guint32 guVal = g_value_get_uint (value);

      if (guVal >= GST_OMX_H264_OUTPUT_TURBOMODE_DISABLE
          && guVal <= GST_OMX_H264_OUTPUT_TURBOMODE_ENABLE) {
        self->turboMode = guVal;
      } else {
        GST_FIXME_OBJECT (self, "[%s] Not in range\n", __FUNCTION__);
      }
      break;
    }
    case PROP_AUTORESIZE:
    {
      const gchar *gstrVal = g_value_get_string (value);

      if (strcmp(gstrVal, "FHD") == 0) {
        strcpy(self->autoResize, "FHD");
      }
      else if(strcmp(gstrVal, "HD") == 0){
        strcpy(self->autoResize, "HD");
      }
      else if(strcmp(gstrVal, "SD") == 0){
        strcpy(self->autoResize, "SD");
      }
      else if(strcmp(gstrVal, "NONE") == 0){
        strcpy(self->autoResize, "NONE");
      } else {
        GST_FIXME_OBJECT (self, "[%s] Not in range\n", __FUNCTION__);
      }
      break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_omx_h264_dec_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  GstOMXH264Dec *self = GST_OMX_H264_DEC (object);

  switch (prop_id) {
    case PROP_WIDTH:
      g_value_set_uint (value, self->width);
      break;
    case PROP_HEIGHT:
      g_value_set_uint (value, self->height);
      break;
    case PROP_FPS:
      g_value_set_uint (value, self->fps);
      break;
    case PROP_TURBOMODE:
      g_value_set_uint (value, self->turboMode);
      break;
    case PROP_AUTORESIZE:
      g_value_set_string (value, self->autoResize);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_omx_h264_dec_set_scaling (GstOMXVideoDec * dec)
{
  GstOMXH264Dec *self = GST_OMX_H264_DEC (dec);
  guint32 param[4];

  if ((self->width != GST_OMX_OUTPUT_WIDTH_INVALID
          && self->height != GST_OMX_OUTPUT_HEIGHT_INVALID)
      || self->fps != GST_OMX_OUTPUT_FPS_INVALID) {
    param[0] = self->width;
    param[1] = self->height;
    param[2] = self->fps;
    if(strcmp(self->autoResize, "NONE") != 0) {
        param[3] = 1;
    } else {
        param[3] = 0;
    }
  } else {
    param[0] = GST_OMX_OUTPUT_WIDTH_INVALID;
    param[1] = GST_OMX_OUTPUT_HEIGHT_INVALID;
    param[2] = GST_OMX_OUTPUT_FPS_INVALID;
    param[3] = 0;
  }

  GST_OBJECT_LOCK (self);

  if (dec->dec != NULL) {
    OMX_ERRORTYPE err;

    err =
        gst_omx_component_set_parameter (dec->dec,
        OMX_realtek_android_index_notifyVeScaling, &param);

    if (err != OMX_ErrorNone) {
      GST_ERROR_OBJECT (self, "Failed to set video width/height: %s (0x%08x)",
          gst_omx_error_to_string (err), err);
    }
  }

  GST_OBJECT_UNLOCK (self);

}

static void
gst_omx_h264_dec_set_turboMode (GstOMXVideoDec * dec)
{
  GstOMXH264Dec *self = GST_OMX_H264_DEC (dec);

  GST_OBJECT_LOCK (self);

  if (dec->dec != NULL) {
    OMX_ERRORTYPE err;

    err =
        gst_omx_component_set_parameter (dec->dec,
        OMX_realtek_android_index_skipNonRefFrame, &self->turboMode);

    if (err != OMX_ErrorNone) {
      GST_ERROR_OBJECT (self, "Failed to set video turbo mode: %s (0x%08x)",
          gst_omx_error_to_string (err), err);
    }
  }

  GST_OBJECT_UNLOCK (self);

}
#endif
