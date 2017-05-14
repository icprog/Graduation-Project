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

/*******************************************************************************
  ͷ�ļ�����
*******************************************************************************/

#include "apollo.h"
#include "lyric.h"

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

/*******************************************************************************
  �궨��
*******************************************************************************/

/** \brief ״̬���ڲ�״̬�л� */
#define NEXT_STATE(state, next_state) (state = next_state)

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

static uint8_t  str_sub_buf[LRC_TIME_TAG_MAX_LENTH]; /**< \brief ������ */
static uint8_t  p_buf[LRC_TIME_TAG_MAX_LENTH];       /**< \brief ������ */
static uint8_t  p_buf1[LRC_TIME_TAG_MAX_LENTH];      /**< \brief ������ */
static uint32_t g_row_end_idx[LRC_TIME_TAG_MAX_ROW]; /**< \brief ��¼ÿ������ */

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/**
 * \brief ��ȡָ����ʼ���������֮����ַ���
 *
 * \param[in]  str         ������ַ���
 * \param[in]  char_start  ��ʼ��
 * \param[in]  char_end    ������
 * \param[out] p_out       ��ȡ�����ַ���
 * \param[out] p_start_idx ��ʼ����λ�ã�����NULL��ʾ����Ҫ
 * \param[out] p_end_idx   ��������λ�ã�����NULL��ʾ����Ҫ
 *
 * \return С��0��������Ϊ��ȡ�ַ����ĳ���
 */
int str_sub (uint8_t  *p_str,
             uint8_t   char_start,
             uint8_t   char_end,
             uint8_t  *p_out,
             uint32_t *p_start_idx,
             uint32_t *p_end_idx)
{
    uint32_t i             = 0;
    uint32_t  counter      = 0;  /* ��ȡ�����ַ��������� */
    uint8_t   is_start     = 0;  /* �Ƿ��⵽��ʼ�ַ� 0:δ��⵽ 1:��⵽ */
    uint8_t   is_end       = 0;  /* �Ƿ��⵽�����ַ� 0:δ��⵽ 1:��⵽ */
    uint32_t  src_str_len  = 0;  /* ����ȡ�ַ������� */

    if (NULL == p_str) {
        return -1;
    }

    /* �жϴ���ȡ�ַ������� */
    src_str_len = strlen(p_str);

    while (1) {
        /* �����ʼ�ַ� */
        if (0 == is_start) {
            if (p_str[i++] != char_start) {
                if (i >= src_str_len) {
                    break;
                }
                continue;
            } else {
                is_start = 1;
                if (p_start_idx != NULL) {
                    *p_start_idx = i - 1;
                }
            }
        }

        /* �������ַ� */
        if (p_str[i] == char_end) {
            is_end = 1;
            if (p_end_idx != NULL) {
                *p_end_idx = i;
            }
            break;
        }

        /* ����ַ��������� */
        if ('\0' == p_str[i]) {
            break;
        }

        if (1 == is_start) {
            str_sub_buf[counter++] = p_str[i];
        }
        if (counter >= LRC_TIME_TAG_MAX_LENTH) {
            break;
        }

        i++;
    }

    if ((0 == is_start) || (0 == is_end)) {
        return -1;
    } else {
        for (i = 0; i < counter; i++) {
            p_out[i] = str_sub_buf[i];
        }
        p_out[i] = '\0';

        return counter;
    }
}

/**
 * \brief �жϽ��������ַ����ǲ������֣���':'/'.'/']'/'\0'����
 *
 * \param[in]  p_filedata *.lrc�ļ�����
 * \param[out] p_eoc      �����Ľ�����������NULL��ʾ����Ҫ
 * \param[out] p_end_idx  ��������λ�ã�����NULL��ʾ����Ҫ
 *
 * \return С��0��������Ϊ������
 */
int is_num (uint8_t *p_str, uint8_t *p_eoc, uint32_t *p_end_idx)
{
    uint32_t i = 0;
    int is_num = 0;

    if (NULL == p_str) {
        return -1;
    }

    while (1) {
        if ((':' == p_str[i]) ||
            ('.' == p_str[i]) ||
            (']' == p_str[i]) ||
            ('\0' == p_str[i])) {
            if (p_eoc != NULL) {
                *p_eoc = p_str[i];
            }
            if (p_end_idx != NULL) {
                *p_end_idx = i;
            }
            if (0 == i) {
                is_num = -1;
            }
            break;
        }
        if ((p_str[i] < '0') || (p_str[i] > '9')) {
            is_num = -1;
        }
        i++;
    }

    return is_num;
}

