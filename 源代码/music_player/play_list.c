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
 * \brief 播放列表
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-15 张进科, 创建该文件
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "apollo.h"
#include "play_list.h"

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "aw_assert.h"
#include "io/aw_unistd.h"
#include "aw_vdebug.h"
#include "aw_imx28_sound.h"
#include "aw_time.h"
#include "dirent.h"
#include "WM.h"
#include "custom_dialog.h"
#include "font_xbf.h"
#include "wav_decoder.h"
#include "mp3_decoder.h"
#include "flac_decoder.h"
#include "gbk_to_unicode.h"
#include "player_global.h"

/*******************************************************************************
  宏定义
*******************************************************************************/

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

music_info_t g_play_list[PLAY_LIST_MAX_SIZE]; /**< \brief 定义播放列表 */
list_state_t g_list_state;                    /**< \brief 播放列表状态 */

aw_imx28_snd_device_t g_snd_dev;              /**< \brief 音频设备 */

static char buf0[255];                        /**< \brief 缓冲区 */
static char buf1[255];                        /**< \brief 缓冲区 */

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 获取文件后缀名
 *
 * \param[in]  p_name 文件名
 * \param[out] p_str  后缀名, 此缓冲区大小不能小于10
 *
 * \return 小于0出错，其它为后缀名在文件名中的位置
 */
int32_t get_file_type (const char *p_name, char *p_str)
{
    uint32_t srt_lenth = 0;
    uint32_t i         = 0;

    if ((NULL == p_name) || (NULL == p_str)) {
        return -1;
    }

    /* 获取文件名长度 */
    srt_lenth = strlen(p_name);
    if ((0 == srt_lenth) || (srt_lenth >= 255)) {
        return -1;
    }

    /* 获取‘.’位置 */
    for (i = srt_lenth - 1; i > 0; i--) {
        if ('.' == p_name[i]) {
            break;
        }
    }

    /* 判断后缀名长度 */
    if ((srt_lenth - i) > (10 - 1)) {
        return -1;
    }

    memcpy(p_str, p_name + i + 1, srt_lenth - i);
    p_str[srt_lenth - i - 1] = '\0';

    return i;
}

/**
 * \brief 二分搜索与快速排序库函数所需的比较函数
 *
 * \param[in] p_a 歌曲信息A
 * \param[in] p_b 歌曲信息B
 *
 * \return a大于b返回小于0，a等于b返回0，a小于b返回大于0
 */
static int inc_compar (const void *p_a, const void *p_b)
{
    return ((int)(((music_info_t *)p_a)->index)) -
           ((int)(((music_info_t *)p_b)->index));
}

/**
 * \brief 获取列表内歌曲数目
 *
 * \param[in] p_play_list 播放列表
 * \param[in] lenth       播放列表长度
 *
 * \return 歌曲数目
 */
int music_count_get (music_info_t *p_play_list, uint32_t lenth)
{
    uint32_t i = 0;

    for (i = 0; i < lenth; i++) {
        if (0xFFFF == p_play_list[i].index) {
            break;
        }
    }

    return i;
}

/**
 * \brief 清空播放列表
 *
 * \param[in] p_play_list 播放列表
 * \param[in] lenth       播放列表长度
 *
 * \return 无
 */
void play_list_clean (music_info_t *p_play_list, uint32_t lenth)
{
    uint32_t i = 0;

    g_list_state.sdcard_start = 0;
    g_list_state.sdcard_count = 0;
    g_list_state.usb_start = 0;
    g_list_state.usb_count = 0;
    g_list_state.play_list_count = 0;
    memset(p_play_list, 0, sizeof(music_info_t) * lenth);
    for (i = 0; i < lenth; i++) {
        p_play_list[i].index = 0xFFFF;
    }
}

/**
 * \brief 清空播放列表中sd卡内的歌曲
 *
 * \param[in] p_play_list 播放列表
 * \param[in] lenth       播放列表长度
 *
 * \return 无
 */
