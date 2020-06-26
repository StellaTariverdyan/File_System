#include <linux/init.h>								// Standard kernel headers
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/sched.h>							//Task_struct
#include <asm/uaccess.h>
#include <asm/current.h>							//Process Info
#include <linux/proc_fs.h>							// For procfs communication Kernel-To-User
#include <linux/seq_file.h>
#include "fs.h"								// User defined includes

#define FS_MAGIC	0x10040203


/*=======================================================
File Operations
=========================================================
*defined in fs.h:1507
- file_operations (Data structure for file operations)
	- .readdr
- fs_read
- fs_write
=========================================================*/


ssize_t fs_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
	printk(KERN_INFO "fs_read (Not functional)\n");
/*
    char *buffer[100];
    int length = 3;
    buffer[0] = (char)file->private_data;
    buffer[1] = '\n';
    buffer[2] = '\0';
    if(*offset > length)
        return 0;
        if(len > length - *offset)
            len = length - *offset;
    //int copy_to_user(void *dst, const void *src, unsigned int size);
    if(copy_to_user(buf, buffer + *offset, len))
            return -1;
    *offset += len;
    return len;
*/
	return 1;
}

ssize_t fs_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
	printk(KERN_INFO fs_write (Not functional)\n");
	return 1;
}

const struct file_operations fs_file_operations = {
	.read  = fs_read,
	.write = fs_write,
};



/*=======================================================
Inode Operations
=========================================================
- fs_get_inode
	- Function should return an inode.
- inode_operations
	- .lookup
	- .create
=========================================================*/
struct dentry *fs_lookup(struct inode *parent, struct dentry *child, unsigned int flags)
{
	printk(KERN_INFO "fs_lookup (Not Functional)\n");
	//In Progress
	return NULL;
}

static int fs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool boo)
{	
	printk(KERN_INFO "fs_create (Not functional)\n");
	//In Progress
	return 1;
}

static struct inode_operations fs_inode_ops = {				//Defined in fs.h:1541
	.lookup = fs_lookup,
	.create = fs_create,
};

struct inode *fs_get_inode(struct super_block *sb, const struct inode *directory, umode_t mode, dev_t dev)
{
	struct inode *inode = new_inode(sb);
	printk(KERN_INFO "Initializing: fs_get_inode\n");
	if(inode){
		inode->i_ino = get_next_ino();					//stores next available inode # into inode->i_ino
		inode_init_owner(inode, directory, mode);			//sets permissions etc
		inode->i_blocks = 10;
		inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
   		switch(mode & S_IFMT) {						//bitwise AND between mode and S_IFMT can extract the file type code from a mode value. That new value should then be compared to the following cases.										
			case S_IFDIR:						//S_IFDIR = file type const of a directory file
                    		inode->i_op = &fs_inode_ops; 		//Inode operations
                    		//inode->i_fop = &fs_directory_operations; 	//simple directory operations
			     	inc_nlink(inode);
			     	break;
			case S_IFREG:						//S_IFREG = non-zero if file is a regular file
                   		 inode->i_op = &fs_inode_ops;
                   		 //inode->i_fop = &fs_file_operations;
										//S_ISLNK = non-zero if file is symbolic link (Non-Fuctional)
			default: printk(KERN_ERR "Not DIR or REG file.\n");
				return 0;
				break;
		} 
	} 
	return inode;
}

/*=======================================================
Super Block
=========================================================
	- fs_fill_superblock
	- Edit: Add Inode Allocation
=========================================================*/

static int fs_fill_superblock(struct super_block *sb, void *data, int silent)
{
	struct inode *inode = NULL;
	printk(KERN_INFO "Initializing: fs_fill_superblock\n");
	
	sb->s_magic = FS_MAGIC;						//identifer for the filesystem type
	inode = fs_get_inode(sb, NULL, S_IFDIR, 0);
	inode->i_op = &fs_inode_ops;						//inode operations table
	//inode->i_fop = &fs_directory_operations;				//file operations table
	sb->s_root = d_make_root(inode);					//creates root inode for superblock ; root directory

	if(!sb->s_root)
	{	
		printk(KERN_ERR "Root Creation has Failed\n");
		return -ENOMEM; 
	}
	return 0;
}
/*=====================================================
File System Types
=======================================================
- dentry *fs_mount
	- Defined in linux/fs/super.c
	- mount_bdev() uses disk instead of prog memory
- fs_kill_superblock
- file_system_type
	- Definitions
- fs_proc_fops
	- fs_proc_show | fs_proc_open
		- /proc filesystem
======================================================*/

static int fs_proc_show(struct seq_file *m, void *v) {
  	seq_printf(m, "Hello from fs+proc!\n");
  	return 0;
}

static int fs_proc_open(struct inode *inode, struct  file *file) {
	printk(KERN_INFO "Initializing: fs_proc_open\n");
	printk(KERN_INFO "Test        :$cat /proc/fs_proc\n");
  	return single_open(file, fs_proc_show, NULL);
}

static struct dentry *fs_mount(struct file_system_type *filesystem_type, int flags, const char *device_name, void *data)
{
	struct dentry *det = mount_bdev(filesystem_type, flags, device_name, data, fs_fill_superblock);	

	if(IS_ERR(det))
		printk(KERN_ERR "Error Mounting File System\n");
	else
		printk(KERN_INFO "Initializing: fs_mount \n");
	return det;
}

static void fs_kill_superblock(struct super_block *sb)
{
	printk(KERN_INFO "Initializing: fs_kill_superblock\n");
	kill_block_super(sb);
	return;
}

struct file_system_type fs = {
	.owner = THIS_MODULE,
	.name = "fs",
	.mount = fs_mount,
	.kill_sb = fs_kill_superblock,
};

static const struct file_operations fs_proc_fops = {
  .owner = THIS_MODULE,
  .open = fs_proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};

/*=======================================================
Register / Unregister
=========================================================
- fs_init
- fs_cleanup
- remove_proc_entry
	- Testing the procfs communication. 
========================================================*/

static int fs_init(void) {
	printk(KERN_INFO "||Registering File System  ||\n\n"); 
	printk(KERN_INFO "Userspace Process:'%s'\n", current->comm); 
	printk(KERN_INFO "Userspace PID:'%d'\n", current->pid); 

	proc_create("fs_proc", 0666, NULL, &fs_proc_fops);		// Create a procfs file for communication
	return register_filesystem(&fs);
}

static void fs_cleanup(void) {
 	printk(KERN_INFO "\n||Unregistering File System||\n");
	
 	remove_proc_entry("fs_proc", NULL);					// Remove the procfs file
 	unregister_filesystem(&fs);
}

module_init(fs_init);
module_exit(fs_cleanup);