/**
 * \brief �����������ж��ٸ�ʱ���ǩ
 *
 * \param[in] p_filedata *.lrc����
 * \param[in] lenth      ���ݳ���
 *
 * \return С��0��������Ϊ��������ʱ���ǩ����
 */
int time_tag_num_analyze (uint8_t *p_data, uint32_t lenth)
{
    uint32_t i            = 0;
    uint32_t state        = TIME_TAG_ST_IDLE; /* ״̬��Ĭ��Ϊ����̬ */
    uint8_t  eoc          = 0;                /* ���ս����ַ� */
    uint32_t end_idx      = 0;                /* ���ս������� */
    int      recv         = 0;                /* ���պ�������ֵ */
    int      tag_num      = 0;

    if (NULL == p_data) {
        return -1;
    }

    for (i = 0; i < lenth; i++) {
        switch (state) {

        case TIME_TAG_ST_IDLE:              /* ����̬ */ 
            if ('[' == p_data[i]) {
                i--;
                NEXT_STATE(state, TIME_TAG_ST_FIRST_BRACKETS);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            }

            break;                          
                                            
        case TIME_TAG_ST_FIRST_BRACKETS:    /* ��һ�������� */
            if (is_num(p_data + i + 1, &eoc, &end_idx) < 0) {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_FIRST_NUMBER);
            }

            break;                          
                                            
        case TIME_TAG_ST_FIRST_NUMBER:      /* ��һ������ */
            if (':' == eoc) {
                i += end_idx - 1;
                NEXT_STATE(state, TIME_TAG_ST_COLON);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            }

            break;

        case TIME_TAG_ST_COLON:             /* ð�� */
            if (is_num(p_data + i + 1, &eoc, &end_idx) < 0) {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_SECOND_NUMBER);
            }

            break;

        case TIME_TAG_ST_SECOND_NUMBER:     /* �ڶ������� */
            if ('.' == eoc) {
                i += end_idx - 1;
                NEXT_STATE(state, TIME_TAG_ST_DOT);
            } else if (']' == eoc) {
                i += end_idx - 1;
                NEXT_STATE(state, TIME_TAG_ST_SECOND_BRACKETS);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            }

            break;

        case TIME_TAG_ST_DOT:               /* ��� */
            if (is_num(p_data + i + 1, &eoc, &end_idx) < 0) {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_THIRD_NUMBER);
            }

            break;

        case TIME_TAG_ST_THIRD_NUMBER:      /* ���������� */
            if (']' == eoc) {
                i += end_idx - 1;
                NEXT_STATE(state, TIME_TAG_ST_SECOND_BRACKETS);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            }

            break;

        case TIME_TAG_ST_SECOND_BRACKETS:   /* �ڶ��������� */
            tag_num++;
            NEXT_STATE(state, TIME_TAG_ST_IDLE);

            break;
        
        default: NEXT_STATE(state, TIME_TAG_ST_IDLE);
        }
    }

    return tag_num;
}
/**
 * \brief �����������ж��ٸ���ʶ��ǩ
 *
 * \param[in] p_data *.lrc����
 * \param[in] lenth  ���ݳ���
 *
 * \return С��0��������Ϊ��������ʱ���ǩ����
 */