void sd_play_list_clean (music_info_t *p_play_list, uint32_t lenth)
{
    uint32_t i = 0;

    /* 将u盘内音乐移动到播放列表前端 */
    memmove(p_play_list,
           &p_play_list[g_list_state.usb_start],
            g_list_state.usb_count * sizeof(music_info_t));

    memset(p_play_list + g_list_state.usb_count,
           0,
           sizeof(music_info_t) * (lenth - g_list_state.usb_count));
    for (i = g_list_state.usb_count; i < lenth; i++) {
        p_play_list[i].index = 0xFFFF;
    }
    g_list_state.play_list_count = g_list_state.usb_count;
    g_list_state.sdcard_start = 0;
    g_list_state.sdcard_count = 0;
    g_list_state.usb_start = 0;
}

/**
 * \brief 清空播放列表中u盘内的歌曲
 *
 * \param[in] p_play_list 播放列表
 * \param[in] lenth       播放列表长度
 *
 * \return 无
 */
void usb_play_list_clean (music_info_t *p_play_list, uint32_t lenth)
{
    uint32_t i = 0;

    /* 将sd卡内音乐移动到播放列表前端 */
    memmove(p_play_list,
           &p_play_list[g_list_state.sdcard_start],
            g_list_state.sdcard_count * sizeof(music_info_t));

    memset(p_play_list + g_list_state.sdcard_count,
           0,
           sizeof(music_info_t) * (lenth - g_list_state.sdcard_count));
    for (i = g_list_state.sdcard_count; i < lenth; i++) {
        p_play_list[i].index = 0xFFFF;
    }
    g_list_state.play_list_count = g_list_state.sdcard_count;
    g_list_state.usb_start = 0;
    g_list_state.usb_count = 0;
    g_list_state.sdcard_start = 0;
}

/**
 * \brief 在列表中添加一首歌
 *
 * \param[in] p_play_list  播放列表
 * \param[in] lenth        播放列表长度
 * \param[in] p_music_info 待添加的音乐, index可以不用初始化
 *
 * \return 小于0出错，其它为添加的歌曲在列表中的索引
 */
int32_t play_list_add (music_info_t *p_play_list,
                       uint32_t      lenth,
                       music_info_t *p_music_info)
{
    uint32_t i   = 0;
    int32_t  ret = -1;

#if 0
    AW_INFOF(("%s - %s tpye: %d\n",
              p_music_info->artist,
              p_music_info->name,
              p_music_info->type));
#endif

    for (i = 0; i < lenth; i++) {
        if (0xFFFF == p_play_list[i].index) {
            memcpy(&p_play_list[i], p_music_info, sizeof(music_info_t));
            p_play_list[i].index = i;
            g_list_state.play_list_count++;
            ret = i;
            break;
        }
    }

    return ret;
}

/**
 * \brief 在列表中删除一首歌
 *
 * \param[in] p_play_list  播放列表
 * \param[in] lenth        播放列表长度
 * \param[in] p_music_info 待删除的音乐
 *
 * \retval AW_OK      成功
 * \retval -AW_ERROR  失败
 * \retval -AW_EINVAL 无效参数
 */
aw_err_t play_list_del (music_info_t *p_play_list,
                        uint32_t      lenth,
                        music_info_t *p_music_info)
{

    int           idx               = 0;
    int32_t       ret               = -AW_ERROR;
    music_info_t *p_music_info_recv = NULL;

    if ((NULL == p_play_list) || (NULL == p_music_info)) {
        return -AW_EINVAL;
    }

    p_music_info_recv = (music_info_t  *)bsearch(p_music_info,
                                                 p_play_list,
                                                 lenth,
                                                 sizeof(music_info_t),
                                                 inc_compar);
    if (p_music_info_recv != NULL) {
        idx = p_music_info_recv->index;
        if (strcmp(p_play_list[idx].file_name, p_music_info->file_name) == 0) {
            memset(&p_play_list[idx], 0, sizeof(music_info_t));
            p_play_list[idx].index = 0xFFFF;
            qsort(p_play_list, lenth, sizeof(music_info_t), inc_compar);
            for (idx = 0; idx < lenth; idx++) {
                if (p_play_list[idx].index != 0xFFFF) {
                    p_play_list[idx].index = idx;
                } else {
                    break;
                }
            }
            g_list_state.play_list_count--;
            ret = AW_OK;
        }
    }

    return ret;
}

