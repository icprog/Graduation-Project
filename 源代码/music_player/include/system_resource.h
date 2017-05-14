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
 * \brief ϵͳ��Դ
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-17 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __SYSTEM_RESOURCE_H
#define __SYSTEM_RESOURCE_H

#ifndef GUI_CONST_STORAGE
    #define GUI_CONST_STORAGE const
#endif

/** \brief xbf������Դ */
const unsigned char g_xbf_adobe_hei_std[7569637UL + 1];

/** \brief jpg��ʽ����ͼƬ��Դ */
const unsigned char g_res_background_1[31786UL + 1];

/** \brief jpg��ʽ����ͼƬ��Դ */
const unsigned char g_res_background_2[148351UL + 1];

/** \brief jpg��ʽ����ͼƬ��Դ */
const unsigned char g_res_background_3[55996UL + 1];

/** \brief jpg��ʽ����ͼƬ��Դ */
const unsigned char g_res_background_4[55442UL + 1];

/** \brief ���Ű�ť����״̬��Դ */
GUI_CONST_STORAGE GUI_BITMAP g_play;

/** \brief ���Ű�ť��ͣ״̬��Դ */
GUI_CONST_STORAGE GUI_BITMAP g_pause;

/** \brief ���Ű�ťδ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_play_normal;

/** \brief ���Ű�ť������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_play_pressed;

/** \brief ��һ����ť��Դ */
GUI_CONST_STORAGE GUI_BITMAP g_up;

/** \brief ��һ����ť��Դ */
GUI_CONST_STORAGE GUI_BITMAP g_down;

/** \brief ��һ������һ����ťδ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_up_down_normal;

/** \brief ��һ������һ����ť������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_up_down_pressed;

/** \brief ������Ű�ťδ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_radom_normal;

/** \brief ������Ű�ť������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_radom_pressed;

/** \brief ˳�򲥷Ű�ťδ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_repeat_normal;

/** \brief ˳�򲥷Ű�ť������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_repeat_pressed;

/** \brief ����ѭ����ťδ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_repeat_o_normal;

/** \brief ����ѭ����ť������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_repeat_o_pressed;

/** \brief ������Ϣ��ťδ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_info_normal;

/** \brief ������Ϣ��ť������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_info_pressed;

/** \brief ϵͳ���ð�ťδ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_setting_normal;

/** \brief ϵͳ���ð�ť������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_setting_pressed;

/** \brief �����б�ťδ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_list_normal;

/** \brief �����б�ť������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_list_pressed;

/** \brief ����ʱ�������������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_slider_shaft;

/** \brief ����ʱ������������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_slider_thumb_l;

/** \brief ����ʱ��������Ҷ���Դ */
GUI_CONST_STORAGE GUI_BITMAP g_slider_thumb_r;

/** \brief ��������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_volume_shaft;

/** \brief �����������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_volume_thumb_l;

/** \brief �������Ҷ���Դ */
GUI_CONST_STORAGE GUI_BITMAP g_volume_thumb_r;

/** \brief ��͸����ɫ����Դ */
GUI_CONST_STORAGE GUI_BITMAP g_strip;

/** \brief ����ͼ����Դ */
GUI_CONST_STORAGE GUI_BITMAP g_beep;

/** \brief ���ʱ������������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_line_l;

/** \brief ���ʱ��������Ҷ���Դ */
GUI_CONST_STORAGE GUI_BITMAP g_line_r;

/** \brief ���ʱ�������ť��Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_lrc;


/** \brief playlist logo��Դ */
GUI_CONST_STORAGE GUI_BITMAP g_text_playlist;

/** \brief ���Ű�ť����״̬��ɫ��Դ */
GUI_CONST_STORAGE GUI_BITMAP g_list_play_white;

/** \brief ���Ű�ť����״̬��ɫ��Դ */
GUI_CONST_STORAGE GUI_BITMAP g_list_play_black;

/** \brief ���Ű�ť��ͣ״̬��Դ */
GUI_CONST_STORAGE GUI_BITMAP g_list_pause;

/** \brief ���Ű�ť��Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_list_play;

/** \brief ɾ����ťδ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_del_normal;

/** \brief ɾ����ť������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_del_pressed;

/** \brief ˢ�°�ťδ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_refush_normal;

/** \brief ˢ�°�ť������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_refush_pressed;

/** \brief �رհ�ťδ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_close_normal;

/** \brief �رհ�ť������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_btn_close_pressed;


/** \brief ϵͳ������ѡ������Դ */
GUI_CONST_STORAGE GUI_BITMAP g_on;

/** \brief ϵͳ������ѡ���ر���Դ */
GUI_CONST_STORAGE GUI_BITMAP g_off;

#endif /* __SYSTEM_RESOURCE_H */

/* end of file */
