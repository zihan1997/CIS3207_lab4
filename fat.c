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
    superBlock.data_blocks = 11;
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
    rootDir.num_entries = 0;
    rootDir.entries = NULL;

    // discriptors
    discriptors.size = 0;
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
int umount_fs(char *disk_name){
    if(!disk_name){
        fprintf(stdout, "umount_fs: disk_name invalid\n");
        return -1;
    }
    // close disk
    close_disk();

    //
    return 0;
}

// find the path the file
int find(char path[2][16], int size){
    Entry *one;
    // printf("1st name %s\n", path[0]);
    int index = 0;
    for(int i = 0; i < rootDir.num_entries; i++){
        // printf("name %s --- %s\n", rootDir.entries[i].name, path[0]);
        if( strcmp((rootDir.entries + i)->name, path[0]) == 0){
            one = &(rootDir.entries[i]);
            index = i;
            printf("found %s\n", one->name);
        }
    }
    if(size == 1){
        return index;
    }

    // for size == 2
    for(int i = 0; i < one->num_entries; i++){
        if(strcmp( (one->entries + i)->name, path[1]) == 0){
            one = (one->entries + i);
            return 1000 * index + i;
        }
    }
    return -1;
}

// The file specified by name (in this case, name is a path 
// to the file that is to be opened, including the actual 
// filename part of the path) is opened for reading and writing, 
// and the file descriptor corresponding to this file is returned 
// to the calling function.
int fs_open(char *name){
    // store the path to the file entry
    char path[2][16];

    // strtok the name
    char temp[20];
    strcpy(temp, name);
    char *token = NULL;
    char* rest = temp;
    int num_folder = 0;

    while((token = strtok_r(rest, "/", &rest))){
        strcpy(path[num_folder], token);
        // printf("%s\n", token);
        num_folder+=1;
    }
    // test the 2d array
    // for(int x = 0; x < i; x++){
    //     printf("%s\n", path[x]);
    // }

    // file discriptor <= 63
    if(discriptors.size == 64){
        fprintf(stdout, "file discriptor <= 63\n");
        return -1;
    }
    // File is in the rootDir
    int index = find(path, num_folder);
    int index_1 = -1;
    if(num_folder == 1){
        strcpy(discriptors.file_discs[discriptors.size].file_name 
                , rootDir.entries[index].name);

    }else if(num_folder == 2){ // file in one of dirs of rootdir
        index /= 1000;
        index_1 = index % 1000;
        strcpy(discriptors.file_discs[discriptors.size].file_name 
                , rootDir.entries[index].entries[index_1].name);
    }else{
        fprintf(stdout, "only one level below the root dir\n");
        return -1;
    }
    discriptors.file_discs[discriptors.size].file_disc = discriptors.size;
    discriptors.size += 1;
    
    return 1;
}

// The file descriptor fildes is closed
int fs_close(int fildes){
    if(close(fildes)){
        return -1;
    }else{
        return 0;
    }
}


// This function creates a new file with name name in your 
// file system (name is the path to the file including the 
// name of the file itself). 
int fs_create(char *name){
    if(!name){
        fprintf(stdout, "fs_create: name invalid\n");
        return -1;
    }
    return 1;
}
int fs_delete(char *name);
int fs_mkdir(char *name);
int fs_read(int fildes, void *buf, size_t nbyte);
int fs_write(int fildes, void *buf, size_t nbyte);
int fs_get_filesize(int fildes);
int fs_lseek(int fildes, off_t offset);
int fs_truncate(int fildes, off_t length);


int main(int argc, char const *argv[])
{
    // char* filename = "disk";
    // make_fs(filename);
    // mount_fs(filename);
    // char* filename = "/a/b/disk";
    // fs_open(filename);
    Entry one;
    one.attribute = 1;
    one.num_entries = 2;

    Entry file1 ;
    file1.attribute = 0;
    strcpy(file1.name, "xx");

    Entry file2;
    file2.attribute = 0;
    strcpy(file2.name, "yy");

    rootDir.num_entries = 2;
    rootDir.entries = malloc(2 * sizeof(Entry));
    rootDir.entries[0] = file1;
    rootDir.entries[1] = file2;

    // printf("%s\n", rootDir.entries[0].name);
    char path[2][16];
    strcpy(path[0], "xx");
    path[1][0] = '\0';


    int index = 0;
    Entry third;
    strcpy(third.name, "");
    if((index = find(path, 1)) == 0){
        // printf(">>%s\n", third.name);
        printf(">>%s\n", rootDir.entries[index].name);
    }else{
        puts("wrong");
    }
    return 0;
}
