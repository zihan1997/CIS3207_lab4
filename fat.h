#ifndef _FAT_H
#define _FAT_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>


typedef struct SuperBlock
{
    uint16_t root; // location of root directory
    uint16_t data_blocks; // start location of data location
    uint16_t fat; // location of fat
}SuperBlock;

typedef struct FAT
{
    unsigned int total_blocks;
    unsigned int used_blocks;
    int* map;
}FAT;

typedef struct Entry{
    char name[4];
    char ext[2];
    uint8_t attribute; // 1 byte
    uint16_t create_time; // 2 bytes
    uint16_t create_date;
    uint16_t last_access;
    uint16_t modified_time;
    uint16_t modified_date;
    uint16_t start_block;
    uint32_t size;
}Entry;

typedef struct RootDir
{
    // list of entries
    Entry* entries;
}RootDir;

typedef struct Volume
{
    char name[NAME_MAX]; /*identifer for the virtualdisk*/ 
    char img[PATH_MAX]; /*disk file's file pointer*/
    int state;
    unsigned int used_size;
    struct VolumeHeader *vbr; /*header*/
    struct FAT *fat; /*pointer for FAT*/
    struct RootDir *root;
    // struct Data_blub; /*beginning of the data region*/
}Volume;

typedef struct VolumeHeader{
    uint16_t num_fats; // 12 bytes, number of fat
    uint16_t root_entries; //2 bytes,number of root entrie 
}VolumeHeader;

// Data structures
static FAT fat;
static SuperBlock superBlock;
static Entry entry;
static Volume volume;
static VolumeHeader volumeHeader;

// Functions
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