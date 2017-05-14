/*******************************************************************************
*                              Music Player
*                       ----------------------------
*                            Graduation Design
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.embedcontrol.com
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief mp3����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-17 �Ž���, �������ļ�
 * \endinternal
 */

/*******************************************************************************
  ͷ�ļ�����
*******************************************************************************/

#include "apollo.h"
#include "mp3_decoder.h"

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "aw_imx28_sound.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_dirent.h"
#include "io/sys/aw_stat.h"
#include "id3tag.h"
#include "mad.h"
#include "frame.h"
#include "player_global.h"
#include "resource.h"

/*******************************************************************************
  �궨��
*******************************************************************************/

#define READBUF_SIZE         (4000)          /**< \brief ��ȡ��������С */
#define OUTBUF_SIZE          (1152 * 2 * 4)  /**< \brief �����������С */

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

static uint8_t g_mp3_read_buf[READBUF_SIZE]; /**< \brief ��ȡ������ */
static uint8_t g_mp3_out_buf[OUTBUF_SIZE];   /**< \brief ��������� */

static mad_timer_t       g_play_time;        /**< \brief ��ǰ����ʱ�� */
static struct mad_header g_header;           /**< \brief mad_header�ṹ */
static struct mad_stream g_stream;           /**< \brief mad_stream�ṹ */
static mp3_info_t        g_mp3_info;         /**< \brief mp3��Ϣ�ṹ */

static uint32_t          g_err_count = 0;    /**< \brief ������� */
/*******************************************************************************
  ���غ�������
*******************************************************************************/

/**
 * \brief ��������ص�
 *
 * \param[in] p_data    �Զ������ݵ�ָ��
 * \param[out] p_stream mad_stream�ṹ
 *
 * \retval MAD_FLOW_CONTINUE ��������
 * \retval MAD_FLOW_STOP     ֹͣ����
 */
static enum mad_flow input_cb (void *p_data, struct mad_stream *p_stream)
{
    struct mp3_buffer *p_buffer      = p_data;
    uint32_t           lb            = 0;
    uint32_t           rb            = 0;
    uint32_t           frame_set_idx = 0;
    uint32_t           data_set_idx  = 0;

    if (!p_buffer->length) {
        return MAD_FLOW_STOP;
    }

    if ((g_play_ctrl.event != PLAY_EVENT_NULL) &&
       (g_play_ctrl.event != PLAY_EVENT_SET_TIME)) {
        return MAD_FLOW_STOP;
    }

    /* �����Ҫ��ͣ��������ǰ���� */
    while (g_play_ctrl.paly_state != PALY_STATE_PLAY) {
        aw_mdelay(100);
        if (PLAY_EVENT_STOP == g_play_ctrl.event) {
            return MAD_FLOW_STOP;
        }
    }

    if (PLAY_EVENT_STOP == g_play_ctrl.event) {
        return MAD_FLOW_STOP;
    }
    if (PLAY_EVENT_SET_TIME == g_play_ctrl.event) {
        g_play_ctrl.event = PLAY_EVENT_NULL;
        frame_set_idx = g_move_time / g_mp3_info.frame_time;
        data_set_idx = frame_set_idx * g_mp3_info.frame_size;
        data_set_idx += g_mp3_info.data_start;
        if (aw_lseek(g_play_ctrl.fd, data_set_idx, SEEK_SET) <0) {
            AW_ERRF(("file %s lseek failed\n", g_play_ctrl.fd));
            return MAD_FLOW_STOP;
        }
        mad_timer_set(&g_play_time,
                      g_move_time / 1000,
                      g_move_time % 1000,
                      MAD_UNITS_MILLISECONDS);

        lb = aw_read(g_play_ctrl.fd,
                     (void *)p_buffer->p_start,
                     p_buffer->length);
        if (p_buffer->length != lb) {
            return MAD_FLOW_STOP;
        }
        mad_stream_init(p_stream);
        AW_LOGF(("set to %d ms\n", g_move_time));
    } else {

        /* ��ȡ�ļ� */
        if (p_stream->this_frame && p_stream->next_frame) {
            rb = p_buffer->length -
                 (uint32_t)(p_stream->next_frame - p_stream->buffer);
            memmove((void *)p_stream->buffer, (void *)p_stream->next_frame, rb);
            lb = aw_read(g_play_ctrl.fd,
                         (void *)(p_stream->buffer + rb),
                         p_buffer->length - rb);
            if (p_buffer->length - rb != lb) {
                return MAD_FLOW_STOP;
            }
        } else {
            lb = aw_read(g_play_ctrl.fd,
                         (void *)p_buffer->p_start,
                         p_buffer->length);
            if (p_buffer->length != lb) {
                return MAD_FLOW_STOP;
            }
        }
    }

