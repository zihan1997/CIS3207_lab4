#include "fat.h"

uint8_t get_hour_mon(uint16_t t){
    // time_t rawtime;
    // struct tm *timeinfo;
    // time(&rawtime);
    // timeinfo = localtime ( &rawtime );
    
    // int x = (timeinfo->tm_hour << 8) + timeinfo->tm_min;
    return t >> 8;
}
uint8_t get_min_day(uint16_t t){

    // int x = (timeinfo->tm_hour << 8) + timeinfo->tm_min;
    return t & 255;
}

void initialStructures(){
    
    // Super block
    superBlock.fat = 3; // Block 3 & 4
    superBlock.fat1 = 5; // Block 5 & 6
    superBlock.root = 7; // Block 7 & 8
    superBlock.data_blocks = 9;
    // Volume Header
    superBlock.volume.vbr.num_fats = 2;
    superBlock.volume.vbr.root_entries = 2;
    // Volume
    strcpy(superBlock.volume.name, "C");
    superBlock.volume.used_size = 32000; // bytes
    superBlock.volume.fat = &fat;
    superBlock.volume.root = &rootDir;

    // FAT
    fat.total_blocks = 16384;
    fat.used_blocks = 8; // structures blocks
    // fat.maps = NULL;

    // Root Dir
    strcpy(rootDir.name, "/");
    strcpy(rootDir.ext, "\0");
    rootDir.attribute = 1;
    
    // adjust time
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime ( &rawtime );
    rootDir.create_time = (timeinfo->tm_hour << 8) + timeinfo->tm_min;
    rootDir.create_date = (timeinfo->tm_mon << 8) + timeinfo->tm_mday;
    rootDir.last_access = rootDir.create_date;
    rootDir.modified_time = 0;
    rootDir.modified_date = 0;
    rootDir.size = 0; // folder no size;
    rootDir.entries = NULL;

}

// This function creates a fresh (and empty) file system 
// on the virtual disk with name disk_name.
int make_fs(char* disk_name){
    // name is invalid
    if(!disk_name){
        fprintf(stderr, "make_fs: invalid file name\n");
        return -1;
    }
    // create empty virtual disk
    if(make_disk(disk_name) != 0){
        return 0;
    }
    initialStructures();
    // printf("variables initialized\n");
    return 0;
}

// This function mounts a file system that is stored 
// on a virtual disk with name disk_name.
int mount_fs(char *disk_name){
    if(!disk_name){
        fprintf(stderr, "make_disk: invalid file name\n");
        return -1;
    }
    // use mmap to map
    open_disk(disk_name);

    void *region = mmap(
        0,
        4096,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        handle,
        0
    );
    if(region == MAP_FAILED){
        fprintf(stderr, "mmap failed\n");
        return -1;
    }

    //
    // block_write(0, "1st try");
    // block_write(1, "2nd try");


    // char data[4096] = "";
    // block_read(1, data);
    // printf("%s\n", data);
    // munmap(region, sizeof(region));

    // puts("end");
    return 0;
}

// This function unmounts your file system 
// from a virtual disk with name disk_name.
int umount_fs(char *disk_name);
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


int main(int argc, char const *argv[])
{
    char* filename = "disk";
    make_fs(filename);
    mount_fs(filename);
    return 0;
}
