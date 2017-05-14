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
 * \brief 自定义对话框
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-06 张进科, 创建该文件
 * \endinternal
 */

#ifndef __CUSTOM_DIALOG_H
#define __CUSTOM_DIALOG_H

#include <stdint.h>
#include "WM.h"
#include "GUI.h"

int g_custom_dialog_state; /**< \brief 对话框状态 0:未创建 1:已创建 */

/**
 * \brief 对话框属性结构体
 */
typedef struct dialog_info {

    uint8_t   blocking;             /**< \brief 是否阻塞 0: 非阻塞 1: 阻塞 */
    uint8_t   modal;                /**< \brief 模态 0: 非模态 1: 模态 */
    uint8_t   btn_num;              /**< \brief 按键数量 [1-2] */
    uint32_t  btn_height;           /**< \brief 按键高度 (像素) */
    GUI_COLOR color_title;          /**< \brief 标题文本颜色 */
    GUI_COLOR color_text;           /**< \brief 信息文本颜色 */
    GUI_COLOR color_background;     /**< \brief 背景颜色 */
    GUI_COLOR color_line;           /**< \brief 线条颜色 */
    GUI_COLOR btn_color_text[2];    /**< \brief 按键文本颜色 */
    GUI_COLOR btn_color_normal[2];  /**< \brief 按键未按下颜色 */
    GUI_COLOR btn_color_pressed[2]; /**< \brief 按键按下颜色 */
    GUI_FONT *font_title;           /**< \brief 标题字体 */
    GUI_FONT *font_text;            /**< \brief 信息字体 */
    GUI_FONT *font_btn[2];          /**< \brief 按键文本字体 */

    GUI_RECT rect;                  /**< \brief 对话框大小 */
    GUI_RECT title_rect;            /**< \brief 标题显示范围 */
    GUI_RECT text_rect;             /**< \brief 信息文本显示范围 */

    int  r;                         /**< \brief 圆角半径 */
    char title[256];                /**< \brief 标题 */
    char text[256];                 /**< \brief 信息 */
    char btn_text[2][64];           /**< \brief 按键文本 */

} dialog_info_t; 

/**
 * \brief 初始化对话框信息结构体
 *
 * \param[in] p_info 对话框信息
 *
 * \return 无
 */
void dialog_info_init (dialog_info_t *p_info);

/**
 * \brief 提取指定起始符与结束符之间的字符串
 *
 * \param[in] x0       父窗口在窗口坐标中的左上 X 位置
 * \param[in] y0       父窗口在窗口坐标中的左上 Y 位置
 * \param[in] x_size   窗口的 X 尺寸。如果为 0，则用父窗口客户区的 X 尺寸
 * \param[in] y_size   窗口的 Y 尺寸。如果为 0，则用父窗口客户区的 Y 尺寸
 * \param[in] h_parent 父窗口的句柄
 * \param[in] style    窗口创建标识(请参阅 WM_CreateWindow())
 * \param[in] info     对话框属性
 *
 * \retval 0 未点击按钮
 * \retval 1 点击按钮0
 * \retval 2 点击按钮1
 */
int dialog_show (int            x0,
                 int            y0,
                 int            x_size,
                 int            y_size,
                 GUI_HWIN       h_parent,
                 uint32_t       style,
                 dialog_info_t *p_info);

/**
 * \brief 关闭对话框
 */
void dialog_close (void);

/**
 * \brief 设置text
 *
 * \param[in] text 待设置的text
 *
 * \return 无
 */
void text_set (const char *p_text);

#endif /* __CUSTOM_DIALOG_H */

/* end of file */
