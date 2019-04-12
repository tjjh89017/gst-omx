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

#ifndef __GST_OMX_MPEG4_VIDEO_ENC_H__
#define __GST_OMX_MPEG4_VIDEO_ENC_H__

#include <gst/gst.h>
#include "gstomxvideoenc.h"

G_BEGIN_DECLS

#define GST_TYPE_OMX_MPEG4_VIDEO_ENC \
  (gst_omx_mpeg4_video_enc_get_type())
#define GST_OMX_MPEG4_VIDEO_ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_OMX_MPEG4_VIDEO_ENC,GstOMXMPEG4VideoEnc))
#define GST_OMX_MPEG4_VIDEO_ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_OMX_MPEG4_VIDEO_ENC,GstOMXMPEG4VideoEncClass))
#define GST_OMX_MPEG4_VIDEO_ENC_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_OMX_MPEG4_VIDEO_ENC,GstOMXMPEG4VideoEncClass))
#define GST_IS_OMX_MPEG4_VIDEO_ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_OMX_MPEG4_VIDEO_ENC))
#define GST_IS_OMX_MPEG4_VIDEO_ENC_CLASS(obj) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_OMX_MPEG4_VIDEO_ENC))

typedef struct _GstOMXMPEG4VideoEnc GstOMXMPEG4VideoEnc;
typedef struct _GstOMXMPEG4VideoEncClass GstOMXMPEG4VideoEncClass;

struct _GstOMXMPEG4VideoEnc
{
  GstOMXVideoEnc parent;
};

struct _GstOMXMPEG4VideoEncClass
{
  GstOMXVideoEncClass parent_class;
};

GType gst_omx_mpeg4_video_enc_get_type (void);

G_END_DECLS

#endif /* __GST_OMX_MPEG4_VIDEO_ENC_H__ */
