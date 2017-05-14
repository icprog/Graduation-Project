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
 * \brief 音乐播放器列表界面
 *
 * \internal
 * \par Modification history
 * - 1.00 17-02-19 张进科, 创建该文件
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "apollo.h"
#include "music_list.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "io/aw_unistd.h"
#include "DIALOG.h"
#include "GUI.h"
#include "player_global.h"
#include "main_menu.h"
#include "system_resource.h"
#include "custom_dialog.h"
#include "player_global.h"
#include "resource.h"
#include "font_xbf.h"

/*******************************************************************************
  宏定义
*******************************************************************************/

#define ID_WINDOW_0    (GUI_ID_USER + 0x10) /**< \brief 播放列表窗口ID */
#define ID_BUTTON_0    (GUI_ID_USER + 0x11) /**< \brief 删除键ID */
#define ID_BUTTON_1    (GUI_ID_USER + 0x12) /**< \brief 刷新键ID */
#define ID_BUTTON_2    (GUI_ID_USER + 0x13) /**< \brief 关闭键ID */

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

static WM_HWIN g_h_winpar       = 0; /**< \brief 播放列表父窗体句柄 */
static WM_HWIN g_h_winlist_0    = 0; /**< \brief 播放列表窗口句柄 */
static WM_HWIN g_h_button_0     = 0; /**< \brief 删除键句柄 */
static WM_HWIN g_h_button_1     = 0; /**< \brief 刷新键句柄 */
static WM_HWIN g_h_button_2     = 0; /**< \brief 关闭键句柄 */

static WM_HWIN g_h_winhead      = 0; /**< \brief 播放列表容器句柄 */
static WM_HWIN g_h_winlist      = 0; /**< \brief 播放列表句柄，条目由按键组成 */

static uint8_t g_is_not_refresh = 0; /**< \brief 播放列表句柄，条目由按键组成 */

/** \brief 对话框资源表 */
static const GUI_WIDGET_CREATE_INFO g_dialog_create[] = {
    { WINDOW_CreateIndirect, "Window",  ID_WINDOW_0,
        0, 0, 200, 248, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Delete",  ID_BUTTON_0,
        0, 222, 66, 26, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Reflush", ID_BUTTON_1,
        67, 222, 66, 26, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Close",   ID_BUTTON_2,
        134, 222, 66, 26, 0, 0x0, 0 },
};

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 播放列表窗口回调函数
 *
 * \param[in] p_msg 指向 WM_MESSAGE结构的指针
 *
 * \return 无
 */
static void list_callback (WM_MESSAGE *p_msg);

/**
 * \brief 更新播放列表的显示
 */
void refresh_list (void)
{
    GUI_RECT win_list_rect;
    int      current_y0 = 0;
    int      next_y0    = 0;
    int      shift      = 0;
    double   time       = 0.75;
    double   speed      = 0;

    if (g_is_not_refresh) {
        g_is_not_refresh = 0;
        return;
    }

    WM_GetClientRectEx(g_h_winlist, &win_list_rect);
    current_y0 = WM_GetWindowOrgY(g_h_winlist) - 76;

    next_y0 = g_play_ctrl.play_idx * 34 - 34 * 2;
    if (next_y0 <= 0) {
        next_y0 = 0;
    } else if (next_y0 > win_list_rect.y1 - 169) {
        next_y0 = win_list_rect.y1 - 169;
    }

    WM_MoveChildTo(g_h_winlist, 0, -next_y0);
}

/**
 * \brief 重新创建播放列表窗口
 */
