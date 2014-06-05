/*

AR.Pwn Hook Program
Copyright 2012-2013 Jeremy Rand, Team SNARC / VECLabs
Hooks the program.elf program from the AR.Drone firmware, and dumps interesting data relating to sensor devices.

"arm-none-linux-gnueabi-gcc.exe" -shared -fPIC -ldl -o libhook.so hook.c
LD_PRELOAD=./libhook.so ./program_backup.elf >./dump.txt &
LD_PRELOAD=./libhook.so ./program_backup.elf >/dev/null &

Currently hooks:
open() for video0/video1
8 mmap() calls for video0/video1
ioctl VIDIOC_DQBUF for video0/video1
ioctl VIDIOC_QUERYBUF for video0/video1
Dump video0/video1 to file in DQBUF hook

Untested:

ToDo:
All calls for navdata


*/

#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <linux/videodev2.h>

// We're overriding open, so rename it in this file
#define open OPEN_WHICH_WE_ARE_OVERRIDING
#include <fcntl.h>
#undef open

#define VIDEO0_BUFFER "/tmp/video0_buffer"
#define VIDEO0_READY "/tmp/video0_ready"

#define VIDEO1_BUFFER "/tmp/video1_buffer"
#define VIDEO1_READY "/tmp/video1_ready"

#define VIDEO0_MARKED_BUFFER "/tmp/video0_marked_buffer"
#define VIDEO0_MARKED_READY "/tmp/video0_marked_ready"

#define VIDEO1_MARKED_BUFFER "/tmp/video1_marked_buffer"
#define VIDEO1_MARKED_READY "/tmp/video1_marked_ready"

int hook_handle_video0 = -1;
//void* hook_buffer_video0 = NULL;

