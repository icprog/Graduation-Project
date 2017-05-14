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
 * \brief emwin ttf字体
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-04 张进科, 创建该文件
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "apollo.h"
#include "font_xbf.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "aw_vdebug.h"
#include "GUI.h"  

/*******************************************************************************
  宏定义
*******************************************************************************/

#define XBF_FONT11_BASE       0x00000000 /**< \brief FONT11首地址 */
#define XBF_FONT13_BASE       0x00000FCC /**< \brief FONT13首地址 */
#define XBF_FONT14_BASE       0x00001F98 /**< \brief FONT14首地址 */
#define XBF_FONT18_BASE       0x002E7032 /**< \brief FONT18首地址 */

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

/** \brief xbf字体 */
GUI_FONT g_font11, g_font13, g_font14, g_font18;

/** \brief 点阵定义 */
GUI_XBF_DATA g_xbf_data11, g_xbf_data13, g_xbf_data14, g_xbf_data18;

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief XBF字体的回调函数
 *
 * \param[in]  off       地址偏移
 * \param[in]  num_bytes 需要读出的字节数
 * \param[in]  p_void    XBF字体数据
 * \param[out] p_buffer  获取字体的点阵数据
 *
 * \retval 0 读取成功
 */
static int data11_get_cb (uint32_t off,
                          uint16_t num_bytes,
                          void    *p_void,
                          void    *p_buffer)
{
    /* 读取点阵数据 */
    memcpy(p_buffer, (uint8_t *)p_void + XBF_FONT11_BASE + off, num_bytes);

    return 0;
}

/**
 * \brief XBF字体的回调函数
 *
 * \param[in]  off       地址偏移
 * \param[in]  num_bytes 需要读出的字节数
 * \param[in]  p_void    XBF字体数据
 * \param[out] p_buffer  获取字体的点阵数据
 *
 * \retval 0 读取成功
 */
static int data13_get_cb (uint32_t off,
                          uint16_t num_bytes,
                          void    *p_void,
                          void    *p_buffer)
{
    /* 读取点阵数据 */
    memcpy(p_buffer, (uint8_t *)p_void + XBF_FONT13_BASE + off, num_bytes);

    return 0;
}

/**
 * \brief XBF字体的回调函数
 *
 * \param[in]  off       地址偏移
 * \param[in]  num_bytes 需要读出的字节数
 * \param[in]  p_void    XBF字体数据
 * \param[out] p_buffer  获取字体的点阵数据
 *
 * \retval 0 读取成功
 */
static int data14_get_cb (uint32_t off,
                          uint16_t num_bytes,
                          void    *p_void,
                          void    *p_buffer)
{
    /* 读取点阵数据 */
    memcpy(p_buffer, (uint8_t *)p_void + XBF_FONT14_BASE + off, num_bytes);

    return 0;
}

/**
 * \brief XBF字体的回调函数
 *
 * \param[in]  off       地址偏移
 * \param[in]  num_bytes 需要读出的字节数
 * \param[in]  p_void    XBF字体数据
 * \param[out] p_buffer  获取字体的点阵数据
 *
 * \retval 0 读取成功
 */
static int data18_get_cb (uint32_t off,
                          uint16_t num_bytes,
                          void    *p_void,
                          void    *p_buffer)
{
    /* 读取点阵数据 */
    memcpy(p_buffer, (uint8_t *)p_void + XBF_FONT18_BASE + off, num_bytes);

    return 0;
}

/**
 * \brief 初始化xfb字体
 *
 * \param[in] p_xbf_data 字体数据
 *
 * \return 无
 */
void xbf_init (void *p_xbf_data)
{
    if (NULL == p_xbf_data) {
        AW_ERRF(("xbf init failed\n"));
        return;
    }

    /* 创建 11 点阵字体 */
    GUI_XBF_CreateFont(&g_font11,                 /* GUI_FONT 类型变量  */
                      &g_xbf_data11,             /* GUI_XBF_DATA 类型变量 */
                       GUI_XBF_TYPE_PROP_AA4_EXT, /* 字体类型  */
                       data11_get_cb,             /* 回调函数  */
                       p_xbf_data);               /* 回调函数参数   */

    /* 创建 13 点阵字体 */
    GUI_XBF_CreateFont(&g_font13,                 /* GUI_FONT 类型变量  */
                      &g_xbf_data13,             /* GUI_XBF_DATA 类型变量 */
                       GUI_XBF_TYPE_PROP_AA4_EXT, /* 字体类型  */
                       data13_get_cb,             /* 回调函数  */
                       p_xbf_data);               /* 回调函数参数   */

    /* 创建 14 点阵字体 */
    GUI_XBF_CreateFont(&g_font14,                 /* GUI_FONT 类型变量  */
                      &g_xbf_data14,             /* GUI_XBF_DATA 类型变量 */
                       GUI_XBF_TYPE_PROP_AA4_EXT, /* 字体类型  */
                       data14_get_cb,             /* 回调函数  */
                       p_xbf_data);               /* 回调函数参数   */

    /* 创建 18 点阵字体 */
    GUI_XBF_CreateFont(&g_font18,                 /* GUI_FONT 类型变量  */
                      &g_xbf_data18,             /* GUI_XBF_DATA 类型变量 */
                       GUI_XBF_TYPE_PROP_AA4_EXT, /* 字体类型  */
                       data18_get_cb,             /* 回调函数  */
                       p_xbf_data);               /* 回调函数参数   */
}

/* end of file */
