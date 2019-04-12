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

#ifndef __GST_OMX_IMAGE_ENC_H__
#define __GST_OMX_IMAGE_ENC_H__

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideoencoder.h>

#include "gstomx.h"
#ifdef __LINUX_MEDIA_NAS__
#include <sys/time.h>
#endif

G_BEGIN_DECLS

#define GST_TYPE_OMX_IMAGE_ENC \
  (gst_omx_image_enc_get_type())
#define GST_OMX_IMAGE_ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_OMX_IMAGE_ENC,GstOMXImageEnc))
#define GST_OMX_IMAGE_ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_OMX_IMAGE_ENC,GstOMXImageEncClass))
#define GST_OMX_IMAGE_ENC_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_OMX_IMAGE_ENC,GstOMXImageEncClass))
#define GST_IS_OMX_IMAGE_ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_OMX_IMAGE_ENC))
#define GST_IS_OMX_IMAGE_ENC_CLASS(obj) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_OMX_IMAGE_ENC))

typedef struct _GstOMXImageEnc GstOMXImageEnc;
typedef struct _GstOMXImageEncClass GstOMXImageEncClass;

struct _GstOMXImageEnc
{
  GstVideoEncoder parent;

  /* < protected > */
  GstOMXComponent *enc;
  GstOMXPort *enc_in_port, *enc_out_port;

  /* < private > */
  GstVideoCodecState *input_state;
  /* TRUE if the component is configured and saw
   * the first buffer */
  gboolean started;

  GstClockTime last_upstream_ts;

  /* Draining state */
  GMutex drain_lock;
  GCond drain_cond;
  /* TRUE if EOS buffers shouldn't be forwarded */
  gboolean draining;

  GstFlowReturn downstream_flow_ret;
};

struct _GstOMXImageEncClass
{
  GstVideoEncoderClass parent_class;

  GstOMXClassData cdata;

  gboolean            (*set_format)          (GstOMXImageEnc * self, GstOMXPort * port, GstVideoCodecState * state);
  GstCaps            *(*get_caps)           (GstOMXImageEnc * self, GstOMXPort * port, GstVideoCodecState * state);
  GstFlowReturn       (*handle_output_frame) (GstOMXImageEnc * self, GstOMXPort * port, GstOMXBuffer * buffer, GstVideoCodecFrame * frame);
};

GType gst_omx_image_enc_get_type (void);

G_END_DECLS

#endif /* __GST_OMX_IMAGE_ENC_H__ */
