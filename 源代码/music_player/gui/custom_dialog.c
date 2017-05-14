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

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "custom_dialog.h"

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "GUI.h"  
#include "WM.h"  
#include "DIALOG.h"  

/*******************************************************************************
  宏定义
*******************************************************************************/

#define ID_WINDOW_0    (GUI_ID_USER + 0x00) /**< \brief 对话框ID */
#define ID_BUTTON_0    (GUI_ID_USER + 0x01) /**< \brief 按键0ID */
#define ID_BUTTON_1    (GUI_ID_USER + 0x02) /**< \brief 按键1ID */

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

static WM_HWIN g_h_winpar   = 0; /**< \brief 父窗体句柄 */
static WM_HWIN g_h_dialog   = 0; /**< \brief 对话框句柄 */
static WM_HWIN g_h_button_0 = 0; /**< \brief 按键0句柄 */
static WM_HWIN g_h_button_1 = 0; /**< \brief 按键1句柄 */

static dialog_info_t g_info; /**< \brief 对话框属性 */

static int g_ret   = 0; /**< \brief 返回值 */

int g_custom_dialog_state = 0; /**< \brief 对话框状态 0:未创建 1:已创建 */

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 设置text
 *
 * \param[in] text 待设置的text
 *
 * \return 无
 */
void text_set (const char *p_text)
{
    strcpy(g_info.text, p_text);
    WM_Paint(g_h_dialog);
}

/**
 * \brief 关闭对话框
 */
void dialog_close (void)
{
    if (1 == g_custom_dialog_state) {
        g_ret = -2;
        WM_DeleteWindow(g_h_dialog);
        g_custom_dialog_state = 0;
    }
}

/**
 * \brief 按键换肤回调函数
 *
 * \param[in] p_item_draw_info 类型WIDGET_ITEM_DRAW_INFO的数据结构的指针
 *
 * \retval 0 成功绘制
 */
static int button_drawskin (const WIDGET_ITEM_DRAW_INFO *p_item_draw_info)
{
    int       idx        = 0;
    int       is_pressed = 0;
    WM_HWIN   win_handle = p_item_draw_info->hWin;
    GUI_COLOR color;
    GUI_RECT  rect;
    GUI_RECT  text_rect;

    WM_GetClientRect(&rect);

    switch (p_item_draw_info->Cmd) {

    case WIDGET_ITEM_DRAW_BACKGROUND:

        is_pressed = BUTTON_IsPressed(win_handle);

        BUTTON_GetUserData(win_handle, &idx, sizeof(idx));

        if (is_pressed) {
            color = g_info.btn_color_pressed[idx];
        } else {
            color = g_info.btn_color_normal[idx];
        }
        GUI_SetColor(color);
        GUI_AA_FillRoundedRectEx(&rect, g_info.r);
        if (0 == idx) {
            GUI_FillRect(rect.x0, 
                         rect.y0, 
                         rect.x0 + g_info.r,
                         rect.y0 + g_info.r);
            GUI_FillRect(rect.x1 - g_info.r,
                         rect.y0, 
                         rect.x1, 
                         rect.y0 + g_info.r);
            if (g_info.btn_num != 1) {
                GUI_FillRect(rect.x1 - g_info.r,
                             rect.y1 - g_info.r,
                             rect.x1, 
                             rect.y1);
            }
        } else {
            GUI_FillRect(rect.x0, 
                         rect.y0, 
                         rect.x0 + g_info.r,
                         rect.y0 + g_info.r);
            GUI_FillRect(rect.x1 - g_info.r,
                         rect.y0, 
                         rect.x1, 
                         rect.y0 + g_info.r);
            GUI_FillRect(rect.x0, 
                         rect.y1 - g_info.r,
                         rect.x0 + g_info.r,
                         rect.y1);
        }

        GUI_SetFont(g_info.font_btn[idx]);
        GUI_SetTextMode(GUI_TEXTMODE_TRANS);
        text_rect.x0 = rect.x0;
        text_rect.x1 = rect.x1;
        text_rect.y0 = rect.y0;
        text_rect.y1 = rect.y1;
        GUI_SetColor(g_info.btn_color_text[idx]);
        GUI_DispStringInRect(g_info.btn_text[idx],
                           &text_rect,
                            GUI_TA_VCENTER | GUI_TA_HCENTER);
        break;
    }
    return 0;
}

