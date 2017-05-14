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
 * \brief emwin xbf����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-04 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __FONT_XBF_H
#define __FONT_XBF_H

#include <stdint.h>
#include "GUI.h"  

/** \brief xbf���� */
extern GUI_FONT g_font11, g_font13, g_font14, g_font18;

/**
 * \brief ��ʼ��xfb����
 *
 * \param[in] p_xbf_data ��������
 *
 * \return ��
 */
void xbf_init (void *p_xbf_data);

#endif /* __FONT_XBF_H */

/* end of file */
