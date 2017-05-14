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
 * \brief ��Դ����
 *
 * \internal
 * \par Modification history
 * - 1.00 17-03-10 �Ž���, �������ļ�
 * \endinternal
 */

/*******************************************************************************
  ͷ�ļ�����
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
  �궨��
*******************************************************************************/

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

/** \brief �����б�·�� */
static const char *gp_play_list_path = "/sd0/system/play_list.bin";

/** \brief �����ļ�·�� */
static const char *gp_config_path = "/sd0/system/config.bin";

#if 0
/** \brief ����·�� */
static const char *gp_ttf_path = "/sd0/system/Adobe Hei Std.xbf";

/** \brief ����ͼƬ·�� */
const char *gp_background_path = "/sd0/system/background/2.jpg";
#endif

/** \brief ����Ŀ¼ */
const char *gp_sdcard_music_path = "/sd0/music";
const char *gp_udisk_music_path  = "/udisk/music";

/** \brief �Ƿ������SD�� 0:δ���� 1:�ѹ��� */
uint8_t g_is_mount_sdcard = 0;

/** \brief �Ƿ������U�� 0:δ���� 1:�ѹ��� */
uint8_t g_is_mount_udisk = 0;

/** \brief ������Ϣ */
config_info_t g_config_info;

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/**
 * \brief ��ȡ������Ϣ
 *
 * \param[in] p_config_info ������Ϣ
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
 */
aw_err_t config_read (config_info_t *p_config_info)
{
    config_info_t config_info = {0, -1, 0};
    int           fd          = 0;
    int32_t       size        = 0;

    if (NULL == p_config_info) {
        return -AW_EINVAL;
    }

    /* ���ļ� */
    fd = aw_open(gp_config_path, O_RDONLY, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_ERROR;
    }

    /* ��ȡ�ļ���С */
    if ((size = file_size_get(fd)) < sizeof(config_info_t)) {
        goto fail;
    }

    /* ��ȡ�ļ� */
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
 * \brief ����������Ϣ
 *
 * \param[in] p_config_info ������Ϣ
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
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

    /* ���ļ� */
    fd = aw_open(gp_config_path, O_RDWR | O_CREAT, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_EINVAL;
    }

    /* д���ļ� */
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
 * \brief ��ȡ�����б�
 *
 * \param[in] p_play_list  �����б�
 * \param[in] p_list_state �����б�״̬
 * \param[in] lenth        �����б���
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
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

    /* ���ļ� */
    fd = aw_open(gp_play_list_path, O_RDONLY, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_ERROR;
    }

    /* ��ȡ�ļ���С */
    if ((size = file_size_get(fd)) < sizeof(config_info_t)) {
        goto fail;
    }

    if (size < sizeof(music_info_t) * lenth + sizeof(check_sum0)) {
        goto fail;
    }

    /* ��ȡ�����б� */
    if (aw_read(fd, p_play_list, sizeof(music_info_t) * lenth) !=
        sizeof(music_info_t) * lenth) {
        AW_ERRF(("read %s error!\n", gp_play_list_path));
        goto fail;
    }

    /* ��ȡ�����б�״̬ */
    if (aw_read(fd, p_list_state, sizeof(list_state_t)) !=
        sizeof(list_state_t)) {
        AW_ERRF(("read %s error!\n", gp_play_list_path));
        goto fail;
    }

    /* ��ȡУ��� */
    if (aw_read(fd, &check_sum0, sizeof(check_sum0)) != sizeof(check_sum0)) {
        AW_ERRF(("read %s error!\n", gp_play_list_path));
        goto fail;
    }

    /* ����У��� */
    for (int i = 0; i < sizeof(music_info_t) * lenth; i++)
    {
        check_sum1 += ((uint8_t *)p_play_list)[i];
    }
    for (int i = 0; i < sizeof(list_state_t); i++)
    {
        check_sum1 += ((uint8_t *)p_list_state)[i];
    }

    /* �Ա�У��� */
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
 * \brief ���沥���б�
 *
 * \param[in] p_play_list  �����б�
 * \param[in] p_list_state �����б�״̬
 * \param[in] lenth        �����б���
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
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

    /* ���ļ� */
    fd = aw_open(gp_play_list_path, O_RDWR | O_CREAT, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_EINVAL;
    }

    /* ����У��� */
    for (int i = 0; i < sizeof(music_info_t) * lenth; i++)
    {
        check_sum += ((uint8_t *)p_play_list)[i];
    }
    for (int i = 0; i < sizeof(list_state_t); i++)
    {
        check_sum += ((uint8_t *)p_list_state)[i];
    }

    /* д�벥���б� */
    if (aw_write(fd, p_play_list, sizeof(music_info_t) * lenth) !=
        sizeof(music_info_t) * lenth) {
        AW_ERRF(("write %s error!\n", gp_play_list_path));
        goto fail;
    }

    /* д�벥���б�״̬ */
    if (aw_write(fd, p_list_state, sizeof(list_state_t)) !=
        sizeof(list_state_t)) {
        AW_ERRF(("write %s error!\n", gp_play_list_path));
        goto fail;
    }

    /* д��У��� */
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
 * \brief ��ȡ�ļ���С
 *
 * \param[in] fd �Ѿ��򿪵��ļ����
 *
 * \return С��0��������Ϊ��ȡ�����ļ���С
 */
int file_size_get (int fd)
{
    off_t   old_off = 0;
    int32_t size    = 0;

    /* ���浱ǰ�ļ���дλ�� */
    if ((old_off = aw_lseek(fd, 0, SEEK_CUR)) < 0) {
        goto fail;
    }

    /* ��ȡ�ļ���С */
    if ((size = aw_lseek(fd, 0, SEEK_END)) < 0) {
        goto fail;
    }

    /* �ָ���֮ǰ��λ�� */
    if ((old_off = aw_lseek(fd, old_off, SEEK_SET)) < 0) {
        goto fail;
    }

    return size;

fail:
    return -1;
}

/**
 * \brief ��ָ��·�����ļ���ȡ���ڴ���
 *
 * \param[in]  p_path       �ļ�����·��
 * \param[out] p_buf        ��ȡ����λ��
 * \param[in]  lenth        ��������С
 * \param[out] p_read_lenth ��ȡ�����ļ���С
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
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

    /* ���ļ� */
    fd = aw_open(p_path, O_RDONLY, 0777);
    if (fd < 0) {
        AW_ERRF(("open file error: %d\n", fd));
        return -AW_ERROR;
    }

    /* ��ȡ�ļ���С */
    if ((size = file_size_get(fd)) < 0) {
        AW_ERRF(("%s size get error!\n", p_path));
        aw_close(fd);
        return -AW_ERROR;
    }

    /* ��黺�����Ƿ��㹻 */
    if (size > buf_lenth) {
        aw_close(fd);
        AW_ERRF(("%s too large!\n", p_path));
        return -AW_ERROR;
    }

    /* ��ȡ�ļ� */
    if (aw_read(fd, p_buf, size) != size) {
        aw_close(fd);
        AW_ERRF(("read %s error!\n", p_path));
        return -AW_ERROR;
    }

    /* �����ļ���С */
    if (NULL != p_read_lenth) {
        *p_read_lenth = size;
    }

    aw_close(fd);
    return AW_OK;
}

