#include "disk.h"
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
        fprintf(stderr, "make_disk: invalid file name\n");
        return -1;
    }
    // create empty virtual disk
    if(make_disk(disk_name) != 0){
        return 0;
    }
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
    printf("%lu\n", sizeof(int));

    uint8_t hour;
    get_hour(&hour);
    printf("hour %d\n", hour);
    uint8_t min;
    get_min(&min);
    printf("min %d\n", min);
    return 0;
}
