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
 * \brief ��Ƶ��������
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-10 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __PLAY_TASK_H
#define __PLAY_TASK_H

#include "apollo.h"
#include "aw_sem.h"

#include <stdint.h>
#include "play_list.h"

#define PALY_STATE_PAUSE      (0)                  /**< \brief ��ͣ���� */
#define PALY_STATE_PLAY       (1)                  /**< \brief ���� */
#define PALY_STATE_STOP       (2)                  /**< \brief ֹͣ */

/**
 * \brief ���ֲ����¼�
 */
typedef enum play_event {

    PLAY_EVENT_NULL     = 0,              /**< \brief ���¼� */
    PLAY_EVENT_PREV     = AW_SBF(1UL, 0), /**< \brief ��һ�� */
    PLAY_EVENT_NEXT     = AW_SBF(1UL, 1), /**< \brief ��һ�� */
    PLAY_EVENT_CHOOSE   = AW_SBF(1UL, 2), /**< \brief ָ������ */
    PLAY_EVENT_SET_TIME = AW_SBF(1UL, 3), /**< \brief ���õ�ǰ����ʱ�� */
    PLAY_EVENT_STOP     = AW_SBF(1UL, 4), /**< \brief ֹͣ���� */

} play_event_t;

/**
 * \brief ���ֲ��ſ��ƽṹ
 */
typedef struct play_ctrl {

    int          fd;            /**< \brief ��ǰ�����ļ� */
    uint8_t      event;         /**< \brief �¼� */
    uint8_t      paly_state;    /**< \brief ����״̬ 0:��ͣ 1:����  2:ֹͣ */
    uint8_t      paly_mode;     /**< \brief ����ģʽ 0:������� 1:˳�򲥷� 2:����ѭ�� */
    uint32_t     current_time;  /**< \brief ��ǰ����ʱ��  ��λ:ms*/
    int32_t      play_idx;      /**< \brief ��ǰ���ڲ��ŵ��������� */
    music_info_t info;          /**< \brief ��ǰ������Ϣ */

    AW_SEMB_DECL(gui_ok);       /**< \brief �ź�������GUI��ʼ����ɺ��ʹ��ź��� */

} play_ctrl_t;

/**
 * \brief ��ʼ����Ƶ��������
 */
void play_task_init(void);

/**
 * \brief ���Ͳ����¼�
 *
 * \param[in] event �����͵��¼�
 */
void play_event_send (play_event_t event);

/**
 * \brief ֹͣ����
 */
void play_stop (void);

#endif /* __PLAY_TASK_H */

/* end of file */
