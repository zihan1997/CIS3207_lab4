#include "fat.c"

int main(int argc, char const *argv[])
{
    puts("start");
    char disk_name[10] = "disk";
    make_fs(disk_name);
    mount_fs(disk_name);

    int flag = 1;
    int option;

    char name[20];
    while(flag == 1){
        printf( "\n"
                "1. create file\n"
                "2. open file\n"
                "3. close file\n"
                "4. make folder\n"
                "5. write file\n"
                "6. read file\n"
                "7. ls\n"
                "-1. quit\n"
                ">> ");
        
        scanf("%d", &option);
        
        switch (option){
        case 1:
            printf("name > ");
            scanf("%s", name);
            fs_create(name);
            break;
        case 2:
            printf("name > ");
            scanf("%s", name);
            fs_open(name);
            break;
        case 3:
            printf("name > ");
            scanf("%s", name);
            fs_delete(name);
            break;
        case 4:
            printf("name > ");
            scanf("%s", name);
            fs_mkdir(name);
            break;
        case 5:
            printf("name > ");
            scanf("%s", name);
            
            int index = 0;
            while(strcpy(discriptors.file_discs[index].file_name, name ) != 0){
                index+=1;
            }
            printf("write in \n>> ");
            char buff[100];
            scanf("%s", buff);
            fs_write(discriptors.file_discs[index].file_disc, buff, sizeof(buff));
        case 6:
            printf("name > ");
            scanf("%s", name);
            
            int index1 = 0;
            while(strcpy(discriptors.file_discs[index1].file_name, name ) != 0){
                index1+=1;
            }
            printf("read in\n ");
            scanf("%s", buff);
            // fs_read(discriptors.file_discs[index1].file_disc, buff, sizeof(buff));
        case 7:
            for(int i = 0; i < rootDir.num_entries; i++){
                printf("%d: %s\n", i, rootDir.entries[i].name);
            }
            break;
            // printf("num_entries: %d\n", rootDir.num_entries);
        case -1:
            // umount_fs(disk_name);
            flag = 0;
            break;
        }
    }
    return 0;
}
