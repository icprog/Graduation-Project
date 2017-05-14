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

#ifndef __WAV_DECODER_H
#define __WAV_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ID_RIFF  (0x46464952) /**< \brief RIFF块 */
#define ID_WAVE  (0x45564157) /**< \brief WAVE块 */
#define ID_FMT   (0x20746D66) /**< \brief FMT块 */
#define ID_FACT  (0x74636166) /**< \brief FACT块 */
#define ID_LIST  (0x5453494C) /**< \brief LIST块 */
#define ID_DATA  (0x61746164) /**< \brief DATA块 */

#pragma pack(push)
#pragma pack(2)

/**
 * \brief RIFF块
 */
typedef struct chunk_riff {

    uint32_t chunk_id;        /**< \brief chunk id;这里固定为"RIFF",即0x46464952 */
    uint32_t chunk_size;      /**< \brief 集合大小;文件总大小-8 */
    uint32_t format;          /**< \brief 格式;wave,即0x45564157 */

} chunk_riff_t;

/**
 * \brief fmt块
 */
typedef struct chunk_fmt {

    uint32_t chunk_id;        /**< \brief chunk id;这里固定为"fmt ",即0x20746d66 */
    uint32_t chunk_size;      /**< \brief 子集合大小(不包括id和size);这里为:20. */
    uint16_t audio_format;    /**< \brief 音频格式; 0x01:线性pcm 0x11: ima adpcm */
    uint16_t num_of_channels; /**< \brief 通道数量;1,表示单声道;2,表示双声道; */
    uint32_t sample_rate;     /**< \brief 采样率;0x1f40,表示8khz */
    uint32_t byte_rate;       /**< \brief 字节速率; */
    uint16_t block_align;     /**< \brief 块对齐(字节); */
    uint16_t bits_per_sample; /**< \brief 单个采样数据大小;4位adpcm,设置为4 */
    uint16_t byte_extra_data; /**< \brief 附加的数据字节;2个; 线性pcm,没有这个参数 */

} chunk_fmt_t;

/**
 * \brief fact块
 */
typedef struct chunk_fact {

    uint32_t chunk_id;        /**< \brief chunk id; 这里固定为"fact" */
    uint32_t chunk_size;      /**< \brief 子集合大小(不包括id和size);这里为:4. */
    uint32_t num_of_samples;  /**< \brief 采样的数量; */

} chunk_fact_t;

/**
 * \brief LIST块
 */
typedef struct chunk_list {

    uint32_t chunk_id;        /**< \brief chunk id;这里固定为"LIST"; */
    uint32_t chunk_size;      /**< \brief 子集合大小(不包括id和size);这里为:4. */

} chunk_list_t;

/**
 * \brief data块
 */
typedef struct chunk_data {

    uint32_t chunk_id;        /**< \brief chunk id;这里固定为"data" */
    uint32_t chunk_size;      /**< \brief 子集合大小(不包括id和size) */

} chunk_data_t;

#pragma pack(pop)

/**
 * \brief wav头
 */
typedef struct wave_header { 

    chunk_riff_t *riff;             /**< \brief riff块 */
    chunk_fmt_t  *fmt;              /**< \brief fmt块 */
    chunk_fact_t *fact;             /**< \brief fact块 线性pcm,没有这个结构体 */
    chunk_data_t *data;             /**< \brief data块 */

} wave_header_t; 

/**
 * \brief 播放控制结构体
 */
typedef struct wav_ctrl {

    uint16_t audio_format;    /**< \brief 音频格式; 0x01:线性pcm 0x11: ima adpcm */
    uint16_t nchannels;       /**< \brief 通道数量;1,表示单声道;2,表示双声道 */
    uint16_t block_align;     /**< \brief 块对齐(字节) */
    uint32_t data_size;       /**< \brief WAV数据大小 */

    uint32_t total_time;      /**< \brief 整首歌时长 单位:毫秒 */
    uint32_t current_time;    /**< \brief 当前播放时长 单位:毫秒 */
    
    uint32_t bit_rate;        /**< \brief 比特率(位速) */
    uint32_t sample_rate;     /**< \brief 采样率 */
    uint16_t bps;             /**< \brief 位数,比如16bit,24bit,32bit */
    
    uint32_t data_start;      /**< \brief 数据帧开始的位置(在文件里面的偏移) */
    uint32_t file_size;       /**< \brief 文件大小(byte) */

} wav_ctrl_t; 


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
                          wav_ctrl_t *p_wav_ctrl);

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
aw_err_t wav_info_get (const char *p_name, wav_ctrl_t *p_wav_ctrl);

/**
 * \brief 播放WAV文件
 *
 * \param[in] p_name 文件名
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t wav_play (const char *p_name);

#ifdef __cplusplus
}
#endif

#endif /* __WAV_DECODER_H */

/* end of file */
