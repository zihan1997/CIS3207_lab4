#ifndef _FAT_H
#define _FAT_H

// FAT table
typedef struct FAT
{
    unsigned int total_blocks;
    unsigned int used_blocks;
    int* map;
}FAT;

// Functions
int make_fs(char* disk_name);
int mount_fs(char *disk_name);
int umount_fs(char *disk_name);;


#endif