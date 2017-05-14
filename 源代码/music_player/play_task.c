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
 * \brief 音频播放任务
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-10 张进科, 创建该文件
 * \endinternal
 */

/*******************************************************************************
  头文件包含
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
  宏定义
*******************************************************************************/

#define PLAY_TASK_PRIO          8             /**< \brief 任务优先级 */
#define PLAY_TASK_STACK_SIZE    (1024 * 100)  /**< \brief 任务栈大小 */

/** \brief 定义任务(静态) */
AW_TASK_DECL_STATIC(play_task, PLAY_TASK_STACK_SIZE);

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

play_ctrl_t g_play_ctrl; /**< \brief 音乐播放控制结构 */

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 发送播放事件
 *
 * \param[in] event 待发送的事件
 *
 * \return 无
 */
void play_event_send (play_event_t event)
{
    g_play_ctrl.event |= event;
}

/**
 * \brief 停止播放
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
 * \brief 音乐播放任务入口
 */
void play_task_entry (void *p_arg)
{
    aw_time_t time;

    /* 等待GUI初始化完成 */
    AW_SEMB_TAKE(g_play_ctrl.gui_ok, AW_SEM_WAIT_FOREVER);

    /* 构建音频设备 */
    aw_imx28_snd_mkdev(&g_snd_dev,                     /* 音频设备 */
                       0,                              /* SAIF0 */
                       SOUND_DEVICE,                   /* 声卡 */
                       256,                            /* 一次中断传输数据大小 */
                       128,                            /* 内部环形缓冲区大小 */
                       16,                             /* 位数 */
                       2,                              /* 通道数 */
                       44100,                          /* 采样率 */
                       AW_IMX28_SND_STREAM_PLAYBACK);  /* 播放音频 */

    /* 打开声卡 */
    if (aw_imx28_snd_open(&g_snd_dev) != AW_OK) {
        AW_ERRF(("aw_imx28_snd_open failed\n"));
    }

    AW_FOREVER {

        /* 播放索引合法性检查 */
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

            /* 保存当前播放索引以及音量配置 */
            g_config_info.volume = volume_get();
            g_config_info.play_idx = g_play_ctrl.play_idx;
            config_write(&g_config_info);

            /* 初始化参数 */
            g_play_ctrl.event = PLAY_EVENT_NULL;
            g_play_ctrl.current_time = 0;
            g_play_ctrl.info = g_play_list[g_play_ctrl.play_idx];

            AW_INFOF(("play music %s\n", g_play_ctrl.info.name));

            /* 根据歌曲的格式调用不同的解码器 */
            switch (g_play_ctrl.info.type) {

            case MUSIC_TYPE_WAV:  /* WAV播放 */
                if (AW_OK != wav_play(g_play_ctrl.info.file_name)) {
                    g_play_ctrl.play_idx++;
                    continue;
                }
                break;

            case MUSIC_TYPE_MP3:  /* MP3播放 */
                if (AW_OK != mp3_play(g_play_ctrl.info.file_name)) {
                    g_play_ctrl.play_idx++;
                    continue;
                }

                break;

            case MUSIC_TYPE_FLAC: /* FLAC播放 */
                if (AW_OK != flac_play(g_play_ctrl.info.file_name)) {
                    g_play_ctrl.play_idx++;
                    continue;
                }
                break;

            default:
                ; /* VOID */
            }

            /* 计算下一曲索引 */
            if (g_play_ctrl.event & PLAY_EVENT_CHOOSE) {
                ; /* VOID */
            } else {
                switch (g_play_ctrl.paly_mode) {

                case PALY_MODE_RANDOM: /* 随机播放 */

                    /* 以当前时间为种子产生随机数 */
                    aw_time(&time);
                    srandom(time);
                    g_play_ctrl.play_idx = (double)random() / RAND_MAX *
                                           (g_list_state.play_list_count - 1);
                    break;

                case PALY_MODE_ORDER:  /* 顺序播放 */

                    /* 上一曲 */
                    if (g_play_ctrl.event & PLAY_EVENT_PREV) {
                        g_play_ctrl.play_idx--;
                        if (g_play_ctrl.play_idx < 0) {
                            g_play_ctrl.play_idx =
                                               g_list_state.play_list_count - 1;
                        }

                    /* 下一曲 */
                    } else {
                        g_play_ctrl.play_idx++;
                        if (g_play_ctrl.play_idx >=
                            g_list_state.play_list_count) {
                            g_play_ctrl.play_idx = 0;
                        }
                    }
                    break;

                case PALY_MODE_SINGLE: /* 单曲循环 */

                    /* 上一曲 */
                    if (g_play_ctrl.event & PLAY_EVENT_PREV) {
                        g_play_ctrl.play_idx--;
                        if (g_play_ctrl.play_idx < 0) {
                            g_play_ctrl.play_idx =
                                               g_list_state.play_list_count - 1;
                        }

                    /* 下一曲 */
                    } else if (g_play_ctrl.event & PLAY_EVENT_NEXT) {
                        g_play_ctrl.play_idx++;
                        if (g_play_ctrl.play_idx >=
                            g_list_state.play_list_count) {
                            g_play_ctrl.play_idx = 0;
                        }

                    /* 单曲循环 */
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
 * \brief 初始化音频播放任务
 */
void play_task_init (void)
{

    /* 初始化任务 */
    AW_TASK_INIT(play_task,            /* 任务实体 */
                 "play_task",          /* 任务名字 */
                 PLAY_TASK_PRIO,       /* 任务优先级 */
                 PLAY_TASK_STACK_SIZE, /* 任务堆栈大小 */
                 play_task_entry,      /* 任务入口函数 */
                 (void *)NULL);        /* 任务入口参数 */

    /* 启动任务 */
    AW_TASK_STARTUP(play_task);
}

/* end of file */
