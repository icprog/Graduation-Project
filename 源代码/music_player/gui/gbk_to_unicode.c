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
 * \brief gbk编码转换为unicode
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-22 张进科, 创建该文件
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "gbk_to_unicode.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "GUI.h"
#include "option/cc936.h"

/*******************************************************************************
  宏定义
*******************************************************************************/

/** \brief 字符范围 */
#define DF1S    0x81
#define DF1E    0xFE
#define DS1S    0x40
#define DS1E    0x7E
#define DS2S    0x80
#define DS2E    0xFE

/** \brief 字符范围判断 */
#define IsDBCS1(c)    ((BYTE)(c) >= DF1S && (BYTE)(c) <= DF1E)
#define IsDBCS2(c)    (((BYTE)(c) >= DS1S && (BYTE)(c) <= DS1E) || \
                      ((BYTE)(c) >= DS2S && (BYTE)(c) <= DS2E))

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

static char g_unicode[255]; /**< \brief 编码转换缓冲区 */

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 将一个字符的Unicode(UCS-2和UCS-4)编码转换成UTF-8编码
 *
 * \param[in]  unic     字符的Unicode编码值
 * \param[out] p_output 指向输出的用于存储UTF8编码值的缓冲区的指针
 *
 * \note 1. UTF8没有字节序问题, 但是Unicode有字节序要求;
 *          字节序分为大端(Big Endian)和小端(Little Endian)两种;
 *          在Intel处理器中采用小端法表示, 在此采用小端法表示. (低地址存低位)
 *       2. 请保证 p_output 缓冲区有最少有 6 字节的空间大小!
 *
 * \return 返回转换后的字符的UTF8编码所占的字节数, 如果出错则返回 0
 */
int unicode_to_utf8_one (uint16_t unic, uint8_t *p_output)
{
    if (unic <= 0x0000007F) {

        /* 0xxxxxxx */
        *p_output     = (unic & 0x7F);
        return 1;
    } else if (unic >= 0x00000080 && unic <= 0x000007FF) {

        /* 110xxxxx 10xxxxxx */
        *(p_output+1) = (unic & 0x3F) | 0x80;
        *p_output     = ((unic >> 6) & 0x1F) | 0xC0;
        return 2;
    } else if (unic >= 0x00000800 && unic <= 0x0000FFFF) {
        /* 1110xxxx 10xxxxxx 10xxxxxx */

        *(p_output+2) = (unic & 0x3F) | 0x80;
        *(p_output+1) = ((unic >>  6) & 0x3F) | 0x80;
        *p_output     = ((unic >> 12) & 0x0F) | 0xE0;
        return 3;
    } else if (unic >= 0x00010000 && unic <= 0x001FFFFF) {
        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */

        *(p_output+3) = (unic & 0x3F) | 0x80;
        *(p_output+2) = ((unic >>  6) & 0x3F) | 0x80;
        *(p_output+1) = ((unic >> 12) & 0x3F) | 0x80;
        *p_output     = ((unic >> 18) & 0x07) | 0xF0;
        return 4;
    } else if (unic >= 0x00200000 && unic <= 0x03FFFFFF) {
        /* 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */

        *(p_output+4) = (unic & 0x3F) | 0x80;
        *(p_output+3) = ((unic >>  6) & 0x3F) | 0x80;
        *(p_output+2) = ((unic >> 12) & 0x3F) | 0x80;
        *(p_output+1) = ((unic >> 18) & 0x3F) | 0x80;
        *p_output     = ((unic >> 24) & 0x03) | 0xF8;
        return 5;
    } else if (unic >= 0x04000000 && unic <= 0x7FFFFFFF) {
        /* 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */

        *(p_output+5) = (unic & 0x3F) | 0x80;
        *(p_output+4) = ((unic >>  6) & 0x3F) | 0x80;
        *(p_output+3) = ((unic >> 12) & 0x3F) | 0x80;
        *(p_output+2) = ((unic >> 18) & 0x3F) | 0x80;
        *(p_output+1) = ((unic >> 24) & 0x3F) | 0x80;
        *p_output     = ((unic >> 30) & 0x01) | 0xFC;
        return 6;
    }

    return 0;
}

