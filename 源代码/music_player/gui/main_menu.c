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
 * \brief 音乐播放器主界面
 *
 * 创建相关控件以及处理相关事件
 *
 * \internal
 * \par Modification history
 * - 1.00 17-02-17 张进科, 创建该文件
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "apollo.h"
#include "main_menu.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_types.h"
#include "aw_touch_screen.h"
#include "io/aw_dirent.h"
#include "fs/aw_mount.h"
#include "DIALOG.h"
#include "GUI.h"
#include "system_resource.h"
#include "player_global.h"
#include "music_list.h"
#include "setting.h"
#include "resource.h"
#include "custom_dialog.h"
#include "font_xbf.h"
#include "lyric.h"  


/*******************************************************************************
  宏定义
*******************************************************************************/

#define PLAYER_GUI_TASK_PRIO          15           /**< \brief 任务优先级 */
#define PLAYER_GUI_TASK_STACK_SIZE    (1024 * 24)  /**< \brief 任务栈大小 */

/** \brief 定义任务(静态) */
AW_TASK_DECL_STATIC(player_gui_task, PLAYER_GUI_TASK_STACK_SIZE);

#define ID_WINDOW_0  (GUI_ID_USER + 0x00) /**< \brief 主界面ID */
#define ID_IMAGE_0   (GUI_ID_USER + 0x01) /**< \brief 歌手图片ID */
#define ID_IMAGE_1   (GUI_ID_USER + 0x02) /**< \brief 修饰图ID */
#define ID_BUTTON_0  (GUI_ID_USER + 0x03) /**< \brief 播放/暂停键ID */
#define ID_BUTTON_1  (GUI_ID_USER + 0x04) /**< \brief 上一曲键ID */
#define ID_BUTTON_2  (GUI_ID_USER + 0x05) /**< \brief 下一曲键ID */
#define ID_TEXT_0    (GUI_ID_USER + 0x06) /**< \brief 歌曲播放时间ID */
#define ID_TEXT_1    (GUI_ID_USER + 0x07) /**< \brief 歌曲总时间ID */
#define ID_SLIDER_0  (GUI_ID_USER + 0x08) /**< \brief 歌曲进度条ID */
#define ID_SLIDER_1  (GUI_ID_USER + 0x09) /**< \brief 音量条ID */
#define ID_IMAGE_2   (GUI_ID_USER + 0x0A) /**< \brief 喇叭图标ID */
#define ID_BUTTON_3  (GUI_ID_USER + 0x0B) /**< \brief 播放模式键ID */
#define ID_BUTTON_4  (GUI_ID_USER + 0x0C) /**< \brief 音乐信息键ID */
#define ID_BUTTON_5  (GUI_ID_USER + 0x0D) /**< \brief 系统设置键ID */
#define ID_BUTTON_6  (GUI_ID_USER + 0x0E) /**< \brief 播放列表键ID */
#define ID_BUTTON_7  (GUI_ID_USER + 0x0F) /**< \brief 歌词拖动确认键ID */

#define LRC_COL_HIGHT  (17)                 /**< \brief 歌词行高 */
#define LRC_COL_PITCH  (10)                 /**< \brief 歌词行间距 */

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

uint8_t  g_ttf_res[7569637];            /**< \brief 字体数据 */
uint32_t g_ttf_res_lenth = 0;           /**< \brief 字体数据大小 */

uint8_t  g_singer_res[1024 * 1024 * 2]; /**< \brief 歌手图片数据 */
uint32_t g_singer_res_lenth = 0;        /**< \brief 歌手图片数据大小 */

uint8_t  g_lrc_res[1024 * 1024];        /**< \brief 歌词数据 */
uint32_t g_lrc_res_lenth = 0;           /**< \brief 歌词数据大小 */

lyric_info_t g_lyric;                   /**< \brief 歌词结构 */

uint8_t g_is_slider0_move = 0;          /**< \brief 进度条是否移动 0:未移动 1:移动 */

uint32_t g_move_time = 0;               /**< \brief 当前歌词移动到的时间 */

uint32_t g_old_row = 0xFFFFFFFF;        /**< \brief 当前歌词行数 */

/** \brief 歌词是否移动, 手动滑动歌词后应该显示调整线。 0:未移动 1:移动 */
uint8_t g_is_lrc_move = 0;

/** \brief 音量条的值 */
static int g_volume_value = 0;

static WM_HWIN g_h_winmain_0 = 0; /**< \brief 主界面句柄 */
static WM_HWIN g_h_image_0   = 0; /**< \brief 歌手图片句柄 */
static WM_HWIN g_h_image_1   = 0; /**< \brief 修饰图句柄 */
static WM_HWIN g_h_button_0  = 0; /**< \brief 播放/暂停键句柄 */
static WM_HWIN g_h_button_1  = 0; /**< \brief 上一曲键句柄 */
static WM_HWIN g_h_button_2  = 0; /**< \brief 下一曲键句柄 */
static WM_HWIN g_h_text_0    = 0; /**< \brief 歌曲播放时间句柄 */
static WM_HWIN g_h_text_1    = 0; /**< \brief 歌曲总时间句柄 */
static WM_HWIN g_h_slider_0  = 0; /**< \brief 歌曲进度条句柄 */
static WM_HWIN g_h_slider_1  = 0; /**< \brief 音量条句柄 */
static WM_HWIN g_h_image_2   = 0; /**< \brief 喇叭图标句柄 */
static WM_HWIN g_h_button_3  = 0; /**< \brief 播放模式键句柄 */
static WM_HWIN g_h_button_4  = 0; /**< \brief 音乐信息键句柄 */
static WM_HWIN g_h_button_5  = 0; /**< \brief 系统设置键句柄 */
static WM_HWIN g_h_button_6  = 0; /**< \brief 播放列表键句柄 */
static WM_HWIN g_h_button_7  = 0; /**< \brief 歌词拖动确认键句柄 */

static WM_HWIN g_h_winlist_0 = 0; /**< \brief 播放列表界面句柄 */
static WM_HWIN g_h_winhead   = 0; /**< \brief 歌词容器句柄 */
static WM_HWIN g_h_winlyrics = 0; /**< \brief 歌词句柄 */

static WM_HWIN g_h_setting_0 = 0; /**< \brief 系统设置界面句柄 */

static WM_HWIN g_h_timer_0   = 0; /**< \brief 定时更新窗口指定内容 */
static WM_HWIN g_h_timer_1   = 0; /**< \brief 定时更新歌词 */
static WM_HWIN g_h_timer_2   = 0; /**< \brief 滑动歌词调整进度时间阈值 */

