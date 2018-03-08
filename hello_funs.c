#include "hello_header.h"

//
// Created by vyam on 28/1/18.
//

//int stack_top = 98; // magic number?


int write_disk_block(long offset, block *buf)
{
    printf("write disk block called! %ld\n",offset);    

    // seek to offset
    if( fseek(mem_fil,offset,SEEK_SET) != 0)
    {
        printf("fseek error in write_disk_block! \n");
        return -1;
    }

    // write it back
    if( fwrite(buf, sizeof(block), 1, mem_fil) != 1)
    {
        printf("fwrite error in write_disk_block! \n");
        return -1;
    
    }    
    return 1; // write success!
}





int read_disk_block(long offset, block *buf)
{
    printf("read disk block called! %ld\n",offset);
    // seek to offset
    if( fseek(mem_fil,offset,SEEK_SET) != 0)
    {
        printf("fseek error in read_disk_block! \n");
        return -1;
    }
    // read the struct
    if( fread(buf, sizeof(block), 1, mem_fil) != 1)
    {
        printf("fread error in read_disk_block! \n");
        return -1;
    
    }
    return 1; // read success!
}


int write_disk_inode(struct inode *buf)
{
    // working with the assumption that sizeof buff is size of data block, manage that in read and write!

    // seek to offset
    long offset = buf->i_num;
    offset *= sizeof(struct inode);
    if( fseek(mem_fil,offset  + INODE_OFFSET,SEEK_SET) != 0)
    {
        printf("fseek error in write_disk_block! \n");
        return -1;
    }

    // write it back
    if( fwrite(buf, sizeof(struct inode), 1, mem_fil) != 1)
    {
        printf("fwrite error in write_disk_block! \n");
        return -1;

    }

    printf("write_disk_inode: inode %s write success\n", buf->name);
    return 1; // write success!
}


int read_disk_inode(long offset, struct inode *buf)
{
    // seek to offset
    if( fseek(mem_fil,offset,SEEK_SET) != 0)
    {
        printf("fseek error in read_disk_block! \n");
        return -1;
    }
    // read the struct
    //printf("kdjskflds\n");
    if( fread(buf, sizeof(struct inode), 1, mem_fil) != 1)
    {   
        printf("fread error in read_disk_block! \n");
        return -1;

    }
    //printf("kdjskflds\n");

    return 1; // read success!
}

int update_inodes_list(int option, long *buf) {

    int top;

    if( fseek(mem_fil,STACK_TOP,SEEK_SET) != 0)
    {
        printf("fseek error in read_disk_block! \n");
        return -1;
    }
    if( fread(&top, sizeof(int), 1, mem_fil) != 1)
    {
        printf("fread error in read_disk_block! \n");
        return -1;
    }

    printf("Update called Top: %d\n", top);

    if( fseek(mem_fil,INODES_LIST+(sizeof(long)*top),SEEK_SET) != 0)
    {
        printf("fseek error in read_disk_block! \n");
        return -1;
    }
    printf("Update called\n");

    if( option == 1){
        if(fread(buf, sizeof(long), 1, mem_fil) != 1) {
            printf("fread error in read_disk_block! \n");
            return -1;
        }
        else {
            //fwrite top--
            top--;
            if( fseek(mem_fil,STACK_TOP,SEEK_SET) != 0)
            {
                printf("fseek error in read_disk_block! \n");
                return -1;
            }
            if( fwrite(&top, sizeof(int), 1, mem_fil) != 1)
            {
                printf("fread error in read_disk_block! \n");
                return -1;
            }

        }
    }
    else {
        //fwrite top++
        //seek to top
        //fwrite buf
        // printf("Unlink called!\n");
        if( fseek(mem_fil,STACK_TOP,SEEK_SET) != 0)
        {
            printf("fseek error in write_disk_block! \n");
            return -1;
        }
       // printf("Unlink called!\n");
        // write it back
        printf("Update called\n");
        top++;
        if( fwrite(&top, sizeof(int), 1, mem_fil) != 1)
        {
            printf("fwrite error in write_disk_block! \n");
            return -1;

        }
      //  printf("Unlink called!\n");
        printf("Update called\n");
        if( fseek(mem_fil,INODES_LIST+(sizeof(long)*top),SEEK_SET) != 0)
        {
            printf("fseek error in write_disk_block! \n");
            return -1;
        }
       // printf("Unlink called!\n");
        printf("Update calledssssss\n");
        if( fwrite(buf, sizeof(long), 1, mem_fil) != 1)
        {
            printf("fwrite error in write_disk_block! \n");
            return -1;

        }
        printf("Update calledssssss\n");

    }
    printf("ssssssUnlink calledsfsdfsdd!\n");


}

