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
 * \brief ϵͳ��ʼ��
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-10 �Ž���, �������ļ�
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

    /* ��ʼ���������Դ */
    resource_init();

    /* ��ʼ��GUI���� */
    player_gui_task_init();

    /* ��ʼ������������ */
    touch_screen_task_init();

    /* ��ʼ����Ƶ�������� */
    play_task_init();
}

/* end of file */