/**
 * \brief 解析歌曲名称与歌手名字
 *
 * \param[in] p_music_info 音乐信息结构
 * \param[in] p_dirent     文件夹信息
 * \param[in] p_dir        文件夹路径
 * \param[in] dot_idx      类型前‘.’的索引
 *
 * \return 无
 */
static void path_anlysis (music_info_t     *p_music_info,
                          struct aw_dirent *p_dirent,
                          const char       *p_dir,
                          int32_t           dot_idx)
{
    uint32_t dir_lenth   = 0;
    uint32_t name_lenth  = 0;
    int32_t  buf1_lenth  = 0;

    char   *p_token     = NULL;
    int32_t token_idx   = 0;
    int32_t token_lenth = 0;


    aw_assert(NULL != p_music_info);
    aw_assert(NULL != p_dirent);
    aw_assert(NULL != p_dir);
    name_lenth = strlen(p_dirent->d_name);

    /* 获取路径长度 */
    dir_lenth = strlen(p_dir);

    memcpy(p_music_info->file_name, p_dir, dir_lenth);
    p_music_info->file_name[dir_lenth] = '/';
    memcpy(p_music_info->file_name + dir_lenth + 1,
           p_dirent->d_name,
           name_lenth);
    p_music_info->file_name[dir_lenth + name_lenth + 1] = '\0';
    memcpy(buf0, p_dirent->d_name, dot_idx);
    buf0[dot_idx] = '\0';

    gbk_to_utf8(buf0, buf1, 255);
    buf1_lenth = strlen(buf1);
    token_idx = 0;
    p_token = strtok(buf1, "-");
    if (strlen(p_token) != buf1_lenth) {
        while (p_token != NULL) {
            token_lenth = strlen(p_token);
            if (token_idx == 0) {
                memcpy(p_music_info->artist, p_token, token_lenth - 1);
                p_music_info->artist[token_lenth - 1] = '\0';
            } else if (token_idx == 1) {
                memcpy(p_music_info->name, p_token + 1, token_lenth);
                p_music_info->name[token_lenth - 1] = '\0';
            }
            p_token = strtok(NULL, "-");
            token_idx++;
        }
    } else {
        memcpy(p_music_info->name, buf1, strlen(buf1) + 1);
    }
}

/**
 * \brief 搜索文件夹内歌曲并添加进播放列表
 *
 * \param[in] p_play_list 播放列表
 * \param[in] lenth       播放列表长度
 * \param[in] p_dir       音乐所在文件夹
 * \param[in] show_info   0: 不显示info 1:显示info
 *
 * \return 无
 */
