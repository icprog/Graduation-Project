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
 * \brief lrc解码
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-05 张进科, 创建该文件
 * \endinternal
 */

#ifndef __LYRIC_H
#define __LYRIC_H

#include <stdint.h>
#include "GUI.h"  

#define LRC_ID_TAG_MAX_LENTH      (50)
#define LRC_TIME_TAG_MAX_ROW      (1024)
#define LRC_TIME_TAG_MAX_LENTH    (512)

/** \brief 歌词信息结构体 */
typedef struct lyric_info {

    uint8_t  artist[LRC_ID_TAG_MAX_LENTH]; /**< \brief 艺术家名字 */
    uint8_t  title[LRC_ID_TAG_MAX_LENTH];  /**< \brief 歌曲名字 */
    uint8_t  au[LRC_ID_TAG_MAX_LENTH];     /**< \brief 歌词作者-作曲家 */
    uint8_t  album[LRC_ID_TAG_MAX_LENTH];  /**< \brief 专辑 */
    uint8_t  by[LRC_ID_TAG_MAX_LENTH];     /**< \brief LRC歌词文件制作者 */
    int32_t  offset;                       /**< \brief 补偿时值 单位: 毫秒 */
    uint32_t total_time;                   /**< \brief 总时间 单位: 秒 */
    uint32_t row_num;                      /**< \brief 歌词行数 */
    uint32_t time[LRC_TIME_TAG_MAX_ROW];   /**< \brief 每行歌词的时间 单位:ms */

    /** \brief 歌词 */
    uint8_t  lrc[LRC_TIME_TAG_MAX_ROW][LRC_TIME_TAG_MAX_LENTH]; 

} lyric_info_t;

/** \brief 时间标签解析状态机状态 */
typedef enum time_tag_st {

    TIME_TAG_ST_IDLE = 0,             /**< \brief 空闲态 */
    TIME_TAG_ST_FIRST_BRACKETS,       /**< \brief 第一个方括号 */
    TIME_TAG_ST_FIRST_NUMBER,         /**< \brief 第一个数字 */
    TIME_TAG_ST_COLON,                /**< \brief 冒号 */
    TIME_TAG_ST_SECOND_NUMBER,        /**< \brief 第二个数字 */
    TIME_TAG_ST_DOT,                  /**< \brief 点号 */
    TIME_TAG_ST_THIRD_NUMBER,         /**< \brief 第三个数字 */
    TIME_TAG_ST_SECOND_BRACKETS,      /**< \brief 第二个方括号 */

} time_tag_st_t;

/** \brief 标识标签解析状态机状态 */
typedef enum id_tag_st {

    ID_TAG_ST_IDLE = 0,             /**< \brief 空闲态 */
    ID_TAG_ST_FIRST_BRACKETS,       /**< \brief 第一个方括号 */
    ID_TAG_ST_FIRST_VALUE,          /**< \brief 第一个值 */
    ID_TAG_ST_COLON,                /**< \brief 冒号 */
    ID_TAG_ST_SECOND_VALUE,         /**< \brief 第二个值 */
    ID_TAG_ST_SECOND_BRACKETS,      /**< \brief 第二个方括号 */

} id_tag_st_t;

/** \brief 标识标签类型 */
typedef enum id_tag_tpye {

    ID_TAG_TPYE_AR = 0,             /**< \brief 艺术家名字 */
    ID_TAG_TPYE_TI,                 /**< \brief 歌曲名字 */
    ID_TAG_TPYE_AU,                 /**< \brief 歌词作者-作曲家 */
    ID_TAG_TPYE_AL,                 /**< \brief 专辑 */
    ID_TAG_TPYE_BY,                 /**< \brief LRC歌词文件制作者 */
    ID_TAG_TPYE_OFFSET,             /**< \brief 补偿时值 */
    ID_TAG_TPYE_T_TIME,             /**< \brief 总时间 */
    ID_TAG_TPYE_UNDEFINE,           /**< \brief 未定义的标签 */

} id_tag_tpye_t;

/**
 * \brief 解析lrc歌词
 *
 * \param[in]  p_data *.lrc文件数据
 * \param[in]  lenth  数据长度
 * \param[out] lyric  解析出的歌词数据
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t lrc_analyze (uint8_t *p_data, uint32_t lenth, lyric_info_t *p_lyric);

#endif /* __LYRIC_H */

/* end of file */