int id_tag_num_analyze (uint8_t *p_data, uint32_t lenth)
{
    uint32_t i            = 0;
    uint32_t state        = ID_TAG_ST_IDLE;          /* ״̬��Ĭ��Ϊ����̬ */
    uint8_t  eoc          = 0;                       /* ���ս����ַ� */
    uint32_t end_idx      = 0;                       /* ���ս������� */
    int      recv         = 0;                       /* ���պ�������ֵ */
    int      tag_num      = 0;

    if (NULL == p_data) {
        return -1;
    }

    for (i = 0; i < lenth; i++) {
        switch (state) {

        case ID_TAG_ST_IDLE:              /* ����̬ */ 
            if ('[' == p_data[i]) {
                i--;
                NEXT_STATE(state, ID_TAG_ST_FIRST_BRACKETS);
            } else {
                NEXT_STATE(state, ID_TAG_ST_IDLE);
            }

            break;                          
                                            
        case ID_TAG_ST_FIRST_BRACKETS:    /* ��һ�������� */
            if (is_num(p_data + i + 1, &eoc, &end_idx) < 0) {
                if (':' == eoc) {
                    NEXT_STATE(state, ID_TAG_ST_FIRST_VALUE);
                } else {
                    NEXT_STATE(state, ID_TAG_ST_IDLE);
                }
            } else {
                NEXT_STATE(state, ID_TAG_ST_IDLE);
            }

            break;                          
                                            
        case ID_TAG_ST_FIRST_VALUE:      /* ��һ��ֵ */
            if (':' == eoc) {
                i += end_idx - 1;
                NEXT_STATE(state, ID_TAG_ST_COLON);
            } else {
                NEXT_STATE(state, ID_TAG_ST_IDLE);
            }

            break;

        case ID_TAG_ST_COLON:             /* ð�� */
            is_num(p_data + i + 1, &eoc, &end_idx);
            if (']' == eoc) {
                NEXT_STATE(state, ID_TAG_ST_SECOND_VALUE);
            } else if (':' == eoc) {
                i += end_idx + 1;
                NEXT_STATE(state, ID_TAG_ST_COLON);
            } else {
                NEXT_STATE(state, ID_TAG_ST_IDLE);
            }

            break;

        case ID_TAG_ST_SECOND_VALUE:     /* �ڶ���ֵ */
            if (']' == eoc) {
                i += end_idx - 1;
                NEXT_STATE(state, ID_TAG_ST_SECOND_BRACKETS);
            } else {
                NEXT_STATE(state, ID_TAG_ST_IDLE);
            }

            break;

        case ID_TAG_ST_SECOND_BRACKETS:   /* �ڶ��������� */
            tag_num++;
            NEXT_STATE(state, ID_TAG_ST_IDLE);

            break;
        
        default:
            NEXT_STATE(state, ID_TAG_ST_IDLE);
        }
    }

    return tag_num;
}

/**
 * \brief �жϱ�ʶ��ǩ����
 *
 * \param[in] p_str ��ʶ��ǩ
 *
 * \return С��0��������Ϊ��ǩ����
 */
int id_tag_type_get (uint8_t *p_str)
{
    uint32_t str_lenth   = 0;
    int      id_tag_type = -1;

    if (NULL == p_str) {
        return -1;
    }

    str_lenth = strlen(p_str);
    if (str_lenth > 10) {
        return -1;
    }

    if ((strcmp(p_str,"ar") == 0) || (strcmp(p_str,"AR") == 0)) {
        id_tag_type = ID_TAG_TPYE_AR;
    } else if ((strcmp(p_str,"ti") || (strcmp(p_str,"TI") == 0)) == 0) {
        id_tag_type = ID_TAG_TPYE_TI;
    } else if ((strcmp(p_str,"au") || (strcmp(p_str,"AU") == 0)) == 0) {
        id_tag_type = ID_TAG_TPYE_AU;
    } else if ((strcmp(p_str,"al") || (strcmp(p_str,"AL") == 0)) == 0) {
        id_tag_type = ID_TAG_TPYE_AL;
    } else if ((strcmp(p_str,"by") || (strcmp(p_str,"BY") == 0)) == 0) {
        id_tag_type = ID_TAG_TPYE_BY;
    } else if ((strcmp(p_str,"offset") || (strcmp(p_str,"OFFSET") == 0)) == 0) {
        id_tag_type = ID_TAG_TPYE_OFFSET;
    } else if ((strcmp(p_str,"t_time") || (strcmp(p_str,"T_TIME") == 0)) == 0) {
        id_tag_type = ID_TAG_TPYE_T_TIME;
    } else {
        id_tag_type = -1;
    }

    return id_tag_type;
}

/**
 * \brief �����ַ�����ָ���ַ����ִ���
 *
 * \param[in] p_str  �ַ���
 * \param[in] g_char ���������ַ�
 *
 * \return С��0��������Ϊ�ַ�����ָ���ַ����ִ���
 */