/**
 * \brief 对话框回调函数
 *
 * \param[in] p_msg 指向 WM_MESSAGE 结构的指针
 *
 * \retval 无
 */
static void dialog_callback (WM_MESSAGE *p_msg) 
{
    int     NCode;
    int     Id;
    GUI_RECT rect;
    GUI_RECT rect_back;
    WM_HWIN h_button;

    WM_GetClientRectEx(p_msg->hWin, &rect);

    switch (p_msg->MsgId) {

    case WM_INIT_DIALOG: /* 窗口初始化 */
        g_h_button_0 = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_0);
        g_h_button_1 = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_1);
        WM_SetHasTrans(p_msg->hWin);
        WM_SetHasTrans(g_h_button_0);
        WM_SetHasTrans(g_h_button_1);
        break;

    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(p_msg->hWinSrc);
        NCode = p_msg->Data.v;
        switch (NCode) {

        case WM_NOTIFICATION_CLICKED:
            break;

        case WM_NOTIFICATION_RELEASED:
                if (0 == (Id - GUI_ID_BUTTON0)) {
                    g_ret = 1;
                } else {
                    g_ret = 2;
                }
                WM_DeleteWindow(p_msg->hWin);
                g_custom_dialog_state = 0;
            break;
        }
        break;

    case WM_PAINT:

        /* 绘制背景 */
        rect_back = rect;
        rect_back.y1 -= g_info.r;
        GUI_SetColor(g_info.color_background);
        GUI_AA_FillRoundedRectEx(&rect_back, g_info.r);

        /* 绘制分割线 */
        GUI_SetColor(g_info.color_line);
        GUI_DrawHLine(rect.y1 - g_info.btn_height, rect.x0, rect.x1);
        GUI_DrawVLine(rect.x1 / 2, rect.y1 - g_info.btn_height, rect.y1);
        
        GUI_SetTextMode(GUI_TEXTMODE_TRANS);

        /* 绘制title */
        GUI_SetColor(g_info.color_title);
        GUI_SetFont(g_info.font_title);
        GUI_DispStringInRect(g_info.title,
                            &g_info.title_rect,
                             GUI_TA_VCENTER | GUI_TA_HCENTER);

        /* 绘制text */
        GUI_SetColor(g_info.color_text);
        GUI_SetFont(g_info.font_text);
        GUI_DispStringInRectWrap(g_info.text,
                                &g_info.text_rect,
                                 GUI_TA_VCENTER | GUI_TA_HCENTER,
                                 GUI_WRAPMODE_CHAR);
        break;

    case WM_CREATE:
        for (int i = 0; i < g_info.btn_num; i++) {
            if (0 == i) {
                h_button = BUTTON_CreateUser(
                    0,
                    rect.y1 - g_info.btn_height + 1,
                    g_info.btn_num == 1 ? rect.x1 + 1 : rect.x1 / 2,
                    g_info.btn_height,
                    p_msg->hWin,
                    WM_CF_SHOW | WM_CF_HASTRANS,
                    0,
                    GUI_ID_BUTTON0 + i,
                    sizeof(i));
            } else {
                h_button = BUTTON_CreateUser(
                    rect.x1 - rect.x1 / 2,
                    rect.y1 - g_info.btn_height + 1,
                    rect.x1 - rect.x1 / 2,
                    g_info.btn_height,
                    p_msg->hWin,
                    WM_CF_SHOW | WM_CF_HASTRANS,
                    0,
                    GUI_ID_BUTTON0 + i,
                    sizeof(i));
            }
            BUTTON_SetSkin(h_button, button_drawskin);
            BUTTON_SetUserData(h_button, &i, sizeof(i));
        }

        break;

    default:
        WM_DefaultProc(p_msg);
        break;
    }
}

