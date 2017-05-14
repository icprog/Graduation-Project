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
 * \brief 系统设置界面
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-10 张进科, 创建该文件
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "setting.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "DIALOG.h"  
#include "GUI.h"  
#include "main_menu.h"
#include "system_resource.h"
#include "custom_dialog.h"  
#include "resource.h"
#include "play_list.h"
#include "player_global.h"
#include "font_xbf.h"

/*******************************************************************************
  宏定义
*******************************************************************************/

#define ID_BUTTON_0    (GUI_ID_USER + 0x11) /**< \brief 取消按钮ID */
#define ID_BUTTON_1    (GUI_ID_USER + 0x12) /**< \brief 确认按钮ID */
#define ID_BUTTON_2    (GUI_ID_USER + 0x13) /**< \brief 按钮ID */
#define ID_BUTTON_3    (GUI_ID_USER + 0x14) /**< \brief 按钮ID */

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

static WM_HWIN g_h_winpar   = 0; /**< \brief 父窗体句柄 */
static WM_HWIN g_h_win_0    = 0; /**< \brief 窗体句柄 */
static WM_HWIN g_h_button_0 = 0; /**< \brief 取消按钮ID */
static WM_HWIN g_h_button_1 = 0; /**< \brief 确认按钮ID */
static WM_HWIN g_h_button_2 = 0; /**< \brief 按钮ID */
static WM_HWIN g_h_button_3 = 0; /**< \brief 按钮ID */
                            
static uint8_t g_btn_state0 = 0; /**< \brief 按钮状态 0: off 1: on */
static uint8_t g_btn_state1 = 0; /**< \brief 按钮状态 0: off 1: on */

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 刷新选项状态
 */
void setting_refresh (void)
{
    /* 获取配置文件 */
    config_read(&g_config_info);
    if ((1 == g_config_info.is_effect) &&
        (g_config_info.play_idx < g_list_state.play_list_count)) {
        g_btn_state0 = g_config_info.lrc_show;
        g_btn_state1 = g_config_info.singer_show;
    } else {
        g_btn_state0 = 1;
        g_btn_state1 = 1;
    }
}

/**
 * \brief 按键回调函数
 *
 * \param[in] p_msg 指向WM_MESSAGE结构的指针
 *
 * \return 无
 */
static void button_callback (WM_MESSAGE *p_msg)
{
    WM_HWIN   h_win = p_msg->hWin;
    GUI_RECT  rect;
    GUI_COLOR color;

    switch (p_msg->MsgId) {

    case WM_PAINT:
        WM_GetClientRect(&rect);
        if (h_win == g_h_button_0) {
            if (BUTTON_IsPressed(h_win)) {
                color = 0x292421;
            }
            else {
                color = 0x544A43;
            }
            GUI_SetColor(color);
            GUI_AA_FillRoundedRectEx(&rect, 5);
            GUI_FillRect(rect.x0,
                         rect.y0,
                         rect.x0 + 5,
                         rect.y0 + 5);
            GUI_FillRect(rect.x1 - 5,
                         rect.y0,
                         rect.x1,
                         rect.y0 + 5);
            GUI_FillRect(rect.x1 - 5,
                         rect.y1 - 5,
                         rect.x1,
                         rect.y1);

            GUI_SetFont(&g_font14);
            GUI_SetTextMode(GUI_TEXTMODE_TRANS);
            GUI_SetColor(GUI_WHITE);
            GUI_DispStringInRect("取消",
                                &rect,
                                 GUI_TA_VCENTER | GUI_TA_HCENTER);
        }
        else if (h_win == g_h_button_1) {
            if (BUTTON_IsPressed(h_win)) {
                color = 0x292421;
            }
            else {
                color = 0x544A43;
            }
            GUI_SetColor(color);
            GUI_AA_FillRoundedRectEx(&rect, 5);
            GUI_FillRect(rect.x0,
                         rect.y0,
                         rect.x0 + 5,
                         rect.y0 + 5);
            GUI_FillRect(rect.x1 - 5,
                         rect.y0,
                         rect.x1,
                         rect.y0 + 5);
            GUI_FillRect(rect.x0,
                         rect.y1 - 5,
                         rect.x0 + 5,
                         rect.y1);

            GUI_SetFont(&g_font14);
            GUI_SetTextMode(GUI_TEXTMODE_TRANS);
            GUI_SetColor(GUI_WHITE);
            GUI_DispStringInRect("确定",
                                &rect,
                                 GUI_TA_VCENTER | GUI_TA_HCENTER);
        }
        else if (h_win == g_h_button_2) {
            if (0 == g_btn_state0) {
                GUI_DrawBitmap(&g_off, 0, 0);
            } else {
                GUI_DrawBitmap(&g_on, 0, 0);
            }
        }
        else if (h_win == g_h_button_3) {
            if (0 == g_btn_state1) {
                GUI_DrawBitmap(&g_off, 0, 0);
            } else {
                GUI_DrawBitmap(&g_on, 0, 0);
            }
        }
        else {
        }

        break;

    default:
        BUTTON_Callback(p_msg);
    }
}