    /* ��ȡ�������ļ�֮���˳����� */
    if (0 == lb) {
        return MAD_FLOW_STOP;
    } else {
        p_buffer->length = lb + rb;
    }

    mad_stream_buffer(p_stream, p_buffer->p_start, p_buffer->length);

    return MAD_FLOW_CONTINUE;
}

/**
 * \brief ֡ͷ����ص�����
 *
 * \param[in] p_data   �Զ������ݵ�ָ��
 * \param[in] p_header mad_header�ṹ
 *
 * \retval MAD_FLOW_CONTINUE ��������
 */
static enum mad_flow header_cb (void *p_data, struct mad_header const *p_header)
{
    mad_timer_add(&g_play_time, p_header->duration);
    g_play_ctrl.current_time = mad_timer_count(g_play_time,
                                               MAD_UNITS_MILLISECONDS);

    return MAD_FLOW_CONTINUE;
}

/**
 * \brief ��libmad�������������ת��Ϊ16λPCM����
 *
 * \param[in] sample��ת��������
 *
 * \return 16λPCMλ����
 */
static inline signed int scale (mad_fixed_t sample)
{
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));

    /* clip */
    if (sample >= MAD_F_ONE) {
        sample = MAD_F_ONE - 1;
    } else if (sample < -MAD_F_ONE) {
        sample = -MAD_F_ONE;
    }

    /* quantize */
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}

/**
 * \brief ��������ص�
 *
 * \param[in] p_data   �Զ������ݵ�ָ��
 * \param[in] p_header ֡ͷ��Ϣ
 * \param[in] p_pcm    pcm����
 *
 * \retval MAD_FLOW_CONTINUE ��������
 */
static enum mad_flow output_cb (void                    *p_data,
                                struct mad_header const *p_header,
                                struct mad_pcm          *p_pcm)
{
    int16_t   sample   = 0;
    uint16_t *p_outbuf = (uint16_t *)g_mp3_out_buf;

    for (int i = 0; i < p_pcm->length; i++) {
        *(p_outbuf++) = (uint16_t)scale(p_pcm->samples[0][i]);
        if (2 == p_pcm->channels) {
            *(p_outbuf++) = (uint16_t)scale(p_pcm->samples[1][i]);
        }
    }

    if (1 == p_pcm->channels) {
        aw_imx28_snd_write(&g_snd_dev, g_mp3_out_buf, p_pcm->length * 2 * 1);
    } else if (2 == p_pcm->channels) {
        aw_imx28_snd_write(&g_snd_dev, g_mp3_out_buf, p_pcm->length * 2 * 2);
    } else {
        AW_ERRF(("samplerate:%d channels:%d length:%d\n",
        p_pcm->samplerate, p_pcm->channels, p_pcm->length));
    }

    return MAD_FLOW_CONTINUE;
}