void winlist_recreate (void)
{
    WM_DeleteWindow(g_h_winlist);

    g_h_winlist = WM_CreateWindowAsChild(
                    0,             /* 父窗口在窗口坐标中的左上 X 位置 */
                    0,             /* 父窗口在窗口坐标中的左上 Y 位置 */
                    200,           /* 窗口的 X 尺寸 */
                    34 * g_list_state.play_list_count, /* 窗口的 Y 尺寸 */
                    g_h_winhead,   /* 父窗口的句柄 */
                    WM_CF_SHOW | WM_CF_MOTION_Y,
                    list_callback, /* 回调例程的指针 */
                    0);            /* 要分配的额外字节数 */
    refresh_list();
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
    WM_HWIN  h_win = p_msg->hWin;
    GUI_RECT rect;

    switch (p_msg->MsgId) {

    case WM_PAINT:
        WM_GetClientRect(&rect);
        if (h_win == g_h_button_0) {
            if (BUTTON_IsPressed(h_win)) {
                GUI_DrawBitmap(&g_btn_del_pressed, rect.x0, rect.y0);
            }
            else {
                GUI_DrawBitmap(&g_btn_del_normal, rect.x0, rect.y0);
            }
        }
        else if (h_win == g_h_button_1) {
            if (BUTTON_IsPressed(h_win)) {
                GUI_DrawBitmap(&g_btn_refush_pressed, rect.x0, rect.y0);
            }
            else {
                GUI_DrawBitmap(&g_btn_refush_normal, rect.x0, rect.y0);
            }
        }
        else if (h_win == g_h_button_2) {
            if (BUTTON_IsPressed(h_win)) {
                GUI_DrawBitmap(&g_btn_close_pressed, rect.x0, rect.y0);
            }
            else {
                GUI_DrawBitmap(&g_btn_close_normal, rect.x0, rect.y0);
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
 * \brief 按键换肤回调函数
 *
 * \param[in] p_item_draw_info 指向 WIDGET_ITEM_DRAW_INFO结构的指针
 *
 * \return 0 成功绘制
 */
static int button_drawskin (const WIDGET_ITEM_DRAW_INFO *p_item_draw_info)
{
    WM_HWIN         win_handle = p_item_draw_info->hWin;
    GUI_COLOR       color;
    int             idx                = 0;
    int             is_pressed         = 0;
    int             minute = 0, second = 0;
    int             y0                 = 0;
    char            buf[100];
    static uint32_t old_play_idx       = 0;
    GUI_RECT        rect;
    GUI_RECT        win_list_rect;
    GUI_RECT        str_rect;

    WM_GetClientRect(&rect);

    switch (p_item_draw_info->Cmd) {

    case WIDGET_ITEM_DRAW_BACKGROUND:

        /* 获取按钮信息 */
        is_pressed = BUTTON_IsPressed(win_handle);
        BUTTON_GetUserData(win_handle, &idx, sizeof(idx));

        /* 计算歌曲时长 */
        second = g_play_list[idx].total_time / 1000 % 60;
        minute = ((g_play_list[idx].total_time / 1000) -
                 (g_play_list[idx].total_time / 1000 % 60)) / 60 % 60;

        /* 绘制按钮背景色 */
        if (is_pressed) {
            color = 0xCCCCCC;
        } else {
            color = GUI_WHITE;
        }
        GUI_SetBkColor(color);
        GUI_Clear();

        /* 绘制分割线 */
        GUI_SetPenSize(1);
        GUI_SetColor(0xe5e3e2);
        GUI_DrawHLine(rect.y1, rect.x0, rect.x1);

        /* 绘制按钮图标 */
        if (idx == g_play_ctrl.play_idx) {
            GUI_SetColor(0x68d4a0);
            GUI_DrawBitmap(&g_btn_list_play, 7, 8);
            if (0 == g_play_ctrl.paly_state) {
                GUI_DrawBitmap(&g_list_play_white, 14, 13);
            } else {
                GUI_DrawBitmap(&g_list_pause, 13, 14);
            }
        } else {
            GUI_DrawBitmap(&g_list_play_black, 13, 13);
            GUI_SetColor(0xbeb1a8);
        }

        str_rect.y0 = rect.y0 + 1;
        str_rect.y1 = rect.y1 - 1;

        /* 绘制歌曲序号 */
        str_rect.x0 = 27;
        str_rect.x1 = idx < 100 ? 44 : 51;
        sprintf(buf,"%02d", idx);
        GUI_SetFont(&g_font13);
        GUI_DispStringInRect(buf, &str_rect, GUI_TA_VCENTER);

        /* 绘制歌曲名 */
        GUI_SetFont(&g_font14);
        str_rect.x0 = idx < 100 ? 45 : 52;
        str_rect.x1 = 160;
        GUI_DispStringInRect(g_play_list[idx].name, &str_rect, GUI_TA_VCENTER);

        /* 绘制歌曲时长 */
        sprintf(buf,"%02d:%02d", minute, second);
        GUI_SetFont(&g_font13);
        str_rect.x0 = 165;
        str_rect.x1 = 200;
        GUI_DispStringInRect(buf, &str_rect, GUI_TA_VCENTER);

        break;
    }
    return 0;
}

/**
 * \brief 播放列表窗口回调函数
 *
 * \param[in] p_msg 指向 WM_MESSAGE结构的指针
 *
 * \return 无
 */
static void list_callback (WM_MESSAGE *p_msg)
{
    WM_HWIN         h_win    = p_msg->hWin;
    WM_MOTION_INFO *pInfo    = 0;
    WM_HWIN         h_button = 0;
    int             id       = 0;
    int             ncode    = 0;
    GUI_RECT        rect;
    static GUI_RECT rect_clicked;
    static GUI_RECT rect_released;

    WM_GetClientRect(&rect);

    switch (p_msg->MsgId) {

    case WM_NOTIFY_PARENT:
        id    = WM_GetId(p_msg->hWinSrc);
        ncode = p_msg->Data.v;
        switch (ncode) {

        case WM_NOTIFICATION_CLICKED:
            WM_GetWindowRectEx(g_h_winlist, &rect_clicked);
            break;

        case WM_NOTIFICATION_RELEASED:
            WM_GetWindowRectEx(g_h_winlist, &rect_released);
            if (abs(rect_released.y0 - rect_clicked.y0) < 10) {
                g_is_not_refresh = 1;
                if ((id - GUI_ID_BUTTON0) == g_play_ctrl.play_idx) {
                    g_play_ctrl.paly_state = !g_play_ctrl.paly_state;
                } else {
                    g_play_ctrl.play_idx = id - GUI_ID_BUTTON0;
                    g_play_ctrl.paly_state = 1;
                    play_event_send(PLAY_EVENT_CHOOSE); /* 发送选择歌曲事件 */
                }
                WM_Paint(g_h_winpar);
            }
            break;
        }
        break;

    case WM_CREATE:
        for (int i = 0; i < g_list_state.play_list_count; i++) {
            h_button = BUTTON_CreateUser(0, i * 34, 200, 34,
                    h_win, WM_CF_SHOW, 0, GUI_ID_BUTTON0 + i, sizeof(i));
            BUTTON_SetSkin(h_button, button_drawskin);
            BUTTON_SetUserData(h_button, &i, sizeof(i));
        }

        if (g_list_state.play_list_count <= 5) {
            WM_MOTION_SetMoveable(h_win, WM_CF_MOTION_Y, 0);
        } else {
            WM_MOTION_SetMoveable(h_win, WM_CF_MOTION_Y, 1);
        }

        if (0 == g_list_state.play_list_count) {
            WM_HideWindow(g_h_winhead);
        } else {
            WM_ShowWindow(g_h_winhead);
        }
        break;

    default:
        WM_DefaultProc(p_msg);
    }
}

/**
 * \brief 播放列表窗口回调函数
 *
 * \param[in] p_msg 指向 WM_MESSAGE结构的指针
 *
 * \return 无
 */
static void list_dlg_callback (WM_MESSAGE *p_msg) 
{
    int           ncode    = 0;
    int           id       = 0;
    int           play_idx = 0;
    GUI_RECT      rect;
    dialog_info_t info;

    switch (p_msg->MsgId) {

    case WM_INIT_DIALOG: /* 窗口初始化 */
        g_h_winlist_0 = p_msg->hWin;                  /* 获取窗体句柄 */
        g_h_button_0 = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_0);
        g_h_button_1 = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_1);
        g_h_button_2 = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_2);

        WINDOW_SetBkColor(g_h_winlist_0, 0x00FFFFFF); /* 窗口背景色设置 */

        WM_SetCallback(g_h_button_0, button_callback);
        WM_SetCallback(g_h_button_1, button_callback);
        WM_SetCallback(g_h_button_2, button_callback);

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
                play_idx = g_play_ctrl.play_idx;

                dialog_info_init(&info);
                info.font_title  = &g_font18;
                info.font_text   = &g_font14;
                info.font_btn[0] = &g_font14;
                info.font_btn[1] = &g_font14;
                info.btn_color_text[1] = 0xD4881E;
                strcpy(info.title, "删除歌曲");
                sprintf(info.text,
                        "确定彻底删除\"%s\"歌曲？",
                        g_play_list[play_idx].name);
                strcpy(info.btn_text[0], "取消");
                strcpy(info.btn_text[1], "删除");
                if (dialog_show(130,
                            76,
                            info.rect.x1 + 1,
                            info.rect.y1 + 1,
                            g_h_winpar,
                            WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_STAYONTOP,
                           &info) == 2)
                {
//                    aw_close(g_play_ctrl.fd);
                    play_stop();
                    if (0 == aw_unlink(g_play_list[play_idx].file_name)) {
                        play_list_del(g_play_list,
                                      sizeof(g_play_list) / sizeof(g_play_list[0]),
                                      &g_play_list[play_idx]);

                        winlist_recreate();
                        play_list_write(g_play_list,
                                       &g_list_state,
                                        PLAY_LIST_MAX_SIZE);
                    } else {
                        AW_ERRF(("aw_unlink %s failed\n",
                                 g_play_list[play_idx].file_name));
                    }

//                    play_list_init(g_play_list,
//                                   sizeof(g_play_list) / sizeof(g_play_list[0]),
//                                   1);
                }
                break;

            default: ;
            }
            break;

        case ID_BUTTON_1:
            switch(ncode) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                WM_DisableWindow(g_h_button_1);
                play_list_init(g_play_list,
                               sizeof(g_play_list) / sizeof(g_play_list[0]),
                               1,
                               SEARCH_RANGE_SD_USB);

                winlist_recreate();
                play_list_write(g_play_list,
                               &g_list_state,
                                PLAY_LIST_MAX_SIZE);
                WM_EnableWindow(g_h_button_1);
                break;

            default: ;
            }
            break;

        case ID_BUTTON_2:
            switch(ncode) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                WM_HideWindow(g_h_winlist_0);
                break;

            default: ;
            }
            break;
        }
        break;

    case WM_PAINT:
        GUI_SetBkColor(GUI_WHITE);
        GUI_Clear();
        WM_GetClientRect(&rect);
        GUI_DrawBitmap(&g_text_playlist, rect.x0 + 7, rect.y0 + 9);
        GUI_SetPenSize(1);
        GUI_SetColor(0xe5e3e2);
        GUI_DrawHLine(51, rect.x0, rect.x1);
        GUI_DrawHLine(221, rect.x0, rect.x1);
        GUI_DrawVLine(66, 225, 244);
        GUI_DrawVLine(133, 225, 244);
        break;

    default:
        WM_DefaultProc(p_msg);
        break;
    }
}


