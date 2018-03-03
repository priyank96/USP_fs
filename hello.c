/*
 ******************************** Credits *****************************************

  FUSE: Filesystem in Userspace
  FOR THE BASIC PROGRAM STRUCTURE ONLY
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>
  
  
 **********************************************************************************

	linked list approach to block management
 	One block for data, one block to point to next block of data
 	50% overhead !!

 Build and execute:
    gcc -c hello_funs.c && gcc -Wall hello_funs.o hello.c `pkg-config fuse --cflags --libs` -o hello
    mkdir blah
    ./hello -f blah

 To unmount:
    sudo umount -l blah


 */


#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
//#include <zconf.h> only cmake
#include "hello_header.h"


extern struct inode *root;

static int hello_mkdir(const char *path_name, mode_t mode)
{
    /*inode new_dir;

    char path[MAX_PATH_LEN];
    strncpy(path, path_name, MAX_PATH_LEN);

    if (*path_name == NULL)
    {
        printf(stderr, "error: path = NULL during mkdir\n");
        return -1;
    }
    new_dir.i_num = inode_ctr++;
    new_dir.is_dir = 1;
    // new_dir.parent = parse path to get parent
    new_dir.st_nlink =2;
    return 1;

    */
    printf("root: %s\n", root->name);
    char *path = path_name;
    struct inode *temp = resolve_path(path, 1);
    printf("New dir name: %s\n", temp->name);
    return 0;

}

static int hello_getattr(const char *path, struct stat *stbuf) {
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    // file access modes, is wrong have  to fix
    // change all this
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;

    } else if ((strcmp(path, "..") != 0) && (strcmp(path, ".") != 0)) {

        char *string;
        string = strdup(path);
        /*char *dir_array[MAX_LEVEL];
        char *found;
        int i = -2;

        while ((found = strsep(&string, "/")) != NULL) {
            i++;
            printf("%s", found);
            dir_array[i] = found;
        }

        printf("hdsfjdsf: %s\n", dir_array[0]);

        if (i > -1) {
            struct inode *temp = root;
            temp = child_exists(temp, dir_array[0]);
            if (temp == NULL) {
                printf("Blah\n");
                res = -ENOENT;
            } else {
                printf("jhkjhkjhdfsdfsdf\n");
                stbuf->st_mode = (temp->is_dir == 1)?(S_IFDIR| 0755):(S_IFREG | 0444);
                stbuf->st_nlink = temp->st_nlink;
                stbuf->st_size = 0;
                stbuf->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
                stbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
                stbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
                stbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now
            }
        }
        */
            struct inode *temp;// = root;
            temp = resolve_path(string, 2);
            if (temp == NULL) {
                printf("Resolve path in getattr did not work\n");
                res = -ENOENT;
            } else {
                printf("temp: %s\n", temp->name);
                stbuf->st_mode = (temp->is_dir == 1)?(S_IFDIR| 0755):(S_IFREG | 0444);
                stbuf->st_nlink = temp->st_nlink;
                stbuf->st_size = 0;
                stbuf->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
                stbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
                stbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
                stbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now
            }

    }
    else
        res = -ENOENT;
    return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{

    (void) offset;
    (void) fi;

    // this code has to be changed
    /*
    if (strcmp(path, "/") != 0)
        return -ENOENT;
    */

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    if(strcmp(path, "/") == 0 && root->st_nlink > 2) {
        for(int i = 0; i < root->st_nlink - 2; i++){
            //struct inode *temp = root->children[0];
            //printf("Children: %d\n",temp);
            filler(buf, root->children[i]->name, NULL, 0);
        }
    }

    else {
        
    }


    return 0;
}


static int hello_open(const char* path, struct fuse_file_info* fi)
{

    printf("Opened a file! %s\n",path);

    char *a = strdup(path);
/*<<<<<<< Updated upstream
    inode *i = createChild(root, a+1, 0);
    
    printf("Open done: %s\n", i->name);
=======
    //inode *i = createChild(root, a+1, 0);

    //printf("Open done: %s\n", i->name);
>>>>>>> Stashed changes*/

    // make a filehandle and enclose in fi, extract in hello_write and do write
    filehandle *fh = (filehandle *)malloc(sizeof(filehandle));
    if(fh == NULL)
    {
        return -1;
    }
    // Locate the file
    char *hj = strdup(path);
    fh->node = resolve_path(path,0); // 0 because we dont want to create a dir, use this when multi level directories are ready
    //fh->node = createChild(root, hj+1 ,0); // 0 because we dont want to create a dir
    if(fh->node == NULL)
    {
        return -1;
    }    
    fi->fh = fh;    
    return 0;

}

static int hello_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    printf("Created a file %s\n",path);

    return hello_open(path,fi);
}