/**
 * \brief 设置窗体回调函数
 *
 * \param[in] p_msg 指向 WM_MESSAGE结构的指针
 *
 * \return 无
 */
static void dlg_callback (WM_MESSAGE *p_msg) 
{
    int     ncode;
    int     id;

    GUI_RECT rect;
    GUI_RECT rect_back;

    dialog_info_t info;

    GUI_RECT title_rect  = {5, 10, 344, 50};
    GUI_RECT info_rect   = {5, 50, 344, 110};
    GUI_RECT lable_rect0 = {5, 146 - 5, 87, 146 + 5};
    GUI_RECT lable_rect1 = {175, 146 - 5, 262, 146 + 5};


    switch (p_msg->MsgId) {

    case WM_INIT_DIALOG: /* 窗口初始化 */
        g_h_win_0 = p_msg->hWin;
        g_h_button_0 = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_0);
        g_h_button_1 = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_1);
        g_h_button_2 = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_2);
        g_h_button_3 = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_3);

        break;

    case WM_NOTIFY_PARENT:
        id    = WM_GetId(p_msg->hWinSrc);
        ncode = p_msg->Data.v;
        switch(id) {

        case ID_BUTTON_0:
            switch(ncode) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                WM_HideWindow(g_h_win_0);
                setting_refresh();
                break;

            default: ;
            }
            break;

        case ID_BUTTON_1:
            switch(ncode) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                WM_HideWindow(g_h_win_0);

                /* 获取配置文件 */
                config_read(&g_config_info);
                if ((1 == g_config_info.is_effect) &&
                    (g_config_info.play_idx < g_list_state.play_list_count)) {
                    g_config_info.volume = volume_get();
                    g_config_info.play_idx = g_play_ctrl.play_idx;
                    g_config_info.lrc_show = g_btn_state0;
                    g_config_info.singer_show = g_btn_state1;

                    config_write(&g_config_info);
                } else {
                    g_play_ctrl.play_idx = 0;
                    g_config_info.lrc_show = 1;
                    g_config_info.singer_show = 1;
                }
                break;

            default: ;
            }
            break;

        case ID_BUTTON_2:
            switch(ncode) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                g_btn_state0 = !g_btn_state0;
                break;

            default: ;
            }
            break;

        case ID_BUTTON_3:
            switch(ncode) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                g_btn_state1 = !g_btn_state1;
                break;

            default: ;
            }
            break;
        }
        break;

    case WM_PAINT:
        WM_GetClientRect(&rect);

        /* 绘制背景 */
        rect_back = rect;
        rect_back.y1 -= 5;
        GUI_SetColor(0x544A43);
        GUI_AA_FillRoundedRectEx(&rect_back, 5);

        /* 绘制分割线 */
        GUI_SetColor(0x999999);
        GUI_DrawHLine(172, rect.x0, rect.x1);
        GUI_DrawVLine(174, 172, rect.y1);

        GUI_SetTextMode(GUI_TEXTMODE_TRANS);

        /* 绘制title */
        GUI_SetColor(GUI_WHITE);
        GUI_SetFont(&g_font18);
        GUI_DispStringInRect("系统设置",
                            &title_rect,
                             GUI_TA_VCENTER | GUI_TA_HCENTER);

        /* 绘制info */
        GUI_SetColor(GUI_WHITE);
        GUI_SetFont(&g_font14);
        GUI_DispStringInRect("基于i.MX283处理器的音乐播放器设计\n" \
                             "重庆理工大学\n" \
                             "机械工程学院    " \
                             "测控技术与仪器\n" \
                             "张进科    " \
                             "11307030328",
                            &info_rect,
                             GUI_TA_VCENTER | GUI_TA_HCENTER);

        /* 绘制lable0 */
        GUI_SetColor(GUI_WHITE);
        GUI_SetFont(&g_font14);
        GUI_DispStringInRect("显示歌词",
                            &lable_rect0,
                             GUI_TA_VCENTER | GUI_TA_HCENTER);

        /* 绘制lable1 */
        GUI_SetColor(GUI_WHITE);
        GUI_SetFont(&g_font14);
        GUI_DispStringInRect("显示歌手图片",
                            &lable_rect1,
                             GUI_TA_VCENTER | GUI_TA_HCENTER);
        break;

    default:
        WM_DefaultProc(p_msg);
        break;
    }
}

