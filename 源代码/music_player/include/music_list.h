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
 * \brief ���ֲ������б����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-02-19 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __MUSIC_LIST_H
#define __MUSIC_LIST_H

#include <stdint.h>
#include "WM.h"

/**
 * \brief ���²����б����ʾ
 */
void refresh_list (void);

/**
 * \brief ���´��������б���
 */
void winlist_recreate (void);

/**
 * \brief ���������б���
 *
 * \param[in] parent �����ھ��
 *
 * \return �����б�����
 */
WM_HWIN music_list_create (WM_HWIN parent);

#endif /* __MUSIC_LIST_H */

/* end of file */
