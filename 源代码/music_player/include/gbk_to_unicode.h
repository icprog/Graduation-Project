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

#ifndef __GBK_TO_UNICODE_H
#define __GBK_TO_UNICODE_H

#include <stdint.h>

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
int gbk_to_utf8 (const char *p_src, char *p_dst, int buf_size);

#endif /* __GBK_TO_UNICODE_H */

/* end of file */
