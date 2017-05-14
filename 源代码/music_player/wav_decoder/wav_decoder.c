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
 * \brief wav解码
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-11 张进科, 创建该文件
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "apollo.h"
#include "wav_decoder.h"

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
#include "resource.h"

/*******************************************************************************
  宏定义
*******************************************************************************/

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

static uint8_t  g_wav_decbuf[1024 * 8]; /**< \brief 解码缓冲区 */
static uint32_t g_wav_outbuf[1024 * 8]; /**< \brief 输出缓冲区 */

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 解析WAV数据
 *
 * \param[in]  p_data     wav文件数据
 * \param[in]  lenth      文件长度
 * \param[out] p_wav_ctrl wav控制结构体
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t wav_decode_init (uint8_t    *p_data,
                          uint32_t    lenth,
                          wav_ctrl_t *p_wav_ctrl)
{
    wave_header_t wave_header;

    if ((NULL == p_data) || (NULL == p_wav_ctrl)) {
        return -AW_EINVAL;
    }

    /* 判断ID_RIFF */
    memset(p_wav_ctrl, 0, sizeof(wav_ctrl_t));
    wave_header.riff = (chunk_riff_t *)p_data;
    if (wave_header.riff->chunk_id != ID_RIFF) {
        return -AW_ERROR;
    }

    wave_header.fmt = (chunk_fmt_t *)(p_data + 12);
    wave_header.fact = (chunk_fact_t *)(p_data + 12 + 8 +
                                        wave_header.fmt->chunk_size);

    if (wave_header.fact->chunk_id != ID_DATA) {
        p_wav_ctrl->data_start = 12 + 8 + wave_header.fmt->chunk_size +
                                 8 + wave_header.fact->chunk_size;
    } else {
        p_wav_ctrl->data_start = 12 + 8 + wave_header.fmt->chunk_size;
    }

    /* 判断ID_DATA */
    wave_header.data = (chunk_data_t *)(p_data + p_wav_ctrl->data_start);
    if (wave_header.data->chunk_id != ID_DATA) {
        return -AW_ERROR;
    }

    /** 保存解析出的参数, 分别为:
     *     音频格式、通道数、采样率、位速、块对齐、位数、
     *     数据块大小、数据流开始的地方、歌曲总长度(单位:秒)
     */
    p_wav_ctrl->audio_format = wave_header.fmt->audio_format;
    p_wav_ctrl->nchannels    = wave_header.fmt->num_of_channels;
    p_wav_ctrl->sample_rate  = wave_header.fmt->sample_rate;
    p_wav_ctrl->bit_rate     = wave_header.fmt->byte_rate * 8;
    p_wav_ctrl->block_align  = wave_header.fmt->block_align;
    p_wav_ctrl->bps          = wave_header.fmt->bits_per_sample;

    p_wav_ctrl->data_size    = wave_header.data->chunk_size;
    p_wav_ctrl->data_start   = p_wav_ctrl->data_start + 8;

    p_wav_ctrl->total_time   = ((p_wav_ctrl->data_size) /
                               (p_wav_ctrl->bit_rate / 8.0)) *
                               1000;

    return AW_OK;
}

/**
 * \brief 解析WAV文件
 *
 * \param[in]  p_name     文件名
 * \param[out] p_wav_ctrl wav控制结构体
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t wav_info_get (const char *p_name, wav_ctrl_t *p_wav_ctrl)
{
    int     fd;
    int     size;
    uint8_t buf[512];

    if ((NULL == p_name) || (NULL == p_wav_ctrl)) {
        return -AW_EINVAL;
    }

    /* 打开文件 */
    fd = aw_open(p_name, O_RDONLY, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_ERROR;
    }

    /* 读取文件 */
    if (aw_read(fd, buf, 512) != 512) {
        aw_close(fd);
        AW_ERRF(("read file error!\n"));
        return -AW_ERROR;
    }

    /* 获取文件大小 */
    if ((size = file_size_get(fd)) < 0) {
        AW_ERRF(("%s size get error!\n", p_name));
        aw_close(fd);
        return -AW_ERROR;
    }

    /* 初始化wav控制结构 */
    if (wav_decode_init(buf, 512, p_wav_ctrl) != AW_OK)
    {
        aw_close(fd);
        AW_ERRF(("wav_decode_init failed\n"));
        return -AW_ERROR;
    }
    p_wav_ctrl->file_size = size;

    aw_close(fd);
    return AW_OK;
}

