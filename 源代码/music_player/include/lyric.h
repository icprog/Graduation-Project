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
 * \brief lrc����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-05 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __LYRIC_H
#define __LYRIC_H

#include <stdint.h>
#include "GUI.h"  

#define LRC_ID_TAG_MAX_LENTH      (50)
#define LRC_TIME_TAG_MAX_ROW      (1024)
#define LRC_TIME_TAG_MAX_LENTH    (512)

/** \brief �����Ϣ�ṹ�� */
typedef struct lyric_info {

    uint8_t  artist[LRC_ID_TAG_MAX_LENTH]; /**< \brief ���������� */
    uint8_t  title[LRC_ID_TAG_MAX_LENTH];  /**< \brief �������� */
    uint8_t  au[LRC_ID_TAG_MAX_LENTH];     /**< \brief �������-������ */
    uint8_t  album[LRC_ID_TAG_MAX_LENTH];  /**< \brief ר�� */
    uint8_t  by[LRC_ID_TAG_MAX_LENTH];     /**< \brief LRC����ļ������� */
    int32_t  offset;                       /**< \brief ����ʱֵ ��λ: ���� */
    uint32_t total_time;                   /**< \brief ��ʱ�� ��λ: �� */
    uint32_t row_num;                      /**< \brief ������� */
    uint32_t time[LRC_TIME_TAG_MAX_ROW];   /**< \brief ÿ�и�ʵ�ʱ�� ��λ:ms */

    /** \brief ��� */
    uint8_t  lrc[LRC_TIME_TAG_MAX_ROW][LRC_TIME_TAG_MAX_LENTH]; 

} lyric_info_t;

/** \brief ʱ���ǩ����״̬��״̬ */
typedef enum time_tag_st {

    TIME_TAG_ST_IDLE = 0,             /**< \brief ����̬ */
    TIME_TAG_ST_FIRST_BRACKETS,       /**< \brief ��һ�������� */
    TIME_TAG_ST_FIRST_NUMBER,         /**< \brief ��һ������ */
    TIME_TAG_ST_COLON,                /**< \brief ð�� */
    TIME_TAG_ST_SECOND_NUMBER,        /**< \brief �ڶ������� */
    TIME_TAG_ST_DOT,                  /**< \brief ��� */
    TIME_TAG_ST_THIRD_NUMBER,         /**< \brief ���������� */
    TIME_TAG_ST_SECOND_BRACKETS,      /**< \brief �ڶ��������� */

} time_tag_st_t;

/** \brief ��ʶ��ǩ����״̬��״̬ */
typedef enum id_tag_st {

    ID_TAG_ST_IDLE = 0,             /**< \brief ����̬ */
    ID_TAG_ST_FIRST_BRACKETS,       /**< \brief ��һ�������� */
    ID_TAG_ST_FIRST_VALUE,          /**< \brief ��һ��ֵ */
    ID_TAG_ST_COLON,                /**< \brief ð�� */
    ID_TAG_ST_SECOND_VALUE,         /**< \brief �ڶ���ֵ */
    ID_TAG_ST_SECOND_BRACKETS,      /**< \brief �ڶ��������� */

} id_tag_st_t;

/** \brief ��ʶ��ǩ���� */
typedef enum id_tag_tpye {

    ID_TAG_TPYE_AR = 0,             /**< \brief ���������� */
    ID_TAG_TPYE_TI,                 /**< \brief �������� */
    ID_TAG_TPYE_AU,                 /**< \brief �������-������ */
    ID_TAG_TPYE_AL,                 /**< \brief ר�� */
    ID_TAG_TPYE_BY,                 /**< \brief LRC����ļ������� */
    ID_TAG_TPYE_OFFSET,             /**< \brief ����ʱֵ */
    ID_TAG_TPYE_T_TIME,             /**< \brief ��ʱ�� */
    ID_TAG_TPYE_UNDEFINE,           /**< \brief δ����ı�ǩ */

} id_tag_tpye_t;

/**
 * \brief ����lrc���
 *
 * \param[in]  p_data *.lrc�ļ�����
 * \param[in]  lenth  ���ݳ���
 * \param[out] lyric  �������ĸ������
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t lrc_analyze (uint8_t *p_data, uint32_t lenth, lyric_info_t *p_lyric);

#endif /* __LYRIC_H */

/* end of file */
