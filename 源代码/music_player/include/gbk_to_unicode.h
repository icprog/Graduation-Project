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

#ifndef __GBK_TO_UNICODE_H
#define __GBK_TO_UNICODE_H

#include <stdint.h>

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
int gbk_to_utf8 (const char *p_src, char *p_dst, int buf_size);

#endif /* __GBK_TO_UNICODE_H */

/* end of file */