static int hello_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{

    printf("beginning write!\n");
    block *write_block = malloc(sizeof(block));
    char *hj = strdup(path);
    // does not support negative offset!
    if(offset<0)
    {
        free(write_block);
        return 0;
    }
    // locate the file
    filehandle *fh =  (filehandle *)fi->fh;
    inode *fil =  (inode *)fh->node;        
    
    if(fil == NULL)
    {
        printf("Could not resolve path, in hello_write");
        free(write_block);
        return 0;
    }
    
    //new file, allocate memory and write
    if(fil->head == -1)
    {
        fil->head = get_free_block();
        if(fil->head == -1)
        {
            printf("Didn't get free block! in hello_write");
            free(write_block);
            return -1;
        }
       
    }
    else
    {
        read_disk_block(fil->head,write_block);
    }
    
    // seek to offset
    int write_blk_offset = 0; //offset within a block
    long block_disk_position = fil->head; // where to write modified blocks back
    
    while(offset>0)
    {
        if(offset>sizeof(write_block->data))
        {
            offset -= sizeof(write_block->data);
            
            if(write_block->next == -1)
            {
               write_block->next = get_free_block();
               write_disk_block(block_disk_position,write_block); // update next on disk 
               if(write_block->next == -1)
                {
                    printf("2 Didn't get free block! in hello_write");
                    free(write_block);
                    return -1;
                }

               
            }
            block_disk_position = write_block->next;
            read_disk_block(write_block->next, write_block); // write_block = write_block->next
                    
        }
        else
        {
            write_blk_offset = offset;
            offset = 0;
        }
    }
    
    //start writing from buff
    int blk_ctr;
    while(buf!=NULL)
    {
        blk_ctr=0;
        while(write_blk_offset+blk_ctr<sizeof(write_block->data))
        {
            if(buf == NULL)
            {
                write_block->data[write_blk_offset+blk_ctr] = '\0';
                write_disk_block(block_disk_position,write_block);                
                free(write_block);
                return 0; // done writing, return
            }
            write_block->data[write_blk_offset+blk_ctr] = buf[0];
            buf = buf+1;
            blk_ctr++;
        }
        // run out of block go to next block
        if(write_block->next == -1)
        {
            write_block->next = get_free_block();
            write_disk_block(block_disk_position,write_block); // update next on disk            
            if(write_block->next == -1)
            {
                free(write_block);
                return 0;
            }
        }
        write_blk_offset = 0;
        block_disk_position = write_block->next;
        read_disk_block(write_block->next,write_block);
        
    }
    free(write_block);
    return 0; // never reached
}

int hello_read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
    //Read size bytes from the given file into the buffer buf, beginning offset bytes into the file. See read(2) for full details.
    // Returns the number of bytes transferred,         
    // or 0 if offset was at or beyond the end of the file. Required for any sensible filesystem. 
    filehandle *fh =  (filehandle *)fi->fh;
    inode *fil =  (inode *)fh->node;       
    if(fil == NULL)
    {
        return -1;
    }
    
    // seek to offset
    int read_blk_offset = 0; //offset within a block
    block *read_block = malloc(sizeof(block)); // remember to free this
    if( read_disk_block(fil->head, read_block) != 1)
    {
        printf("read disk block failed in read\n!");
        return -1;
    }
    while(offset>0)
    {
        if(offset>sizeof(read_block->data))
        {
            offset -= sizeof(read_block->data);
            
            if(read_block->next == -1) // -1 means no next block
            {
                return -1; // invalid seek
            }
            if( read_disk_block(read_block->next,read_block) != 1)
            {
                printf(" 2 read disk block failed in read\n!");
                free(read_block);
                return -1;
            }
        }
        else
        {
            read_blk_offset = offset;
            offset = 0;
        }
    }
    
    // begin reading
    char *temp = buf;
    int i = 0; // local counter variable    
    int bytes_read = 0;
    while(size > 0)
    {
        i = 0;
        while(size>0 && i+read_blk_offset < sizeof(read_block->data) )
        {
            buf[0] = read_block->data[i+read_blk_offset];
            buf++;
            i++;
            bytes_read++;
            size--;

        }
        read_blk_offset = 0;
        if( read_disk_block(read_block->next,read_block) != 1)
        {
            free(read_block);
            return bytes_read;
        }
    }
    free(read_block);
    return bytes_read;

}   


static struct fuse_operations hello_oper = {
        .getattr	= hello_getattr,
        .readdir	= hello_readdir,
        .open		= hello_open,
        .read		= hello_read,
        .write      = hello_write,
        .mkdir      = hello_mkdir,
        .create     = hello_create,
        
};

int main(int argc, char *argv[])
{
    if(init_storage()==-1) // initialises storage and creates a root directory
    {
        return -1;
    }

    return fuse_main(argc, argv, &hello_oper, NULL);
}
