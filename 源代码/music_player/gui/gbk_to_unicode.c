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
 * \brief gbk����ת��Ϊunicode
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-22 �Ž���, �������ļ�
 * \endinternal
 */

/*******************************************************************************
  ͷ�ļ�����
*******************************************************************************/

#include "gbk_to_unicode.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "GUI.h"
#include "option/cc936.h"

/*******************************************************************************
  �궨��
*******************************************************************************/

/** \brief �ַ���Χ */
#define DF1S    0x81
#define DF1E    0xFE
#define DS1S    0x40
#define DS1E    0x7E
#define DS2S    0x80
#define DS2E    0xFE

/** \brief �ַ���Χ�ж� */
#define IsDBCS1(c)    ((BYTE)(c) >= DF1S && (BYTE)(c) <= DF1E)
#define IsDBCS2(c)    (((BYTE)(c) >= DS1S && (BYTE)(c) <= DS1E) || \
                      ((BYTE)(c) >= DS2S && (BYTE)(c) <= DS2E))

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

static char g_unicode[255]; /**< \brief ����ת�������� */

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/**
 * \brief ��һ���ַ���Unicode(UCS-2��UCS-4)����ת����UTF-8����
 *
 * \param[in]  unic     �ַ���Unicode����ֵ
 * \param[out] p_output ָ����������ڴ洢UTF8����ֵ�Ļ�������ָ��
 *
 * \note 1. UTF8û���ֽ�������, ����Unicode���ֽ���Ҫ��;
 *          �ֽ����Ϊ���(Big Endian)��С��(Little Endian)����;
 *          ��Intel�������в���С�˷���ʾ, �ڴ˲���С�˷���ʾ. (�͵�ַ���λ)
 *       2. �뱣֤ p_output �������������� 6 �ֽڵĿռ��С!
 *
 * \return ����ת������ַ���UTF8������ռ���ֽ���, ��������򷵻� 0
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
 * \brief gbk�ַ�תunicode�ַ�
 *
 * \param[in]  chr ��ת�����ַ�
 * \param[out] dir 0: unicodeתgbk 1: gbkתunicode
 *
 * \return ת������unicode�ַ�
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
 * \brief gbk�ַ���תunicode�ַ���
 *
 * \param[in]  p_scr  ָ���ת����gbk�ַ�����ָ��
 * \param[out] p_dest ָ������д�����Ļ�������ָ��
 *
 * \return ת������unicode�ַ���
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
 * \brief ��������˫�ֽ�Unicode�ַ���ת��ΪUTF-8��ʽ
 *
 * \param[in]  p_s      ָ���ת����Unicode�ַ�����ָ��
 * \param[in]  len      ��ת����Unicode�ַ�������
 * \param[out] p_buffer ָ������д�����Ļ�������ָ��
 * \param[in]  size     ��������С �����ֽ�Ϊ��λ��
 *
 * \note ����UTF-8������ַ�������ʹ��3���ֽڡ�������������黺�����ߴ�Ϊ��Unicode�ַ���*3��
 *       ����������޷��洢ȫ��������������ڻ���������ʱ����
 *
 * \return д�뻺�������ֽ���
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
 * \brief gbkתutf8
 *
 * \param[in]  p_src    ָ���ת����gbk�ַ�����ָ��
 * \param[out] p_dst    ָ������д�����Ļ�������ָ��
 * \param[in]  buf_size ��������С �����ֽ�Ϊ��λ��
 *
 * \note ����UTF-8������ַ�������ʹ��3���ֽڡ�������������黺�����ߴ�Ϊ��gbk�ַ���*3��
 *       ����������޷��洢ȫ��������������ڻ���������ʱ����
 *
 * \return д�뻺�������ֽ���
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
