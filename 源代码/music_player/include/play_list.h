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
 * \brief 播放列表
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-15 张进科, 创建该文件
 * \endinternal
 */

#ifndef __PLAY_LIST_H
#define __PLAY_LIST_H

#include <stdint.h>

#define SEARCH_RANGE_SD_USB              (0)    /** \brief 搜索SD卡和U盘 */
#define SEARCH_RANGE_SD                  (1)    /** \brief 搜索SD卡 */
#define SEARCH_RANGE_USB                 (2)    /** \brief 搜索U盘 */

#define PLAY_LIST_MAX_SIZE               (1000) /** \brief 列表最大长度 */
#define MUSIC_INFO_ARTIST_MAX_LENTH      (100)  /** \brief 歌手名最大长度 */
#define MUSIC_INFO_NAME_MAX_LENTH        (50)   /** \brief 歌名最大长度 */
#define MUSIC_INFO_FILE_PATH_MAX_LENTH   (255)  /** \brief 文件路径最大长度 */

/**
 * \brief 音频格式
 */
typedef enum music_type {

    MUSIC_TYPE_WAV = 0,    /** \brief WAV格式 */
    MUSIC_TYPE_MP3,        /** \brief MP3格式 */
    MUSIC_TYPE_FLAC,       /** \brief FLAC格式 */
    MUSIC_TYPE_OTHER,      /** \brief 其它格式 */

} music_type_t;

/**
 * \brief 歌曲信息结构
 */
typedef struct music_info {

    /** \brief 索引 */
    uint16_t     index;

    /** \brief 艺术家名字 */
    uint8_t      artist[MUSIC_INFO_ARTIST_MAX_LENTH];

    /** \brief 歌曲名字 */
    uint8_t      name[MUSIC_INFO_NAME_MAX_LENTH];

    /** \brief 文件名 */
    uint8_t      file_name[MUSIC_INFO_FILE_PATH_MAX_LENTH];

    uint32_t     file_size;     /**< \brief 文件大小(byte) */
    music_type_t type;          /**< \brief 格式 */
    uint32_t     total_time;    /**< \brief 总时间 (ms) */
    uint8_t      bits_per_word; /**< \brief 位数（一般为8,16,24,32等） */
    uint8_t      channels;      /**< \brief 通道数 */
    uint32_t     samples_rate;  /**< \brief 采样率 */
    uint32_t     bitrate;       /**< \brief 比特率 */

} music_info_t;

/**
 * \brief 列表状态结构
 */
typedef struct list_state {

    uint32_t play_list_count; /**< \brief 播放列表中音乐的个数 */
    uint32_t sdcard_start;    /**< \brief SD卡内音乐起始索引 */
    uint32_t sdcard_count;    /**< \brief SD卡内音乐数量 */
    uint32_t usb_start;       /**< \brief u盘内音乐起始索引 */
    uint32_t usb_count;       /**< \brief u盘内音乐数量 */

} list_state_t;

/**
 * \brief 获取文件后缀名
 *
 * \param[in]  p_name 文件名
 * \param[out] p_str  后缀名, 此缓冲区大小不能小于10
 *
 * \return 小于0出错，其它为后缀名在文件名中的位置
 */
int32_t get_file_type (const char *p_name, char *p_str);

/**
 * \brief 获取列表内歌曲数目
 *
 * \param[in] p_play_list 播放列表
 * \param[in] lenth       播放列表长度
 *
 * \return 歌曲数目
 */
int music_count_get (music_info_t *p_play_list, uint32_t lenth);

/**
 * \brief 清空播放列表
 *
 * \param[in] p_play_list 播放列表
 * \param[in] lenth       播放列表长度
 *
 * \return 无
 */
void play_list_clean (music_info_t *p_play_list, uint32_t lenth);

/**
 * \brief 清空播放列表中u盘内的歌曲
 *
 * \param[in] p_play_list 播放列表
 * \param[in] lenth       播放列表长度
 *
 * \return 无
 */
void usb_play_list_clean (music_info_t *p_play_list, uint32_t lenth);

/**
 * \brief 在列表中添加一首歌
 *
 * \param[in] p_play_list  播放列表
 * \param[in] lenth        播放列表长度
 * \param[in] p_music_info 待添加的音乐, index可以不用初始化
 *
 * \return 小于0出错，其它为添加的歌曲在列表中的索引
 */
int32_t play_list_add (music_info_t *p_play_list,
                       uint32_t      lenth,
                       music_info_t *p_music_info);

/**
 * \brief 在列表中删除一首歌
 *
 * \param[in] p_play_list  播放列表
 * \param[in] lenth        播放列表长度
 * \param[in] p_music_info 待删除的音乐
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t play_list_del (music_info_t *p_play_list,
                        uint32_t      lenth,
                        music_info_t *p_music_info);

/**
 * \brief 搜索文件夹内歌曲并添加进播放列表
 *
 * \param[in] p_play_list 播放列表
 * \param[in] lenth       播放列表长度
 * \param[in] p_dir       音乐所在文件夹
 * \param[in] show_info   0: 不显示info 1:显示info
 *
 * \return 无
 */
void play_list_scan (music_info_t *p_play_list,
                     uint32_t      lenth,
                     const char   *p_dir,
                     uint8_t       show_info);

/**
 * \brief 初始化播放列表
 *
 * \param[in] p_play_list 播放列表
 * \param[in] lenth       播放列表长度
 * \param[in] show_info   0: 不显示info 1:显示info
 * \param[in] range       0: 搜索SD卡和U盘 1:搜索SD卡 2:搜索U盘
 *
 * \return 无
 */
void play_list_init (music_info_t *p_play_list,
                     uint32_t      lenth,
                     uint8_t       show_info,
                     uint8_t       range);

#endif /* __PLAY_LIST_H */

/* end of file */