/**
 * \brief ��ȡ������ͬ�ļ����ĸ��
 *
 * \param[in]  p_path       �����ļ���
 * \param[out] p_buf        ��ȡ����λ��
 * \param[in]  lenth        ��������С
 * \param[out] p_read_lenth ��ȡ�����ļ���С
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
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

    /* ��·�������������� */
    lenth = strlen(p_path);
    memcpy(lrc_path, p_path, lenth + 1);

    /* ��ָ�붨λ����׺ */
    dot_idx = strrchr(lrc_path, '.') + 1;

    /* ���ĺ�׺ */
    memcpy(dot_idx, "lrc", 4);

    return file_to_mem(lrc_path, p_buf, buf_lenth, p_read_lenth);
}

/**
 * \brief ��ȡ������ͬ�ļ�����ͼƬ
 *
 * \param[in]  p_path       �����ļ���
 * \param[out] p_buf        ��ȡ����λ��
 * \param[in]  lenth        ��������С
 * \param[out] p_read_lenth ��ȡ�����ļ���С
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_EINVAL ��Ч����
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

    /* ��·�������������� */
    lenth = strlen(p_path);
    memcpy(lrc_path, p_path, lenth + 1);

    /* ��ָ�붨λ����׺ */
    dot_idx = strrchr(lrc_path, '.') + 1;

    /* ���ĺ�׺ */
    memcpy(dot_idx, "jpg", 4);

    return file_to_mem(lrc_path, p_buf, buf_lenth, p_read_lenth);
}

/**
 * \brief ����SD��
 */
void sd_mount (void)
{

    /* �ļ�ϵͳ���� */
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
 * \brief ���Թ���SD����U��
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
 * \brief ȫ�ֱ�����ʼ��
 */
void global_init (void)
{
    memset(&g_play_ctrl, 0, sizeof(g_play_ctrl));

    AW_SEMB_INIT(g_play_ctrl.gui_ok, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
}

/**
 * \brief ��ʼ���������Դ
 */
void resource_init (void)
{
    aw_time_t time0;
    aw_time_t time1;

    /* ��ʼ��ȫ�ֱ��� */
    global_init();

#if 0
    /* ����SD�� */
    sd_mount();
#endif

#if 0
    /* ��ȡ���� */
    file_to_mem(gp_ttf_path, g_ttf_res, sizeof(g_ttf_res), &g_ttf_res_lenth);

    aw_time(&time0);
    /* ��ʼ�������б� */
    play_list_init(g_play_list, PLAY_LIST_MAX_SIZE);
    aw_time(&time1);

    AW_INFOF(("search time: %d second\n", time1 - time0));
#endif
}

/* end of file */