/**
 * \brief 播放WAV文件
 *
 * \param[in] p_name 文件名
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t wav_play (const char *p_name)
{
    int        size;
    wav_ctrl_t wav_ctrl;
    size_t     read_len;
    size_t     write_len;
    uint32_t   data_counter = 0;
    uint32_t   data_set_idx = 0;
    int        i;

    if (NULL == p_name) {
        return -AW_EINVAL;
    }

    /* 打开文件 */
    g_play_ctrl.fd = aw_open(p_name, O_RDONLY, 0777);
    if (g_play_ctrl.fd < 0) {
        AW_ERRF(("open file error: %d\n", g_play_ctrl.fd));
        return -AW_ERROR;
    }

    /* 获取文件大小 */
    if ((size = file_size_get(g_play_ctrl.fd)) < 0) {
        AW_ERRF(("%s size get error!\n", p_name));
        goto fail;
    }

    /* 检查缓冲区是否足够 */
    if (size < 512) {
        AW_ERRF(("file too small!\n"));
        goto fail;
    }

    /* 读取文件 */
    if (aw_read(g_play_ctrl.fd, g_wav_decbuf, 512) != 512) {
        AW_ERRF(("read file error!\n"));
        goto fail;
    }

    /* 初始化wav控制结构 */
    if (wav_decode_init(g_wav_decbuf, 512, &wav_ctrl) < 0)
    {
        AW_ERRF(("wav_decode_init failed\n"));
        goto fail;
    } else {
        g_play_ctrl.info.total_time    = wav_ctrl.total_time;
        g_play_ctrl.info.bits_per_word = wav_ctrl.bps;
        g_play_ctrl.info.channels      = wav_ctrl.nchannels;
        g_play_ctrl.info.samples_rate  = wav_ctrl.sample_rate;
        g_play_ctrl.info.bitrate       = wav_ctrl.bit_rate;
    }

    if (aw_lseek(g_play_ctrl.fd, wav_ctrl.data_start, SEEK_SET) <0) {
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
                       wav_ctrl.nchannels,            /* 通道数 */
                       wav_ctrl.sample_rate,          /* 采样率 */
                       AW_IMX28_SND_STREAM_PLAYBACK); /* 播放音频 */

    /* 打开声卡 */
    if (aw_imx28_snd_open(&g_snd_dev) != AW_OK) {
        AW_ERRF(("aw_imx28_snd_open failed\n"));
        goto fail;
    }

    /* 播放音乐 */
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
                data_set_idx = ((double)g_move_time /
                               wav_ctrl.total_time *
                               wav_ctrl.data_size);
                data_set_idx -= data_set_idx % wav_ctrl.block_align;
                data_set_idx += wav_ctrl.data_start;
                if (aw_lseek(g_play_ctrl.fd, data_set_idx, SEEK_SET) <0) {
                    AW_ERRF(("file %s lseek failed\n", g_play_ctrl.fd));
                    goto fail;
                }
                data_counter = data_set_idx;
            }
            read_len = aw_read(g_play_ctrl.fd,
                          g_wav_decbuf,
                          1024 * wav_ctrl.block_align);
            data_counter += read_len;

            if (8 == wav_ctrl.bps) {
                write_len = read_len * 2 / 1;
                for (i = 0; i < read_len; i++) {
                    if (i % 2 == 0) {
                        g_wav_outbuf[i / 2] = g_wav_decbuf[i] << 7 |
                                              g_wav_decbuf[i + 1] << 23;
                    }
                }
            } else if (16 == wav_ctrl.bps) {
                write_len = read_len;
                memcpy(g_wav_outbuf, g_wav_decbuf, read_len);
            } else if (24 == wav_ctrl.bps) {
                write_len = read_len * 2 / 3;
                for (i = 0; i < read_len; i++) {
                    if (i % 6 == 0) {
                        g_wav_outbuf[i / 6] = g_wav_decbuf[i + 1] |
                                              g_wav_decbuf[i + 2] << 8 |
                                              g_wav_decbuf[i + 4] << 16 |
                                              g_wav_decbuf[i + 5] << 24;
                    }
                }
            } else if (32 == wav_ctrl.bps) {
                write_len = read_len * 2 / 4;
                for (i = 0; i < read_len; i++) {
                    if (i % 8 == 0) {
                        g_wav_outbuf[i / 8] = g_wav_decbuf[i + 2] |
                                              g_wav_decbuf[i + 3] << 8 |
                                              g_wav_decbuf[i + 6] << 16 |
                                              g_wav_decbuf[i + 7] << 24;
                    }
                }
            }

            if (write_len > 0) {
                aw_imx28_snd_write(&g_snd_dev,
                                    (const uint8_t *)g_wav_outbuf,
                                    write_len);
            }

            /* 计算当前播放时间 */
            wav_ctrl.current_time = (uint32_t)(data_counter /
                                    (double)wav_ctrl.data_size *
                                    wav_ctrl.total_time);
            g_play_ctrl.current_time = wav_ctrl.current_time;
        }
    } while (write_len > 0);

    /* 关闭声卡 */
    aw_imx28_snd_close(&g_snd_dev);

    aw_close(g_play_ctrl.fd);
    return AW_OK;

    /* 错误处理 */
fail:
    aw_imx28_snd_close(&g_snd_dev);
    aw_close(g_play_ctrl.fd);
    g_play_ctrl.paly_state = PALY_STATE_PAUSE;
    return -AW_ERROR;
}
/* end of file */