int char_count (uint8_t *p_str, uint8_t char_search)
{
    uint32_t i = 0;
    uint32_t count = 0;

    if (NULL == p_str) {
        return -1;
    }

    while (1) {
        if ('\0' == p_str[i]) {
            break;
        } else if (p_str[i] == char_search) {
            count++;
        }

        i++;
    }
    return count;
}

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
aw_err_t lrc_analyze (uint8_t *p_data, uint32_t lenth, lyric_info_t *p_lyric)
{
    uint32_t  i                = 0;
    uint32_t  j                = 0;
    uint32_t  row_num          = 1;             /* �ж������ַ��� */
    uint32_t  row_lenth        = 0;             /* ��ǰ�г��� */
    uint32_t  time_tag_tnum    = 0;             /* ʱ���ǩ�ܸ��� */
    uint32_t  time_tag_counter = 0;             /* ʱ���ǩ������ */
    uint32_t  max_lenth        = 0;             /* ����ַ������� */
    uint32_t  time_tag_num     = 0;             /* ÿ���е�ʱ���ǩ�������� */
    uint32_t  id_tag_num       = 0;             /* ÿ���еı�ʶ��ǩ�������� */
    uint32_t  tag_num          = 0;             /* ÿ���еı�ǩ�������� */
    uint32_t  start_idx        = 0;             /* �����ַ�������ʼ���� */
    uint32_t  end_idx          = 0;             /* �����ַ����Ľ������� */
    uint32_t  col_idx          = 0;             /* ������������ */
    uint32_t  time_tag_lenth   = 0;             /* ʱ���ǩ���� */
    uint32_t  lyrics_lenth     = 0;             /* һ���ʳ��� */
    uint32_t  time             = 0;             /* ʱ�� ��λ: ���� */

    int       recv             = 0;             /* ���պ�������ֵ */

    id_tag_tpye_t  id_tag_tpye = ID_TAG_TPYE_UNDEFINE;


    if ((NULL == p_data) || (NULL == p_lyric) || (lenth < 3)) {
        return -AW_EINVAL;
    }

    /* ��ʼ���ṹ�� */
    memset(p_lyric, 0, sizeof(lyric_info_t));

    /* �����ж��ٸ�ʱ���ǩ */
    recv = time_tag_num_analyze(p_data, lenth);
    if (recv < 0) {
        return -AW_ERROR;
    }
    time_tag_tnum = recv;

    /* �����ж��ٸ���ʶ��ǩ */
    recv = id_tag_num_analyze(p_data, lenth);
    if (recv < 0) {
        return -AW_ERROR;
    }

    /* �����ж������ַ��� */
    for (i = 0; i < lenth - 1; i++) {
        if ((0x0D == p_data[i]) && (0x0A == p_data[i + 1])) {
            row_num++;
        }
    }
    if (row_num > LRC_TIME_TAG_MAX_ROW) {
        return -AW_ERROR;
    }

    /* ��¼��β���� */
    j = 0;
    for (i = 0; i < lenth - 1; i++) {
        if ((0x0D == p_data[i]) && (0x0A == p_data[i + 1])) {
            g_row_end_idx[j++] = i + 1;
        }
    }
    if (0 == j) {
        return -AW_ERROR;
    }

    g_row_end_idx[j] = lenth - 1;

    /* ����������� */
    max_lenth = g_row_end_idx[0] + 1;
    for (i = 1; i < row_num; i++) {
        if (max_lenth < g_row_end_idx[i] - g_row_end_idx[i - 1]) {
            max_lenth = g_row_end_idx[i] - g_row_end_idx[i - 1];
        }
    }
    if (max_lenth > LRC_TIME_TAG_MAX_LENTH) {
        return -AW_ERROR;
    }

    /* ����ÿ�и�� */
    for (i = 0; i < row_num; i++) {
        memset(p_buf, 0, max_lenth);

        /* ��һ�е����ݶ��뻺����������Ϊ�ַ��� */
        if (0 == i) {                            /* ��һ�� */
            row_lenth = g_row_end_idx[0] - 1;
            memcpy(p_buf, p_data, row_lenth);
        } else {
            if (i < row_num - 1) {               /* �м��� */
                row_lenth = g_row_end_idx[i] - g_row_end_idx[i - 1] - 2;
                memcpy(p_buf, p_data + g_row_end_idx[i - 1] + 1, row_lenth);
            } else {                             /* ���һ�� */
                row_lenth = g_row_end_idx[i] - g_row_end_idx[i - 1];
                memcpy(p_buf, p_data + g_row_end_idx[i - 1] + 1, row_lenth);
            }
        }
        if ((strlen(p_buf) == 0) || (strlen(p_buf) == char_count(p_buf, ' '))){
            continue;
        }

        /* ����һ�����ж��ٸ���ʶ��ǩ */
        recv = id_tag_num_analyze(p_buf, row_lenth);
        if (recv < 0) {
            continue;
        }
        id_tag_num = recv;

        /* ����һ�����ж��ٸ�ʱ���ǩ */
        recv = time_tag_num_analyze(p_buf, row_lenth);
        if (recv < 0) {
            continue;
        }
        time_tag_num = recv;

        col_idx = 0;
        for (j = 0; j < id_tag_num; j++) {
            /* ��ȡ��һ������ */
            recv = str_sub(p_buf + col_idx, 
                           '[', ':', 
                           p_buf1, 
                          &start_idx, 
                          &end_idx);
            if (recv < 0) {
                break;
            }
            col_idx += end_idx;
            id_tag_tpye = id_tag_type_get(p_buf1);

            /* ��ȡ�ڶ������� */
            recv = str_sub(p_buf + col_idx, 
                           ':', ']', 
                           p_buf1, 
                          &start_idx, 
                          &end_idx);
            if (recv < 0) {
                continue;
            }
            col_idx += end_idx;
            recv = strlen(p_buf1);

            if (ID_TAG_TPYE_AR == id_tag_tpye) {
                memcpy(p_lyric->artist, p_buf1, recv + 1);
            } else if (ID_TAG_TPYE_TI == id_tag_tpye) {
                memcpy(p_lyric->title, p_buf1, recv + 1);
            } else if (ID_TAG_TPYE_AU == id_tag_tpye) {
                memcpy(p_lyric->au, p_buf1, recv + 1);
            } else if (ID_TAG_TPYE_AL == id_tag_tpye) {
                memcpy(p_lyric->album, p_buf1, recv + 1);
            } else if (ID_TAG_TPYE_BY == id_tag_tpye) {
                memcpy(p_lyric->by, p_buf1, recv + 1);
            } else if (ID_TAG_TPYE_OFFSET == id_tag_tpye) {
                p_lyric->offset = atoi(p_buf1);
            } else if (ID_TAG_TPYE_T_TIME == id_tag_tpye) {
                if (('(' == p_buf1[0]) &&
                    (':' == p_buf1[3]) &&
                    (')' == p_buf1[6])) {
                    p_lyric->total_time = ((p_buf1[1] - 0x30) * 10 * 60) +
                                        ((p_buf1[2] - 0x30) * 60) + 
                                        ((p_buf1[4] - 0x30) * 10) + 
                                        (p_buf1[5] - 0x30);
                }
            }       
        }

        col_idx = 0;
        for (j = 0; j < time_tag_num; j++) {
            /* �����ǩ���� */
            recv = str_sub(p_buf + col_idx, '[', ']', p_buf1, NULL, NULL);
            if (recv < 0) {
                break;
            } else {
                time_tag_lenth = recv;
            }
            
            /* ��ȡ��һ������ */
            recv = str_sub(p_buf + col_idx, 
                           '[', ':', 
                           p_buf1, 
                          &start_idx, 
                          &end_idx);
            if (recv < 0) {
                break;
            }
            col_idx += end_idx;
            time = atoi(p_buf1) * 60 * 1000;

            if (5 == time_tag_lenth) {
                /* ��ȡ�ڶ������� */
                recv = str_sub(p_buf + col_idx, 
                               ':', ']', 
                               p_buf1, 
                              &start_idx, 
                              &end_idx);
                if (recv < 0) {
                    continue;
                }
                col_idx += end_idx;
                time += atoi(p_buf1) * 1000;
            } else if (8 == time_tag_lenth) {

                /* ��ȡ�ڶ������� */
                recv = str_sub(p_buf + col_idx, 
                               ':', '.', 
                               p_buf1, 
                              &start_idx, 
                              &end_idx);
                if (recv < 0) {
                    continue;
                }
                col_idx += end_idx;
                time += atoi(p_buf1) * 1000;

                /* ��ȡ���������� */
                recv = str_sub(p_buf + col_idx, 
                               '.', ']', 
                               p_buf1, 
                              &start_idx, 
                              &end_idx);
                if (recv < 0) {
                    continue;
                }
                col_idx += end_idx;
                time += atoi(p_buf1) * 10;
                p_lyric->time[time_tag_counter++] = time;
            }

        }

        lyrics_lenth = strlen(p_buf + col_idx + 1);
        for (j = 0; j < time_tag_num; j++) {
            memcpy(p_lyric->lrc[time_tag_counter - j - 1],
                   p_buf + col_idx + 1, 
                   lyrics_lenth + 1);
        }
    }
    p_lyric->row_num = time_tag_counter;

    return AW_OK;
}

/* end of file */