/**
 * \brief 默认窗口回调函数
 *
 * \param[in] p_msg 指向 WM_MESSAGE 结构的指针
 *
 * \return 无
 */
static void dummy_callback (WM_MESSAGE *p_msg)
{
    switch (p_msg->MsgId) {

    default:
        WM_DefaultProc(p_msg);
    }
}

/**
 * \brief 创建播放列表窗体
 *
 * \param[in] parent 父窗口句柄
 *
 * \return 播放列表窗体句柄
 */
WM_HWIN music_list_create (WM_HWIN parent)
{
    g_h_winpar = parent;

    g_h_winlist_0 = GUI_CreateDialogBox(g_dialog_create, /* 控件资源表 */
                        GUI_COUNTOF(g_dialog_create),    /* 控件数量 */ 
                        list_dlg_callback, /* 对话框过程函数的回调函数 */
                        parent,            /* 父窗口的句柄*/
                        280,               /* 相对于父窗口的 X 轴位置 */
                        24);               /* 相对于父窗口的 Y 轴位置 */

    g_h_winhead = WM_CreateWindowAsChild(
                      0,              /* 父窗口在窗口坐标中的左上 X 位置 */
                      52,             /* 父窗口在窗口坐标中的左上 Y 位置 */
                      200,            /* 窗口的 X 尺寸 */
                      169,            /* 窗口的 Y 尺寸 */
                      g_h_winlist_0,  /* 父窗口的句柄 */
                      WM_CF_SHOW | WM_CF_HASTRANS, /* 窗口创建标识 */
                      dummy_callback, /* 回调例程的指针 */ 
                      0);             /* 要分配的额外字节数 */

    g_h_winlist = WM_CreateWindowAsChild(
                        0,               /* 父窗口在窗口坐标中的左上 X 位置 */
                        0,               /* 父窗口在窗口坐标中的左上 Y 位置 */
                        200,             /* 窗口的 X 尺寸 */
                        34 * g_list_state.play_list_count, /* 窗口的 Y 尺寸 */
                        g_h_winhead,     /* 父窗口的句柄 */
                        WM_CF_SHOW | WM_CF_MOTION_Y, /* 标识 */
                        list_callback,   /* 回调例程的指针 */
                        0);              /* 要分配的额外字节数 */

    return g_h_winlist_0;
}

/* end of file */
