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
 * \brief �Զ���Ի���
 *
 * \internal
 * \par Modification history
 * - 1.00 17-04-06 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __CUSTOM_DIALOG_H
#define __CUSTOM_DIALOG_H

#include <stdint.h>
#include "WM.h"
#include "GUI.h"

int g_custom_dialog_state; /**< \brief �Ի���״̬ 0:δ���� 1:�Ѵ��� */

/**
 * \brief �Ի������Խṹ��
 */
typedef struct dialog_info {

    uint8_t   blocking;             /**< \brief �Ƿ����� 0: ������ 1: ���� */
    uint8_t   modal;                /**< \brief ģ̬ 0: ��ģ̬ 1: ģ̬ */
    uint8_t   btn_num;              /**< \brief �������� [1-2] */
    uint32_t  btn_height;           /**< \brief �����߶� (����) */
    GUI_COLOR color_title;          /**< \brief �����ı���ɫ */
    GUI_COLOR color_text;           /**< \brief ��Ϣ�ı���ɫ */
    GUI_COLOR color_background;     /**< \brief ������ɫ */
    GUI_COLOR color_line;           /**< \brief ������ɫ */
    GUI_COLOR btn_color_text[2];    /**< \brief �����ı���ɫ */
    GUI_COLOR btn_color_normal[2];  /**< \brief ����δ������ɫ */
    GUI_COLOR btn_color_pressed[2]; /**< \brief ����������ɫ */
    GUI_FONT *font_title;           /**< \brief �������� */
    GUI_FONT *font_text;            /**< \brief ��Ϣ���� */
    GUI_FONT *font_btn[2];          /**< \brief �����ı����� */

    GUI_RECT rect;                  /**< \brief �Ի����С */
    GUI_RECT title_rect;            /**< \brief ������ʾ��Χ */
    GUI_RECT text_rect;             /**< \brief ��Ϣ�ı���ʾ��Χ */

    int  r;                         /**< \brief Բ�ǰ뾶 */
    char title[256];                /**< \brief ���� */
    char text[256];                 /**< \brief ��Ϣ */
    char btn_text[2][64];           /**< \brief �����ı� */

} dialog_info_t; 

/**
 * \brief ��ʼ���Ի�����Ϣ�ṹ��
 *
 * \param[in] p_info �Ի�����Ϣ
 *
 * \return ��
 */
void dialog_info_init (dialog_info_t *p_info);

/**
 * \brief ��ȡָ����ʼ���������֮����ַ���
 *
 * \param[in] x0       �������ڴ��������е����� X λ��
 * \param[in] y0       �������ڴ��������е����� Y λ��
 * \param[in] x_size   ���ڵ� X �ߴ硣���Ϊ 0�����ø����ڿͻ����� X �ߴ�
 * \param[in] y_size   ���ڵ� Y �ߴ硣���Ϊ 0�����ø����ڿͻ����� Y �ߴ�
 * \param[in] h_parent �����ڵľ��
 * \param[in] style    ���ڴ�����ʶ(����� WM_CreateWindow())
 * \param[in] info     �Ի�������
 *
 * \retval 0 δ�����ť
 * \retval 1 �����ť0
 * \retval 2 �����ť1
 */
int dialog_show (int            x0,
                 int            y0,
                 int            x_size,
                 int            y_size,
                 GUI_HWIN       h_parent,
                 uint32_t       style,
                 dialog_info_t *p_info);

/**
 * \brief �رնԻ���
 */
void dialog_close (void);

/**
 * \brief ����text
 *
 * \param[in] text �����õ�text
 *
 * \return ��
 */
void text_set (const char *p_text);

#endif /* __CUSTOM_DIALOG_H */

/* end of file */
