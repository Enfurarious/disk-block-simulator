/*
* Kevin Velasquez
* COMP 362 - Operating Systems
* Based on coursework provided by Kevin Scrivnor
*
* Tests the libciblk device by writing/reading random data
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "lib/libciblk.h"

/*
Don't forget to make your device file with:
dd if=/dev/zero of=ciblk.img bs=1024 count=1
*/
#define bytes 1024

void    // maybe this should be called generate_garbo()
generate_data(char* buffer, int len)
{
    for(int i = 0; i < len-1; i++) {
        // ' ' is first printable
        // '~' is last printable
        // generate a random character somewhere in between
        int rchar = ' ' + (rand() % ('~' - ' '));
        buffer[i] = rchar;
    }
    buffer[len-1] = '\0';
}

int
main(int argc, char** argv)
{
    if(argc != 2) {
        fprintf(stderr, "Error, please provide a filename\n\tUsage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ci_blk_dev_attrs_t attrs;
    ci_blk_dev_t* dev = ci_blk_create(&attrs);
    if(!dev) {
        fprintf(stderr, "Error, device not initialized. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    if(ci_blk_open(dev, argv[1]) != CI_BLK_SUCCESS) {
        fprintf(stderr, "Error, ci_blk_open() failed. Exiting...\n");
        exit(EXIT_FAILURE);
    }

    srand((unsigned int) time(NULL));

    int blocks = bytes / attrs.block_size; // 24 for testing

    char* data = malloc(attrs.block_size * blocks);
    char* compare = malloc(attrs.block_size * blocks);

    // simulate read and writes
    for(int i = 0; i < 10; i++) {
        // Generate a random data size that is at most as big the disk
        int size = rand() % (attrs.block_size * blocks); 
        int data_blocks = size / attrs.block_size;
        if(size % attrs.block_size != 0) data_blocks++;

        // Pick a random starting block within the disk
        int start_block = rand() % blocks;

        // Determine if random data size and starting block will cause an issue or not
        int should_error = false;
        if(start_block + data_blocks >= blocks) should_error = true;

        // Generate the random data
        generate_data(data, size);
        
        printf("===============================================================================\n");
        printf("TRIAL %d\n", i+1);
        printf("Starting Block: %d\tEnding Block: %d\tMax Block: %d\n", start_block, start_block+data_blocks, blocks);
        printf("Data (%d bytes, %d blocks):\n%s\n", size, data_blocks, data);
        int ret = ci_blk_write(dev, start_block, data, size);
        if(should_error) { // write should fail, skip read
            if(ret != CI_BLK_SUCCESS) {
                printf("SUCCESS -- Write fails as expected, skipping Read test\n");
            } else {
                printf("ERROR -- Write expected to fail!\nStopping trials\n");
                exit(EXIT_FAILURE);
            }
        } else { // write supposed to work, check read next
            if(ret == CI_BLK_SUCCESS) {
                ret = ci_blk_read(dev, start_block, compare, size);
                if(ret == CI_BLK_SUCCESS) {
                    printf("Data read:\n%s\n", compare);
                    if(strcmp(data, compare) != 0) { // read works, but data doesn't match
                        printf("ERROR -- Data mismatch\nStopping trials\n");
                        exit(EXIT_FAILURE);
                    } else { // ALL GOOD
                        printf("SUCCESS -- Read data and data matches\n");
                    }
                } else { // read didn't work when it was supposed to
                    printf("ERROR -- Read expected to succeed!\nStopping trials\n");
                    exit(EXIT_FAILURE);
                } 
            } else { // write didn't work when it was supposed to
                printf("ERROR -- Write expected to succeed!\nStopping trials\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    free(data);
    free(compare);

    ci_blk_close(dev);
    ci_blk_destroy(dev);
}
