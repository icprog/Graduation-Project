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
 * \brief flac����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-28 �Ž���, �������ļ�
 * \endinternal
 */

/*******************************************************************************
  ͷ�ļ�����
*******************************************************************************/

#include "apollo.h"
#include "flac_decoder.h"

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_imx28_sound.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_dirent.h"
#include "io/sys/aw_stat.h"
#include "player_global.h"
#include "flacdecoder.h"
#include "resource.h"

/*******************************************************************************
  �궨��
*******************************************************************************/

#define READBUF_SIZE         (1024 * 50) /**< \brief ��ȡ��������С */
#define DECBUF_SIZE          (1024 * 25) /**< \brief ���뻺������С */
#define OUTBUF_SIZE          (1024 * 50) /**< \brief �����������С */

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

static uint8_t g_flac_read_buf[READBUF_SIZE];  /**< \brief ��ȡ������ */
static uint8_t g_flac_dec_buf0[DECBUF_SIZE];   /**< \brief ���뻺����0 */
static uint8_t g_flac_dec_buf1[DECBUF_SIZE];   /**< \brief ���뻺����1 */
static uint8_t g_flac_out_buf[OUTBUF_SIZE];    /**< \brief ��������� */


/*******************************************************************************
  ���غ�������
*******************************************************************************/

/**
 * \brief ����flac����
 *
 * \param[in]  file_data  flac�ļ�����
 * \param[in]  lenth      �ļ�����
 * \param[out] p_flac_ctrl ��ʼ����flac�ṹ��
 *
 * \retval AW_OK      �����ɹ�
 * \retval -AW_ERROR  ����ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t flac_decode_init (const char  *p_name,
                           flac_ctrl_t *p_flac_ctrl,
                           FLACContext *p_flac)
{
    flac_tag_t      *p_flac_tag_t;
    md_block_head_t *p_flac_block_head;
    uint8_t          end_of_meta_data = 0; /* ���һ��metadata��� */
    uint8_t          res;
    uint8_t          buf[512];
    uint32_t         br;
    int              block_length;
    int              fd;
    int              size;

    if ((NULL == p_flac) || (NULL == p_flac_ctrl)) {
        return -AW_EINVAL;
    }

    memset(p_flac_ctrl, 0, sizeof(flac_ctrl_t));

    /* ���ļ� */
    fd = aw_open(p_name, O_RDONLY, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_ERROR;
    }

    /* ��ȡ�ļ���С */
    if ((size = file_size_get(fd)) < 0) {
        AW_ERRF(("file size get error!\n"));
        return -AW_ERROR;
    }

    /* ��ȡ�ļ� */
    br = aw_read(fd, buf, 4);
    p_flac_tag_t = (flac_tag_t*)buf;
    if (strncmp("fLaC", (char*)p_flac_tag_t->id, 4) != 0)
    {
        goto fail;
    }
    while(!end_of_meta_data)
    {
        if (aw_read(fd, buf, 4) != 4) {
            break;
        }
        p_flac_block_head = (md_block_head_t *)buf;

        /* �ж��ǲ������һ��block */
        end_of_meta_data = p_flac_block_head->head & 0X80;

        /* �õ����С */
        block_length = ((uint32_t)p_flac_block_head->size[0] << 16) |
                       ((uint16_t)p_flac_block_head->size[1] << 8) |
                       (p_flac_block_head->size[2]);

        /* head���7λΪ0,���ʾ��STREAMINFO�� */
        if ((p_flac_block_head->head & 0x7f) == 0)
        {
            if (aw_read(fd, buf, block_length) != block_length) {
                break;
            }

            /* ��С���С */
            p_flac->min_blocksize = ((uint16_t)buf[0] << 8) | buf[1];

            /* �����С */
            p_flac->max_blocksize = ((uint16_t)buf[2] << 8) | buf[3];

            /* ��С֡��С */
            p_flac->min_framesize = ((uint32_t)buf[4] << 16) |
                                    ((uint16_t)buf[5] << 8) |
                                    buf[6];

            /* ���֡��С */
            p_flac->max_framesize = ((uint32_t)buf[7] << 16) |
                                    ((uint16_t)buf[8] << 8) |
                                    buf[9];

            /* ������ */
            p_flac->samplerate = ((uint32_t)buf[10] << 12) |
                                 ((uint16_t)buf[11] << 4) |
                                 ((buf[12] & 0xf0) >> 4);

            /* ��Ƶͨ���� */
            p_flac->channels = ((buf[12]&0x0e) >> 1) + 1;

            /* ����λ�� */
            p_flac->bps = ((((uint16_t)buf[12] & 0x01) << 4) |
                          ((buf[13] & 0xf0) >> 4)) + 1;

            /* һ���������ܲ����� */
            p_flac->totalsamples = ((uint32_t)buf[14] << 24) |
                                   ((uint32_t)buf[15] << 16) |
                                   ((uint16_t)buf[16] << 8) |
                                   buf[17];

            p_flac_ctrl->sample_rate = p_flac->samplerate;

            /* �õ���ʱ�� */
            p_flac_ctrl->total_time = (double)p_flac->totalsamples /
                                      p_flac->samplerate *
                                      1000;
        } else {

            /* ��������֡�Ĵ��� */
            if (aw_lseek(fd, block_length, SEEK_CUR) <0) {
                AW_LOGF(("file %s lseek failed\n", fd));
                goto fail;
            }
        }
    }

    if(p_flac_ctrl->total_time)
    {
        /* PCM���������(*2,��ʾ2��������������) */
        p_flac_ctrl->out_samples = p_flac->max_blocksize * 2;

        p_flac_ctrl->nchannels = p_flac->channels;

        /* ����λ��(16/24/32) */
        p_flac_ctrl->bps = p_flac->bps;

        /* FLAC����֡��ʼ�ĵ�ַ */
        p_flac_ctrl->data_start = aw_lseek(fd, 0, SEEK_CUR);

        /* �õ�FLAC��λ�� */
        p_flac_ctrl->bit_rate = ((size - p_flac_ctrl->data_start) * 8.0) /
                                p_flac_ctrl->total_time *
                                1000;
        p_flac_ctrl->file_size = size;
    } else {
        goto fail;    /* ��ʱ��Ϊ0 �������flac�ļ� */
    }

    aw_close(fd);
    return AW_OK;

