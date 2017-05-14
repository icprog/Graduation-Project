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
 * \brief lrc解码
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-05 张进科, 创建该文件
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "apollo.h"
#include "lyric.h"

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

/*******************************************************************************
  宏定义
*******************************************************************************/

/** \brief 状态机内部状态切换 */
#define NEXT_STATE(state, next_state) (state = next_state)

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

static uint8_t  str_sub_buf[LRC_TIME_TAG_MAX_LENTH]; /**< \brief 缓冲区 */
static uint8_t  p_buf[LRC_TIME_TAG_MAX_LENTH];       /**< \brief 缓冲区 */
static uint8_t  p_buf1[LRC_TIME_TAG_MAX_LENTH];      /**< \brief 缓冲区 */
static uint32_t g_row_end_idx[LRC_TIME_TAG_MAX_ROW]; /**< \brief 记录每行列数 */

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 提取指定起始符与结束符之间的字符串
 *
 * \param[in]  str         传入的字符串
 * \param[in]  char_start  起始符
 * \param[in]  char_end    结束符
 * \param[out] p_out       提取出的字符串
 * \param[out] p_start_idx 起始符的位置，传入NULL表示不需要
 * \param[out] p_end_idx   结束符的位置，传入NULL表示不需要
 *
 * \return 小于0出错，其它为提取字符串的长度
 */
int str_sub (uint8_t  *p_str,
             uint8_t   char_start,
             uint8_t   char_end,
             uint8_t  *p_out,
             uint32_t *p_start_idx,
             uint32_t *p_end_idx)
{
    uint32_t i             = 0;
    uint32_t  counter      = 0;  /* 截取出的字符串计数器 */
    uint8_t   is_start     = 0;  /* 是否检测到起始字符 0:未检测到 1:检测到 */
    uint8_t   is_end       = 0;  /* 是否检测到结束字符 0:未检测到 1:检测到 */
    uint32_t  src_str_len  = 0;  /* 待截取字符串长度 */

    if (NULL == p_str) {
        return -1;
    }

    /* 判断待截取字符串长度 */
    src_str_len = strlen(p_str);

    while (1) {
        /* 检测起始字符 */
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

        /* 检测结束字符 */
        if (p_str[i] == char_end) {
            is_end = 1;
            if (p_end_idx != NULL) {
                *p_end_idx = i;
            }
            break;
        }

        /* 检测字符串结束符 */
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
 * \brief 判断接下来的字符串是不是数字，以':'/'.'/']'/'\0'结束
 *
 * \param[in]  p_filedata *.lrc文件数据
 * \param[out] p_eoc      遇到的结束符，传入NULL表示不需要
 * \param[out] p_end_idx  结束符的位置，传入NULL表示不需要
 *
 * \return 小于0出错，其它为是数字
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
 * \brief 解析数据中有多少个时间标签
 *
 * \param[in] p_filedata *.lrc数据
 * \param[in] lenth      数据长度
 *
 * \return 小于0出错，其它为解析出的时间标签个数
 */
int time_tag_num_analyze (uint8_t *p_data, uint32_t lenth)
{
    uint32_t i            = 0;
    uint32_t state        = TIME_TAG_ST_IDLE; /* 状态机默认为空闲态 */
    uint8_t  eoc          = 0;                /* 接收结束字符 */
    uint32_t end_idx      = 0;                /* 接收结束索引 */
    int      recv         = 0;                /* 接收函数返回值 */
    int      tag_num      = 0;

    if (NULL == p_data) {
        return -1;
    }

    for (i = 0; i < lenth; i++) {
        switch (state) {

        case TIME_TAG_ST_IDLE:              /* 空闲态 */ 
            if ('[' == p_data[i]) {
                i--;
                NEXT_STATE(state, TIME_TAG_ST_FIRST_BRACKETS);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            }

            break;                          
                                            
        case TIME_TAG_ST_FIRST_BRACKETS:    /* 第一个方括号 */
            if (is_num(p_data + i + 1, &eoc, &end_idx) < 0) {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_FIRST_NUMBER);
            }

            break;                          
                                            
        case TIME_TAG_ST_FIRST_NUMBER:      /* 第一个数字 */
            if (':' == eoc) {
                i += end_idx - 1;
                NEXT_STATE(state, TIME_TAG_ST_COLON);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            }

            break;

        case TIME_TAG_ST_COLON:             /* 冒号 */
            if (is_num(p_data + i + 1, &eoc, &end_idx) < 0) {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_SECOND_NUMBER);
            }

            break;

        case TIME_TAG_ST_SECOND_NUMBER:     /* 第二个数字 */
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

        case TIME_TAG_ST_DOT:               /* 点号 */
            if (is_num(p_data + i + 1, &eoc, &end_idx) < 0) {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_THIRD_NUMBER);
            }

            break;

        case TIME_TAG_ST_THIRD_NUMBER:      /* 第三个数字 */
            if (']' == eoc) {
                i += end_idx - 1;
                NEXT_STATE(state, TIME_TAG_ST_SECOND_BRACKETS);
            } else {
                NEXT_STATE(state, TIME_TAG_ST_IDLE);
            }

            break;

        case TIME_TAG_ST_SECOND_BRACKETS:   /* 第二个方括号 */
            tag_num++;
            NEXT_STATE(state, TIME_TAG_ST_IDLE);

            break;
        
        default: NEXT_STATE(state, TIME_TAG_ST_IDLE);
        }
    }

    return tag_num;
}
/**
 * \brief 解析数据中有多少个标识标签
 *
 * \param[in] p_data *.lrc数据
 * \param[in] lenth  数据长度
 *
 * \return 小于0出错，其它为解析出的时间标签个数
 */
