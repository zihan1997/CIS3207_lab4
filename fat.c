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

    // FAT
    fat.total_blocks = 2;
    fat.used_blocks = 2; // structures blocks
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
    rootDir.entries = malloc(sizeof(Entry) * 200);

    // discriptors
    discriptors.size = 0;
    discriptors.next_avail = 0;

    // Volume Header
    vbr = malloc(sizeof(VolumeHeader));
    vbr->num_fats = 2;
    vbr->root_entries = 2;

    // Volume
    disk = malloc(sizeof(Volume));
    strcpy(disk->name, "C");
    strcpy(disk->img, "f");
    disk->used_size = 40960;
    disk->fat = &fat;
    disk->root = &rootDir;
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

    disk = mmap(
        0,
        4096,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        handle,
        0
    );
    if(disk == MAP_FAILED){
        fprintf(stderr, "mmap failed\n");
        return -1;
    }

    return 0;
}

// This function unmounts your file system 
// from a virtual disk with name disk_name.
int umount_fs(char *disk_name){
    if(!disk_name){
        fprintf(stdout, "umount_fs: disk_name invalid\n");
        return -1;
    }
    // free pointers
    free(vbr);
    free(disk);
    // close disk
    close_disk();

    return 0;
}

// find the path the file
int find(char path[2][16], int size){
    Entry *one;
    // printf("1st name %s size is %d\n", path[0], size);
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
        discriptors.file_discs[discriptors.size].index = index;
        strcpy(discriptors.file_discs[discriptors.size].file_name 
                , rootDir.entries[index].name);

    }else if(num_folder == 2){ // file in one of dirs of rootdir
        discriptors.file_discs[discriptors.size].index = index;
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
    discriptors.next_avail = discriptors.size;
    
    return 1;
}

// The file descriptor fildes is closed
int fs_close(int fildes){
    if(fildes > discriptors.size){
        fprintf(stdout, "file dicriptor is not valid\n");
        return -1;
    }
    for(int i = 0; i < discriptors.size; i++){
        if(discriptors.file_discs[i].file_disc == fildes){
            // change file name to null
            strcpy(discriptors.file_discs[i].file_name, NULL);
            // change the file disc to 0
            discriptors.file_discs[i].file_disc = 0;
            // assign next avail one to this one
            discriptors.next_avail = i;
            // decrement the size
            discriptors.size -= 1;
            return 0;
        }
    }
    return -1;
}

void match_name_ext(Entry *one, char* name){
    char* temp = strdup(name);

    char *token = NULL;
    char *rest = temp;
    if((token = strtok_r(rest, ".", &rest)) != NULL ){
        char *name = one->name;
        strcpy(name, token);
        char *ext = one->ext;
        strcpy(ext, rest);

    }
}

// This function creates a new file with name in your 
// file system (name is the path to the file including the 
// name of the file itself). 
int fs_create(char *name){
    if(!name){
        fprintf(stdout, "fs_create: name invalid\n");
        return -1;
    }

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
    // adjust time
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime ( &rawtime );
    
    // puts("find start");
    // int index = find(path, num_folder-1);
    // puts("find end");
    Entry *a;
    if(num_folder == 1){
        a = &rootDir.entries[rootDir.num_entries];
        rootDir.num_entries += 1;
    }else if(num_folder == 2){
        int index = find(path, rootDir.num_entries);
        a = &rootDir.entries[index].entries[rootDir.entries[index].num_entries];
        rootDir.entries[index].num_entries += 1;
    }else{
        fprintf(stdout, "fs_create: one two level\n");
        return -1;
    }
    match_name_ext(a, path[0]);
    a->attribute = 0;
    a->create_time = (timeinfo->tm_hour << 8) + timeinfo->tm_min;
    a->create_date = (timeinfo->tm_mon << 8) + timeinfo->tm_mday;
    a->start_block = -1;
    a->size = 0;
    a->num_entries = 0;
    a->entries = NULL;

    // printf("%s %s\n", rootDir.entries[0].name, rootDir.entries[0].ext);

    return 1;
}

// This function deletes the file with the path and name name 
// from the directory of your file system and frees all data 
// blocks and meta-information that correspond to that file
int fs_delete(char *name){
    if(!name){
        fprintf(stdout, "fs_delete: name invalid\n");
        return -1;
    }
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

    for(int i = 0; i < 64; i++){
        if(strncmp(discriptors.file_discs[i].file_name, 
                    path[num_folder-1],
                    strlen(discriptors.file_discs[i].file_name)) == 0){
            fs_close(discriptors.file_discs[i].file_disc);
        }
    }
    // name earsing
    int index = find(path, num_folder);
    if(num_folder == 1){
        strcpy(rootDir.entries[index].name, "");
    }
    // delete from fat
    for(int i = 0; i < 999; i++){
        if(fat.maps[i].input == rootDir.entries[index].start_block){
            fat.maps[i].input = -1;
            break;
        }
    }
    return 0;
}

// This function attempts to create a directory with the name name. 
int fs_mkdir(char *name){
    if(!name){
        fprintf(stdout, "fs_mkdir: name invalid\n");
        return -1;
    }

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
    // adjust time
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime ( &rawtime );
    
    Entry *one;
    if(num_folder == 1){
        one = &rootDir.entries[rootDir.num_entries];
        rootDir.num_entries += 1;
    }else if(num_folder == 2){
        int index = find(path, num_folder-1);
        one = &rootDir.entries[index].entries[rootDir.entries[index].num_entries];
        rootDir.entries[index].num_entries += 1;
    }else{
        fprintf(stdout, "two level only\n");
        return -1;
    }

    char *name1 = one->name;
    if(num_folder == 1){
        strcpy(name1, path[0]);
    }else if(num_folder == 2){
        strcpy(name1, path[1]);
    }

    one->attribute = 1;
    one->create_time = (timeinfo->tm_hour << 8) + timeinfo->tm_min;
    one->create_date = (timeinfo->tm_mon << 8) + timeinfo->tm_mday;
    one->start_block = -1;
    one->size = 0;
    one->num_entries = 0;
    one->entries = malloc(20 * sizeof(Entry));
    puts("all-done");
    return 0;
}


int fs_read(int fildes, void *buf, size_t nbyte);
int fs_write(int fildes, void *buf, size_t nbyte);
int fs_get_filesize(int fildes);
int fs_lseek(int fildes, off_t offset);
int fs_truncate(int fildes, off_t length);
