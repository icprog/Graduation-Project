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
 * \brief flac解码
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-28 张进科, 创建该文件
 * \endinternal
 */

/*******************************************************************************
  头文件包含
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
  宏定义
*******************************************************************************/

#define READBUF_SIZE         (1024 * 50) /**< \brief 读取缓冲区大小 */
#define DECBUF_SIZE          (1024 * 25) /**< \brief 解码缓冲区大小 */
#define OUTBUF_SIZE          (1024 * 50) /**< \brief 输出缓冲区大小 */

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

static uint8_t g_flac_read_buf[READBUF_SIZE];  /**< \brief 读取缓冲区 */
static uint8_t g_flac_dec_buf0[DECBUF_SIZE];   /**< \brief 解码缓冲区0 */
static uint8_t g_flac_dec_buf1[DECBUF_SIZE];   /**< \brief 解码缓冲区1 */
static uint8_t g_flac_out_buf[OUTBUF_SIZE];    /**< \brief 输出缓冲区 */


/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 解析flac数据
 *
 * \param[in]  file_data  flac文件数据
 * \param[in]  lenth      文件长度
 * \param[out] p_flac_ctrl 初始化的flac结构体
 *
 * \retval AW_OK      解析成功
 * \retval -AW_ERROR  解析失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t flac_decode_init (const char  *p_name,
                           flac_ctrl_t *p_flac_ctrl,
                           FLACContext *p_flac)
{
    flac_tag_t      *p_flac_tag_t;
    md_block_head_t *p_flac_block_head;
    uint8_t          end_of_meta_data = 0; /* 最后一个metadata标记 */
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

    /* 打开文件 */
    fd = aw_open(p_name, O_RDONLY, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_ERROR;
    }

    /* 获取文件大小 */
    if ((size = file_size_get(fd)) < 0) {
        AW_ERRF(("file size get error!\n"));
        return -AW_ERROR;
    }

    /* 读取文件 */
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

        /* 判断是不是最后一个block */
        end_of_meta_data = p_flac_block_head->head & 0X80;

        /* 得到块大小 */
        block_length = ((uint32_t)p_flac_block_head->size[0] << 16) |
                       ((uint16_t)p_flac_block_head->size[1] << 8) |
                       (p_flac_block_head->size[2]);

        /* head最低7位为0,则表示是STREAMINFO块 */
        if ((p_flac_block_head->head & 0x7f) == 0)
        {
            if (aw_read(fd, buf, block_length) != block_length) {
                break;
            }

            /* 最小块大小 */
            p_flac->min_blocksize = ((uint16_t)buf[0] << 8) | buf[1];

            /* 最大块大小 */
            p_flac->max_blocksize = ((uint16_t)buf[2] << 8) | buf[3];

            /* 最小帧大小 */
            p_flac->min_framesize = ((uint32_t)buf[4] << 16) |
                                    ((uint16_t)buf[5] << 8) |
                                    buf[6];

            /* 最大帧大小 */
            p_flac->max_framesize = ((uint32_t)buf[7] << 16) |
                                    ((uint16_t)buf[8] << 8) |
                                    buf[9];

            /* 采样率 */
            p_flac->samplerate = ((uint32_t)buf[10] << 12) |
                                 ((uint16_t)buf[11] << 4) |
                                 ((buf[12] & 0xf0) >> 4);

            /* 音频通道数 */
            p_flac->channels = ((buf[12]&0x0e) >> 1) + 1;

            /* 采样位数 */
            p_flac->bps = ((((uint16_t)buf[12] & 0x01) << 4) |
                          ((buf[13] & 0xf0) >> 4)) + 1;

            /* 一个声道的总采样数 */
            p_flac->totalsamples = ((uint32_t)buf[14] << 24) |
                                   ((uint32_t)buf[15] << 16) |
                                   ((uint16_t)buf[16] << 8) |
                                   buf[17];

            p_flac_ctrl->sample_rate = p_flac->samplerate;

            /* 得到总时间 */
            p_flac_ctrl->total_time = (double)p_flac->totalsamples /
                                      p_flac->samplerate *
                                      1000;
        } else {

            /* 忽略其他帧的处理 */
            if (aw_lseek(fd, block_length, SEEK_CUR) <0) {
                AW_LOGF(("file %s lseek failed\n", fd));
                goto fail;
            }
        }
    }

    if(p_flac_ctrl->total_time)
    {
        /* PCM输出数据量(*2,表示2个声道的数据量) */
        p_flac_ctrl->out_samples = p_flac->max_blocksize * 2;

        p_flac_ctrl->nchannels = p_flac->channels;

        /* 采样位数(16/24/32) */
        p_flac_ctrl->bps = p_flac->bps;

        /* FLAC数据帧开始的地址 */
        p_flac_ctrl->data_start = aw_lseek(fd, 0, SEEK_CUR);

        /* 得到FLAC的位速 */
        p_flac_ctrl->bit_rate = ((size - p_flac_ctrl->data_start) * 8.0) /
                                p_flac_ctrl->total_time *
                                1000;
        p_flac_ctrl->file_size = size;
    } else {
        goto fail;    /* 总时间为0 有问题的flac文件 */
    }

    aw_close(fd);
    return AW_OK;

fail:
    aw_close(fd);
    return -AW_ERROR;
}

/**
 * \brief 解析flac文件
 *
 * \param[in]  p_name      文件名
 * \param[out] p_flac_ctrl 初始化的flac结构体
 *
 * \retval AW_OK      解析成功
 * \retval -AW_ERROR  解析失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t get_flac_info (const char *p_name, flac_ctrl_t *p_flac_ctrl)
{
    FLACContext flac;

    return flac_decode_init(p_name, p_flac_ctrl, &flac);
}

/**
 * \brief 播放flac文件
 *
 * \param[in] p_name 文件名
 *
 * \retval AW_OK      播放成功
 * \retval -AW_ERROR  播放失败
 * \retval -AW_EINVAL 无效参数
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

    /* 打开文件 */
    g_play_ctrl.fd = aw_open(p_name, O_RDONLY, 0777);
    if (g_play_ctrl.fd < 0) {
        AW_ERRF(("open file error: %d\n", g_play_ctrl.fd));
        return -AW_ERROR;
    }

    /* 获取文件大小 */
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

    /* 构建音频设备 */
    aw_imx28_snd_mkdev(&g_snd_dev,                    /* 音频设备 */
                       0,                             /* SAIF0 */
                       SOUND_DEVICE,                  /* 声卡 */
                       256,                           /* 一次中断传输数据大小 */
                       128,                           /* 内部环形缓冲区大小 */
                       16,                            /* 位数 */
                       flac_ctrl.nchannels,           /* 通道数 */
                       flac_ctrl.sample_rate,         /* 采样率 */
                       AW_IMX28_SND_STREAM_PLAYBACK); /* 播放音频 */

    /* 打开声卡 */
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
