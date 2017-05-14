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
 * \brief ϵͳ���ý���
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-10 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __SETTING_H
#define __SETTING_H

#include "WM.h"

/**
 * \brief �������ô���
 *
 * \param[in] parent ������
 *
 * \return ���ô�����
 */
WM_HWIN setting_create (WM_HWIN parent);

/**
 * \brief ˢ��ѡ��״̬
 */
void setting_refresh (void);

#endif /* __SETTING_H */

/* end of file */