/**
 * \brief ������Ϣ�ص�
 *
 * \param[in] p_data   �Զ������ݵ�ָ��
 * \param[in] p_stream mad_stream�ṹ
 * \param[in] p_frame  mad_frame�ṹ
 *
 * \retval MAD_FLOW_CONTINUE ��������
 * \retval MAD_FLOW_STOP     ֹͣ����
 * \retval MAD_FLOW_IGNORE   ���Ե�ǰ֡
 */
static enum mad_flow error_cb (void              *p_data,
                               struct mad_stream *p_stream,
                               struct mad_frame  *p_frame)
{
    struct mp3_buffer *p_buffer = p_data;
    signed long        tagsize  = 0;

    g_err_count++;
    if (g_err_count > 100) {
        return MAD_FLOW_STOP;
    }

    switch (p_stream->error) {

    case MAD_ERROR_BADDATAPTR:
        return MAD_FLOW_CONTINUE;

    case MAD_ERROR_LOSTSYNC:
        tagsize = id3_tag_query(p_stream->this_frame,
                                p_stream->bufend - p_stream->this_frame);
        if (tagsize > 0) {
            mad_stream_skip(p_stream, tagsize);
            AW_LOGF(("mad p_stream skip id3 size %d\n", tagsize));
            return MAD_FLOW_CONTINUE;
        }
        break;
    }

    if (MAD_ERROR_BADCRC == p_stream->error) {
        mad_frame_mute(p_frame);

        return MAD_FLOW_IGNORE;
    }

    AW_ERRF(("decoding error 0x%04x (%s) at byte offset %u\n",
             p_stream->error, mad_stream_errorstr(p_stream),
             p_stream->this_frame - p_buffer->p_start));

    return MAD_FLOW_CONTINUE;
}

/**
 * \brief ����mp3����
 *
 * \param[in] p_start ��ȡ�������׵�ַ
 * \param[in] length  ��ȡ����������
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
static aw_err_t decode (unsigned char const *p_start, unsigned long length)
{
    struct mp3_buffer  buffer;
    struct mad_decoder decoder;
    int                result;

    if (NULL == p_start) {
        return -AW_EINVAL;
    }

    buffer.p_start = p_start;
    buffer.length  = length;

    mad_decoder_init(&decoder,
                    &buffer,
                     input_cb,
                     header_cb,
                     0 /* filter */,
                     output_cb,
                     error_cb,
                     0 /* message */);

    result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

    mad_decoder_finish(&decoder);

    return result;
}

