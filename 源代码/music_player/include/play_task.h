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

#ifndef __PLAY_TASK_H
#define __PLAY_TASK_H

#include "apollo.h"
#include "aw_sem.h"

#include <stdint.h>
#include "play_list.h"

#define PALY_STATE_PAUSE      (0)                  /**< \brief 暂停播放 */
#define PALY_STATE_PLAY       (1)                  /**< \brief 播放 */
#define PALY_STATE_STOP       (2)                  /**< \brief 停止 */

/**
 * \brief 音乐播放事件
 */
typedef enum play_event {

    PLAY_EVENT_NULL     = 0,              /**< \brief 无事件 */
    PLAY_EVENT_PREV     = AW_SBF(1UL, 0), /**< \brief 上一曲 */
    PLAY_EVENT_NEXT     = AW_SBF(1UL, 1), /**< \brief 下一曲 */
    PLAY_EVENT_CHOOSE   = AW_SBF(1UL, 2), /**< \brief 指定歌曲 */
    PLAY_EVENT_SET_TIME = AW_SBF(1UL, 3), /**< \brief 设置当前播放时间 */
    PLAY_EVENT_STOP     = AW_SBF(1UL, 4), /**< \brief 停止播放 */

} play_event_t;

/**
 * \brief 音乐播放控制结构
 */
typedef struct play_ctrl {

    int          fd;            /**< \brief 当前播放文件 */
    uint8_t      event;         /**< \brief 事件 */
    uint8_t      paly_state;    /**< \brief 播放状态 0:暂停 1:播放  2:停止 */
    uint8_t      paly_mode;     /**< \brief 播放模式 0:随机播放 1:顺序播放 2:单曲循环 */
    uint32_t     current_time;  /**< \brief 当前播放时间  单位:ms*/
    int32_t      play_idx;      /**< \brief 当前正在播放的音乐索引 */
    music_info_t info;          /**< \brief 当前音乐信息 */

    AW_SEMB_DECL(gui_ok);       /**< \brief 信号量，当GUI初始化完成后发送此信号量 */

} play_ctrl_t;

/**
 * \brief 初始化音频播放任务
 */
void play_task_init(void);

/**
 * \brief 发送播放事件
 *
 * \param[in] event 待发送的事件
 */
void play_event_send (play_event_t event);

/**
 * \brief 停止播放
 */
void play_stop (void);

#endif /* __PLAY_TASK_H */

/* end of file */