long get_free_block()
{
    printf("get free block called! free_blk: %ld\n",free_blks);    
    if (free_blks == -1)
    {
        printf("Couldn't find free blocks! in get_free_block");
        return -1;
    }
    block *data_block = malloc(sizeof(block));
    
    long ret_val = free_blks;
    read_disk_block(free_blks, data_block);      
    // update the new free block head to it's next    
    free_blks = data_block->next;
    fseek(mem_fil,0,SEEK_SET);
    fprintf(mem_fil, "%ld",free_blks);
    // set the new block's next to -1, reuses the data_block 
    data_block->next = -1;
    write_disk_block(ret_val,data_block); // setting the new block's next to -1
    free(data_block);
    
    fseek(mem_fil,0,SEEK_SET);
    long temp;    
    fscanf(mem_fil, "%ld",&temp);
    printf("Free_blks on disk: %ld",temp); 

    return ret_val;
}

int init_storage()
{
    FILE *file_pointer;
    /*------------------------ DATA SECTION ---------------------------------------------*/
    mem_fil = fopen(FILE_NAME, "r+b");
    if( mem_fil == NULL) //file does not exist, make and populate a new one
    {

        mem_fil = fopen(FILE_NAME, "w+b");

        printf("Initializing storage..\n");

        int file_status = fseek(mem_fil,DATA_OFFSET,SEEK_SET); // seek to the start of the data section
        if(file_status != 0)
        {

            if(mem_fil == NULL)
            {
                printf("Could not create a mem_fil!\n");
                return -1;
            }


            int file_status = fseek(mem_fil,DATA_OFFSET,SEEK_SET); // seek to the start of the data section
            if(file_status != 0)
            {

                printf("Could not Seek in file, in init!\n");
                return -1;
            }

            free_blks = DATA_OFFSET; // all blocks are free
            fseek(mem_fil,FREE_BLK,SEEK_SET);
            fprintf(mem_fil,"%ld",free_blks);
            fseek(mem_fil,DATA_OFFSET,SEEK_SET);
            printf("Could not Seek in file, in init!\n");
            return -1;
        }

        free_blks = DATA_OFFSET; // all blocks are free
        fseek(mem_fil,FREE_BLK,SEEK_SET);
        fprintf(mem_fil, "%ld", free_blks);
        fseek(mem_fil,DATA_OFFSET,SEEK_SET);

        printf("Initializing storage..\n");
        
        int mem_size = NUM_BLKS * BLK_SIZE;
        int i = 0; //local counter

        block fil_block;
        int write_status = 0; // status flag
        for(i;i<NUM_BLKS;i++)
        {
            fil_block.next = ftell(mem_fil) + sizeof(fil_block); // points to next data block
            write_status = fwrite(&fil_block,sizeof(fil_block),1, mem_fil);
            if(write_status != 1)
            {
                printf("fwrite failed! in init\n");
                return -1;
            }
        }

        file_status = fseek(mem_fil, INODE_OFFSET, SEEK_SET); // seek to the start of the data section
        if(file_status != 0)
        {
            printf("Could not Seek in file, in init!\n");
            return -1;
        }

        printf("Populating inode block.....\n");

        struct inode fil_inode;
        write_status = 0; // status flag

        for(i = 0;i<NUM_INODES;i++)
        {
            fil_inode.i_num = i;
            write_status = fwrite(&fil_inode,sizeof(fil_inode), 1, mem_fil);
            if(write_status != 1)
            {
                printf("fwrite failed! in init\n");
                return -1;
            }
        }

        int j;
        long k = 0;


        for(j = 0, k = DATA_OFFSET - sizeof(struct inode);k > INODE_OFFSET; j++, k -= sizeof(struct inode)) {
            free_inodes_list[j] = k;
            if( fseek(mem_fil,INODES_LIST + (sizeof(long)*j),SEEK_SET) != 0)
            {
                printf("fseek error in write_disk_block! \n");
                return -1;
            }
            printf("Initializing storage..\n");
            if( fwrite(&k, sizeof(long), 1, mem_fil) != 1)
            {
                printf("fwrite error in write_disk_block! \n");
                return -1;
            }
            printf("%ld\n", free_inodes_list[j]);
        }

        if( fseek(mem_fil,STACK_TOP,SEEK_SET) != 0)
        {
            printf("fseek error in write_disk_block! \n");
            return -1;
        }

        int top = NUM_INODES - 2;
        if( fwrite(&top, sizeof(int), 1, mem_fil) != 1)
        {
            printf("fwrite error in write_disk_block! \n");
            return -1;

        }



        root = (struct inode *)malloc(sizeof(struct inode));
        // make root directory
        strcpy(root->name, "/");
        root->i_num = 0;
        root->is_dir = 1;
        root->parent = NULL;
        root->st_nlink = 2;
        root->st_size = 0;
        root->head = -1;
        root->actual_nlink = 2;

        write_disk_inode(root);
        printf("Checking for inode init....%ld\n", INODE_OFFSET);

        struct inode *blah = (struct inode *)malloc(sizeof(struct inode));
        read_disk_inode(INODE_OFFSET, blah);

        printf("Checking for inode init....Read inode from disk\n");
        
    }
    else
    {
        printf("Loading saved fs!\n");
        root = (struct inode *)malloc(sizeof(struct inode));
        read_disk_inode(INODE_OFFSET, root);
        printf("root name: %s\n\n", root->name);
        long old_pos = ftell(mem_fil);
        fseek(mem_fil,FREE_BLK,SEEK_SET);
        fscanf(mem_fil,"%ld",&free_blks);
        fseek(mem_fil,old_pos,SEEK_SET);
        printf("free data blocks start at: %ld\n",free_blks);    
    }



    /*------------------------ INODE SUPER_BLK SECTION -----------------------------------*/
    
    
 





    
    /* ---------------- For Non Persistent -----------------------------------------------
    free_blks = (block *)calloc(sizeof(block), mem_size/sizeof(block));

    block *temp = free_blks; // local variable
    int i=0; // local counter

    printf("1...\n");

    // initialize every block to point to the next block
    while(i++ < NUM_BLKS-2)
    {
        temp->next = temp+1;
        temp = temp+1;
    }
    temp = temp +1;
    temp->next = NULL; // last block in memory can't point to anything

    printf("2...\n");

    if(free_blks == NULL)
    {
        printf("init_storage: Allocation of memory failed!");
        return -1;
    }
    
    */
    printf("Init done!\n");
    return 1;
}

