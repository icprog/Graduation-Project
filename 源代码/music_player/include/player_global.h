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
 * \brief 全局变量
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-10 张进科, 创建该文件
 * \endinternal
 */

#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stdint.h>
#include "play_list.h"
#include "play_task.h"
#include "aw_imx28_sound.h"
#include "driver/saif/awbl_imx28_sound.h"

#define PALY_MODE_RANDOM     (0)
#define PALY_MODE_ORDER      (1)
#define PALY_MODE_SINGLE     (2)

#define SOUND_DEVICE         (2) /* ES9018K2M */

/** \brief 字体数据 */
extern uint8_t  g_ttf_res[7569637];
extern uint32_t g_ttf_res_lenth;

/** \brief 歌手图片文件夹 */
extern const char *gp_background_path;

/** \brief 歌曲目录 */
extern const char *gp_sdcard_music_path;
extern const char *gp_udisk_music_path;

/** \brief 歌手图片数据 */
extern uint8_t  g_singer_res[1024 * 1024 * 2];
extern uint32_t g_singer_res_lenth;

/** \brief 歌词数据 */
extern uint8_t  g_lrc_res[1024 * 1024];
extern uint32_t g_lrc_res_lenth;

/** \brief 播放列表 */
extern music_info_t g_play_list[PLAY_LIST_MAX_SIZE];

/** \brief 播放列表状态 */
extern list_state_t g_list_state;

/** \brief 音乐播放控制结构 */
extern play_ctrl_t g_play_ctrl;

/** \brief 音频设备结构 */
extern aw_imx28_snd_device_t g_snd_dev;

/** \brief 当前歌词移动到的时间 */
extern uint32_t g_move_time;

/** \brief 是否挂载了SD卡 0:未挂载 1:已挂载 */
uint8_t g_is_mount_sdcard;

/** \brief 是否挂载了U盘 0:未挂载 1:已挂载 */
uint8_t g_is_mount_udisk;

#endif /* __GLOBAL_H */

/* end of file */