/**
 * \brief ����mp3�ļ�
 *
 * \param[in]  p_name     �ļ���
 * \param[out] p_wav_ctrl ��ʼ����mp3�ṹ��
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t mp3_info_get (const char *p_name, mp3_info_t *p_mp3_info)
{
    int         size      = 0;
    int         fd        = 0;
    signed long tagsize   = 0;
    uint8_t     is_tag_v1 = 0;

    if ((NULL == p_name) || (NULL == p_mp3_info)) {
        return -AW_EINVAL;
    }

    mad_stream_init(&g_stream);

    /* ���ļ� */
    fd = aw_open(p_name, O_RDONLY, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_ERROR;
    }

    /* ��ȡ�ļ���С */
    if ((size = file_size_get(fd)) < 0) {
        AW_ERRF(("%s size get error!\n", p_name));
        goto fail;
    }

    /* �ƶ����ļ�ĩβ */
    if (aw_lseek(fd, -128, SEEK_END) <0) {
        AW_ERRF(("file %s lseek failed\n", fd));
        goto fail;
    }

    /* ��ȡ�ļ� */
    if (aw_read(fd, g_mp3_read_buf, 128) != 128) {
        AW_ERRF(("read file error!\n"));
        goto fail;
    }

    tagsize = id3_tag_query(g_mp3_read_buf, 128);
    is_tag_v1 = (tagsize == 128) ? 1 : 0;

    /* �ƶ����ļ�ͷ */
    if (aw_lseek(fd, 0, SEEK_SET) <0) {
        AW_ERRF(("file %s lseek failed\n", fd));
        goto fail;
    }

    /* ��ȡ�ļ� */
    if (aw_read(fd, g_mp3_read_buf, 128) != 128) {
        AW_ERRF(("read file error!\n"));
        goto fail;
    }

    tagsize = id3_tag_query(g_mp3_read_buf, 128);

    /* �ƶ�������֡ */
    if (aw_lseek(fd, tagsize, SEEK_SET) <0) {
        AW_ERRF(("file %s lseek failed\n", fd));
        goto fail;
    }

    /* ��ȡ�ļ� */
    if (aw_read(fd, g_mp3_read_buf, 4096) != 4096) {
        AW_ERRF(("read file error!\n"));
        goto fail;
    }
    mad_stream_buffer(&g_stream, g_mp3_read_buf, 4096);
    if (mad_stream_sync(&g_stream) == -1) {
        goto fail;
    }
    if (decode_header(&g_header, &g_stream) == -1) {
        goto fail;
    }

    p_mp3_info->file_size    = size;
    p_mp3_info->current_time = 0;
    p_mp3_info->data_start   = tagsize;
    p_mp3_info->bit_rate     = g_header.bitrate;
    p_mp3_info->nchannels    = g_header.mode == MAD_MODE_SINGLE_CHANNEL ? 1 : 2;
    p_mp3_info->sample_rate  = g_header.samplerate;
    p_mp3_info->data_size    = size -
                               tagsize -
                               ((is_tag_v1 == 1) ? 128 : 0);

    p_mp3_info->frame_time   = 1000 * 1152.0 / g_header.samplerate;
    p_mp3_info->frame_size   = (g_header.bitrate * 1152.0 /
                               g_header.samplerate /
                               8);
    p_mp3_info->frame_num = p_mp3_info->data_size / p_mp3_info->frame_size;
    p_mp3_info->total_time = p_mp3_info->frame_num * p_mp3_info->frame_time;

    aw_close(fd);
    return AW_OK;

fail:
    aw_close(fd);
    return -AW_ERROR;
}

/**
 * \brief ����MP3�ļ�
 *
 * \param[in] file_name �ļ���
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t mp3_play (const char *p_name)
{
    int      size         = 0;
    int      recv         = 0;
    size_t   len          = 0;
    uint32_t data_counter = 0;

    if (NULL == p_name) {
        return -AW_EINVAL;
    }

    if (mp3_info_get(p_name, &g_mp3_info) != AW_OK) {
        AW_ERRF(("get %s info error: %d\n", p_name));
        return -AW_ERROR;
    }

    g_err_count = 0;
    g_play_time = mad_timer_zero;

    /* ���ļ� */
    g_play_ctrl.fd = aw_open(p_name, O_RDONLY, 0777);
    if (g_play_ctrl.fd < 0) {
        AW_ERRF(("open file error: %d\n", g_play_ctrl.fd));
        return -AW_ERROR;
    }

    /* ������Ƶ�豸 */
    aw_imx28_snd_mkdev(&g_snd_dev,                    /* ��Ƶ�豸 */
                       0,                             /* SAIF0 */
                       SOUND_DEVICE,                  /* ���� */
                       256,                           /* һ���жϴ������ݴ�С */
                       128,                           /* �ڲ����λ�������С */
                       16,                            /* λ�� */
                       g_mp3_info.nchannels,          /* ͨ���� */
                       g_mp3_info.sample_rate,        /* ������ */
                       AW_IMX28_SND_STREAM_PLAYBACK); /* ������Ƶ */

    /* ������ */
    if (aw_imx28_snd_open(&g_snd_dev) != AW_OK) {
        aw_close(g_play_ctrl.fd);
        return -AW_ENODEV;
    }

    recv = decode(g_mp3_read_buf, sizeof(g_mp3_read_buf));

    aw_imx28_snd_close(&g_snd_dev);

    aw_close(g_play_ctrl.fd);

    return recv;
}

/* end of file */