struct inode *resolve_path(char *path, int is_dir) {

    char *string,*found;
    char *dir_array[MAX_LEVEL];
    string = strdup(path);

    printf("1..path: %s\n", path);


    /*Split path by '/' and store in an array
    Testing required*/
    int i = -2; //-2 because strsep returns two garbage values before actual inodes
    while( (found = strsep(&string,"/")) != NULL ) {
        i++;
        //printf("%s", found);
        dir_array[i] = found;
    }
    for(int k = 0; k <= i; k++) {
        printf("Path %d: %s\n", k, dir_array[k]);
    }
    printf("2.. Added all to array\n");

    //printf("dir_array: %s\n", dir_array[0]);
    //printf("i: %d\n", i);
    if(i == 0) {
        inode *t = child_exists(root, dir_array[0]);
        if(t != NULL) return t;//For getattr
        else if(is_dir != 2) return createChild(root, dir_array[0], is_dir);
        else return NULL;
    }


    //Get to the deepest directory in the path. Assumes child_exists works
    struct inode *temp = (struct inode*)malloc(sizeof(struct inode));
    read_disk_inode(INODE_OFFSET, temp);
    int j;

    for(j = 0; j < i; j++) {
        if((temp = child_exists(temp, dir_array[j])) == NULL) {
            printf("Directory %s does not exist\n", dir_array[j]);
            return NULL;
        }
    }

