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

#ifndef __flac_DECODER_H
#define __flac_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#pragma pack(push)
#pragma pack(2)

/**
 * \brief flaC标签
 */
typedef struct flac_tag {

    uint8_t id[3];            /**< \brief ID,在文件起始位置,必须是flaC4个字母 */

} flac_tag_t;

/**
 * \brief metadata 数据块头信息结构体
 */
typedef struct md_block_head {

    uint8_t head;             /**< \brief metadata block头 */
    uint8_t size[3];          /**< \brief metadata block数据长度 */

} md_block_head_t;

/**
 * \brief FLAC控制结构体
 */
typedef struct flac_ctrl {

    uint16_t nchannels;       /**< \brief 通道数量;1,表示单声道;2,表示双声道 */
    uint32_t total_time;      /**< \brief 整首歌时长 单位:毫秒 */
    uint32_t current_time;    /**< \brief 当前播放时长 单位:毫秒 */
    uint32_t bit_rate;        /**< \brief 比特率(位速) */
    uint32_t sample_rate;     /**< \brief 采样率 */
    uint16_t bps;             /**< \brief 位数,比如16bit,24bit,32bit */
    uint32_t data_start;      /**< \brief 数据帧开始的位置(在文件里面的偏移) */
    uint32_t file_size;       /**< \brief 文件大小(byte) */
    uint32_t out_samples;     /**< \brief PCM输出数据量 */

} flac_ctrl_t;

#pragma pack(pop)

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
aw_err_t get_flac_info (const char *p_name, flac_ctrl_t *p_flac_ctrl);

/**
 * \brief 播放flac文件
 *
 * \param[in] p_name 文件名
 *
 * \retval AW_OK      播放成功
 * \retval -AW_ERROR  播放失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t flac_play (const char *p_name);

#ifdef __cplusplus
}
#endif

#endif /* __FLAC_DECODER_H */

/* end of file */
