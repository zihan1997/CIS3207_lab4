#ifndef _FAT_H
#define _FAT_H

#define FOLDER_ENTRIES 256

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "disk.c"

// 41 Bytes
typedef struct Entry{
    char name[16]; //  16 bytes
    char ext[3]; // 3 bytes
    uint8_t attribute; // 1 byte; 1 for folder, 0 for file
    uint16_t create_time; // 2 bytes, 1 byte for hour
    uint16_t create_date; // 2 bytes, 
    uint16_t last_access; // 2
    uint16_t modified_time; // 2
    uint16_t modified_date; // 2
    uint16_t start_block; // 2
    uint32_t size;
    // use if the file is a folder
    uint8_t num_entries;
    struct Entry* entries;  
}Entry;



// Totally 8 bytes
typedef struct SuperBlock
{
    uint16_t fat; // location of fat1
    uint16_t fat1; // location of fat2
    uint16_t root; // location of root directory
    uint16_t data_blocks; // start location of data location
}SuperBlock;

// Individual data structure in FAT
// Totally 4 bytes
typedef struct map
{
    uint16_t input;
    uint16_t output;
}map;

// Totally 4 KB
typedef struct FAT
{
    uint16_t total_blocks; // 2 bytes
    uint16_t used_blocks; // 2 bytes
    map maps[999];
}FAT;

typedef struct Discriptor{
    char file_name[16]; // file name
    uint8_t file_disc; // file discriptor
}Discriptor;

typedef struct Discriptors{
    uint8_t next_avail;
    uint8_t size;
    Discriptor file_discs[64];
}Discriptors;

// 4 bytes
typedef struct VolumeHeader{
    uint16_t num_fats; // 2 bytes, number of fat
    uint16_t root_entries; //2 bytes,number of root entrie 
}VolumeHeader;

// 50 bytes
typedef struct Volume
{
    char name[6]; /*identifer for the virtualdisk*/ 
    char img[PATH_MAX]; /*disk file's file pointer*/
    // int state;
    uint32_t used_size; // 4 bytes
    VolumeHeader *vbr; // 4 bytes
    SuperBlock *super_block;
    FAT *fat; // 4
    Entry* root; // 32
    // struct Data_blub; /*beginning of the data region*/
}Volume;

// Data structures
static VolumeHeader *vbr;
static Volume *disk;
static FAT fat;
static SuperBlock superBlock;
// static Entry entry;
static Entry rootDir;
// file discriptors
static Discriptors discriptors;

// Functions
void initialStructures();
int make_fs(char* disk_name);
int mount_fs(char *disk_name);
int umount_fs(char *disk_name);;
int fs_open(char *name);
int fs_close(int fildes);
int fs_create(char *name);
int fs_delete(char *name);
int fs_mkdir(char *name);
int fs_read(int fildes, void *buf, size_t nbyte);
int fs_write(int fildes, void *buf, size_t nbyte);
int fs_get_filesize(int fildes);
int fs_lseek(int fildes, off_t offset);
int fs_truncate(int fildes, off_t length);

#endif