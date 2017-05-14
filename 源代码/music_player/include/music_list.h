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
 * \brief 音乐播放器列表界面
 *
 * \internal
 * \par Modification history
 * - 1.00 17-02-19 张进科, 创建该文件
 * \endinternal
 */

#ifndef __MUSIC_LIST_H
#define __MUSIC_LIST_H

#include <stdint.h>
#include "WM.h"

/**
 * \brief 更新播放列表的显示
 */
void refresh_list (void);

/**
 * \brief 重新创建播放列表窗口
 */
void winlist_recreate (void);

/**
 * \brief 创建播放列表窗体
 *
 * \param[in] parent 父窗口句柄
 *
 * \return 播放列表窗体句柄
 */
WM_HWIN music_list_create (WM_HWIN parent);

#endif /* __MUSIC_LIST_H */

/* end of file */
