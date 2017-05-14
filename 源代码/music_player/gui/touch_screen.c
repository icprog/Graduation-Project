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
 * \brief 触摸屏处理
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
#include "touch_screen.h"

#include <stdint.h>
#include <stddef.h>
#include "aw_task.h"
#include "aw_touch_screen.h"
#include "aw_delay.h"
#include "GUI.h"

/*******************************************************************************
  宏定义
*******************************************************************************/

#define TOUCH_SCREEN_TASK_PRIO          14          /**< \brief 任务优先级 */
#define TOUCH_SCREEN_TASK_STACK_SIZE    (1024 * 4)  /**< \brief 任务栈大小 */

/** \brief 定义任务(静态) */
AW_TASK_DECL_STATIC(touch_screen_task, TOUCH_SCREEN_TASK_STACK_SIZE);

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 触摸屏处理任务入口
 *
 * \param[in] p_arg 参数
 *
 * \return 无
 */
void touch_screen_task_entry (void *p_arg)
{
    GUI_PID_STATE         th;
    struct aw_touch_state ts_state;

    th.Layer = 0;

    AW_FOREVER {
        aw_touch_get_log_state(&ts_state);  /* 获取抽象触摸设备当前逻辑坐标状态 */

        th.x = ts_state.x;
        th.y = ts_state.y;
        th.Pressed = ts_state.pressed;

        GUI_TOUCH_StoreStateEx(&th);        /* 把结果存储进emwin */

        aw_touch_exec();                    /* 定期对触摸设备进行轮询 */

        aw_mdelay(10);
    }
}

/**
 * \brief 初始化触摸屏处理任务
 */
void touch_screen_task_init (void)
{
    /* 初始化任务 */
    AW_TASK_INIT(touch_screen_task,            /* 任务实体 */
                 "touch_screen_task",          /* 任务名字 */
                 TOUCH_SCREEN_TASK_PRIO,       /* 任务优先级 */
                 TOUCH_SCREEN_TASK_STACK_SIZE, /* 任务堆栈大小 */
                 touch_screen_task_entry,      /* 任务入口函数 */
                 (void *)NULL);                /* 任务入口参数 */

    /* 启动任务 */
    AW_TASK_STARTUP(touch_screen_task);
}

/* end of file */
