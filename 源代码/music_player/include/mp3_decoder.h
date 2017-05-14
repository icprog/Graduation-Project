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
 * \brief mp3����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-17 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __MP3_DECODER_H
#define __MP3_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "mad.h"

/**
 * \brief MP3���Ż���
 */
struct mp3_buffer {

    unsigned char const *p_start; /**< \brief �������׵�ַ */
    unsigned long        length;  /**< \brief ���������� */

};

/**
 * \brief ���ſ��ƽṹ��
 */
typedef struct mp3_info {

    uint16_t nchannels;       /**< \brief ͨ������;1,��ʾ������;2,��ʾ˫���� */
    uint32_t total_time;      /**< \brief ���׸�ʱ�� ��λ:���� */
    uint32_t current_time;    /**< \brief ��ǰ����ʱ�� ��λ:���� */
    uint32_t bit_rate;        /**< \brief ������ */
    uint32_t sample_rate;     /**< \brief ������ */
    uint32_t data_start;      /**< \brief ����֡��ʼ��λ��(���ļ������ƫ��) */
    uint32_t data_size;       /**< \brief ���ݴ�С */
    uint32_t file_size;       /**< \brief �ļ���С(byte) */
    double   frame_num;       /**< \brief ֡�� */
    double   frame_size;      /**< \brief ֡��С */
    double   frame_time;      /**< \brief ֡ʱ�� ��λ:ms */

} mp3_info_t;


/**
 * \brief ����mp3�ļ�
 *
 * \param[in]  p_name     �ļ���
 * \param[out] p_wav_ctrl ��ʼ����mp3�ṹ��
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t mp3_info_get (const char *p_name, mp3_info_t *p_mp3_info);

/**
 * \brief ����MP3�ļ�
 *
 * \param[in] file_name �ļ���
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t mp3_play (const char *p_name);

#ifdef __cplusplus
}
#endif

#endif /* __MP3_DECODER_H */

/* end of file */
