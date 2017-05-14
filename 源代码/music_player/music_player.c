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
 * \brief 系统初始化
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-10 张进科, 创建该文件
 * \endinternal
 */

#include "music_player.h"

#include "main_menu.h"
#include "resource.h"
#include "touch_screen.h"
#include "play_task.h"

/**
 * \brief music_player
 */
void music_player (void)
{

    /* 初始化所需的资源 */
    resource_init();

    /* 初始化GUI任务 */
    player_gui_task_init();

    /* 初始化触摸屏任务 */
    touch_screen_task_init();

    /* 初始化音频播放任务 */
    play_task_init();
}

/* end of file */
