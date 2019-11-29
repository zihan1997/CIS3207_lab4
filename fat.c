#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "disk.h"
#include "fat.h"

// This function creates a fresh (and empty) file system 
// on the virtual disk with name disk_name.
int make_fs(char* disk_name){
    assert(make_disk(disk_name) == 0);
    return 1;
}

// This function mounts a file system that is stored 
// on a virtual disk with name disk_name.
int mount_fs(char *disk_name);

// This function unmounts your file system 
// from a virtual disk with name disk_name.
int umount_fs(char *disk_name);

int main(int argc, char const *argv[])
{
    printf("--%d\n", make_fs("C"));
    return 0;
}
