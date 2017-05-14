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
 * \brief 资源管理
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-10 张进科, 创建该文件
 * \endinternal
 */

#ifndef __RESOURCE_H
#define __RESOURCE_H

#include "aw_errno.h"
#include "play_list.h"

/**
 * \brief 配置信息结构体
 */
typedef struct config_info {

    int32_t volume;        /**< \brief 音量 */
    int32_t play_idx;      /**< \brief 当前正在播放的音乐索引 */
    uint8_t lrc_show;      /**< \brief 是否显示歌词 0:不显示 1:显示 */
    uint8_t singer_show;   /**< \brief 是否显示歌手图片 0:不显示 1:显示 */
    uint8_t is_effect;     /**< \brief 配置是否有效 0:无效 1:有效 */

} config_info_t;

/** \brief 配置信息 */
extern config_info_t g_config_info;

/**
 * \brief 读取播放列表
 *
 * \param[in] p_play_list  播放列表
 * \param[in] p_list_state 播放列表状态
 * \param[in] lenth        播放列表长度
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t play_list_read (music_info_t *p_play_list,
                         list_state_t *p_list_state,
                         uint32_t      lenth);

/**
 * \brief 保存播放列表
 *
 * \param[in] p_play_list  播放列表
 * \param[in] p_list_state 播放列表状态
 * \param[in] lenth        播放列表长度
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t play_list_write (music_info_t *p_play_list,
                          list_state_t *p_list_state,
                          uint32_t      lenth);
/**
 * \brief 读取配置信息
 *
 * \param[in] p_config_info 配置信息
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t config_read (config_info_t *p_config_info);

/**
 * \brief 保存配置信息
 *
 * \param[in] p_config_info 配置信息
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t config_write (config_info_t *p_config_info);

/**
 * \brief 获取文件大小
 *
 * \param[in] fd 已经打开的文件句柄
 *
 * \return 小于0出错，其它为获取到的文件大小
 */
int file_size_get (int fd);

/**
 * \brief 将指定路径的文件读取到内存中
 *
 * \param[in]  p_path       文件绝对路径
 * \param[out] p_buf        读取到的位置
 * \param[in]  lenth        缓冲区大小
 * \param[out] p_read_lenth 读取出的文件大小
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t file_to_mem (const uint8_t *p_path,
                      uint8_t       *p_buf,
                      uint32_t       buf_lenth,
                      uint32_t      *p_read_lenth);

/**
 * \brief 读取与音乐同文件名的歌词
 *
 * \param[in]  p_path       音乐文件名
 * \param[out] p_buf        读取到的位置
 * \param[in]  lenth        缓冲区大小
 * \param[out] p_read_lenth 读取出的文件大小
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t lrc_read (const uint8_t  *p_path,
                   uint8_t        *p_buf,
                   uint32_t        buf_lenth,
                   uint32_t       *p_read_lenth);

/**
 * \brief 读取与音乐同文件名的图片
 *
 * \param[in]  p_path       音乐文件名
 * \param[out] p_buf        读取到的位置
 * \param[in]  lenth        缓冲区大小
 * \param[out] p_read_lenth 读取出的文件大小
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t jpg_read (const uint8_t  *p_path,
                   uint8_t        *p_buf,
                   uint32_t        buf_lenth,
                   uint32_t       *p_read_lenth);

/**
 * \brief 尝试挂载SD卡和U盘
 */
void mount_try (void);

/**
 * \brief 初始化所需的资源
 */
void resource_init (void);

#endif /* __RESOURCE_H */

/* end of file */