void play_list_scan (music_info_t *p_play_list,
                     uint32_t      lenth,
                     const char   *p_dir,
                     uint8_t       show_info)
{
    DIR              *p_dir_t  = NULL;
    struct aw_dirent *p_dirent = NULL;

    int32_t  recv = 0;
    char     sub_str[10];

    music_info_t music_info;
    wav_ctrl_t   wav_ctrl;
    mp3_info_t   mp3_info;
    flac_ctrl_t  flac_ctrl;

    dialog_info_t info;

    int       i = 0;
    aw_time_t time0;
    aw_time_t time1;

    uint8_t old_paly_state = g_play_ctrl.paly_state;

    aw_assert(NULL != p_play_list);
    aw_assert(NULL != p_dir);

    aw_time(&time0);
    g_play_ctrl.paly_state = PALY_STATE_PAUSE;
    WM_Paint(WM_HBKWIN);

    if (0 != show_info) {
        if (0 !=g_custom_dialog_state) {
            dialog_close();
        }

        dialog_info_init(&info);
        info.blocking = 0;
        info.modal = 0;
        info.font_title  = &g_font14;
        info.font_text   = &g_font14;
        info.font_btn[0] = &g_font14;
        info.font_btn[1] = &g_font14;
        info.btn_color_text[1] = 0xD4881E;
        strcpy(info.title, "正在搜索歌曲");
        strcpy(info.text, "");
        strcpy(info.btn_text[0], "确定");
        info.btn_num = 1;
        dialog_show(130,
                    76,
                    info.rect.x1 + 1,
                    info.rect.y1 + 1,
                    WM_HBKWIN,
                    WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_STAYONTOP,
                   &info);
    }

    /* 打开文件夹 */
    p_dir_t = aw_opendir(p_dir);
    if (p_dir_t == NULL) {
        return;
    }

    /* 遍历文件夹内文件 */
    while ((p_dirent = aw_readdir(p_dir_t))) {

        /* 获取文件类型 */
        if ((recv = get_file_type(p_dirent->d_name, sub_str)) > 0) {
            memset(&music_info, 0, sizeof(music_info));
            music_info.type = MUSIC_TYPE_OTHER;

            if ((strcmp(sub_str, "wav") == 0) ||
                (strcmp(sub_str, "WAV") == 0)) {
                path_anlysis(&music_info, p_dirent, p_dir, recv);

                music_info.type = MUSIC_TYPE_WAV;
                if (wav_info_get(music_info.file_name, &wav_ctrl) == AW_OK) {
                    music_info.total_time    = wav_ctrl.total_time;
                    music_info.bits_per_word = wav_ctrl.bps;
                    music_info.channels      = wav_ctrl.nchannels;
                    music_info.samples_rate  = wav_ctrl.sample_rate;
                    music_info.bitrate       = wav_ctrl.bit_rate;
                    music_info.file_size     = wav_ctrl.file_size;
                    play_list_add(p_play_list, lenth, &music_info);
                } else {
                    AW_ERRF(("%s add failed\n", music_info.file_name));
                }
            } else if ((strcmp(sub_str, "mp3") == 0) ||
                       (strcmp(sub_str, "MP3") == 0)) {
                path_anlysis(&music_info, p_dirent, p_dir, recv);

                music_info.type = MUSIC_TYPE_MP3;
                if (mp3_info_get(music_info.file_name, &mp3_info) == AW_OK) {
                    music_info.total_time    = mp3_info.total_time;
                    music_info.bits_per_word = 0;
                    music_info.channels      = mp3_info.nchannels;
                    music_info.samples_rate  = mp3_info.sample_rate;
                    music_info.bitrate       = mp3_info.bit_rate;
                    music_info.file_size     = mp3_info.file_size;
                    play_list_add(p_play_list, lenth, &music_info);
                } else {
                    AW_ERRF(("%s add failed\n", music_info.file_name));
                }
            } else if ((strcmp(sub_str, "flac") == 0) ||
                       (strcmp(sub_str, "FLAC") == 0)) {
                path_anlysis(&music_info, p_dirent, p_dir, recv);

                music_info.type = MUSIC_TYPE_FLAC;
                if (get_flac_info(music_info.file_name, &flac_ctrl) == AW_OK) {
                    music_info.total_time    = flac_ctrl.total_time;
                    music_info.bits_per_word = flac_ctrl.bps;
                    music_info.channels      = flac_ctrl.nchannels;
                    music_info.samples_rate  = flac_ctrl.sample_rate;
                    music_info.bitrate       = flac_ctrl.bit_rate;
                    music_info.file_size     = flac_ctrl.file_size;
                    play_list_add(p_play_list, lenth, &music_info);
                } else {
                    AW_ERRF(("%s add failed\n", music_info.file_name));
                }
            } else {

            }

            if ((0 != show_info) &&
                (MUSIC_TYPE_OTHER != music_info.type) &&
                (0 != g_custom_dialog_state)) {
                gbk_to_utf8(music_info.file_name, buf1, 255);
                text_set(buf1);
                GUI_Exec();
            }

        } else {
            AW_INFOF(("%d  %s\n", p_dirent->d_ino, p_dirent->d_name));
        }
    }

    aw_closedir(p_dir_t);

    aw_time(&time1);
    AW_INFOF(("search time is %d second\n", time1 - time0));
    g_play_ctrl.paly_state = old_paly_state;

    if (0 != show_info) {
        if (0 != g_custom_dialog_state) {
            dialog_close();
        }

        dialog_info_init(&info);
        info.font_title  = &g_font14;
        info.font_text   = &g_font14;
        info.font_btn[0] = &g_font14;
        info.font_btn[1] = &g_font14;
        info.btn_color_text[1] = 0xD4881E;
        strcpy(info.title, "正在搜索歌曲");
        strcpy(info.text, "搜索完成");
        strcpy(info.btn_text[0], "确定");
        info.btn_num = 1;
        info.blocking = 0;
        dialog_show(130,
                    76,
                    info.rect.x1 + 1,
                    info.rect.y1 + 1,
                    WM_HBKWIN,
                    WM_CF_SHOW | WM_CF_HASTRANS | WM_CF_STAYONTOP,
                   &info);
    }
}