/**
 * \brief gbk字符转unicode字符
 *
 * \param[in]  chr 待转换的字符
 * \param[out] dir 0: unicode转gbk 1: gbk转unicode
 *
 * \return 转换出的unicode字符
 */
static WCHAR convert (WCHAR chr, UINT dir)
{
    const WCHAR *p;
    WCHAR        c;
    int i, n, li, hi;

    if (chr < 0x80) {    /* ASCII */
        c = chr;
    } else {
        if (dir) {       /* OEMCP to unicode */
            p = oem2uni;
            hi = sizeof(oem2uni) / 4 - 1;
        } else {         /* Unicode to OEMCP */
            p = uni2oem;
            hi = sizeof(uni2oem) / 4 - 1;
        }
        li = 0;
        for (n = 16; n; n--) {
            i = li + (hi - li) / 2;
            if (chr == p[i * 2]) break;
            if (chr > p[i * 2])
                li = i;
            else
                hi = i;
        }
        c = n ? p[i * 2 + 1] : 0;
    }

    return c;
}

/**
 * \brief gbk字符串转unicode字符串
 *
 * \param[in]  p_scr  指向待转换的gbk字符串的指针
 * \param[out] p_dest 指向用于写入结果的缓冲器的指针
 *
 * \return 转换出的unicode字符数
 */
static int gbk_to_unicode (TCHAR *p_scr, WCHAR *p_dest)
{
    BYTE  b;
    WCHAR w;
    int counter = 0;

    while (*p_scr != '\0') {
        w = *p_scr++;
        w &= 0xFF;
        if (IsDBCS1(w)) {
            b = (BYTE)*p_scr;
            w = (w << 8) + b;
            if (*p_scr != '\0') {
                p_scr++;
            }
        }
        w = convert(w, 1);
        *p_dest++ = w;
        counter++;
    }
    *p_dest = 0;

    return counter;
}

/**
 * \brief 将给定的双字节Unicode字符串转换为UTF-8格式
 *
 * \param[in]  p_s      指向待转换的Unicode字符串的指针
 * \param[in]  len      待转换的Unicode字符的数量
 * \param[out] p_buffer 指向用于写入结果的缓冲器的指针
 * \param[in]  size     缓冲区大小 （以字节为单位）
 *
 * \note 采用UTF-8编码的字符最多可以使用3个字节。保险起见，建议缓冲器尺寸为：Unicode字符数*3。
 *       如果缓冲器无法存储全部结果，则函数会在缓冲器满载时返回
 *
 * \return 写入缓冲器的字节数
 */
int unicode_to_utf8 (const uint16_t *p_s, int len, char *p_buffer, int size)
{
    uint8_t buf[6];
    int num;
    int counter = 0;

    for (int i = 0; i < len; i++) {
        num = unicode_to_utf8_one(p_s[i], buf);
        counter += num;
        if (counter < size) {
            memcpy(p_buffer, buf, num);
            p_buffer += num;
        } else {
            break;
        }
    }

    return counter;
}

/**
 * \brief gbk转utf8
 *
 * \param[in]  p_src    指向待转换的gbk字符串的指针
 * \param[out] p_dst    指向用于写入结果的缓冲器的指针
 * \param[in]  buf_size 缓冲区大小 （以字节为单位）
 *
 * \note 采用UTF-8编码的字符最多可以使用3个字节。保险起见，建议缓冲器尺寸为：gbk字符数*3。
 *       如果缓冲器无法存储全部结果，则函数会在缓冲器满载时返回
 *
 * \return 写入缓冲器的字节数
 */
int gbk_to_utf8 (const char *p_src, char *p_dst, int buf_size)
{
    int num = 0;

    num = gbk_to_unicode((TCHAR *)p_src, (WCHAR *)g_unicode);
    num = unicode_to_utf8((const uint16_t *)g_unicode, num, p_dst, buf_size);
    p_dst[num] = '\0';

    return num + 1;
}

/* end of file */
