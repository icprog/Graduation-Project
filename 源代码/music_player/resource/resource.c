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
 * \brief 资源管理
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-10 张进科, 创建该文件
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "apollo.h"
#include "resource.h"

#include <string.h>
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_time.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_dirent.h"
#include "io/sys/aw_stat.h"
#include "play_list.h"
#include "player_global.h"

/*******************************************************************************
  宏定义
*******************************************************************************/

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

/** \brief 播放列表路径 */
static const char *gp_play_list_path = "/sd0/system/play_list.bin";

/** \brief 配置文件路径 */
static const char *gp_config_path = "/sd0/system/config.bin";

#if 0
/** \brief 字体路径 */
static const char *gp_ttf_path = "/sd0/system/Adobe Hei Std.xbf";

/** \brief 背景图片路径 */
const char *gp_background_path = "/sd0/system/background/2.jpg";
#endif

/** \brief 歌曲目录 */
const char *gp_sdcard_music_path = "/sd0/music";
const char *gp_udisk_music_path  = "/udisk/music";

/** \brief 是否挂载了SD卡 0:未挂载 1:已挂载 */
uint8_t g_is_mount_sdcard = 0;

/** \brief 是否挂载了U盘 0:未挂载 1:已挂载 */
uint8_t g_is_mount_udisk = 0;

/** \brief 配置信息 */
config_info_t g_config_info;

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 读取配置信息
 *
 * \param[in] p_config_info 配置信息
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t config_read (config_info_t *p_config_info)
{
    config_info_t config_info = {0, -1, 0};
    int           fd          = 0;
    int32_t       size        = 0;

    if (NULL == p_config_info) {
        return -AW_EINVAL;
    }

    /* 打开文件 */
    fd = aw_open(gp_config_path, O_RDONLY, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_ERROR;
    }

    /* 获取文件大小 */
    if ((size = file_size_get(fd)) < sizeof(config_info_t)) {
        goto fail;
    }

    /* 读取文件 */
    if (aw_read(fd, &config_info, sizeof(config_info_t)) !=
        sizeof(config_info_t)) {
        AW_ERRF(("read %s error!\n", gp_config_path));
        goto fail;
    }

    if (config_info.is_effect != 1) {
        goto fail;
    }

    memcpy(p_config_info, &config_info, sizeof(config_info_t));

    aw_close(fd);
    return AW_OK;

fail:
    aw_close(fd);
    return -AW_ERROR;
}

/**
 * \brief 保存配置信息
 *
 * \param[in] p_config_info 配置信息
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t config_write (config_info_t *p_config_info)
{
    config_info_t config_info = {0, -1, 0};
    int           fd          = 0;
    int32_t       size        = 0;

    if (NULL == p_config_info) {
        return -AW_EINVAL;
    }

    memcpy(&config_info, p_config_info, sizeof(config_info_t));
    config_info.is_effect = 1;

    /* 打开文件 */
    fd = aw_open(gp_config_path, O_RDWR | O_CREAT, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_EINVAL;
    }

    /* 写入文件 */
    if (aw_write(fd, &config_info, sizeof(config_info_t)) !=
        sizeof(config_info_t)) {
        AW_ERRF(("write %s error!\n", gp_config_path));
        goto fail;
    }

    aw_close(fd);
    return AW_OK;

fail:
    aw_close(fd);
    return -AW_EINVAL;
}

/**
 * \brief 读取播放列表
 *
 * \param[in] p_play_list  播放列表
 * \param[in] p_list_state 播放列表状态
 * \param[in] lenth        播放列表长度
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t play_list_read (music_info_t *p_play_list,
                         list_state_t *p_list_state,
                         uint32_t      lenth)
{
    int     fd         = 0;
    int32_t size       = 0;
    uint8_t check_sum0 = 0;
    uint8_t check_sum1 = 0;

    if (NULL == p_play_list) {
        return -AW_EINVAL;
    }

    /* 打开文件 */
    fd = aw_open(gp_play_list_path, O_RDONLY, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_ERROR;
    }

    /* 获取文件大小 */
    if ((size = file_size_get(fd)) < sizeof(config_info_t)) {
        goto fail;
    }

    if (size < sizeof(music_info_t) * lenth + sizeof(check_sum0)) {
        goto fail;
    }

    /* 读取播放列表 */
    if (aw_read(fd, p_play_list, sizeof(music_info_t) * lenth) !=
        sizeof(music_info_t) * lenth) {
        AW_ERRF(("read %s error!\n", gp_play_list_path));
        goto fail;
    }

    /* 读取播放列表状态 */
    if (aw_read(fd, p_list_state, sizeof(list_state_t)) !=
        sizeof(list_state_t)) {
        AW_ERRF(("read %s error!\n", gp_play_list_path));
        goto fail;
    }

    /* 读取校验和 */
    if (aw_read(fd, &check_sum0, sizeof(check_sum0)) != sizeof(check_sum0)) {
        AW_ERRF(("read %s error!\n", gp_play_list_path));
        goto fail;
    }

    /* 计算校验和 */
    for (int i = 0; i < sizeof(music_info_t) * lenth; i++)
    {
        check_sum1 += ((uint8_t *)p_play_list)[i];
    }
    for (int i = 0; i < sizeof(list_state_t); i++)
    {
        check_sum1 += ((uint8_t *)p_list_state)[i];
    }

    /* 对比校验和 */
    if (check_sum1 != check_sum0) {
        memset(p_play_list, 0xFF, sizeof(music_info_t) * lenth);
        memset(p_list_state, 0, sizeof(list_state_t));
        goto fail;
    }

    aw_close(fd);
    return AW_OK;

