/*
* Kevin Velasquez
* COMP 362 - Operating Systems
* Based on coursework provided by Kevin Scrivnor
*
* Simulates a library to open/read/write/close a block device on a character device
*/

#include "ciblk.h"
#include "libciblk.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

ci_blk_dev_t*
ci_blk_create(ci_blk_dev_attrs_t* attrs)
{
    ci_blk_dev_t* dev = (ci_blk_dev_t*)malloc(sizeof(ci_blk_dev_t));
    if (!dev) return NULL;
    dev->fd = -1;
    dev->block_size = CI_BLK_SIZE;
    dev->blocks = 0;
    if (attrs) attrs->block_size = dev->block_size;
    return dev;
}

void
ci_blk_destroy(ci_blk_dev_t* dev)
{
    if (!dev) return;
    if (dev->fd >= 0) close(dev->fd);
    free(dev);
}

int
ci_blk_open(ci_blk_dev_t* dev, const char* path)
{
    if (!dev) return -CI_BLK_NULL_DEV;
    if (!path || *path == '\0') return -CI_BLK_NO_FILE_PROVIDED;

    struct stat st;
    if (stat(path, &st) != 0) return -CI_BLK_CANNOT_STAT;
    if (!S_ISREG(st.st_mode)) return -CI_BLK_NOT_REGULAR_FILE;

    int fd = open(path, O_RDWR);
    if (fd < 0) return -CI_BLK_OPEN_FILE;

    dev->fd = fd;
    if (dev->block_size <= 0) dev->block_size = CI_BLK_SIZE;
    dev->blocks = (int)(st.st_size / dev->block_size);
    return CI_BLK_SUCCESS;
}

int
ci_blk_close(ci_blk_dev_t* dev)
{
    if (!dev) return -CI_BLK_NULL_DEV;
    if (dev->fd >= 0) {
        close(dev->fd);
        dev->fd = -1;
    }
    return CI_BLK_SUCCESS;
}

int
ci_blk_write(ci_blk_dev_t* dev, int block_num, const void* buffer, ssize_t count)
{
    if (!dev) return -CI_BLK_NULL_DEV;
    if (block_num < 0 || block_num >= dev->blocks) return -CI_BLK_INVALID_BLOCK_NUM;
    if (count < 0) return -CI_BLK_OFFSET;

    int bs = dev->block_size;
    ssize_t need_blocks = (count + bs - 1) / bs;
    if (block_num + need_blocks > dev->blocks) return -CI_BLK_NOT_ENOUGH_SPACE;

    off_t off = (off_t)block_num * bs;
    if (lseek(dev->fd, off, SEEK_SET) < 0) return -CI_BLK_OFFSET;

    const char* p = (const char*)buffer;
    ssize_t left = count;
    for (ssize_t i = 0; i < need_blocks; i++) {
        ssize_t chunk = left > bs ? bs : left;
        if (chunk == 0) chunk = bs;
        ssize_t w = write(dev->fd, p, chunk);
        if (w != chunk) return -CI_BLK_OFFSET;
        if (left > 0) {
            p += chunk;
            if (left >= chunk) left -= chunk; else left = 0;
        }
        if (chunk < bs && left == 0) {
            break;
        }
    }
    return CI_BLK_SUCCESS;
}

int
ci_blk_read(ci_blk_dev_t* dev, int block_num, void* buffer, ssize_t count)
{
    if (!dev) return -CI_BLK_NULL_DEV;
    if (block_num < 0 || block_num >= dev->blocks) return -CI_BLK_INVALID_BLOCK_NUM;
    if (count < 0) return -CI_BLK_OFFSET;

    int bs = dev->block_size;
    ssize_t need_blocks = (count + bs - 1) / bs;
    if (block_num + need_blocks > dev->blocks) return -CI_BLK_NOT_ENOUGH_SPACE;

    off_t off = (off_t)block_num * bs;
    if (lseek(dev->fd, off, SEEK_SET) < 0) return -CI_BLK_OFFSET;

    char* p = (char*)buffer;
    ssize_t left = count;
    for (ssize_t i = 0; i < need_blocks; i++) {
        ssize_t chunk = left > bs ? bs : left;
        if (chunk == 0) chunk = bs;
        ssize_t r = read(dev->fd, p, chunk);
        if (r != chunk) return -CI_BLK_READ_ERROR;
        if (left > 0) {
            p += chunk;
            if (left >= chunk) left -= chunk; else left = 0;
        }
        if (chunk < bs && left == 0) {
            break;
        }
    }
    return CI_BLK_SUCCESS;
}

void
ci_blk_print_error(int error)
{
    switch (error) {
    case CI_BLK_SUCCESS:
        fprintf(stderr, "No error\n");
        break;
    case -CI_BLK_NO_FILE_PROVIDED:
        fprintf(stderr, "Error %d: No file provided\n", error);
        break;
    case -CI_BLK_CANNOT_STAT:
        fprintf(stderr, "Error %d: Cannot stat file\n", error);
        break;
    case -CI_BLK_NOT_REGULAR_FILE:
        fprintf(stderr, "Error %d: File is not a regular file type\n", error);
        break;
    case -CI_BLK_OPEN_FILE:
        fprintf(stderr, "Error %d: Cannot open file\n", error);
        break;
    case -CI_BLK_NULL_DEV:
        fprintf(stderr, "Error %d: Device was not created\n", error);
        break;
    case -CI_BLK_INVALID_BLOCK_NUM:
        fprintf(stderr, "Error %d: Invalid block number\n", error);
        break;
    case -CI_BLK_NOT_ENOUGH_SPACE:
        fprintf(stderr, "Error %d: Not enough space\n", error);
        break;
    case -CI_BLK_OFFSET:
        fprintf(stderr, "Error %d: Cannot seek to offset\n", error);
        break;
    case -CI_BLK_READ_ERROR:
        fprintf(stderr, "Error %d: I/O error with read\n", error);
        break;
    default:
        fprintf(stderr, "Unknown error code %dn", error);
        break;
    }
}
