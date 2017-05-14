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
 * \brief flac����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-28 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __flac_DECODER_H
#define __flac_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#pragma pack(push)
#pragma pack(2)

/**
 * \brief flaC��ǩ
 */
typedef struct flac_tag {

    uint8_t id[3];            /**< \brief ID,���ļ���ʼλ��,������flaC4����ĸ */

} flac_tag_t;

/**
 * \brief metadata ���ݿ�ͷ��Ϣ�ṹ��
 */
typedef struct md_block_head {

    uint8_t head;             /**< \brief metadata blockͷ */
    uint8_t size[3];          /**< \brief metadata block���ݳ��� */

} md_block_head_t;

/**
 * \brief FLAC���ƽṹ��
 */
typedef struct flac_ctrl {

    uint16_t nchannels;       /**< \brief ͨ������;1,��ʾ������;2,��ʾ˫���� */
    uint32_t total_time;      /**< \brief ���׸�ʱ�� ��λ:���� */
    uint32_t current_time;    /**< \brief ��ǰ����ʱ�� ��λ:���� */
    uint32_t bit_rate;        /**< \brief ������(λ��) */
    uint32_t sample_rate;     /**< \brief ������ */
    uint16_t bps;             /**< \brief λ��,����16bit,24bit,32bit */
    uint32_t data_start;      /**< \brief ����֡��ʼ��λ��(���ļ������ƫ��) */
    uint32_t file_size;       /**< \brief �ļ���С(byte) */
    uint32_t out_samples;     /**< \brief PCM��������� */

} flac_ctrl_t;

#pragma pack(pop)

/**
 * \brief ����flac�ļ�
 *
 * \param[in]  p_name      �ļ���
 * \param[out] p_flac_ctrl ��ʼ����flac�ṹ��
 *
 * \retval AW_OK      �����ɹ�
 * \retval -AW_ERROR  ����ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t get_flac_info (const char *p_name, flac_ctrl_t *p_flac_ctrl);

/**
 * \brief ����flac�ļ�
 *
 * \param[in] p_name �ļ���
 *
 * \retval AW_OK      ���ųɹ�
 * \retval -AW_ERROR  ����ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t flac_play (const char *p_name);

#ifdef __cplusplus
}
#endif

#endif /* __FLAC_DECODER_H */

/* end of file */