/**
 * \brief 创建设置窗体
 *
 * \param[in] parent 父窗体
 *
 * \return 设置窗体句柄
 */
WM_HWIN setting_create (WM_HWIN parent)
{
    g_h_winpar = parent;

    g_h_win_0 = WM_CreateWindowAsChild(
                    65,              /* 父窗口在窗口坐标中的左上 X 位置 */
                    34,              /* 父窗口在窗口坐标中的左上 Y 位置 */
                    350,             /* 窗口的 X 尺寸 */
                    204,             /* 窗口的 Y 尺寸 */
                    g_h_winpar,      /* 父窗口的句柄 */
                    WM_CF_SHOW | WM_CF_HASTRANS , /* 窗口创建标识 */
                    dlg_callback,    /* 回调例程的指针 */ 
                    0);              /* 要分配的额外字节数 */

    g_h_button_0 = BUTTON_CreateEx(0, 
                                   173, 
                                   174, 
                                   31, 
                                   g_h_win_0, 
                                   WM_CF_SHOW, 
                                   0, 
                                   ID_BUTTON_0);

    g_h_button_1 = BUTTON_CreateEx(175, 
                                   173, 
                                   175, 
                                   31, 
                                   g_h_win_0,
                                   WM_CF_SHOW, 
                                   0, 
                                   ID_BUTTON_1);

    g_h_button_2 = BUTTON_CreateEx(81, 
                                   135, 
                                   64, 
                                   22, 
                                   g_h_win_0, 
                                   WM_CF_SHOW | WM_CF_HASTRANS, 
                                   0, 
                                   ID_BUTTON_2);

    g_h_button_3 = BUTTON_CreateEx(265, 
                                   135, 
                                   64, 
                                   22, 
                                   g_h_win_0, 
                                   WM_CF_SHOW | WM_CF_HASTRANS, 
                                   0, 
                                   ID_BUTTON_3);

    WM_SetCallback(g_h_button_0, button_callback);
    WM_SetCallback(g_h_button_1, button_callback);
    WM_SetCallback(g_h_button_2, button_callback);
    WM_SetCallback(g_h_button_3, button_callback);

    return g_h_win_0;
}

/* end of file */
