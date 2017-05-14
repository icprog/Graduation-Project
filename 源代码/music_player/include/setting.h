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
 * \brief 系统设置界面
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-10 张进科, 创建该文件
 * \endinternal
 */

#ifndef __SETTING_H
#define __SETTING_H

#include "WM.h"

/**
 * \brief 创建设置窗体
 *
 * \param[in] parent 父窗体
 *
 * \return 设置窗体句柄
 */
WM_HWIN setting_create (WM_HWIN parent);

/**
 * \brief 刷新选项状态
 */
void setting_refresh (void);

#endif /* __SETTING_H */

/* end of file */
