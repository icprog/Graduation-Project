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

/*******************************************************************************
  ͷ�ļ�����
*******************************************************************************/

#include "apollo.h"
#include "play_task.h"

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_time.h"
#include "GUI.h"
#include "play_list.h"
#include "wav_decoder.h"
#include "mp3_decoder.h"
#include "flac_decoder.h"
#include "resource.h"
#include "player_global.h"
#include "main_menu.h"

/*******************************************************************************
  �궨��
*******************************************************************************/

#define PLAY_TASK_PRIO          8             /**< \brief �������ȼ� */
#define PLAY_TASK_STACK_SIZE    (1024 * 100)  /**< \brief ����ջ��С */

/** \brief ��������(��̬) */
AW_TASK_DECL_STATIC(play_task, PLAY_TASK_STACK_SIZE);

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

play_ctrl_t g_play_ctrl; /**< \brief ���ֲ��ſ��ƽṹ */

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/**
 * \brief ���Ͳ����¼�
 *
 * \param[in] event �����͵��¼�
 *
 * \return ��
 */
void play_event_send (play_event_t event)
{
    g_play_ctrl.event |= event;
}

/**
 * \brief ֹͣ����
 */
void play_stop (void)
{
    uint8_t old_paly_state = g_play_ctrl.paly_state;

    g_play_ctrl.event |= PLAY_EVENT_STOP;
    while (!(PALY_STATE_STOP & g_play_ctrl.paly_state)) {
        aw_mdelay(10);
    }

    g_play_ctrl.paly_state = old_paly_state;
    g_play_ctrl.play_idx = 0;
}

/**
 * \brief ���ֲ����������
 */
void play_task_entry (void *p_arg)
{
    aw_time_t time;

    /* �ȴ�GUI��ʼ����� */
    AW_SEMB_TAKE(g_play_ctrl.gui_ok, AW_SEM_WAIT_FOREVER);

    /* ������Ƶ�豸 */
    aw_imx28_snd_mkdev(&g_snd_dev,                     /* ��Ƶ�豸 */
                       0,                              /* SAIF0 */
                       SOUND_DEVICE,                   /* ���� */
                       256,                            /* һ���жϴ������ݴ�С */
                       128,                            /* �ڲ����λ�������С */
                       16,                             /* λ�� */
                       2,                              /* ͨ���� */
                       44100,                          /* ������ */
                       AW_IMX28_SND_STREAM_PLAYBACK);  /* ������Ƶ */

    /* ������ */
    if (aw_imx28_snd_open(&g_snd_dev) != AW_OK) {
        AW_ERRF(("aw_imx28_snd_open failed\n"));
    }

    AW_FOREVER {

        /* ���������Ϸ��Լ�� */
        if ((0 == g_list_state.play_list_count) ||
            (g_play_ctrl.play_idx >= g_list_state.play_list_count)) {
            g_play_ctrl.play_idx = 0;
            g_play_ctrl.paly_state = PALY_STATE_PAUSE;
        }

        if (g_play_ctrl.event & PLAY_EVENT_STOP) {
            g_play_ctrl.paly_state = PALY_STATE_STOP;
            g_play_ctrl.event &= ~PLAY_EVENT_STOP;
        }

        if (g_play_ctrl.paly_state != PALY_STATE_PLAY) {
            aw_mdelay(100);
        } else {
            refresh_volume();

            /* ���浱ǰ���������Լ��������� */
            g_config_info.volume = volume_get();
            g_config_info.play_idx = g_play_ctrl.play_idx;
            config_write(&g_config_info);

            /* ��ʼ������ */
            g_play_ctrl.event = PLAY_EVENT_NULL;
            g_play_ctrl.current_time = 0;
            g_play_ctrl.info = g_play_list[g_play_ctrl.play_idx];

            AW_INFOF(("play music %s\n", g_play_ctrl.info.name));

            /* ���ݸ����ĸ�ʽ���ò�ͬ�Ľ����� */
            switch (g_play_ctrl.info.type) {

            case MUSIC_TYPE_WAV:  /* WAV���� */
                if (AW_OK != wav_play(g_play_ctrl.info.file_name)) {
                    g_play_ctrl.play_idx++;
                    continue;
                }
                break;

            case MUSIC_TYPE_MP3:  /* MP3���� */
                if (AW_OK != mp3_play(g_play_ctrl.info.file_name)) {
                    g_play_ctrl.play_idx++;
                    continue;
                }

                break;

            case MUSIC_TYPE_FLAC: /* FLAC���� */
                if (AW_OK != flac_play(g_play_ctrl.info.file_name)) {
                    g_play_ctrl.play_idx++;
                    continue;
                }
                break;

            default:
                ; /* VOID */
            }

            /* ������һ������ */
            if (g_play_ctrl.event & PLAY_EVENT_CHOOSE) {
                ; /* VOID */
            } else {
                switch (g_play_ctrl.paly_mode) {

                case PALY_MODE_RANDOM: /* ������� */

                    /* �Ե�ǰʱ��Ϊ���Ӳ�������� */
                    aw_time(&time);
                    srandom(time);
                    g_play_ctrl.play_idx = (double)random() / RAND_MAX *
                                           (g_list_state.play_list_count - 1);
                    break;

                case PALY_MODE_ORDER:  /* ˳�򲥷� */

                    /* ��һ�� */
                    if (g_play_ctrl.event & PLAY_EVENT_PREV) {
                        g_play_ctrl.play_idx--;
                        if (g_play_ctrl.play_idx < 0) {
                            g_play_ctrl.play_idx =
                                               g_list_state.play_list_count - 1;
                        }

                    /* ��һ�� */
                    } else {
                        g_play_ctrl.play_idx++;
                        if (g_play_ctrl.play_idx >=
                            g_list_state.play_list_count) {
                            g_play_ctrl.play_idx = 0;
                        }
                    }
                    break;

                case PALY_MODE_SINGLE: /* ����ѭ�� */

                    /* ��һ�� */
                    if (g_play_ctrl.event & PLAY_EVENT_PREV) {
                        g_play_ctrl.play_idx--;
                        if (g_play_ctrl.play_idx < 0) {
                            g_play_ctrl.play_idx =
                                               g_list_state.play_list_count - 1;
                        }

                    /* ��һ�� */
                    } else if (g_play_ctrl.event & PLAY_EVENT_NEXT) {
                        g_play_ctrl.play_idx++;
                        if (g_play_ctrl.play_idx >=
                            g_list_state.play_list_count) {
                            g_play_ctrl.play_idx = 0;
                        }

                    /* ����ѭ�� */
                    } else {
                        ; /* VOID */
                    }
                    break;

                default:
                    ; /* VOID */
                }
            }
        }
    }
}

/**
 * \brief ��ʼ����Ƶ��������
 */
void play_task_init (void)
{

    /* ��ʼ������ */
    AW_TASK_INIT(play_task,            /* ����ʵ�� */
                 "play_task",          /* �������� */
                 PLAY_TASK_PRIO,       /* �������ȼ� */
                 PLAY_TASK_STACK_SIZE, /* �����ջ��С */
                 play_task_entry,      /* ������ں��� */
                 (void *)NULL);        /* ������ڲ��� */

    /* �������� */
    AW_TASK_STARTUP(play_task);
}

/* end of file */
