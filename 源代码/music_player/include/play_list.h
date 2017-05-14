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
 * \brief �����б�
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-15 �Ž���, �������ļ�
 * \endinternal
 */

#ifndef __PLAY_LIST_H
#define __PLAY_LIST_H

#include <stdint.h>

#define SEARCH_RANGE_SD_USB              (0)    /** \brief ����SD����U�� */
#define SEARCH_RANGE_SD                  (1)    /** \brief ����SD�� */
#define SEARCH_RANGE_USB                 (2)    /** \brief ����U�� */

#define PLAY_LIST_MAX_SIZE               (1000) /** \brief �б���󳤶� */
#define MUSIC_INFO_ARTIST_MAX_LENTH      (100)  /** \brief ��������󳤶� */
#define MUSIC_INFO_NAME_MAX_LENTH        (50)   /** \brief ������󳤶� */
#define MUSIC_INFO_FILE_PATH_MAX_LENTH   (255)  /** \brief �ļ�·����󳤶� */

/**
 * \brief ��Ƶ��ʽ
 */
typedef enum music_type {

    MUSIC_TYPE_WAV = 0,    /** \brief WAV��ʽ */
    MUSIC_TYPE_MP3,        /** \brief MP3��ʽ */
    MUSIC_TYPE_FLAC,       /** \brief FLAC��ʽ */
    MUSIC_TYPE_OTHER,      /** \brief ������ʽ */

} music_type_t;

/**
 * \brief ������Ϣ�ṹ
 */
typedef struct music_info {

    /** \brief ���� */
    uint16_t     index;

    /** \brief ���������� */
    uint8_t      artist[MUSIC_INFO_ARTIST_MAX_LENTH];

    /** \brief �������� */
    uint8_t      name[MUSIC_INFO_NAME_MAX_LENTH];

    /** \brief �ļ��� */
    uint8_t      file_name[MUSIC_INFO_FILE_PATH_MAX_LENTH];

    uint32_t     file_size;     /**< \brief �ļ���С(byte) */
    music_type_t type;          /**< \brief ��ʽ */
    uint32_t     total_time;    /**< \brief ��ʱ�� (ms) */
    uint8_t      bits_per_word; /**< \brief λ����һ��Ϊ8,16,24,32�ȣ� */
    uint8_t      channels;      /**< \brief ͨ���� */
    uint32_t     samples_rate;  /**< \brief ������ */
    uint32_t     bitrate;       /**< \brief ������ */

} music_info_t;

/**
 * \brief �б�״̬�ṹ
 */
typedef struct list_state {

    uint32_t play_list_count; /**< \brief �����б������ֵĸ��� */
    uint32_t sdcard_start;    /**< \brief SD����������ʼ���� */
    uint32_t sdcard_count;    /**< \brief SD������������ */
    uint32_t usb_start;       /**< \brief u����������ʼ���� */
    uint32_t usb_count;       /**< \brief u������������ */

} list_state_t;

/**
 * \brief ��ȡ�ļ���׺��
 *
 * \param[in]  p_name �ļ���
 * \param[out] p_str  ��׺��, �˻�������С����С��10
 *
 * \return С��0��������Ϊ��׺�����ļ����е�λ��
 */
int32_t get_file_type (const char *p_name, char *p_str);

/**
 * \brief ��ȡ�б��ڸ�����Ŀ
 *
 * \param[in] p_play_list �����б�
 * \param[in] lenth       �����б���
 *
 * \return ������Ŀ
 */
int music_count_get (music_info_t *p_play_list, uint32_t lenth);

/**
 * \brief ��ղ����б�
 *
 * \param[in] p_play_list �����б�
 * \param[in] lenth       �����б���
 *
 * \return ��
 */
void play_list_clean (music_info_t *p_play_list, uint32_t lenth);

/**
 * \brief ��ղ����б���u���ڵĸ���
 *
 * \param[in] p_play_list �����б�
 * \param[in] lenth       �����б���
 *
 * \return ��
 */
void usb_play_list_clean (music_info_t *p_play_list, uint32_t lenth);

/**
 * \brief ���б������һ�׸�
 *
 * \param[in] p_play_list  �����б�
 * \param[in] lenth        �����б���
 * \param[in] p_music_info ����ӵ�����, index���Բ��ó�ʼ��
 *
 * \return С��0��������Ϊ��ӵĸ������б��е�����
 */
int32_t play_list_add (music_info_t *p_play_list,
                       uint32_t      lenth,
                       music_info_t *p_music_info);

/**
 * \brief ���б���ɾ��һ�׸�
 *
 * \param[in] p_play_list  �����б�
 * \param[in] lenth        �����б���
 * \param[in] p_music_info ��ɾ��������
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t play_list_del (music_info_t *p_play_list,
                        uint32_t      lenth,
                        music_info_t *p_music_info);

/**
 * \brief �����ļ����ڸ�������ӽ������б�
 *
 * \param[in] p_play_list �����б�
 * \param[in] lenth       �����б���
 * \param[in] p_dir       ���������ļ���
 * \param[in] show_info   0: ����ʾinfo 1:��ʾinfo
 *
 * \return ��
 */
void play_list_scan (music_info_t *p_play_list,
                     uint32_t      lenth,
                     const char   *p_dir,
                     uint8_t       show_info);

/**
 * \brief ��ʼ�������б�
 *
 * \param[in] p_play_list �����б�
 * \param[in] lenth       �����б���
 * \param[in] show_info   0: ����ʾinfo 1:��ʾinfo
 * \param[in] range       0: ����SD����U�� 1:����SD�� 2:����U��
 *
 * \return ��
 */
void play_list_init (music_info_t *p_play_list,
                     uint32_t      lenth,
                     uint8_t       show_info,
                     uint8_t       range);

#endif /* __PLAY_LIST_H */

/* end of file */
