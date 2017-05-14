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
 * \brief emwin xbf字体
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-04 张进科, 创建该文件
 * \endinternal
 */

#ifndef __FONT_XBF_H
#define __FONT_XBF_H

#include <stdint.h>
#include "GUI.h"  

/** \brief xbf字体 */
extern GUI_FONT g_font11, g_font13, g_font14, g_font18;

/**
 * \brief 初始化xfb字体
 *
 * \param[in] p_xbf_data 字体数据
 *
 * \return 无
 */
void xbf_init (void *p_xbf_data);

#endif /* __FONT_XBF_H */

/* end of file */
