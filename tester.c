#include <stdio.h>
#include "fat.c"

int main(int argc, char const *argv[])
{
    printf("Disk name is c\n");
    char disk_name[2] = "c";

    printf("make file system\n");
    make_fs(disk_name);

    printf("mount file system\n");
    mount_fs(disk_name);

    printf("create file\n");
    char file1[20] = "/a.txt";
    fs_create(file1);
    printf(">>> check %s.%s\n", rootDir.entries[0].name, rootDir.entries[0].ext);

    printf("create folder\n");
    char folder1[20] = "/folder";
    fs_mkdir(folder1);
    printf(">>> check folder name: %s\n", rootDir.entries[1].name);

    printf("write in file\n");
    fs_open("/a.txt");
    fs_write(0, "hello world\n", 12);

    printf("read file\n");
    char content[13];
    fs_read(0, content, 13 );

    printf("close discriptor\n");
    fs_close(0);
    return 0;
}