/** \brief 对话框控件表 */
static const GUI_WIDGET_CREATE_INFO g_dialog_create[] = {
    { WINDOW_CreateIndirect, "Window",       ID_WINDOW_0,
        0,     0,     480,   272,  0, 0,                              0},
    { IMAGE_CreateIndirect,  "singer",       ID_IMAGE_0,
        0,     0,     480,   234,  0, IMAGE_CF_MEMDEV|IMAGE_CF_ALPHA, 0},
    { IMAGE_CreateIndirect,  "strip",        ID_IMAGE_1,
        0,     205,   480,   29,   0, IMAGE_CF_MEMDEV|IMAGE_CF_ALPHA, 0},
    { BUTTON_CreateIndirect, "play",         ID_BUTTON_0,
        14,    236,   35,    35,   0, 0,                              0},
    { BUTTON_CreateIndirect, "up",           ID_BUTTON_1,
        52,    240,   28,    28,   0, 0,                              0},
    { BUTTON_CreateIndirect, "down",         ID_BUTTON_2,
        83,    240,   28,    28,   0, 0,                              0},
    { TEXT_CreateIndirect,   "play_time",    ID_TEXT_0,
        121,   247,   30,    12,   0, 0x0,                            0},
    { TEXT_CreateIndirect,   "total_time",   ID_TEXT_1,
        353,   247,   30,    12,   0, 0x0,                            0},
    { SLIDER_CreateIndirect, "progress_bar", ID_SLIDER_0,
        153-1, 248-1, 196+2, 10+2, 0, 0x0,                            0},
    { SLIDER_CreateIndirect, "volume",       ID_SLIDER_1,
        409-1, 250-1, 55+2,  6+2,  0, 0x0,                            0},
    { IMAGE_CreateIndirect,  "beep",         ID_IMAGE_2,
        394,   245,   10,    16,   0, 0,                              0},
    { BUTTON_CreateIndirect, "play_mode",    ID_BUTTON_3,
        15,    205,   79,    29,   0, 0,                              0},
    { BUTTON_CreateIndirect, "music_info",   ID_BUTTON_4,
        140,   205,   78,    29,   0, 0,                              0},
    { BUTTON_CreateIndirect, "setting",      ID_BUTTON_5,
        264,   205,   78,    29,   0, 0,                              0},
    { BUTTON_CreateIndirect, "play_list",    ID_BUTTON_6,
        387,   205,   77,    29,   0, 0,                              0},
};

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 根据歌词重设歌词窗体长度
 *
 * \param[in]  lyric 指向lyric_info_t结构的指针
 *
 * \return 无
 */
void reset_lyric_y (lyric_info_t *p_lyric);

/**
 * \brief 获取音量
 *
 * \return 音量
 */
int32_t volume_get (void)
{
    return g_volume_value;
}

/**
 * \brief 设置音量
 *
 * \param[in] volume 音量
 *
 * \return 无
 */
void volume_set (int32_t volume)
{
    SLIDER_SetValue(g_h_slider_1, volume);
}

/**
 * \brief 刷新音量设置
 */
void refresh_volume (void)
{
    aw_imx28_snd_volume_set(&g_snd_dev, 0, 25 * log10(g_volume_value) - 70);
}

/**
 * \brief 更新界面
 */
void refresh (void)
{
    static config_info_t config_info_old;
    static int32_t       play_idx_old = -1;
    static char          old_name[256];
    uint8_t              play_time[6];
    uint8_t              second       = 0;
    uint8_t              minute       = 0;
    int                  recv         = 0;

    g_volume_value = SLIDER_GetValue(g_h_slider_1) + 1;

    if (0 == g_list_state.play_list_count) {
        return;
    }

    if ((play_idx_old != g_play_ctrl.play_idx) ||
        (0 != strcmp(g_play_list[g_play_ctrl.play_idx].name, old_name))) {
        play_idx_old = g_play_ctrl.play_idx;
        strcpy(old_name, g_play_list[g_play_ctrl.play_idx].name);

        g_play_ctrl.current_time = 0;

        /* 计算歌曲分钟和秒数 */
        second = g_play_list[g_play_ctrl.play_idx].total_time / 1000 % 60;
        minute = ((g_play_list[g_play_ctrl.play_idx].total_time / 1000) -
                 (g_play_list[g_play_ctrl.play_idx].total_time / 1000 % 60))
                 / 60 % 60;
        sprintf(play_time, "%02d:%02d", minute, second);
        TEXT_SetText(g_h_text_1, play_time);

        /* 读取歌手图片 */
        recv = jpg_read(g_play_list[g_play_ctrl.play_idx].file_name,
                        g_singer_res,
                        sizeof(g_singer_res),
                       &g_singer_res_lenth);
        if ((recv != AW_OK) || (0 == g_config_info.singer_show)) {
            IMAGE_SetJPEG(g_h_image_0,
                          g_res_background_2,
                          sizeof(g_res_background_2));
        } else {

            /* 打开歌手图片成功，显示 */
            IMAGE_SetJPEG(g_h_image_0, g_singer_res, g_singer_res_lenth);
        }

        /* 读取歌词 */
        if (lrc_read(g_play_list[g_play_ctrl.play_idx].file_name,
                 g_lrc_res,
                 sizeof(g_lrc_res),
                &g_lrc_res_lenth) == AW_OK) {
            if (lrc_analyze(g_lrc_res, g_lrc_res_lenth, &g_lyric) == AW_OK) {

                /* 根据歌词重设歌词窗体长度 */
                reset_lyric_y(&g_lyric);
                g_old_row = 0xFFFFFFFF;
                g_is_lrc_move = 0;
                WM_MoveChildTo(g_h_winlyrics, 0, 0);
                WM_ShowWindow(g_h_winhead);
            } else {
                AW_ERRF(("lrc analyze error!\n"));
                WM_SetYSize(g_h_winlyrics, 0);
                WM_HideWindow(g_h_winhead);
            }
        } else {
            WM_SetYSize(g_h_winlyrics, 0);
            WM_HideWindow(g_h_winhead);
        }

        /* 更新播放列表位置 */
        refresh_list();
    }

    if (0 == g_is_slider0_move) {

        /* 计算歌曲当前分钟和秒数 */
        second = g_play_ctrl.current_time / 1000 % 60;
        minute = ((g_play_ctrl.current_time / 1000) -
                 (g_play_ctrl.current_time / 1000 % 60)) / 60 % 60;
        sprintf(play_time, "%02d:%02d", minute, second);
        TEXT_SetText(g_h_text_0, play_time);

        /* 更新歌曲进度条 */
        SLIDER_SetValue(g_h_slider_0, 1000 *
                                      g_play_ctrl.current_time /
                                      g_play_ctrl.info.total_time);
    }

    if (config_info_old.lrc_show != g_config_info.lrc_show) {
        config_info_old.lrc_show = g_config_info.lrc_show;
        if (0 == g_config_info.lrc_show) {
            WM_HideWindow(g_h_winhead);
        } else {
            WM_ShowWindow(g_h_winhead);
        }
    }

    if (config_info_old.singer_show != g_config_info.singer_show) {
        config_info_old.singer_show = g_config_info.singer_show;

        if (0 == g_config_info.singer_show) {
            /* 显示背景图片 */
            IMAGE_SetJPEG(g_h_image_0,
                          g_res_background_2,
                          sizeof(g_res_background_2));
        } else {
            recv = jpg_read(g_play_list[g_play_ctrl.play_idx].file_name,
                            g_singer_res,
                            sizeof(g_singer_res),
                           &g_singer_res_lenth);
            if (AW_OK != recv) {

                /* 打开歌手图片失败，显示背景图片 */
                IMAGE_SetJPEG(g_h_image_0,
                              g_res_background_2,
                              sizeof(g_res_background_2));
            } else {

                /* 打开歌手图片成功，显示 */
                IMAGE_SetJPEG(g_h_image_0, g_singer_res, g_singer_res_lenth);
            }
        }
    }
}