fail:
    aw_close(fd);
    return -AW_ERROR;
}

/**
 * \brief ����flac�ļ�
 *
 * \param[in]  p_name      �ļ���
 * \param[out] p_flac_ctrl ��ʼ����flac�ṹ��
 *
 * \retval AW_OK      �����ɹ�
 * \retval -AW_ERROR  ����ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t get_flac_info (const char *p_name, flac_ctrl_t *p_flac_ctrl)
{
    FLACContext flac;

    return flac_decode_init(p_name, p_flac_ctrl, &flac);
}

/**
 * \brief ����flac�ļ�
 *
 * \param[in] p_name �ļ���
 *
 * \retval AW_OK      ���ųɹ�
 * \retval -AW_ERROR  ����ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t flac_play (const char *p_name)
{
    int         size         = 0;
    int         bytes_left   = 0;
    int         consumed     = 0;
    int         i            = 0;
    double      data_idx     = 0;
    size_t      len          = 0;
    uint8_t     res          = 0;
    uint32_t    data_set_idx = 0;
    uint32_t    out_data_idx = 0;
    flac_ctrl_t flac_ctrl;
    FLACContext flac;

    if (NULL == p_name) {
        return -AW_EINVAL;
    }

    memset(&flac, 0, sizeof(FLACContext));

    if (flac_decode_init(p_name, &flac_ctrl, &flac) != AW_OK) {
        AW_ERRF(("flac_decode_init failed\n"));
        goto fail;
    } else {
        g_play_ctrl.info.total_time    = flac_ctrl.total_time;
        g_play_ctrl.info.bits_per_word = flac_ctrl.bps;
        g_play_ctrl.info.channels      = flac_ctrl.nchannels;
        g_play_ctrl.info.samples_rate  = flac_ctrl.sample_rate;
        g_play_ctrl.info.bitrate       = flac_ctrl.bit_rate;
    }

    /* ���ļ� */
    g_play_ctrl.fd = aw_open(p_name, O_RDONLY, 0777);
    if (g_play_ctrl.fd < 0) {
        AW_ERRF(("open file error: %d\n", g_play_ctrl.fd));
        return -AW_ERROR;
    }

    /* ��ȡ�ļ���С */
    if ((size = file_size_get(g_play_ctrl.fd)) < 0) {
        AW_ERRF(("file size get error!\n"));
        return -AW_ERROR;
    }

    if ((READBUF_SIZE < flac.max_framesize) ||
        (DECBUF_SIZE < flac.max_blocksize * 4) ||
        (OUTBUF_SIZE < flac.max_blocksize * 8)){
        AW_ERRF(("flac buffer is too small\n"));
        goto fail;
    }

    if (flac.min_blocksize != flac.max_blocksize) {
        goto fail;
    }

    if (aw_lseek(g_play_ctrl.fd, flac_ctrl.data_start, SEEK_SET) <0) {
        AW_ERRF(("file %s lseek failed\n", g_play_ctrl.fd));
        goto fail;
    }

    /* ������Ƶ�豸 */
    aw_imx28_snd_mkdev(&g_snd_dev,                    /* ��Ƶ�豸 */
                       0,                             /* SAIF0 */
                       SOUND_DEVICE,                  /* ���� */
                       256,                           /* һ���жϴ������ݴ�С */
                       128,                           /* �ڲ����λ�������С */
                       16,                            /* λ�� */
                       flac_ctrl.nchannels,           /* ͨ���� */
                       flac_ctrl.sample_rate,         /* ������ */
                       AW_IMX28_SND_STREAM_PLAYBACK); /* ������Ƶ */

    /* ������ */
    if (aw_imx28_snd_open(&g_snd_dev) != AW_OK) {
        AW_ERRF(("aw_imx28_snd_open failed\n"));
        goto fail;
    }

    flac.decoded0 = (int32_t *)g_flac_dec_buf0;
    flac.decoded1 = (int32_t *)g_flac_dec_buf1;

    len = aw_read(g_play_ctrl.fd, g_flac_read_buf, flac.max_framesize);
    bytes_left = len;

    do {
        if ((g_play_ctrl.event != PLAY_EVENT_NULL) &&
            (g_play_ctrl.event != PLAY_EVENT_SET_TIME)) {
            break;
        }
        if (g_play_ctrl.paly_state != PALY_STATE_PLAY) {
            aw_mdelay(100);
        } else {
            if (PLAY_EVENT_SET_TIME == g_play_ctrl.event) {
                g_play_ctrl.event = PLAY_EVENT_NULL;
                out_data_idx = g_move_time /
                               1000.0 *
                               flac.samplerate *
                               (16 / 8) *
                               flac.channels;
                data_set_idx = ((double)g_move_time /
                               flac_ctrl.total_time *
                               size - flac_ctrl.data_start);
                data_set_idx += flac_ctrl.data_start;
                if (aw_lseek(g_play_ctrl.fd, data_set_idx, SEEK_SET) < 0) {
                    AW_ERRF(("file %s lseek failed\n", g_play_ctrl.fd));
                    goto fail;
                }
                if ((len = aw_read(g_play_ctrl.fd,
                                   g_flac_read_buf,
                                   flac.max_framesize)) <= 0) {
                    AW_ERRF(("read file error!\n"));
                    break;
                }

                bytes_left = flac_seek_frame(g_flac_read_buf, len, &flac);
                if (bytes_left >= 0) {
                    aw_lseek(g_play_ctrl.fd,
                             -flac.max_framesize + bytes_left,
                             SEEK_CUR);
                    bytes_left = aw_read(g_play_ctrl.fd,
                                         g_flac_read_buf,
                                         flac.max_framesize);
                } else {
                    printf("flac seek error:%d\r\n", bytes_left);
                }
            }

            if (24 == flac.bps) {
                res = flac_decode_frame24(&flac,
                                           g_flac_read_buf,
                                           bytes_left,
                                           (int32_t*)g_flac_out_buf);
                for (i = 0; i < flac.max_blocksize * 8; i++) {
                    if (i % 8 == 0) {
                        ((int32_t *)g_flac_out_buf)[i / 8] =
                                                   g_flac_out_buf[i + 1] |
                                                   g_flac_out_buf[i + 2] << 8 |
                                                   g_flac_out_buf[i + 5] << 16 |
                                                   g_flac_out_buf[i + 6] << 24;
                    }
                }

            } else {
                res = flac_decode_frame16(&flac,
                                           g_flac_read_buf,
                                           bytes_left,
                                           (int16_t*)g_flac_out_buf);
            }
            if (res != 0) {
                break;
            }

            consumed = flac.gb.index / 8;
            memmove(g_flac_read_buf,
                   &g_flac_read_buf[consumed],
                    bytes_left - consumed);
            bytes_left -= consumed;
            len = aw_read(g_play_ctrl.fd,
                         &g_flac_read_buf[bytes_left],
                          flac.max_framesize - bytes_left);
            if (len > 0) {
                bytes_left += len;
            } else {
                break;
            }

            aw_imx28_snd_write(&g_snd_dev,
                               g_flac_out_buf,
                               flac.max_blocksize * 4);

            out_data_idx += flac.max_blocksize * 4;

            data_idx = aw_lseek(g_play_ctrl.fd, 0, SEEK_CUR);
            flac_ctrl.current_time = 1000.0 *
                                     out_data_idx /
                                     flac.samplerate /
                                     (16 / 8) /
                                     flac.channels;
            g_play_ctrl.current_time = flac_ctrl.current_time;
        }
    } while (len > 0);

    aw_imx28_snd_close(&g_snd_dev);
    aw_close(g_play_ctrl.fd);
    return AW_OK;

fail:
    aw_imx28_snd_close(&g_snd_dev);
    aw_close(g_play_ctrl.fd);
    g_play_ctrl.paly_state = PALY_STATE_PAUSE;
    return -AW_ERROR;
}

/* end of file */
