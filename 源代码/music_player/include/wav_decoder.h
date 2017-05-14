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
 * \brief wav����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-11 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __WAV_DECODER_H
#define __WAV_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ID_RIFF  (0x46464952) /**< \brief RIFF�� */
#define ID_WAVE  (0x45564157) /**< \brief WAVE�� */
#define ID_FMT   (0x20746D66) /**< \brief FMT�� */
#define ID_FACT  (0x74636166) /**< \brief FACT�� */
#define ID_LIST  (0x5453494C) /**< \brief LIST�� */
#define ID_DATA  (0x61746164) /**< \brief DATA�� */

#pragma pack(push)
#pragma pack(2)

/**
 * \brief RIFF��
 */
typedef struct chunk_riff {

    uint32_t chunk_id;        /**< \brief chunk id;����̶�Ϊ"RIFF",��0x46464952 */
    uint32_t chunk_size;      /**< \brief ���ϴ�С;�ļ��ܴ�С-8 */
    uint32_t format;          /**< \brief ��ʽ;wave,��0x45564157 */

} chunk_riff_t;

/**
 * \brief fmt��
 */
typedef struct chunk_fmt {

    uint32_t chunk_id;        /**< \brief chunk id;����̶�Ϊ"fmt ",��0x20746d66 */
    uint32_t chunk_size;      /**< \brief �Ӽ��ϴ�С(������id��size);����Ϊ:20. */
    uint16_t audio_format;    /**< \brief ��Ƶ��ʽ; 0x01:����pcm 0x11: ima adpcm */
    uint16_t num_of_channels; /**< \brief ͨ������;1,��ʾ������;2,��ʾ˫����; */
    uint32_t sample_rate;     /**< \brief ������;0x1f40,��ʾ8khz */
    uint32_t byte_rate;       /**< \brief �ֽ�����; */
    uint16_t block_align;     /**< \brief �����(�ֽ�); */
    uint16_t bits_per_sample; /**< \brief �����������ݴ�С;4λadpcm,����Ϊ4 */
    uint16_t byte_extra_data; /**< \brief ���ӵ������ֽ�;2��; ����pcm,û��������� */

} chunk_fmt_t;

/**
 * \brief fact��
 */
typedef struct chunk_fact {

    uint32_t chunk_id;        /**< \brief chunk id; ����̶�Ϊ"fact" */
    uint32_t chunk_size;      /**< \brief �Ӽ��ϴ�С(������id��size);����Ϊ:4. */
    uint32_t num_of_samples;  /**< \brief ����������; */

} chunk_fact_t;

/**
 * \brief LIST��
 */
typedef struct chunk_list {

    uint32_t chunk_id;        /**< \brief chunk id;����̶�Ϊ"LIST"; */
    uint32_t chunk_size;      /**< \brief �Ӽ��ϴ�С(������id��size);����Ϊ:4. */

} chunk_list_t;

/**
 * \brief data��
 */
typedef struct chunk_data {

    uint32_t chunk_id;        /**< \brief chunk id;����̶�Ϊ"data" */
    uint32_t chunk_size;      /**< \brief �Ӽ��ϴ�С(������id��size) */

} chunk_data_t;

#pragma pack(pop)

/**
 * \brief wavͷ
 */
typedef struct wave_header { 

    chunk_riff_t *riff;             /**< \brief riff�� */
    chunk_fmt_t  *fmt;              /**< \brief fmt�� */
    chunk_fact_t *fact;             /**< \brief fact�� ����pcm,û������ṹ�� */
    chunk_data_t *data;             /**< \brief data�� */

} wave_header_t; 

/**
 * \brief ���ſ��ƽṹ��
 */
typedef struct wav_ctrl {

    uint16_t audio_format;    /**< \brief ��Ƶ��ʽ; 0x01:����pcm 0x11: ima adpcm */
    uint16_t nchannels;       /**< \brief ͨ������;1,��ʾ������;2,��ʾ˫���� */
    uint16_t block_align;     /**< \brief �����(�ֽ�) */
    uint32_t data_size;       /**< \brief WAV���ݴ�С */

    uint32_t total_time;      /**< \brief ���׸�ʱ�� ��λ:���� */
    uint32_t current_time;    /**< \brief ��ǰ����ʱ�� ��λ:���� */
    
    uint32_t bit_rate;        /**< \brief ������(λ��) */
    uint32_t sample_rate;     /**< \brief ������ */
    uint16_t bps;             /**< \brief λ��,����16bit,24bit,32bit */
    
    uint32_t data_start;      /**< \brief ����֡��ʼ��λ��(���ļ������ƫ��) */
    uint32_t file_size;       /**< \brief �ļ���С(byte) */

} wav_ctrl_t; 


/**
 * \brief ����WAV����
 *
 * \param[in]  p_data     wav�ļ�����
 * \param[in]  lenth      �ļ�����
 * \param[out] p_wav_ctrl wav���ƽṹ��
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t wav_decode_init (uint8_t    *p_data,
                          uint32_t    lenth,
                          wav_ctrl_t *p_wav_ctrl);

/**
 * \brief ����WAV�ļ�
 *
 * \param[in]  p_name     �ļ���
 * \param[out] p_wav_ctrl wav���ƽṹ��
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t wav_info_get (const char *p_name, wav_ctrl_t *p_wav_ctrl);

/**
 * \brief ����WAV�ļ�
 *
 * \param[in] p_name �ļ���
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t wav_play (const char *p_name);

#ifdef __cplusplus
}
#endif

#endif /* __WAV_DECODER_H */

/* end of file */