/**
 * \brief 更新歌词
 */
void refresh_lrc (void)
{
    int i;
    int current_y0;
    int next_y0;
    int shift;
    double time = 0.5;
    double speed = 0;

    if (WM_GetWindowSizeY(g_h_winlyrics) == 0) {
        return;
    }

    for (i = 0; i < g_lyric.row_num; i++) {
        if (0 == i) {
            if (g_lyric.time[i] > g_play_ctrl.current_time) {
                if (0 == g_is_lrc_move) {
                    next_y0 = (-i * (LRC_COL_HIGHT + LRC_COL_PITCH)) -
                               LRC_COL_HIGHT/2;
                    WM_MoveChildTo(g_h_winlyrics, 0, next_y0);
                }
            }
        }
        if (i < g_lyric.row_num - 1) {
            if ((g_lyric.time[i] < g_play_ctrl.current_time) &&
               (g_lyric.time[i + 1] > g_play_ctrl.current_time)) {
                if (g_old_row != i) {
                    g_old_row = i;
                    if (0 == g_is_lrc_move) {
                        current_y0 = WM_GetWindowOrgY(g_h_winlyrics);
                        next_y0 = (-i * (LRC_COL_HIGHT + LRC_COL_PITCH)) -
                                   LRC_COL_HIGHT/2;
                        shift = next_y0 - current_y0;
                        speed = shift / time;
                        shift = shift < 0 ? -shift : shift;
                        if ((WM_IsVisible(g_h_winlist_0) == 0) &&
                            (0 == g_custom_dialog_state)) {
                            WM_MOTION_SetMovement(g_h_winlyrics,
                                                  GUI_COORD_Y,
                                                  speed,
                                                  shift);
                        } else {
                            WM_MoveChildTo(g_h_winlyrics, 0, next_y0);
                        }
                        if (0 == i) {
                            WM_Paint(g_h_winlyrics);
                        }
                    }
                }
                break;
            }
        } else {
            if (g_lyric.time[i] < g_play_ctrl.current_time) {
                if (0 == g_is_lrc_move) {
                    current_y0 = WM_GetWindowOrgY(g_h_winlyrics);
                    next_y0 = (-i * (LRC_COL_HIGHT + LRC_COL_PITCH)) -
                               LRC_COL_HIGHT/2;
                    shift = next_y0 - current_y0;
                    speed = shift / time;
                    shift = shift < 0 ? -shift : shift;
                    if ((WM_IsVisible(g_h_winlist_0) == 0) &&
                        (0 == g_custom_dialog_state)) {
                        WM_MOTION_SetMovement(g_h_winlyrics,
                                              GUI_COORD_Y,
                                              speed,
                                              shift);
                    } else {
                        WM_MoveChildTo(g_h_winlyrics, 0, next_y0);
                    }
                }
            }
        }
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
    WM_HWIN win_handle = p_item_draw_info->hWin;

    int x0 = p_item_draw_info->x0; /* 绘制项目时要使用的在窗口坐标中的 X 位置 */
    int y0 = p_item_draw_info->y0; /* 绘制项目时要使用的在窗口坐标中的 Y 位置 */

    int ItemIndex = p_item_draw_info->ItemIndex; /* 要绘制项目的索引 */

    switch (p_item_draw_info->Cmd) {

    case WIDGET_ITEM_DRAW_BACKGROUND:
        if (win_handle == g_h_button_0) {
            if(BUTTON_SKINFLEX_PI_PRESSED == ItemIndex) {
                GUI_DrawBitmap(&g_btn_play_pressed, x0, y0);
            }
            else {
                GUI_DrawBitmap(&g_btn_play_normal, x0, y0);
            }
        }
        else if ((win_handle == g_h_button_1) || 
                 (win_handle == g_h_button_2)) {
            if(BUTTON_SKINFLEX_PI_PRESSED == ItemIndex) {
                GUI_DrawBitmap(&g_btn_up_down_pressed, x0, y0);
            }
            else {
                GUI_DrawBitmap(&g_btn_up_down_normal, x0, y0);
            }
        }
        else if (win_handle == g_h_button_3) {
            if(BUTTON_SKINFLEX_PI_PRESSED == ItemIndex) {
                if (0 == g_play_ctrl.paly_mode) {
                    GUI_DrawBitmap(&g_btn_radom_pressed, x0, y0);
                }
                else if (1 == g_play_ctrl.paly_mode) {
                    GUI_DrawBitmap(&g_btn_repeat_pressed, x0, y0);
                }
                else if (2 == g_play_ctrl.paly_mode) {
                    GUI_DrawBitmap(&g_btn_repeat_o_pressed, x0, y0);
                }
                else {
                }
            }
            else {
                if (0 == g_play_ctrl.paly_mode) {
                    GUI_DrawBitmap(&g_btn_radom_normal, x0, y0);
                }
                else if (1 == g_play_ctrl.paly_mode) {
                    GUI_DrawBitmap(&g_btn_repeat_normal, x0, y0);
                }
                else if (2 == g_play_ctrl.paly_mode) {
                    GUI_DrawBitmap(&g_btn_repeat_o_normal, x0, y0);
                }
                else {
                }
            }
        }
        else if (win_handle == g_h_button_4) {
            if(BUTTON_SKINFLEX_PI_PRESSED == ItemIndex) {
                GUI_DrawBitmap(&g_btn_info_pressed, x0, y0);
            }
            else {
                GUI_DrawBitmap(&g_btn_info_normal, x0, y0);
            }
        }
        else if (win_handle == g_h_button_5) {
            if(BUTTON_SKINFLEX_PI_PRESSED == ItemIndex) {
                GUI_DrawBitmap(&g_btn_setting_pressed, x0, y0);
            }
            else {
                GUI_DrawBitmap(&g_btn_setting_normal, x0, y0);
            }
        }
        else if (win_handle == g_h_button_6) {
            if(BUTTON_SKINFLEX_PI_PRESSED == ItemIndex) {
                GUI_DrawBitmap(&g_btn_list_pressed, x0, y0);
            }
            else {
                GUI_DrawBitmap(&g_btn_list_normal, x0, y0);
            }
        }
        else if (win_handle == g_h_button_7) {
            GUI_DrawBitmap(&g_btn_lrc, x0, y0);
        }
        else {
        }
        break;

    case WIDGET_ITEM_DRAW_TEXT:
        if (win_handle == g_h_button_0) {
            if (0 == g_play_ctrl.paly_state) {
                GUI_DrawBitmap(&g_play, x0 + 14, y0 + 11);
            }
            else {
                GUI_DrawBitmap(&g_pause, x0 + 13, y0 + 11);
            }
        }
        else if (win_handle == g_h_button_1) {
            GUI_DrawBitmap(&g_up, x0 + 8, y0 + 10);
        }
        else if (win_handle == g_h_button_2) {
            GUI_DrawBitmap(&g_down, x0 + 8, y0 + 10);
        }
        else {
        }
        break;
    }

    return 0;
}

/**
 * \brief 滑块换肤回调函数
 *
 * \param[in] p_item_draw_info 指向 WIDGET_ITEM_DRAW_INFO结构的指针
 *
 * \return 0 绘制成功
 */
static int slider_drawskin (const WIDGET_ITEM_DRAW_INFO *p_item_draw_info)
{
    WM_HWIN win_handle = p_item_draw_info->hWin;

    int x0 = p_item_draw_info->x0; /* 绘制项目时要使用的在窗口坐标中的 X 位置 */
    int y0 = p_item_draw_info->y0; /* 绘制项目时要使用的在窗口坐标中的 Y 位置 */
    int x1 = p_item_draw_info->x1; /* 绘制项目时要使用的在窗口坐标中的 X 位置 */
    int y1 = p_item_draw_info->y1; /* 绘制项目时要使用的在窗口坐标中的 Y 位置 */

    switch (p_item_draw_info->Cmd) {

    case WIDGET_ITEM_DRAW_TICKS:
        break;

    case WIDGET_ITEM_DRAW_THUMB: /* 绘制滑块 */
        if (win_handle == g_h_slider_0) {
            GUI_DrawBitmap(&g_slider_thumb_r, x1 - 4, y0);
        }
        else {
            GUI_DrawBitmap(&g_volume_thumb_r, x1 - 2, y0);
        }
        break;

    case WIDGET_ITEM_DRAW_SHAFT: /* 绘制轴 */
        if (win_handle == g_h_slider_0) {
            GUI_DrawBitmap(&g_slider_shaft, x0, y0);
            GUI_DrawBitmap(&g_slider_thumb_l, x0, y0);
            int slider_value = SLIDER_GetValue(g_h_slider_0);
            int x1_shatf = slider_value * (x1 - 8) / 1000 + 8 - 4;
            GUI_DrawGradientH(x0 + 5, y0, x1_shatf, y1, 0x516efc, 0x516efc);
        }
        else {
            GUI_DrawBitmap(&g_volume_shaft, x0, y0);
            GUI_DrawBitmap(&g_volume_thumb_l, x0, y0);
            int slider_value = SLIDER_GetValue(g_h_slider_1);
            int x1_shatf = (x1 - x0 - 6) * slider_value / 100;
            GUI_DrawGradientH(x0 + 2, y0, x0 + 4 + x1_shatf, y1, 
                              0x68d4a0, 0x68d4a0);
        }
        break;

    case WIDGET_ITEM_DRAW_FOCUS:
        break;

    default: 
        return SLIDER_DrawSkinFlex(p_item_draw_info);/* emWin默认控件绘制函数 */  
    }
    return 0;
}

/**
 * \brief 主界面窗口回调函数
 *
 * \param[in] p_msg 指向 WM_MESSAGE结构的指针
 *
 * \return 无
 */
static void main_dlg_callback(WM_MESSAGE *p_msg)
{
    int           n_code = 0;
    int           id     = 0;
    int           value  = 0;
    uint8_t       second = 0;
    uint8_t       minute = 0;
    uint8_t       play_time[6];
    dialog_info_t info;
    char         *p_tpye = NULL;
    GUI_RECT      rect;

    switch (p_msg->MsgId) {

    case WM_INIT_DIALOG:
        g_h_winmain_0 = p_msg->hWin;
        g_h_text_0    = WM_GetDialogItem(p_msg->hWin, ID_TEXT_0);
        g_h_text_1    = WM_GetDialogItem(p_msg->hWin, ID_TEXT_1);
        g_h_image_0   = WM_GetDialogItem(p_msg->hWin, ID_IMAGE_0);
        g_h_image_1   = WM_GetDialogItem(p_msg->hWin, ID_IMAGE_1);
        g_h_image_2   = WM_GetDialogItem(p_msg->hWin, ID_IMAGE_2);
        g_h_button_0  = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_0);
        g_h_button_1  = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_1);
        g_h_button_2  = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_2);
        g_h_button_3  = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_3);
        g_h_button_4  = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_4);
        g_h_button_5  = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_5);
        g_h_button_6  = WM_GetDialogItem(p_msg->hWin, ID_BUTTON_6);
        g_h_slider_0  = WM_GetDialogItem(p_msg->hWin, ID_SLIDER_0);
        g_h_slider_1  = WM_GetDialogItem(p_msg->hWin, ID_SLIDER_1);

        WINDOW_SetBkColor(g_h_winmain_0, 0x00FFFFFF);
        WM_SetHasTrans(g_h_winmain_0);
        WM_SetHasTrans(g_h_image_0);
        WM_SetHasTrans(g_h_image_1);
        WM_SetHasTrans(g_h_image_2);
        WM_SetHasTrans(g_h_button_0);
        WM_SetHasTrans(g_h_button_1);
        WM_SetHasTrans(g_h_button_2);
        WM_SetHasTrans(g_h_button_3);
        WM_SetHasTrans(g_h_button_4);
        WM_SetHasTrans(g_h_button_5);
        WM_SetHasTrans(g_h_button_6);
        WM_SetHasTrans(g_h_slider_0);
        WM_SetHasTrans(g_h_slider_1);

        TEXT_SetFont(g_h_text_0, GUI_FONT_10_ASCII);
        TEXT_SetText(g_h_text_0, "00:00");

        TEXT_SetFont(g_h_text_1, GUI_FONT_10_ASCII);
        TEXT_SetText(g_h_text_1, "00:00");

        IMAGE_SetBitmap(g_h_image_1, &g_strip);
        IMAGE_SetBitmap(g_h_image_2, &g_beep);

        BUTTON_SetSkin(g_h_button_0, button_drawskin);
        BUTTON_SetSkin(g_h_button_1, button_drawskin);
        BUTTON_SetSkin(g_h_button_2, button_drawskin);
        BUTTON_SetSkin(g_h_button_3, button_drawskin);
        BUTTON_SetSkin(g_h_button_4, button_drawskin);
        BUTTON_SetSkin(g_h_button_5, button_drawskin);
        BUTTON_SetSkin(g_h_button_6, button_drawskin);

        SLIDER_SetSkin(g_h_slider_0, slider_drawskin);
        SLIDER_SetSkin(g_h_slider_1, slider_drawskin);
        SLIDER_SetRange(g_h_slider_0, 0, 1000);        /* 设置进度条范围 */

        break;

    case WM_NOTIFY_PARENT:
        id = WM_GetId(p_msg->hWinSrc);
        n_code = p_msg->Data.v;
        switch (id) {

        case ID_BUTTON_0:
            switch (n_code) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                g_play_ctrl.paly_state = !g_play_ctrl.paly_state;
                WM_InvalidateWindow(g_h_winlist_0);
                break;

            default:
                ; /* VOID */
            }
            break;

        case ID_BUTTON_1:
            switch (n_code) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                play_event_send(PLAY_EVENT_PREV); /* 发送上一曲事件 */
                WM_InvalidateWindow(g_h_winlist_0);
                break;

            default:
                ; /* VOID */
            }
            break;

        case ID_BUTTON_2:
            switch (n_code) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                play_event_send(PLAY_EVENT_NEXT); /* 发送下一曲事件 */
                WM_InvalidateWindow(g_h_winlist_0);
                break;

            default:
                ; /* VOID */
            }
            break;
        case ID_BUTTON_3:
            switch (n_code) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                if (g_play_ctrl.paly_mode < 2) {
                    g_play_ctrl.paly_mode++;
                }
                else {
                    g_play_ctrl.paly_mode = 0;
                }
                break;

            default:
                ; /* VOID */
            }
            break;

        case ID_BUTTON_4:
            switch (n_code) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                second = g_play_list[g_play_ctrl.play_idx].total_time /
                         1000 %
                         60;
                minute = ((g_play_list[g_play_ctrl.play_idx].total_time /
                         1000) -
                         (g_play_list[g_play_ctrl.play_idx].total_time /
                         1000 %
                         60)) /
                         60 %
                         60;
                if (MUSIC_TYPE_WAV == g_play_ctrl.info.type) {
                    p_tpye = "wav";
                } else if (MUSIC_TYPE_MP3 == g_play_ctrl.info.type) {
                    p_tpye = "mp3";
                } else if (MUSIC_TYPE_FLAC == g_play_ctrl.info.type) {
                    p_tpye = "flac";
                } else {
                    p_tpye = "未知";
                }

                dialog_info_init(&info);
                info.font_title    = &g_font18;
                info.font_text     = &g_font14;
                info.font_btn[0]   = &g_font14;
                info.font_btn[1]   = &g_font14;
                info.btn_num       = 1;
                info.rect.x1       = 350 - 1;
                info.rect.y1       = 204 - 1;
                info.title_rect.x0 = info.rect.x0 + 5;
                info.title_rect.x1 = info.rect.x1 - 5;
                info.title_rect.y0 = info.rect.y1 * 0.05;
                info.title_rect.y1 = info.rect.y1 * 0.25;
                info.text_rect.x0  = info.rect.x0 + 5;
                info.text_rect.x1  = info.rect.x1 - 5;
                info.text_rect.y0  = info.rect.y1 * 0.25;
                info.text_rect.y1  = info.rect.y1 - info.btn_height;
                strcpy(info.title, "歌曲信息");
                sprintf(info.text,
                        "歌名: %s    歌手: %s\n" \
                        "时长: %02d分%02d秒    格式: %s\n" \
                        "\n" \
                        "采样率: %.1lf Khz    比特率: %.1lf Kbps\n" \
                        "通道数: %d channel    位数: %d bits\n" \
                        "文件大小: %.1lf MiB",
                        g_play_list[g_play_ctrl.play_idx].name,
                        g_play_list[g_play_ctrl.play_idx].artist,
                        minute,
                        second,
                        p_tpye,
                        g_play_list[g_play_ctrl.play_idx].samples_rate / 1000.0,
                        g_play_list[g_play_ctrl.play_idx].bitrate / 1000.0,
                        g_play_list[g_play_ctrl.play_idx].channels,
                        g_play_list[g_play_ctrl.play_idx].bits_per_word,
                        g_play_list[g_play_ctrl.play_idx].file_size /
                        1024.0 /
                        1024.0);
                strcpy(info.btn_text[0], "确定");

                dialog_show(65,
                            34,
                            info.rect.x1 + 1,
                            info.rect.y1 + 1,
                            g_h_winmain_0,
                            WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_STAYONTOP,
                           &info);

                break;

            default:
                ; /* VOID */
            }
            break;

        case ID_BUTTON_5:
            switch (n_code) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                setting_refresh();
                WM_ShowWindow(g_h_setting_0);
                break;

            default:
                ; /* VOID */
            }
            break;

        case ID_BUTTON_6:
            switch (n_code) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                WM_ShowWindow(g_h_winlist_0);

                /* 更新播放列表位置 */
                refresh_list();

                WM_Paint(g_h_winmain_0);
                break;

            default:
                ; /* VOID */
            }
            break;

        case ID_SLIDER_0:
            switch (n_code) {

            case WM_NOTIFICATION_CLICKED:
                g_is_slider0_move = 1;
                break;

            case WM_NOTIFICATION_RELEASED:
                value = SLIDER_GetValue(g_h_slider_0);
                 g_is_slider0_move = 0;
                g_move_time = value / 1000.0 * g_play_ctrl.info.total_time;
                play_event_send(PLAY_EVENT_SET_TIME); /* 发送设置当前时间事件 */
                WM_RestartTimer(g_h_timer_0, 500);
                break;

            case WM_NOTIFICATION_VALUE_CHANGED:
                value = SLIDER_GetValue(g_h_slider_0);
                uint32_t move_time = value /
                                     1000.0 *
                                     g_play_ctrl.info.total_time;

                /* 计算进度条拖动的分钟和秒数 */
                second = move_time / 1000 % 60;
                minute = ((move_time / 1000) -
                         (move_time / 1000 % 60)) / 60 % 60;
                sprintf(play_time, "%02d:%02d", minute, second);
                TEXT_SetText(g_h_text_0, play_time);

                break;

            default:
                ; /* VOID */
            }
            break;

        case ID_SLIDER_1:
            switch (n_code) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                break;

            case WM_NOTIFICATION_VALUE_CHANGED:
                refresh_volume();
                break;

            default:
                ; /* VOID */
            }
            break;

        default:
            ; /* VOID */
        }
        break;

    case WM_TIMER:
        refresh();
        WM_RestartTimer(p_msg->Data.v, 250);
        break;

    default:
        WM_DefaultProc(p_msg);
    }
}

