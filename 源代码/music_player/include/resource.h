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
 * \brief ��Դ����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-10 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __RESOURCE_H
#define __RESOURCE_H

#include "aw_errno.h"
#include "play_list.h"

/**
 * \brief ������Ϣ�ṹ��
 */
typedef struct config_info {

    int32_t volume;        /**< \brief ���� */
    int32_t play_idx;      /**< \brief ��ǰ���ڲ��ŵ��������� */
    uint8_t lrc_show;      /**< \brief �Ƿ���ʾ��� 0:����ʾ 1:��ʾ */
    uint8_t singer_show;   /**< \brief �Ƿ���ʾ����ͼƬ 0:����ʾ 1:��ʾ */
    uint8_t is_effect;     /**< \brief �����Ƿ���Ч 0:��Ч 1:��Ч */

} config_info_t;

/** \brief ������Ϣ */
extern config_info_t g_config_info;

/**
 * \brief ��ȡ�����б�
 *
 * \param[in] p_play_list  �����б�
 * \param[in] p_list_state �����б�״̬
 * \param[in] lenth        �����б���
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t play_list_read (music_info_t *p_play_list,
                         list_state_t *p_list_state,
                         uint32_t      lenth);

/**
 * \brief ���沥���б�
 *
 * \param[in] p_play_list  �����б�
 * \param[in] p_list_state �����б�״̬
 * \param[in] lenth        �����б���
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t play_list_write (music_info_t *p_play_list,
                          list_state_t *p_list_state,
                          uint32_t      lenth);
/**
 * \brief ��ȡ������Ϣ
 *
 * \param[in] p_config_info ������Ϣ
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t config_read (config_info_t *p_config_info);

/**
 * \brief ����������Ϣ
 *
 * \param[in] p_config_info ������Ϣ
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t config_write (config_info_t *p_config_info);

/**
 * \brief ��ȡ�ļ���С
 *
 * \param[in] fd �Ѿ��򿪵��ļ����
 *
 * \return С��0��������Ϊ��ȡ�����ļ���С
 */
int file_size_get (int fd);

/**
 * \brief ��ָ��·�����ļ���ȡ���ڴ���
 *
 * \param[in]  p_path       �ļ�����·��
 * \param[out] p_buf        ��ȡ����λ��
 * \param[in]  lenth        ��������С
 * \param[out] p_read_lenth ��ȡ�����ļ���С
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t file_to_mem (const uint8_t *p_path,
                      uint8_t       *p_buf,
                      uint32_t       buf_lenth,
                      uint32_t      *p_read_lenth);

/**
 * \brief ��ȡ������ͬ�ļ����ĸ��
 *
 * \param[in]  p_path       �����ļ���
 * \param[out] p_buf        ��ȡ����λ��
 * \param[in]  lenth        ��������С
 * \param[out] p_read_lenth ��ȡ�����ļ���С
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t lrc_read (const uint8_t  *p_path,
                   uint8_t        *p_buf,
                   uint32_t        buf_lenth,
                   uint32_t       *p_read_lenth);

/**
 * \brief ��ȡ������ͬ�ļ�����ͼƬ
 *
 * \param[in]  p_path       �����ļ���
 * \param[out] p_buf        ��ȡ����λ��
 * \param[in]  lenth        ��������С
 * \param[out] p_read_lenth ��ȡ�����ļ���С
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t jpg_read (const uint8_t  *p_path,
                   uint8_t        *p_buf,
                   uint32_t        buf_lenth,
                   uint32_t       *p_read_lenth);

/**
 * \brief ���Թ���SD����U��
 */
void mount_try (void);

/**
 * \brief ��ʼ���������Դ
 */
void resource_init (void);

#endif /* __RESOURCE_H */

/* end of file */
