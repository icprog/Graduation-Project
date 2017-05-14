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
 * \brief mp3解码
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-17 张进科, 创建该文件
 * \endinternal
 */

#ifndef __MP3_DECODER_H
#define __MP3_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "mad.h"

/**
 * \brief MP3播放缓冲
 */
struct mp3_buffer {

    unsigned char const *p_start; /**< \brief 缓冲区首地址 */
    unsigned long        length;  /**< \brief 缓冲区长度 */

};

/**
 * \brief 播放控制结构体
 */
typedef struct mp3_info {

    uint16_t nchannels;       /**< \brief 通道数量;1,表示单声道;2,表示双声道 */
    uint32_t total_time;      /**< \brief 整首歌时长 单位:毫秒 */
    uint32_t current_time;    /**< \brief 当前播放时长 单位:毫秒 */
    uint32_t bit_rate;        /**< \brief 比特率 */
    uint32_t sample_rate;     /**< \brief 采样率 */
    uint32_t data_start;      /**< \brief 数据帧开始的位置(在文件里面的偏移) */
    uint32_t data_size;       /**< \brief 数据大小 */
    uint32_t file_size;       /**< \brief 文件大小(byte) */
    double   frame_num;       /**< \brief 帧数 */
    double   frame_size;      /**< \brief 帧大小 */
    double   frame_time;      /**< \brief 帧时长 单位:ms */

} mp3_info_t;


/**
 * \brief 解析mp3文件
 *
 * \param[in]  p_name     文件名
 * \param[out] p_wav_ctrl 初始化的mp3结构体
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t mp3_info_get (const char *p_name, mp3_info_t *p_mp3_info);

/**
 * \brief 播放MP3文件
 *
 * \param[in] file_name 文件名
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t mp3_play (const char *p_name);

#ifdef __cplusplus
}
#endif

#endif /* __MP3_DECODER_H */

/* end of file */