fail:
    aw_close(fd);
    return -AW_ERROR;
}

/**
 * \brief 保存播放列表
 *
 * \param[in] p_play_list  播放列表
 * \param[in] p_list_state 播放列表状态
 * \param[in] lenth        播放列表长度
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t play_list_write (music_info_t *p_play_list,
                          list_state_t *p_list_state,
                          uint32_t      lenth)
{
    int     fd        = 0;
    int32_t size      = 0;
    uint8_t check_sum = 0;

    if (NULL == p_play_list) {
        return -AW_EINVAL;
    }

    /* 打开文件 */
    fd = aw_open(gp_play_list_path, O_RDWR | O_CREAT, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_EINVAL;
    }

    /* 计算校验和 */
    for (int i = 0; i < sizeof(music_info_t) * lenth; i++)
    {
        check_sum += ((uint8_t *)p_play_list)[i];
    }
    for (int i = 0; i < sizeof(list_state_t); i++)
    {
        check_sum += ((uint8_t *)p_list_state)[i];
    }

    /* 写入播放列表 */
    if (aw_write(fd, p_play_list, sizeof(music_info_t) * lenth) !=
        sizeof(music_info_t) * lenth) {
        AW_ERRF(("write %s error!\n", gp_play_list_path));
        goto fail;
    }

    /* 写入播放列表状态 */
    if (aw_write(fd, p_list_state, sizeof(list_state_t)) !=
        sizeof(list_state_t)) {
        AW_ERRF(("write %s error!\n", gp_play_list_path));
        goto fail;
    }

    /* 写入校验和 */
    if (aw_write(fd, &check_sum, sizeof(check_sum)) != sizeof(check_sum)) {
        AW_ERRF(("write %s error!\n", gp_play_list_path));
        goto fail;
    }

    aw_close(fd);
    return AW_OK;

fail:
    aw_close(fd);
    return -AW_EINVAL;
}

/**
 * \brief 获取文件大小
 *
 * \param[in] fd 已经打开的文件句柄
 *
 * \return 小于0出错，其它为获取到的文件大小
 */
int file_size_get (int fd)
{
    off_t   old_off = 0;
    int32_t size    = 0;

    /* 保存当前文件读写位置 */
    if ((old_off = aw_lseek(fd, 0, SEEK_CUR)) < 0) {
        goto fail;
    }

    /* 获取文件大小 */
    if ((size = aw_lseek(fd, 0, SEEK_END)) < 0) {
        goto fail;
    }

    /* 恢复到之前的位置 */
    if ((old_off = aw_lseek(fd, old_off, SEEK_SET)) < 0) {
        goto fail;
    }

    return size;

fail:
    return -1;
}

/**
 * \brief 将指定路径的文件读取到内存中
 *
 * \param[in]  p_path       文件绝对路径
 * \param[out] p_buf        读取到的位置
 * \param[in]  lenth        缓冲区大小
 * \param[out] p_read_lenth 读取出的文件大小
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t file_to_mem (const uint8_t *p_path,
                      uint8_t       *p_buf,
                      uint32_t       buf_lenth,
                      uint32_t      *p_read_lenth)
{
    int fd   = 0;
    int size = 0;

    if ((NULL == p_path) || (NULL == p_buf) || (NULL == p_read_lenth)){
        return -AW_EINVAL;
    }

    /* 打开文件 */
    fd = aw_open(p_path, O_RDONLY, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_ERROR;
    }

    /* 获取文件大小 */
    if ((size = file_size_get(fd)) < 0) {
        AW_ERRF(("%s size get error!\n", p_path));
        aw_close(fd);
        return -AW_ERROR;
    }

    /* 检查缓冲区是否足够 */
    if (size > buf_lenth) {
        aw_close(fd);
        AW_ERRF(("%s too large!\n", p_path));
        return -AW_ERROR;
    }

    /* 读取文件 */
    if (aw_read(fd, p_buf, size) != size) {
        aw_close(fd);
        AW_ERRF(("read %s error!\n", p_path));
        return -AW_ERROR;
    }

    /* 保存文件大小 */
    if (NULL != p_read_lenth) {
        *p_read_lenth = size;
    }

    aw_close(fd);
    return AW_OK;
}