/**
 * \brief 歌词窗口回调函数
 *
 * \param[in] p_msg 指向 WM_MESSAGE 结构的指针
 *
 * \return 无
 */
static void lyrics_callback (WM_MESSAGE *p_msg)
{
    WM_HWIN         h_win  = p_msg->hWin;
    WM_MOTION_INFO *p_info = NULL;
    int             n_code = 0;
    int             id     = 0;

    GUI_RECT rect;           /* 窗体尺寸 */
    GUI_RECT location_rect;  /* 窗体坐标 */
    GUI_RECT str_rect;       /* 字符串显示区域 */

    int  btn_y_idx = 102;
    uint32_t  i    = 0;
    int  row_idx   = 205/2;
    uint8_t play_time[6];
    uint8_t second = 0;
    uint8_t minute = 0;
    uint8_t is_draw_yellow = 0;
    GUI_PID_STATE pid_state;
    int current_y0;
    int next_y0;
    int shift;
    double time = 0.5;
    double speed;

    /* 获取窗体信息 */
    WM_GetWindowRectEx(g_h_winlyrics, &location_rect);
    WM_GetClientRectEx(g_h_winlyrics, &rect);

    switch (p_msg->MsgId) {

    case WM_PAINT:

        /* 配置文本信息 */
        GUI_SetTextMode(GUI_TEXTMODE_TRANS);  /* 透明文本 */
        GUI_SetFont(&g_font18);
        GUI_SetColor(GUI_WHITE);

        /* 绘制歌词 */
        for (i = 0; i < g_lyric.row_num; i++) {
            if (i < g_lyric.row_num - 1) {
                if (((g_lyric.time[i] < g_play_ctrl.current_time) &&
                   (g_lyric.time[i + 1] > g_play_ctrl.current_time)) &&
                   (0 == is_draw_yellow)) {
                    GUI_SetColor(GUI_LIGHTYELLOW);
                    is_draw_yellow = 1;
                } else {
                    GUI_SetColor(GUI_WHITE);
                }
            } else {
                if ((g_lyric.time[i] < g_play_ctrl.current_time) &&
                   (0 == is_draw_yellow)) {
                    GUI_SetColor(GUI_LIGHTYELLOW);
                    is_draw_yellow = 1;
                } else {
                    GUI_SetColor(GUI_WHITE);
                }
            }

            GUI_DispStringHCenterAt(g_lyric.lrc[i], rect.x1/2, row_idx);
            row_idx += LRC_COL_HIGHT + LRC_COL_PITCH;
        }

        if (1 == g_is_lrc_move) {
            WM_ShowWindow(g_h_button_7);

            /* 绘制调整时间 */
            btn_y_idx -= location_rect.y0;
            GUI_SetFont(&g_font11);
            GUI_SetColor(0x79f2f4);
            second = g_move_time / 1000 % 60;
            minute = ((g_move_time/1000) - (g_move_time / 1000 % 60))/60 % 60;
            sprintf(play_time, "%02d:%02d", minute, second);
            str_rect.x0 = 5;
            str_rect.x1 = 48;
            str_rect.y0 = btn_y_idx - 10;
            str_rect.y1 = btn_y_idx + 10;
            GUI_DispStringInRect(play_time,
                                &str_rect,
                                 GUI_TA_HCENTER | GUI_TA_VCENTER);

            /* 绘制调整线 */
            GUI_DrawBitmap(&g_line_l, 48, btn_y_idx);
            GUI_DrawBitmap(&g_line_r, 313, btn_y_idx);

            /* 将按钮移动到指定位置 */
            WM_MoveChildTo(g_h_button_7, 448, btn_y_idx - 8);
        } else {
            WM_HideWindow(g_h_button_7);
        }
        break;

    case WM_NOTIFY_PARENT:
        id = WM_GetId(p_msg->hWinSrc);
        n_code = p_msg->Data.v;
        switch (id) {

        case ID_BUTTON_7:
            switch (n_code) {

            case WM_NOTIFICATION_CLICKED:
                break;

            case WM_NOTIFICATION_RELEASED:
                g_play_ctrl.current_time = g_move_time;
                play_event_send(PLAY_EVENT_SET_TIME); /* 发送设置当前时间事件 */
                g_is_lrc_move = 0;
                WM_Paint(h_win);
                break;

            default:
                ; /* VOID */
            }
            break;
        }
        break;

    case WM_MOTION:
        p_info = (WM_MOTION_INFO *)p_msg->Data.p;
        switch (p_info->Cmd) {

        case WM_MOTION_MOVE:
            GUI_TOUCH_GetState(&pid_state);
            if ((pid_state.Pressed) &&
                (pid_state.y < 205) &&
                (WM_IsVisible(g_h_winlist_0) == 0)) {

                g_is_lrc_move = 1;
                WM_RestartTimer(g_h_timer_2, 2000);
                int y_idx = -location_rect.y0 / (LRC_COL_HIGHT + LRC_COL_PITCH);
                y_idx = (y_idx == g_lyric.row_num) ? y_idx - 1 : y_idx;
                g_move_time = g_lyric.time[y_idx];
                WM_Paint(g_h_winlyrics);
            }
        }

        break;

    case WM_TIMER:

        if (WM_GetTimerId(p_msg->Data.v) == 1) {
            if (PALY_STATE_PLAY == g_play_ctrl.paly_state) {
                refresh_lrc();
            }
            WM_RestartTimer(g_h_timer_1, 100);
        } else {
            g_is_lrc_move = 0;
            current_y0 = WM_GetWindowOrgY(g_h_winlyrics);
            next_y0 = (-g_old_row * (LRC_COL_HIGHT + LRC_COL_PITCH)) -
                      LRC_COL_HIGHT/2;
            shift = next_y0 - current_y0;
            speed = shift / time;
            shift = shift < 0 ? -shift : shift;
            WM_MOTION_SetMovement(g_h_winlyrics,
                                  GUI_COORD_Y,
                                  speed,
                                  shift);
            WM_Paint(g_h_winlyrics);
        }
        break;

    case WM_CREATE:
            g_h_button_7 = BUTTON_CreateEx(448, 102 - 8, 17, 17, h_win,
                                           WM_CF_SHOW, 0, ID_BUTTON_7);
            BUTTON_SetSkin(g_h_button_7, button_drawskin);
            WM_SetHasTrans(g_h_button_7);
        break;

    default:
        WM_DefaultProc(p_msg);
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
    GUI_RECT rect;           /* 窗体尺寸 */

    WM_GetClientRectEx(g_h_winlyrics, &rect);

    switch (p_msg->MsgId) {

    case WM_PAINT:
        GUI_EnableAlpha(1);                   /* 启用Alpha混合 */
        GUI_SetColor((0xC8UL << 24) | GUI_BLACK);
        GUI_FillRectEx(&rect);
        break;

    default:
        WM_DefaultProc(p_msg);
    }
}

/**
 * \brief 根据歌词重设歌词窗体长度
 *
 * \param[in]  lyric 指向lyric_info_t结构的指针
 *
 * \return 无
 */
void reset_lyric_y (lyric_info_t *p_lyric)
{
    int winlyrics_y = 0;

    winlyrics_y = (LRC_COL_HIGHT + LRC_COL_PITCH) * p_lyric->row_num + 205;
    WM_SetYSize(g_h_winlyrics, winlyrics_y);
}

/**
 * \brief 触摸屏校准回调函数
 *
 * \param[out] p_calib_data 校准数据
 *
 * \return 无
 */
void gui_calibrate_cb (void *p_calib_data)
{
    struct aw_ts_lib_five_point_data *pdata =
                               (struct aw_ts_lib_five_point_data *)p_calib_data;

    pdata->log[0].x = 20 - 1;
    pdata->log[0].y = 20 - 1;
    pdata->log[1].x = 460 - 1;
    pdata->log[1].y = 20 - 1;
    pdata->log[2].x = 460 - 1;
    pdata->log[2].y = 252 - 1;
    pdata->log[3].x = 20 - 1;
    pdata->log[3].y = 252 - 1;
    pdata->log[4].x = 239 - 1;
    pdata->log[4].y = 135 - 1;

    pdata->lcd_x_size = 480;
    pdata->lcd_y_size = 272;

    pdata->phy[0].x = 1855;
    pdata->phy[0].y = 431;
    pdata->phy[1].x = 211;
    pdata->phy[1].y = 419;
    pdata->phy[2].x = 208;
    pdata->phy[2].y = 1700;
    pdata->phy[3].x = 1851;
    pdata->phy[3].y = 1718;
    pdata->phy[4].x = 1027;
    pdata->phy[4].y = 1067;
}

/**
 * \brief GUI任务入口
 *
 * \param[in] p_arg 参数
 *
 * \return 无
 */
void player_gui_task_entry (void *p_arg)
{
    uint8_t sd_unmount_test = 0;
    unsigned char  temp = 0;
    dialog_info_t  info;
    struct aw_dir *p_dirp;
    int            recv = 0;

    WM_SetCreateFlags(WM_CF_MEMDEV); /* 窗口使用存储设备 */
    GUI_Init();                      /* 初始化emwin */

    aw_touch_gui_calibrate(AW_TS_LIB_FIVE_POINT, gui_calibrate_cb);

    /* 初始化xbf字体 */
#if 1
    xbf_init((void *)g_xbf_adobe_hei_std);
#else
    xbf_init((void *)g_ttf_res);
#endif
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();

    GUI_UC_SetEncodeUTF8();
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);  /* 透明文本 */

    WM_MOTION_Enable(1);
    BUTTON_SetReactOnLevel();

    WIDGET_SetDefaultEffect(&WIDGET_Effect_None); /* 设置控件风格 */

    /* 创建主菜单 */
    g_h_winmain_0 = GUI_CreateDialogBox(g_dialog_create, /* 控件资源表 */
                        GUI_COUNTOF(g_dialog_create),    /* 控件数量 */
                        main_dlg_callback, /* 对话框过程函数的回调函数 */
                        WM_HBKWIN,         /* 父窗口的句柄*/
                        0,                 /* 相对于父窗口的 X 轴位置 */
                        0);                /* 相对于父窗口的 Y 轴位置 */

    /* 创建主菜单定时器 */
    g_h_timer_0 = WM_CreateTimer(g_h_winmain_0, 0, 500, 0);

    /* 歌词窗体容器 */
    g_h_winhead = WM_CreateWindowAsChild(
                      0,              /* 父窗口在窗口坐标中的左上 X 位置 */
                      0,              /* 父窗口在窗口坐标中的左上 Y 位置 */
                      480,            /* 窗口的 X 尺寸 */
                      205,            /* 窗口的 Y 尺寸 */
                      g_h_winmain_0,  /* 父窗口的句柄 */
                      WM_CF_SHOW | WM_CF_HASTRANS , /* 窗口创建标识 */
                      dummy_callback, /* 回调例程的指针 */ 
                      0);             /* 要分配的额外字节数 */

    /* 创建歌词窗体 */
    g_h_winlyrics = WM_CreateWindowAsChild(
                        0,               /* 父窗口在窗口坐标中的左上 X 位置 */
                        0,               /* 父窗口在窗口坐标中的左上 Y 位置 */
                        480,             /* 窗口的 X 尺寸 */
                        0,               /* 窗口的 Y 尺寸 */
                        g_h_winhead,     /* 父窗口的句柄 */
                        WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_MOTION_Y, /* 标识 */
                        lyrics_callback, /* 回调例程的指针 */
                        0);              /* 要分配的额外字节数 */

    /* 创建歌词定时器 */
    g_h_timer_1 = WM_CreateTimer(g_h_winlyrics, 1, 0, 0);
    g_h_timer_2 = WM_CreateTimer(g_h_winlyrics, 2, 0, 0);

    /* 创建播放列表窗体 */
    g_h_winlist_0 = music_list_create(g_h_winmain_0); /* 创建播放列表窗口 */
    WM_HideWindow(g_h_winlist_0);                     /* 隐藏播放列表窗口 */

    g_h_setting_0 = setting_create(g_h_winmain_0);
    WM_HideWindow(g_h_setting_0);                     /* 隐藏系统设置窗口 */

    IMAGE_SetJPEG(g_h_image_0,
                  g_res_background_2,
                  sizeof(g_res_background_2));

    /* 将之前创建的窗口绘制出来 */
    GUI_Exec();

    /* 释放GUI初始化完成信号量 */
    AW_SEMB_GIVE(g_play_ctrl.gui_ok);

    dialog_info_init(&info);
    info.font_title  = &g_font14;
    info.font_text   = &g_font14;
    info.font_btn[0] = &g_font14;
    info.font_btn[1] = &g_font14;
    info.btn_color_text[1] = 0xD4881E;

    AW_FOREVER {
        if ((p_dirp = aw_opendir("/udisk")) == NULL) {
            if (1 == g_is_mount_udisk) {
                aw_umount("/udisk", 0);
                g_is_mount_udisk = 0;

                play_list_init(g_play_list,
                               sizeof(g_play_list) / sizeof(g_play_list[0]),
                               1,
                               SEARCH_RANGE_USB);

                winlist_recreate();

                strcpy(info.title, "");
                strcpy(info.text, "USB存储器已移除");
                strcpy(info.btn_text[0], "确定");
                info.btn_num = 1;
                info.blocking = 0;
                dialog_show(130,
                            76,
                            info.rect.x1 + 1,
                            info.rect.y1 + 1,
                            g_h_winmain_0,
                            WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_STAYONTOP,
                           &info);
            }
        } else {
            g_is_mount_udisk = 1;
            aw_closedir(p_dirp);
        }

        if (!g_is_mount_udisk) {

            /* 文件系统挂载 */
            recv = aw_mount("/udisk", "/dev/ms2-ud0", "vfat", 0);
            if (AW_OK == recv) {
                g_is_mount_udisk = !g_is_mount_udisk;
                AW_INFOF(("Find a new disk\n"));

                strcpy(info.title, "发现新的USB存储器");
                strcpy(info.text, "是否搜索歌曲");
                strcpy(info.btn_text[0], "取消");
                strcpy(info.btn_text[1], "确定");
                info.btn_num = 2;
                info.blocking = 1;
                if (dialog_show(130,
                                76,
                                info.rect.x1 + 1,
                                info.rect.y1 + 1,
                                g_h_winmain_0,
                                WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_STAYONTOP,
                               &info) == 2)
                {
                    play_list_init(g_play_list,
                                   sizeof(g_play_list) / sizeof(g_play_list[0]),
                                   1,
                                   SEARCH_RANGE_USB);

                    winlist_recreate();
                    play_list_write(g_play_list,
                                   &g_list_state,
                                    PLAY_LIST_MAX_SIZE);
                }
            }
        }
        if (((p_dirp = aw_opendir("/sd0")) == NULL) || (1 == sd_unmount_test)) {
            if (1 == g_is_mount_sdcard) {
                aw_umount("/sd0", 0);
                g_is_mount_sdcard = 0;

                play_list_init(g_play_list,
                               sizeof(g_play_list) / sizeof(g_play_list[0]),
                               0,
                               SEARCH_RANGE_SD);

                winlist_recreate();

                strcpy(info.title, "");
                strcpy(info.text, "SD卡已移除");
                strcpy(info.btn_text[0], "确定");
                info.btn_num = 1;
                info.blocking = 0;
                dialog_show(130,
                            76,
                            info.rect.x1 + 1,
                            info.rect.y1 + 1,
                            g_h_winmain_0,
                            WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_STAYONTOP,
                           &info);
            }
        } else {
            g_is_mount_sdcard = 1;
            aw_closedir(p_dirp);
        }

        if ((!g_is_mount_sdcard) && (0 == sd_unmount_test)) {

            /* 文件系统挂载 */
            recv = aw_mount("/sd0", "/dev/sd0", "vfat", 0);
            if (AW_OK == recv) {
                g_is_mount_sdcard = !g_is_mount_sdcard;
                AW_INFOF(("Find a new sdcard\n"));

                /* 初始化播放列表 */
                recv = play_list_read(g_play_list,
                                     &g_list_state,
                                      PLAY_LIST_MAX_SIZE);
                if (0 == g_is_mount_udisk) {
                    usb_play_list_clean(g_play_list, PLAY_LIST_MAX_SIZE);
                }
                if (AW_OK == recv) {
                    winlist_recreate();
                    strcpy(info.title, "发现新的SD卡");
                    strcpy(info.text, "播放列表读取完成");
                    strcpy(info.btn_text[0], "确定");
                    info.btn_num = 1;
                    info.blocking = 0;
                    dialog_show(130,
                                76,
                                info.rect.x1 + 1,
                                info.rect.y1 + 1,
                                g_h_winmain_0,
                                WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_STAYONTOP,
                               &info);
                    GUI_Delay(1500);
                    dialog_close();
                } else {
                    dialog_info_init(&info);
                    info.font_title  = &g_font14;
                    info.font_text   = &g_font14;
                    info.font_btn[0] = &g_font14;
                    info.font_btn[1] = &g_font14;
                    info.btn_color_text[1] = 0xD4881E;
                    strcpy(info.title, "发现新的SD卡");
                    strcpy(info.text, "是否搜索歌曲");
                    strcpy(info.btn_text[0], "取消");
                    strcpy(info.btn_text[1], "确定");
                    info.btn_num = 2;
                    info.blocking = 1;
                    if (dialog_show(
                                130,
                                76,
                                info.rect.x1 + 1,
                                info.rect.y1 + 1,
                                g_h_winmain_0,
                                WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_STAYONTOP,
                               &info) == 2)
                    {
                        play_list_init(g_play_list,
                                       sizeof(g_play_list) /
                                       sizeof(g_play_list[0]),
                                       1,
                                       SEARCH_RANGE_SD);

                        winlist_recreate();
                        play_list_write(g_play_list,
                                       &g_list_state,
                                        PLAY_LIST_MAX_SIZE);
                    }

                }

                /* 获取配置文件 */
                config_read(&g_config_info);
                if ((1 == g_config_info.is_effect) &&
                    (g_config_info.play_idx <
                    g_list_state.play_list_count)) {
                    volume_set(g_config_info.volume);
                    g_play_ctrl.play_idx = g_config_info.play_idx;

                } else {
                    volume_set(50);
                    g_play_ctrl.play_idx = 0;
                    g_config_info.lrc_show = 1;
                    g_config_info.singer_show = 1;
                    g_config_info.is_effect = 1;
                }
            }
        }

        GUI_Delay(500);
    }
}

/**
 * \brief 初始化GUI任务
 */
void player_gui_task_init (void)
{
    /* 初始化任务 */
    AW_TASK_INIT(player_gui_task,             /* 任务实体 */
                 "player_gui_task",           /* 任务名字 */
                 PLAYER_GUI_TASK_PRIO,        /* 任务优先级 */
                 PLAYER_GUI_TASK_STACK_SIZE,  /* 任务堆栈大小 */
                 player_gui_task_entry,       /* 任务入口函数 */
                 (void *)NULL);               /* 任务入口参数 */

    /* 启动任务 */
    AW_TASK_STARTUP(player_gui_task);
}

/* end of file */
