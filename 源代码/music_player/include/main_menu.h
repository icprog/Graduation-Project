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
* \brief 音乐播放器主界面
*
* \internal
* \par Modification history
* - 1.00 17-02-17 张进科, 创建该文件
* \endinternal
*/

#ifndef __MAIN_MENU_H
#define __MAIN_MENU_H

#include <stdint.h>

/**
 * \brief 获取音量
 *
 * \return 音量
 */
int32_t volume_get (void);

/**
 * \brief 设置音量
 *
 * \param[in] volume 音量
 *
 * \return 无
 */
void volume_set (int32_t volume);

/**
 * \brief 刷新音量设置
 */
void refresh_volume (void);

/**
 * \brief 初始化GUI任务
 */
extern void player_gui_task_init(void);

#endif /* __MAIN_MENU_H */

/* end of file */