/**
 * \brief 读取与音乐同文件名的歌词
 *
 * \param[in]  p_path       音乐文件名
 * \param[out] p_buf        读取到的位置
 * \param[in]  lenth        缓冲区大小
 * \param[out] p_read_lenth 读取出的文件大小
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t lrc_read (const uint8_t  *p_path,
                   uint8_t        *p_buf,
                   uint32_t        buf_lenth,
                   uint32_t       *p_read_lenth)
{
    uint8_t  lrc_path[255];
    uint32_t lenth   = 0;
    char    *dot_idx = NULL;

    if ((NULL == p_path) || (NULL == p_buf) || (NULL == p_read_lenth)){
        return -AW_EINVAL;
    }

    /* 将路径拷贝到缓存区 */
    lenth = strlen(p_path);
    memcpy(lrc_path, p_path, lenth + 1);

    /* 将指针定位到后缀 */
    dot_idx = strrchr(lrc_path, '.') + 1;

    /* 更改后缀 */
    memcpy(dot_idx, "lrc", 4);

    return file_to_mem(lrc_path, p_buf, buf_lenth, p_read_lenth);
}

/**
 * \brief 读取与音乐同文件名的图片
 *
 * \param[in]  p_path       音乐文件名
 * \param[out] p_buf        读取到的位置
 * \param[in]  lenth        缓冲区大小
 * \param[out] p_read_lenth 读取出的文件大小
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t jpg_read (const uint8_t  *p_path,
                   uint8_t        *p_buf,
                   uint32_t        buf_lenth,
                   uint32_t       *p_read_lenth)
{
    uint8_t  lrc_path[255];
    uint32_t lenth   = 0;
    char    *dot_idx = NULL;

    if ((NULL == p_path) || (NULL == p_buf) || (NULL == p_read_lenth)){
        return -AW_EINVAL;
    }

    /* 将路径拷贝到缓存区 */
    lenth = strlen(p_path);
    memcpy(lrc_path, p_path, lenth + 1);

    /* 将指针定位到后缀 */
    dot_idx = strrchr(lrc_path, '.') + 1;

    /* 更改后缀 */
    memcpy(dot_idx, "jpg", 4);

    return file_to_mem(lrc_path, p_buf, buf_lenth, p_read_lenth);
}

/**
 * \brief 挂载SD卡
 */
void sd_mount (void)
{

    /* 文件系统挂载 */
#if 1
    while (aw_mount("/sd0", "/dev/sd0", "vfat", 0) != AW_OK) {
#else
    while (aw_mount("/sd0", "/dev/ms2-ud0", "vfat", 0) != AW_OK) {
#endif
        AW_INFOF(("wait mount SD card\n"));
        aw_mdelay(1000);
    }
    AW_INFOF(("mount OK\n"));

    g_is_mount_sdcard = 1;
}

/**
 * \brief 尝试挂载SD卡和U盘
 */
void mount_try (void)
{
    if (aw_mount("/sd0", "/dev/sd0", "vfat", 0) == AW_OK) {
        g_is_mount_sdcard = 1;
    }

    if (aw_mount("/udisk", "/dev/ms2-ud0", "vfat", 0) == AW_OK) {
        g_is_mount_udisk = 1;
    }
}

/**
 * \brief 全局变量初始化
 */
void global_init (void)
{
    memset(&g_play_ctrl, 0, sizeof(g_play_ctrl));

    AW_SEMB_INIT(g_play_ctrl.gui_ok, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
}

/**
 * \brief 初始化所需的资源
 */
void resource_init (void)
{
    aw_time_t time0;
    aw_time_t time1;

    /* 初始化全局变量 */
    global_init();

#if 0
    /* 挂载SD卡 */
    sd_mount();
#endif

#if 0
    /* 读取字体 */
    file_to_mem(gp_ttf_path, g_ttf_res, sizeof(g_ttf_res), &g_ttf_res_lenth);

    aw_time(&time0);
    /* 初始化播放列表 */
    play_list_init(g_play_list, PLAY_LIST_MAX_SIZE);
    aw_time(&time1);

    AW_INFOF(("search time: %d second\n", time1 - time0));
#endif
}

/* end of file */
