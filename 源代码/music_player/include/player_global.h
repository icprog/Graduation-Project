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
 * \brief ȫ�ֱ���
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-10 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stdint.h>
#include "play_list.h"
#include "play_task.h"
#include "aw_imx28_sound.h"
#include "driver/saif/awbl_imx28_sound.h"

#define PALY_MODE_RANDOM     (0)
#define PALY_MODE_ORDER      (1)
#define PALY_MODE_SINGLE     (2)

#define SOUND_DEVICE         (2) /* ES9018K2M */

/** \brief �������� */
extern uint8_t  g_ttf_res[7569637];
extern uint32_t g_ttf_res_lenth;

/** \brief ����ͼƬ�ļ��� */
extern const char *gp_background_path;

/** \brief ����Ŀ¼ */
extern const char *gp_sdcard_music_path;
extern const char *gp_udisk_music_path;

/** \brief ����ͼƬ���� */
extern uint8_t  g_singer_res[1024 * 1024 * 2];
extern uint32_t g_singer_res_lenth;

/** \brief ������� */
extern uint8_t  g_lrc_res[1024 * 1024];
extern uint32_t g_lrc_res_lenth;

/** \brief �����б� */
extern music_info_t g_play_list[PLAY_LIST_MAX_SIZE];

/** \brief �����б�״̬ */
extern list_state_t g_list_state;

/** \brief ���ֲ��ſ��ƽṹ */
extern play_ctrl_t g_play_ctrl;

/** \brief ��Ƶ�豸�ṹ */
extern aw_imx28_snd_device_t g_snd_dev;

/** \brief ��ǰ����ƶ�����ʱ�� */
extern uint32_t g_move_time;

/** \brief �Ƿ������SD�� 0:δ���� 1:�ѹ��� */
uint8_t g_is_mount_sdcard;

/** \brief �Ƿ������U�� 0:δ���� 1:�ѹ��� */
uint8_t g_is_mount_udisk;

#endif /* __GLOBAL_H */

/* end of file */
