/*
 * Copyright (C) 2011, Hewlett-Packard Development Company, L.P.
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

#include "gstomxjpegenc.h"

#ifdef USE_OMX_TARGET_RPI
#include <OMX_Broadcom.h>
#include <OMX_Index.h>
#endif

GST_DEBUG_CATEGORY_STATIC (gst_omx_jpeg_enc_debug_category);
#define GST_CAT_DEFAULT gst_omx_jpeg_enc_debug_category

/* prototypes */
static gboolean gst_omx_jpeg_enc_set_format (GstOMXImageEnc * enc,
    GstOMXPort * port, GstVideoCodecState * state);
static GstCaps *gst_omx_jpeg_enc_get_caps (GstOMXImageEnc * enc,
    GstOMXPort * port, GstVideoCodecState * state);
static GstFlowReturn gst_omx_jpeg_enc_handle_output_frame (GstOMXImageEnc *
    self, GstOMXPort * port, GstOMXBuffer * buf, GstVideoCodecFrame * frame);
static gboolean gst_omx_jpeg_enc_flush (GstVideoEncoder * enc);
static gboolean gst_omx_jpeg_enc_stop (GstVideoEncoder * enc);

enum
{
  PROP_0
};

/* class initialization */

#define DEBUG_INIT \
  GST_DEBUG_CATEGORY_INIT (gst_omx_jpeg_enc_debug_category, "omxjpegenc", 0, \
      "debug category for gst-omx video encoder base class");

#define parent_class gst_omx_jpeg_enc_parent_class
G_DEFINE_TYPE_WITH_CODE (GstOMXJPEGEnc, gst_omx_jpeg_enc,
    GST_TYPE_OMX_IMAGE_ENC, DEBUG_INIT);

static void
gst_omx_jpeg_enc_class_init (GstOMXJPEGEncClass * klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);
  GstVideoEncoderClass *basevideoenc_class = GST_VIDEO_ENCODER_CLASS (klass);
  GstOMXImageEncClass *videoenc_class = GST_OMX_IMAGE_ENC_CLASS (klass);

  videoenc_class->set_format = GST_DEBUG_FUNCPTR (gst_omx_jpeg_enc_set_format);
  videoenc_class->get_caps = GST_DEBUG_FUNCPTR (gst_omx_jpeg_enc_get_caps);

  basevideoenc_class->flush = gst_omx_jpeg_enc_flush;
  basevideoenc_class->stop = gst_omx_jpeg_enc_stop;

  videoenc_class->cdata.default_src_template_caps = "image/jpeg, "
      "width=(int) [ 16, 4096 ], " "height=(int) [ 16, 4096 ]";
  videoenc_class->handle_output_frame =
      GST_DEBUG_FUNCPTR (gst_omx_jpeg_enc_handle_output_frame);

  gst_element_class_set_static_metadata (element_class,
      "OpenMAX JPEG Image Encoder",
      "Codec/Encoder/Image",
      "Encode images in JPEG format",
      "Realtek");

  gst_omx_set_default_role (&videoenc_class->cdata, "image_encoder.jpg");
}

static void
gst_omx_jpeg_enc_init (GstOMXJPEGEnc * self)
{
}

static gboolean
gst_omx_jpeg_enc_flush (GstVideoEncoder * enc)
{
  return GST_VIDEO_ENCODER_CLASS (parent_class)->flush (enc);
}

static gboolean
gst_omx_jpeg_enc_stop (GstVideoEncoder * enc)
{
  return GST_VIDEO_ENCODER_CLASS (parent_class)->stop (enc);
}

static gboolean
gst_omx_jpeg_enc_set_format (GstOMXImageEnc * enc, GstOMXPort * port,
    GstVideoCodecState * state)
{
  GstOMXJPEGEnc *self = GST_OMX_JPEG_ENC (enc);
  OMX_ERRORTYPE err;
  OMX_PARAM_PORTDEFINITIONTYPE port_def;
  gst_omx_port_get_port_definition (GST_OMX_IMAGE_ENC (self)->enc_out_port,
      &port_def);
  port_def.format.image.eCompressionFormat = OMX_IMAGE_CodingJPEG;
  err =
      gst_omx_port_update_port_definition (GST_OMX_IMAGE_ENC
      (self)->enc_out_port, &port_def);
  if (err != OMX_ErrorNone)
    return FALSE;

  return TRUE;
}

static GstCaps *
gst_omx_jpeg_enc_get_caps (GstOMXImageEnc * enc, GstOMXPort * port,
    GstVideoCodecState * state)
{
  GstCaps *caps;
  caps = gst_caps_new_empty_simple ("image/jpeg");

  return caps;
}

static GstFlowReturn
gst_omx_jpeg_enc_handle_output_frame (GstOMXImageEnc * enc, GstOMXPort * port,
    GstOMXBuffer * buf, GstVideoCodecFrame * frame)
{
  return
      GST_OMX_IMAGE_ENC_CLASS
      (gst_omx_jpeg_enc_parent_class)->handle_output_frame (enc, port, buf,
      frame);
}