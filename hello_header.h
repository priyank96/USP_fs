//
// Created by vyam on 28/1/18.
//

#ifndef USP_FS_HELLO_HEADER_H
#define USP_FS_HELLO_HEADER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef char BLK_DATA_TYPE;

#define NUM_INODES 100
#define NUM_DATA_ELEMS 512
#define NUM_BLKS 1000
#define NUM_DATA_ELEMS 512
#define BLK_SIZE sizeof(block)
#define MAX_PATH_LEN 252
#define MAX_CHILDREN 10
#define MAX_LEVEL 10
#define FREE_BLK 0
#define STACK_TOP sizeof(long)
#define INODES_LIST sizeof(int) + STACK_TOP
#define INODE_OFFSET INODES_LIST+sizeof(long)*NUM_INODES
#define DATA_OFFSET (INODE_OFFSET + sizeof(struct inode)*NUM_INODES)
#define FILE_NAME "./memory_fil.dat"
int init_storage();

/*------------------------------------- FILE DESCRIPTOR ----------------------------------------*/
FILE *mem_fil;

/*------------------------------------- STRUCTURES ---------------------------------------------*/
typedef struct block
{
    BLK_DATA_TYPE data[NUM_DATA_ELEMS];
    long next;
}block;


typedef struct inode
{

    char name[50];
    int i_num;
    int is_dir;

    struct inode *parent;
    long pt;

    struct inode *children[MAX_CHILDREN];
    int chls[MAX_CHILDREN];

    int st_size;
    int st_nlink;
    int st_blocks;
    int actual_nlink;
    long head; // address of first block of memory of this file


}inode;

typedef struct filehandle
{
    inode *node;
    int o_flags;
}filehandle;


long free_inodes_list[100];

/*------------------------------------ METHOD PROTOTYPES -------------------------------------*/
int init_storage();
long get_free_block();


struct inode *root;
long free_blks;
static int inode_ctr=0;

struct inode *resolve_path(char *path, int is_dir);
struct inode *child_exists(struct inode *parent, char *child);
struct inode *createChild(struct inode *parent, char *child, int is_dir);
void write_free_blk_disk(long new_val);
int read_disk_block(long offset, block *buf);
int write_disk_block(long offset, block *buf);
int read_disk_inode(long offset, inode *buf);
int write_disk_inode(inode *buf);
#endif //USP_FS_HELLO_HEADER_H