/**
 * \brief 初始化对话框信息结构体
 *
 * \param[in] p_info 对话框信息
 *
 * \return 无
 */
void dialog_info_init (dialog_info_t *p_info)
{
    p_info->blocking             = 1;
    p_info->modal                = 1;
    p_info->btn_num              = 2;             
    p_info->r                    = 5;
    p_info->rect.x0              = 0;
    p_info->rect.y0              = 0;
    p_info->rect.x1              = 220 - 1;
    p_info->rect.y1              = 128 - 1;
    p_info->btn_height           = p_info->rect.y1 - p_info->rect.y1 * 0.75;
    p_info->title_rect.x0        = p_info->rect.x0 + 5;
    p_info->title_rect.x1        = p_info->rect.x1 - 5;
    p_info->title_rect.y0        = p_info->rect.y1 * 0.1;
    p_info->title_rect.y1        = p_info->rect.y1 * 0.3;
    p_info->text_rect.x0         = p_info->rect.x0 + 5;
    p_info->text_rect.x1         = p_info->rect.x1 - 5;
    p_info->text_rect.y0         = p_info->rect.y1 * 0.3;
    p_info->text_rect.y1         = p_info->rect.y1 * 0.65;
    p_info->color_title          = GUI_WHITE;
    p_info->color_text           = GUI_WHITE;
    p_info->color_background     = 0x544A43;
    p_info->color_line           = 0x999999;
    p_info->btn_color_text[0]    = GUI_WHITE;
    p_info->btn_color_text[1]    = GUI_WHITE;
    p_info->btn_color_normal[0]  = 0x544A43;
    p_info->btn_color_normal[1]  = 0x544A43;
    p_info->btn_color_pressed[0] = 0x292421;
    p_info->btn_color_pressed[1] = 0x292421;
    p_info->font_title           = (GUI_FONT *)&GUI_Font16_ASCII;
    p_info->font_text            = (GUI_FONT *)&GUI_Font16_ASCII;
    p_info->font_btn[0]          = (GUI_FONT *)&GUI_Font16_ASCII;
    p_info->font_btn[1]          = (GUI_FONT *)&GUI_Font16_ASCII;

    strcpy(p_info->title, "title"); 
    strcpy(p_info->text, "text"); 
    strcpy(p_info->btn_text[0], "button0"); 
    strcpy(p_info->btn_text[1], "button1"); 
}

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
 * \retval 0  未点击按钮
 * \retval 1  点击按钮0
 * \retval 2  点击按钮1
 * \retval -1 创建对话框失败
 * \retval -2 通过dialog_close()关闭对话框
 */
int dialog_show (int            x0,
                 int            y0,
                 int            x_size,
                 int            y_size,
                 GUI_HWIN       h_parent,
                 uint32_t       style,
                 dialog_info_t *p_info)
{
    if (1 == g_custom_dialog_state) {
        return -1;
    }

    g_h_winpar = h_parent;
    g_ret = 0;
    memcpy(&g_info, p_info, sizeof(dialog_info_t));
    g_h_dialog = WM_CreateWindowAsChild(
                        x0,              /* 父窗口在窗口坐标中的左上 X 位置 */
                        y0,              /* 父窗口在窗口坐标中的左上 Y 位置 */
                        x_size,          /* 窗口的 X 尺寸 */
                        y_size,          /* 窗口的 Y 尺寸 */
                        g_h_winpar,      /* 父窗口的句柄 */
                        style,           /* 标识 */
                        dialog_callback, /* 回调例程的指针 */
                        0);              /* 要分配的额外字节数 */

    g_custom_dialog_state = 1;

    if (p_info->modal) {
        WM_MakeModal(g_h_dialog);
    }

    while (p_info->blocking) {
        if (0 != g_ret) {
            break;
        }
        GUI_Delay(100);
    }

    return g_ret;
}

/* end of file */