    printf("j: %d d: %s\n", j, dir_array[1]);

    printf("Traversed to last directory\n");

    struct inode *temp1 = temp;
    if((temp = child_exists(temp, dir_array[j])) == NULL){
        printf("Checking last entry: IF\n");
        if(is_dir != 2) return createChild(temp1, dir_array[j], is_dir);
        return NULL;
    }
    else {
        printf("Checking last entry: ELSE temp: %s\n", temp->name);
        return temp;
    }
}

struct inode *child_exists(struct inode *parent, char *child) {
    printf("Checking if child exists\n");
    if(!parent){
        printf("Parent is null\n");
        return NULL;
    }
    if(parent->st_nlink < 3 || parent->is_dir != 1) {
        printf("child_exists: Not a directory or directory empty: %s\n", parent->name);
        return NULL;
    }


    struct inode* temp = (struct inode *)malloc(sizeof(struct inode));

    struct inode* t1 = (struct inode *)malloc(sizeof(struct inode));
    read_disk_inode(parent->chls[0], t1);



//    printf("temp in ex: %s\n", temp->name);
    printf("lksdjflsd %d\n", parent->is_dir);
    printf("Checking for child: %s in parent %s\n", child, parent->name);

    for(int i = 0; i < parent->st_nlink - 2; i++){
        //printf("blyat %s\n", temp->name);
        read_disk_inode((parent->chls[i]) * sizeof(struct inode) + INODE_OFFSET, temp);
        printf("blahablyat %s\n", temp->name);
        if(strcmp(strdup(temp->name), strdup(child)) == 0){
            printf("Child found\n");
            return temp;
        }
        printf("%d\n", i);
    }
    printf("Child not found\n");
    return NULL;
}


struct inode *createChild(struct inode *parent, char *child, int is_dir) {
    struct inode *temp = (struct inode *)malloc(sizeof(struct inode));
    //printf("1..\n");
    long top;
    update_inodes_list(1, &top);
    printf("create_child called parent: %s child: %s\n", parent->name, child);
    printf("kdjshfks%d\n", top);
    read_disk_inode(top, temp);
    strcpy(temp->name, child);
    temp->parent = parent;

    temp->pt = (parent->i_num * sizeof(struct inode)) + INODE_OFFSET;

    printf("%s\n", temp->name);

    temp->is_dir = is_dir;

    temp->st_nlink = (is_dir == 1)?2:1;
    temp->actual_nlink = temp->st_nlink;

    parent->chls[parent->st_nlink - 2] = temp->i_num;

    temp->st_size = 0;
    temp->head = -1;
    temp->st_blocks = 0;
    parent->children[parent->st_nlink - 2] = temp;



//    stack_top--;
    parent->st_nlink += 1;

    if(is_dir) parent->actual_nlink += 1;

    write_disk_inode(parent);
    write_disk_inode(temp);

    return temp;
}

int unlink_inode(struct inode *in) {


    struct inode *parent = (struct inode *)malloc(sizeof(struct inode));
    read_disk_inode(in->pt, parent);
    printf("Unlink called on parent: %s with child %s\n", parent->name, in->name);
    for(int i = 0; i < parent->st_nlink - 2; i++){
        if(parent->chls[i] == in->i_num) {
            parent->chls[i] = parent->chls[parent->st_nlink-3];
            parent->st_nlink -= 1;
            printf("Unlink called!\n");
        }
    }
    printf("Unlink called!\n");
    long temp = (in->i_num * sizeof(struct inode)) + INODE_OFFSET;
    update_inodes_list(2, &temp);
    printf("Unlink calledsfsdfsdd!\n");
    write_disk_inode(parent);
    printf("Unlink called!\n");
    return 0;
}
