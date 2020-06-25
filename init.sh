#!/bin/bash
FS_TYPE="fs"
MODULE_FILE="fs_mod.ko"
MODULE_NAME="fs_mod"
DISK_FILEPATH="/tmp/fs_disk.img"
MOUNT_DIR="/mnt/$FS_TYPE"

# Initialization must be run as root user
if [[ $EUID -ne 0 ]]; then
   echo "Usage: sudo ./init.sh" 1>&2
   exit 1
fi

# Create 4.0GB disk to mount as filesystem
time dd if=/dev/zero of=$DISK_FILEPATH bs=4096 seek=1048576 count=0

# Compile updates
make

# Load updated kernel module
insmod $MODULE_FILE

# Check filesystem registration
if cat /proc/filesystems | grep $FS_TYPE ; then
	echo "Filesystem is registered in /proc/filesystems"
else
	echo "Warning: Filesystem not registered in /proc/filesystems "
fi

# Mount disk image to directory
mkdir -p $MOUNT_DIR
mount -o loop -t $FS_TYPE $DISK_FILEPATH $MOUNT_DIR

# Display info about ProcFS file
echo "Creating file /proc/fs_proc for kernel-user communication.."
echo "ls -lh /proc/fs_proc: `ls -lh /proc/fs_proc`"
echo "cat /proc/fs_proc: `cat /proc/toyfs_proc`"

# Display dmesg logs
echo "Displaying dmesg logs to verify filesystem integration"
dmesg | tail -50