#pragma once

#define CI_BLK_SIZE 16

enum exit_codes {
    CI_BLK_SUCCESS,
    CI_BLK_NO_FILE_PROVIDED,
    CI_BLK_CANNOT_STAT,
    CI_BLK_NOT_REGULAR_FILE,
    CI_BLK_OPEN_FILE,
    CI_BLK_NULL_DEV,
    CI_BLK_INVALID_BLOCK_NUM,
    CI_BLK_NOT_ENOUGH_SPACE,
    CI_BLK_OFFSET,
    CI_BLK_READ_ERROR,
    CI_BLK_NOT_IMPLEMENTED
};

struct ci_blk_dev {
    int fd;
    int blocks;
    int block_size;
};
