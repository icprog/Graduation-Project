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
* \brief ���ֲ�����������
*
* \internal
* \par Modification history
* - 1.00 17-02-17 �Ž���, �������ļ�
* \endinternal
*/

#ifndef __MAIN_MENU_H
#define __MAIN_MENU_H

#include <stdint.h>

/**
 * \brief ��ȡ����
 *
 * \return ����
 */
int32_t volume_get (void);

/**
 * \brief ��������
 *
 * \param[in] volume ����
 *
 * \return ��
 */
void volume_set (int32_t volume);

/**
 * \brief ˢ����������
 */
void refresh_volume (void);

/**
 * \brief ��ʼ��GUI����
 */
extern void player_gui_task_init(void);

#endif /* __MAIN_MENU_H */

/* end of file */
