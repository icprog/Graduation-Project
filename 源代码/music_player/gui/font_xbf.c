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
 * \brief emwin ttf����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-04 �Ž���, �������ļ�
 * \endinternal
 */

/*******************************************************************************
  ͷ�ļ�����
*******************************************************************************/

#include "apollo.h"
#include "font_xbf.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "aw_vdebug.h"
#include "GUI.h"  

/*******************************************************************************
  �궨��
*******************************************************************************/

#define XBF_FONT11_BASE       0x00000000 /**< \brief FONT11�׵�ַ */
#define XBF_FONT13_BASE       0x00000FCC /**< \brief FONT13�׵�ַ */
#define XBF_FONT14_BASE       0x00001F98 /**< \brief FONT14�׵�ַ */
#define XBF_FONT18_BASE       0x002E7032 /**< \brief FONT18�׵�ַ */

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

/** \brief xbf���� */
GUI_FONT g_font11, g_font13, g_font14, g_font18;

/** \brief ������ */
GUI_XBF_DATA g_xbf_data11, g_xbf_data13, g_xbf_data14, g_xbf_data18;

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/**
 * \brief XBF����Ļص�����
 *
 * \param[in]  off       ��ַƫ��
 * \param[in]  num_bytes ��Ҫ�������ֽ���
 * \param[in]  p_void    XBF��������
 * \param[out] p_buffer  ��ȡ����ĵ�������
 *
 * \retval 0 ��ȡ�ɹ�
 */
static int data11_get_cb (uint32_t off,
                          uint16_t num_bytes,
                          void    *p_void,
                          void    *p_buffer)
{
    /* ��ȡ�������� */
    memcpy(p_buffer, (uint8_t *)p_void + XBF_FONT11_BASE + off, num_bytes);

    return 0;
}

/**
 * \brief XBF����Ļص�����
 *
 * \param[in]  off       ��ַƫ��
 * \param[in]  num_bytes ��Ҫ�������ֽ���
 * \param[in]  p_void    XBF��������
 * \param[out] p_buffer  ��ȡ����ĵ�������
 *
 * \retval 0 ��ȡ�ɹ�
 */
static int data13_get_cb (uint32_t off,
                          uint16_t num_bytes,
                          void    *p_void,
                          void    *p_buffer)
{
    /* ��ȡ�������� */
    memcpy(p_buffer, (uint8_t *)p_void + XBF_FONT13_BASE + off, num_bytes);

    return 0;
}

/**
 * \brief XBF����Ļص�����
 *
 * \param[in]  off       ��ַƫ��
 * \param[in]  num_bytes ��Ҫ�������ֽ���
 * \param[in]  p_void    XBF��������
 * \param[out] p_buffer  ��ȡ����ĵ�������
 *
 * \retval 0 ��ȡ�ɹ�
 */
static int data14_get_cb (uint32_t off,
                          uint16_t num_bytes,
                          void    *p_void,
                          void    *p_buffer)
{
    /* ��ȡ�������� */
    memcpy(p_buffer, (uint8_t *)p_void + XBF_FONT14_BASE + off, num_bytes);

    return 0;
}

/**
 * \brief XBF����Ļص�����
 *
 * \param[in]  off       ��ַƫ��
 * \param[in]  num_bytes ��Ҫ�������ֽ���
 * \param[in]  p_void    XBF��������
 * \param[out] p_buffer  ��ȡ����ĵ�������
 *
 * \retval 0 ��ȡ�ɹ�
 */
static int data18_get_cb (uint32_t off,
                          uint16_t num_bytes,
                          void    *p_void,
                          void    *p_buffer)
{
    /* ��ȡ�������� */
    memcpy(p_buffer, (uint8_t *)p_void + XBF_FONT18_BASE + off, num_bytes);

    return 0;
}

/**
 * \brief ��ʼ��xfb����
 *
 * \param[in] p_xbf_data ��������
 *
 * \return ��
 */
void xbf_init (void *p_xbf_data)
{
    if (NULL == p_xbf_data) {
        AW_ERRF(("xbf init failed\n"));
        return;
    }

    /* ���� 11 �������� */
    GUI_XBF_CreateFont(&g_font11,                 /* GUI_FONT ���ͱ���  */
                      &g_xbf_data11,             /* GUI_XBF_DATA ���ͱ��� */
                       GUI_XBF_TYPE_PROP_AA4_EXT, /* ��������  */
                       data11_get_cb,             /* �ص�����  */
                       p_xbf_data);               /* �ص���������   */

    /* ���� 13 �������� */
    GUI_XBF_CreateFont(&g_font13,                 /* GUI_FONT ���ͱ���  */
                      &g_xbf_data13,             /* GUI_XBF_DATA ���ͱ��� */
                       GUI_XBF_TYPE_PROP_AA4_EXT, /* ��������  */
                       data13_get_cb,             /* �ص�����  */
                       p_xbf_data);               /* �ص���������   */

    /* ���� 14 �������� */
    GUI_XBF_CreateFont(&g_font14,                 /* GUI_FONT ���ͱ���  */
                      &g_xbf_data14,             /* GUI_XBF_DATA ���ͱ��� */
                       GUI_XBF_TYPE_PROP_AA4_EXT, /* ��������  */
                       data14_get_cb,             /* �ص�����  */
                       p_xbf_data);               /* �ص���������   */

    /* ���� 18 �������� */
    GUI_XBF_CreateFont(&g_font18,                 /* GUI_FONT ���ͱ���  */
                      &g_xbf_data18,             /* GUI_XBF_DATA ���ͱ��� */
                       GUI_XBF_TYPE_PROP_AA4_EXT, /* ��������  */
                       data18_get_cb,             /* �ص�����  */
                       p_xbf_data);               /* �ص���������   */
}

/* end of file */
