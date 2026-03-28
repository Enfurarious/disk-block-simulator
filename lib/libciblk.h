#pragma once
#include <unistd.h>

#define CI_BLK_SUCCESS 0

// forward definition
// user of library can use ci_blk_dev_t but cannot access members
typedef struct ci_blk_dev ci_blk_dev_t;

// for returning the attributes set by the library
typedef struct ci_blk_dev_attrs {
    int block_size;
} ci_blk_dev_attrs_t;

ci_blk_dev_t* ci_blk_create(ci_blk_dev_attrs_t* attrs);
void ci_blk_destroy(ci_blk_dev_t* dev);

int ci_blk_open(ci_blk_dev_t* dev, const char* path);
int ci_blk_close(ci_blk_dev_t* dev);

int ci_blk_write(ci_blk_dev_t* dev, int block_num, const void* buffer, ssize_t count);
int ci_blk_read(ci_blk_dev_t* dev, int block_num, void* buffer, ssize_t count);

void ci_blk_print_error(int error);
