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
 * \brief ����������
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-10 �Ž���, �������ļ�
 * \endinternal
 */

/*******************************************************************************
  ͷ�ļ�����
*******************************************************************************/

#include "apollo.h"
#include "touch_screen.h"

#include <stdint.h>
#include <stddef.h>
#include "aw_task.h"
#include "aw_touch_screen.h"
#include "aw_delay.h"
#include "GUI.h"

/*******************************************************************************
  �궨��
*******************************************************************************/

#define TOUCH_SCREEN_TASK_PRIO          14          /**< \brief �������ȼ� */
#define TOUCH_SCREEN_TASK_STACK_SIZE    (1024 * 4)  /**< \brief ����ջ��С */

/** \brief ��������(��̬) */
AW_TASK_DECL_STATIC(touch_screen_task, TOUCH_SCREEN_TASK_STACK_SIZE);

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/**
 * \brief �����������������
 *
 * \param[in] p_arg ����
 *
 * \return ��
 */
void touch_screen_task_entry (void *p_arg)
{
    GUI_PID_STATE         th;
    struct aw_touch_state ts_state;

    th.Layer = 0;

    AW_FOREVER {
        aw_touch_get_log_state(&ts_state);  /* ��ȡ�������豸��ǰ�߼�����״̬ */

        th.x = ts_state.x;
        th.y = ts_state.y;
        th.Pressed = ts_state.pressed;

        GUI_TOUCH_StoreStateEx(&th);        /* �ѽ���洢��emwin */

        aw_touch_exec();                    /* ���ڶԴ����豸������ѯ */

        aw_mdelay(10);
    }
}

/**
 * \brief ��ʼ����������������
 */
void touch_screen_task_init (void)
{
    /* ��ʼ������ */
    AW_TASK_INIT(touch_screen_task,            /* ����ʵ�� */
                 "touch_screen_task",          /* �������� */
                 TOUCH_SCREEN_TASK_PRIO,       /* �������ȼ� */
                 TOUCH_SCREEN_TASK_STACK_SIZE, /* �����ջ��С */
                 touch_screen_task_entry,      /* ������ں��� */
                 (void *)NULL);                /* ������ڲ��� */

    /* �������� */
    AW_TASK_STARTUP(touch_screen_task);
}

/* end of file */