int id_tag_num_analyze (uint8_t *p_data, uint32_t lenth)
{
    uint32_t i            = 0;
    uint32_t state        = ID_TAG_ST_IDLE;          /* 状态机默认为空闲态 */
    uint8_t  eoc          = 0;                       /* 接收结束字符 */
    uint32_t end_idx      = 0;                       /* 接收结束索引 */
    int      recv         = 0;                       /* 接收函数返回值 */
    int      tag_num      = 0;

    if (NULL == p_data) {
        return -1;
    }

    for (i = 0; i < lenth; i++) {
        switch (state) {

        case ID_TAG_ST_IDLE:              /* 空闲态 */ 
            if ('[' == p_data[i]) {
                i--;
                NEXT_STATE(state, ID_TAG_ST_FIRST_BRACKETS);
            } else {
                NEXT_STATE(state, ID_TAG_ST_IDLE);
            }

            break;                          
                                            
        case ID_TAG_ST_FIRST_BRACKETS:    /* 第一个方括号 */
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
                                            
        case ID_TAG_ST_FIRST_VALUE:      /* 第一个值 */
            if (':' == eoc) {
                i += end_idx - 1;
                NEXT_STATE(state, ID_TAG_ST_COLON);
            } else {
                NEXT_STATE(state, ID_TAG_ST_IDLE);
            }

            break;

        case ID_TAG_ST_COLON:             /* 冒号 */
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

        case ID_TAG_ST_SECOND_VALUE:     /* 第二个值 */
            if (']' == eoc) {
                i += end_idx - 1;
                NEXT_STATE(state, ID_TAG_ST_SECOND_BRACKETS);
            } else {
                NEXT_STATE(state, ID_TAG_ST_IDLE);
            }

            break;

        case ID_TAG_ST_SECOND_BRACKETS:   /* 第二个方括号 */
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
 * \brief 判断标识标签类型
 *
 * \param[in] p_str 标识标签
 *
 * \return 小于0出错，其它为标签类型
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
 * \brief 搜索字符串中指定字符出现次数
 *
 * \param[in] p_str  字符串
 * \param[in] g_char 待搜索的字符
 *
 * \return 小于0出错，其它为字符串中指定字符出现次数
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
 * \brief 解析lrc歌词
 *
 * \param[in]  p_data *.lrc文件数据
 * \param[in]  lenth  数据长度
 * \param[out] lyric  解析出的歌词数据
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t lrc_analyze (uint8_t *p_data, uint32_t lenth, lyric_info_t *p_lyric)
{
    uint32_t  i                = 0;
    uint32_t  j                = 0;
    uint32_t  row_num          = 1;             /* 有多少行字符串 */
    uint32_t  row_lenth        = 0;             /* 当前行长度 */
    uint32_t  time_tag_tnum    = 0;             /* 时间标签总个数 */
    uint32_t  time_tag_counter = 0;             /* 时间标签计数器 */
    uint32_t  max_lenth        = 0;             /* 最长的字符串长度 */
    uint32_t  time_tag_num     = 0;             /* 每行中的时间标签数量缓存 */
    uint32_t  id_tag_num       = 0;             /* 每行中的标识标签数量缓存 */
    uint32_t  tag_num          = 0;             /* 每行中的标签数量缓存 */
    uint32_t  start_idx        = 0;             /* 搜索字符串的起始索引 */
    uint32_t  end_idx          = 0;             /* 搜索字符串的结束索引 */
    uint32_t  col_idx          = 0;             /* 解析到的列数 */
    uint32_t  time_tag_lenth   = 0;             /* 时间标签长度 */
    uint32_t  lyrics_lenth     = 0;             /* 一句歌词长度 */
    uint32_t  time             = 0;             /* 时间 单位: 毫秒 */

    int       recv             = 0;             /* 接收函数返回值 */

    id_tag_tpye_t  id_tag_tpye = ID_TAG_TPYE_UNDEFINE;


    if ((NULL == p_data) || (NULL == p_lyric) || (lenth < 3)) {
        return -AW_EINVAL;
    }

    /* 初始化结构体 */
    memset(p_lyric, 0, sizeof(lyric_info_t));

    /* 计算有多少个时间标签 */
    recv = time_tag_num_analyze(p_data, lenth);
    if (recv < 0) {
        return -AW_ERROR;
    }
    time_tag_tnum = recv;

    /* 计算有多少个标识标签 */
    recv = id_tag_num_analyze(p_data, lenth);
    if (recv < 0) {
        return -AW_ERROR;
    }

    /* 计算有多少行字符串 */
    for (i = 0; i < lenth - 1; i++) {
        if ((0x0D == p_data[i]) && (0x0A == p_data[i + 1])) {
            row_num++;
        }
    }
    if (row_num > LRC_TIME_TAG_MAX_ROW) {
        return -AW_ERROR;
    }

    /* 记录行尾索引 */
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

    /* 计算最大列数 */
    max_lenth = g_row_end_idx[0] + 1;
    for (i = 1; i < row_num; i++) {
        if (max_lenth < g_row_end_idx[i] - g_row_end_idx[i - 1]) {
            max_lenth = g_row_end_idx[i] - g_row_end_idx[i - 1];
        }
    }
    if (max_lenth > LRC_TIME_TAG_MAX_LENTH) {
        return -AW_ERROR;
    }

    /* 解析每行歌词 */
    for (i = 0; i < row_num; i++) {
        memset(p_buf, 0, max_lenth);

        /* 将一行的数据读入缓冲区并设置为字符串 */
        if (0 == i) {                            /* 第一行 */
            row_lenth = g_row_end_idx[0] - 1;
            memcpy(p_buf, p_data, row_lenth);
        } else {
            if (i < row_num - 1) {               /* 中间行 */
                row_lenth = g_row_end_idx[i] - g_row_end_idx[i - 1] - 2;
                memcpy(p_buf, p_data + g_row_end_idx[i - 1] + 1, row_lenth);
            } else {                             /* 最后一行 */
                row_lenth = g_row_end_idx[i] - g_row_end_idx[i - 1];
                memcpy(p_buf, p_data + g_row_end_idx[i - 1] + 1, row_lenth);
            }
        }
        if ((strlen(p_buf) == 0) || (strlen(p_buf) == char_count(p_buf, ' '))){
            continue;
        }

        /* 查找一行中有多少个标识标签 */
        recv = id_tag_num_analyze(p_buf, row_lenth);
        if (recv < 0) {
            continue;
        }
        id_tag_num = recv;

        /* 查找一行中有多少个时间标签 */
        recv = time_tag_num_analyze(p_buf, row_lenth);
        if (recv < 0) {
            continue;
        }
        time_tag_num = recv;

        col_idx = 0;
        for (j = 0; j < id_tag_num; j++) {
            /* 截取第一个参数 */
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

            /* 截取第二个参数 */
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
            /* 计算标签长度 */
            recv = str_sub(p_buf + col_idx, '[', ']', p_buf1, NULL, NULL);
            if (recv < 0) {
                break;
            } else {
                time_tag_lenth = recv;
            }
            
            /* 截取第一个参数 */
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
                /* 截取第二个参数 */
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

                /* 截取第二个参数 */
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

                /* 截取第三个参数 */
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