__u32 hook_buffer_offsets_video0[] = {0, 0, 0, 0, 0, 0, 0, 0};
void* hook_buffers_video0[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

int hook_handle_video1 = -1;
//void* hook_buffer_video1 = NULL;

__u32 hook_buffer_offsets_video1[] = {0, 0, 0, 0, 0, 0, 0, 0};
void* hook_buffers_video1[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

int open (__const char *__file, int __oflag, ...)
{
  static int (*libc_open)
             (__const char *__file, int __oflag, ...) = NULL;
  void *handle;
  int open_handle;
  char *error;

  if (!libc_open) {
    handle = dlopen("/lib/libc.so.6",
                    RTLD_LAZY);
    if (!handle) {
      fputs(dlerror(), stderr);
      exit(1);
    }
    libc_open = dlsym(handle, "open");
    if ((error = dlerror()) != NULL) {
      fprintf(stderr, "%s\n", error);
      exit(1);
    }
  }
  
  // custom implementation
  
  // we break permissions here, but it probably makes no difference on the AR.Drone
  mode_t mode = 0777;
  
  /*
  if(__oflag )
  va_list arg_ptr;
  va_start(arg_ptr, 1);
  */

  printf("AR.Pwn hooked open(%s, %d, %d)\n", __file, __oflag, mode);
  open_handle = libc_open(__file, __oflag, mode);
  printf("AR.Pwn open() returned %d\n", open_handle);
  
  if(strcmp(__file, "/dev/video0") == 0)
  {
    printf("AR.Pwn Detected open of video0; saving handle.\n");
	hook_handle_video0 = open_handle;
  }
  else if(strcmp(__file, "/dev/video1") == 0)
  {
    printf("AR.Pwn Detected open of video1; saving handle.\n");
	hook_handle_video1 = open_handle;
  }
  
  return open_handle;
}

void *mmap (void *__addr, size_t __len, int __prot, int __flags, int __fd, __off_t __offset)
{
  static void* (*libc_mmap)
             (void *__addr, size_t __len, int __prot, int __flags, int __fd, __off_t __offset) = NULL;
  void *handle;
  void* mmap_buffer;
  char *error;

  if (!libc_mmap) {
    handle = dlopen("/lib/libc.so.6",
                    RTLD_LAZY);
    if (!handle) {
      fputs(dlerror(), stderr);
      exit(1);
    }
    libc_mmap = dlsym(handle, "mmap");
    if ((error = dlerror()) != NULL) {
      fprintf(stderr, "%s\n", error);
      exit(1);
    }
  }
  
  // custom implementation

  printf("AR.Pwn hooked mmap(%d, %d, %d, %d, %d, %d)\n", __addr, __len, __prot, __flags, __fd, __offset);
  mmap_buffer = libc_mmap(__addr, __len, __prot, __flags, __fd, __offset);
  printf("AR.Pwn mmap() returned %d\n", mmap_buffer);
  
  if(hook_handle_video0 != -1 && __fd == hook_handle_video0)
  {
    int index;
	int found = 0;
	
	for(index = 0; index < 8; index++)
	{
		if(__offset == hook_buffer_offsets_video0[index])
		{
			found = 1;
			break;
		}
	}
	
	if(found)
	{
		printf("AR.Pwn Detected mmap of video0 index %d; saving buffer.\n", index);
		hook_buffers_video0[index] = mmap_buffer;
	}
  }
  else if(hook_handle_video1 != -1 && __fd == hook_handle_video1)
  {
    int index;
	int found = 0;
	
	for(index = 0; index < 8; index++)
	{
		if(__offset == hook_buffer_offsets_video1[index])
		{
			found = 1;
			break;
		}
	}
	
	if(found)
	{
		printf("AR.Pwn Detected mmap of video1 index %d; saving buffer.\n", index);
		hook_buffers_video1[index] = mmap_buffer;
	}
  }
  
  return mmap_buffer;
}

int ioctl(int d, int request, ...)
{
  static int (*libc_ioctl)
             (int d, int request, ...) = NULL;
  void *handle;
  int ioctl_val;
  char *error;

  if (!libc_ioctl) {
    handle = dlopen("/lib/libc.so.6",
                    RTLD_LAZY);
    if (!handle) {
      fputs(dlerror(), stderr);
      exit(1);
    }
    libc_ioctl = dlsym(handle, "ioctl");
    if ((error = dlerror()) != NULL) {
      fprintf(stderr, "%s\n", error);
      exit(1);
    }
  }
  
  // custom implementation
  
  printf("AR.Pwn hooked ioctl(%d, %d, ...)\n", d, request);
  
  // Get the dynamically typed argument
  va_list arguments;
  va_start(arguments, request);
  void* arg = va_arg 
  ( arguments, void* );
  
  // Call libc function
  ioctl_val = libc_ioctl(d, request, arg);
  
  printf("AR.Pwn ioctl() returned %d\n", ioctl_val);
  
  // QUERYBUF ioctl
  if(hook_handle_video0 != -1 && d == hook_handle_video0 && request == VIDIOC_QUERYBUF)
  {
    // Get the index of the buffer
    int index = ((struct v4l2_buffer*) arg)->index;
	
	// Check bounds
    if(index >= 0 && index < 8)
	{
		// Save offset for matching with the mmap
		hook_buffer_offsets_video0[index] = ((struct v4l2_buffer*) arg)->m.offset;
		
		printf("AR.Pwn Detected ioctl QUERYBUF of video0 index %d; saving offset.\n", index);
	}
  }
  else if(hook_handle_video1 != -1 && d == hook_handle_video1 && request == VIDIOC_QUERYBUF)
  {
    int index = ((struct v4l2_buffer*) arg)->index;
	
    if(index >= 0 && index < 8)
	{
		hook_buffer_offsets_video1[index] = ((struct v4l2_buffer*) arg)->m.offset;
		
		printf("AR.Pwn Detected ioctl QUERYBUF of video1 index %d; saving offset.\n", index);
	}
  }
  // DQBUF iotcl
  else if(hook_handle_video0 != -1 && d == hook_handle_video0 && request == VIDIOC_DQBUF)
  {
    int index = ((struct v4l2_buffer*) arg)->index;

    printf("AR.Pwn Detected ioctl DQBUF of video0 index %d; dumping frame.\n", index);
	
	// If Ready Flag is not present, or if buffer is not present
	if( ! (access(VIDEO0_READY, F_OK) != -1) || ! (access(VIDEO0_BUFFER, F_OK) != -1) )
	{
		int buffer_file = open(VIDEO0_BUFFER, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		write(buffer_file, hook_buffers_video0[index], 640*480*3/2);
		close(buffer_file);
		
		int ready_file = open(VIDEO0_READY, O_WRONLY | O_CREAT, 0666);
		close(ready_file);
	}
	
	// If a marked frame is available from the vision code...
	if( (access(VIDEO0_MARKED_READY, F_OK) != -1))
	{
		int buffer_file = open(VIDEO0_MARKED_BUFFER, O_RDONLY);
		read(buffer_file, hook_buffers_video0[index], 640*480*3/2);
		close(buffer_file);
		
		remove(VIDEO0_MARKED_READY);
	}
	
	// start is returned by mmap
	//fwrite(buffers[buf.index].start, image_size, 1, file_fd);
	
	// So what we need is to determine which buffer goes to which index (mmap hook), and then access its start... then we'll have data.
	
  }
  else if(hook_handle_video1 != -1 && d == hook_handle_video1 && request == VIDIOC_DQBUF)
  {
    int index = ((struct v4l2_buffer*) arg)->index;

    printf("AR.Pwn Detected ioctl DQBUF of video1 index %d; dumping frame.\n", index);
	
	// If Ready Flag is not present, or if buffer is not present
	if( ! (access(VIDEO1_READY, F_OK) != -1) || ! (access(VIDEO1_BUFFER, F_OK) != -1) )
	{
		int buffer_file = open(VIDEO1_BUFFER, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		write(buffer_file, hook_buffers_video1[index], 176*144*3/2);
		close(buffer_file);
		
		int ready_file = open(VIDEO1_READY, O_WRONLY | O_CREAT, 0666);
		close(ready_file);
	}
	
	// If a marked frame is available from the vision code...
	if( (access(VIDEO1_MARKED_READY, F_OK) != -1))
	{
		int buffer_file = open(VIDEO1_MARKED_BUFFER, O_RDONLY);
		read(buffer_file, hook_buffers_video1[index], 176*144*3/2);
		close(buffer_file);
		
		remove(VIDEO1_MARKED_READY);
	}
  }
  
  return ioctl_val;
}