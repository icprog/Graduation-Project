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
 * \brief 系统资源
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-17 张进科, 创建该文件
 * \endinternal
 */

#ifndef __SYSTEM_RESOURCE_H
#define __SYSTEM_RESOURCE_H

#ifndef GUI_CONST_STORAGE
    #define GUI_CONST_STORAGE const
#endif

/** \brief xbf字体资源 */
const unsigned char g_xbf_adobe_hei_std[7569637UL + 1];

/** \brief jpg格式背景图片资源 */
const unsigned char g_res_background_1[31786UL + 1];

/** \brief jpg格式背景图片资源 */
const unsigned char g_res_background_2[148351UL + 1];

/** \brief jpg格式背景图片资源 */
const unsigned char g_res_background_3[55996UL + 1];

/** \brief jpg格式背景图片资源 */
const unsigned char g_res_background_4[55442UL + 1];

/** \brief 播放按钮播放状态资源 */
GUI_CONST_STORAGE GUI_BITMAP g_play;

/** \brief 播放按钮暂停状态资源 */
GUI_CONST_STORAGE GUI_BITMAP g_pause;

/** \brief 播放按钮未按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_play_normal;

/** \brief 播放按钮按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_play_pressed;

/** \brief 上一曲按钮资源 */
GUI_CONST_STORAGE GUI_BITMAP g_up;

/** \brief 下一曲按钮资源 */
GUI_CONST_STORAGE GUI_BITMAP g_down;

/** \brief 上一曲、下一曲按钮未按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_up_down_normal;

/** \brief 上一曲、下一曲按钮按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_up_down_pressed;

/** \brief 随机播放按钮未按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_radom_normal;

/** \brief 随机播放按钮按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_radom_pressed;

/** \brief 顺序播放按钮未按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_repeat_normal;

/** \brief 顺序播放按钮按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_repeat_pressed;

/** \brief 单曲循环按钮未按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_repeat_o_normal;

/** \brief 单曲循环按钮按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_repeat_o_pressed;

/** \brief 歌曲信息按钮未按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_info_normal;

/** \brief 歌曲信息按钮按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_info_pressed;

/** \brief 系统设置按钮未按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_setting_normal;

/** \brief 系统设置按钮按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_setting_pressed;

/** \brief 播放列表按钮未按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_list_normal;

/** \brief 播放列表按钮按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_list_pressed;

/** \brief 播放时间进度条背景资源 */
GUI_CONST_STORAGE GUI_BITMAP g_slider_shaft;

/** \brief 播放时间进度条左端资源 */
GUI_CONST_STORAGE GUI_BITMAP g_slider_thumb_l;

/** \brief 播放时间进度条右端资源 */
GUI_CONST_STORAGE GUI_BITMAP g_slider_thumb_r;

/** \brief 音量条资源 */
GUI_CONST_STORAGE GUI_BITMAP g_volume_shaft;

/** \brief 音量条左端资源 */
GUI_CONST_STORAGE GUI_BITMAP g_volume_thumb_l;

/** \brief 音量条右端资源 */
GUI_CONST_STORAGE GUI_BITMAP g_volume_thumb_r;

/** \brief 半透明灰色条资源 */
GUI_CONST_STORAGE GUI_BITMAP g_strip;

/** \brief 喇叭图标资源 */
GUI_CONST_STORAGE GUI_BITMAP g_beep;

/** \brief 歌词时间调整线左端资源 */
GUI_CONST_STORAGE GUI_BITMAP g_line_l;

/** \brief 歌词时间调整线右端资源 */
GUI_CONST_STORAGE GUI_BITMAP g_line_r;

/** \brief 歌词时间调整按钮资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_lrc;


/** \brief playlist logo资源 */
GUI_CONST_STORAGE GUI_BITMAP g_text_playlist;

/** \brief 播放按钮播放状态白色资源 */
GUI_CONST_STORAGE GUI_BITMAP g_list_play_white;

/** \brief 播放按钮播放状态黑色资源 */
GUI_CONST_STORAGE GUI_BITMAP g_list_play_black;

/** \brief 播放按钮暂停状态资源 */
GUI_CONST_STORAGE GUI_BITMAP g_list_pause;

/** \brief 播放按钮资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_list_play;

/** \brief 删除按钮未按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_del_normal;

/** \brief 删除按钮按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_del_pressed;

/** \brief 刷新按钮未按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_refush_normal;

/** \brief 刷新按钮按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_refush_pressed;

/** \brief 关闭按钮未按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_close_normal;

/** \brief 关闭按钮按下资源 */
GUI_CONST_STORAGE GUI_BITMAP g_btn_close_pressed;


/** \brief 系统设置内选择框打开资源 */
GUI_CONST_STORAGE GUI_BITMAP g_on;

/** \brief 系统设置内选择框关闭资源 */
GUI_CONST_STORAGE GUI_BITMAP g_off;

#endif /* __SYSTEM_RESOURCE_H */

/* end of file */