/**
 * \brief 初始化播放列表
 *
 * \param[in] p_play_list 播放列表
 * \param[in] lenth       播放列表长度
 * \param[in] show_info   0: 不显示info 1:显示info
 * \param[in] range       0: 搜索SD卡和U盘 1:搜索SD卡 2:搜索U盘
 *
 * \return 无
 */
void play_list_init (music_info_t *p_play_list,
                     uint32_t      lenth,
                     uint8_t       show_info,
                     uint8_t       range)
{
    uint8_t  file_name_old[MUSIC_INFO_FILE_PATH_MAX_LENTH];
    uint32_t i;

    aw_assert(NULL != p_play_list);

    /* 保存正在播放的音乐名字 */
    strcpy(file_name_old, p_play_list[g_play_ctrl.play_idx].file_name);

    /* 先清除播放列表指定的歌曲再搜索，避免同一首歌出现多次 */
    if (SEARCH_RANGE_SD_USB == range) {
        play_list_clean(p_play_list, lenth);
    } else if (SEARCH_RANGE_SD == range) {
        sd_play_list_clean(p_play_list, lenth);
    } else {
        usb_play_list_clean(p_play_list, lenth);
    }

    if ((g_is_mount_sdcard) && (SEARCH_RANGE_USB != range)) {
        g_list_state.sdcard_start = g_list_state.play_list_count;
        play_list_scan(p_play_list, lenth, gp_sdcard_music_path ,show_info);
        g_list_state.sdcard_count = g_list_state.play_list_count;
    }

    if ((g_is_mount_udisk) && (SEARCH_RANGE_SD != range)) {
        g_list_state.usb_start = g_list_state.play_list_count;
        play_list_scan(p_play_list, lenth, gp_udisk_music_path ,show_info);
        g_list_state.usb_count = g_list_state.play_list_count -
                                 g_list_state.sdcard_count;
    }

    /* 找到列表更新后当前歌曲的索引 */
    for (i = 0; i < lenth; i++) {
        if (0 == strcmp(file_name_old, p_play_list[i].file_name)) {
            g_play_ctrl.play_idx = i;
            break;
        }
    }
    if (lenth == i) {
        aw_close(g_play_ctrl.fd);
        play_event_send(PLAY_EVENT_NEXT); /* 发送下一曲事件 */
    }
}

/* end of file */
