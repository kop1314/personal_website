/**
* Finding Filesystems Lab
* CS 241 - Fall 2018
*/

#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#define DBS (16 * KILOBYTE)

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string, "Free blocks: %zd\n"
                            "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    // Thar she blows!
  inode *i = get_inode(fs, path);
  if(!i)
    {
      struct stat buffer;
      if(minixfs_stat(fs, path, &buffer) == -1)
	{
	  errno = ENOENT;
	}
      return -1;
    }
  else
    {
      uint16_t mode = i -> mode;
      uint16_t type = mode >> RWX_BITS_NUMBER;
      mode = new_permissions & 0x1ff;
      mode = mode | (type << RWX_BITS_NUMBER);
      i -> mode = mode;

      if(clock_gettime(CLOCK_REALTIME, &(i -> ctim)) == 0)
	{
	  return 0;
	}
      else
	{
	  return -1;
	}
    }
    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    // Land ahoy!
  inode* i = get_inode(fs, path);
  if(!i)
    {
      struct stat buffer;
      if(minixfs_stat(fs, path, &buffer) == -1)
	{
	  errno = ENOENT;
	}
      return -1;
    }
  else
    {
      if(owner != ((uid_t) -1))
	{
	  i -> uid = owner;
	}
      if(group != ((gid_t) -1))
	{
	  i -> gid = group;
	}
      
      if(clock_gettime(CLOCK_REALTIME, &(i -> ctim)) == 0)
	{
	  return 0;
	}
      else
	{
	  return -1;
	}
    }
    return 0;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    // Land ahoy!

  if(*path == '\0')
    {
      return fs -> inode_root;
      
    }
  if(*path != '/')
    {
      return NULL;
    }
  /*
  char *path_cpy = strdup(path);
  char *tok = strtok(path_cpy, "/");
  
  if(valid_filename(path) != 1)
    {
      return NULL;
    }

  minixfs_dirent d;
  d.inode_number = first_unused_inode(fs);
  inode* parent = parent_directory(fs, path, d.name);
  */
    return NULL;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
  (void)fs;
  if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
    //uint64_t free = 0;
      size_t used = 0;
      uint64_t size = fs -> meta -> dblock_count;
      for(uint64_t i = 0; i < size; i++)
	{
	  if(GET_DATA_MAP(fs -> meta)[i] == 1)
	    {
	      used ++;
	    }
	}
      char *str = block_info_string(used);
      //sprintf(str, "Free blocks: %" PRIu64 "\nUsed blocks: %" PRIu64 "\n", free, used);

      //read
      size_t size_of_str = strlen(str);
      if(*off > (long)size_of_str)
	{
	  return 0;
	  
	}

      size_t r_byte = count;
      if(r_byte > (size_of_str - *off))
	{
	  r_byte = (size_of_str - *off);
	}

      memcpy(buf, str, r_byte);
      *off += r_byte;
      return r_byte;
      
    }
    // TODO implement your own virtual file here
    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    // X marks the spot
  unsigned long c_block = (count + *off) / DBS;
  unsigned long r_block = (count + *off) % DBS;
  if(r_block != 0)
    {
      c_block ++;
    }
  if(c_block > (NUM_DIRECT_INODES + NUM_INDIRECT_INODES))
    {
      errno = ENOSPC;
      return -1;
    }
  if(minixfs_min_blockcount(fs, path, c_block) == -1)
    {
      errno = ENOSPC;
      return -1;
    }

  inode* i = get_inode(fs, path);
  size_t o_count = count;
  size_t done_byte = 0;
  int data_block_index = *off / DBS;
  if(data_block_index >= NUM_DIRECT_INODES)
    {
      int r_byte = *off - NUM_DIRECT_INODES * DBS;
      int indir_offset = r_byte % DBS;
      int indir_index = r_byte / DBS;
      //int done_byte = 0;
      data_block indir = fs -> data_root[i -> indirect];

      while(indir_index < (int)NUM_INDIRECT_INODES)
	{
	  int b_index = indir.data[indir_index * 4];
	  int w_byte = DBS - indir_offset;
	  if(count <= DBS)
	    {
	      memcpy(&(fs -> data_root[b_index].data[indir_offset]), buf + done_byte, count);
	      done_byte += count;
	      count -= count;
	      break;
	    }
	  else
	    {
	      memcpy(&(fs -> data_root[b_index].data[indir_offset]), buf + done_byte, (size_t)w_byte);
	      done_byte += w_byte;
	      count -= w_byte;
	      indir_offset = 0;
	    }
	  indir_index++;
	}

    }
  else
    {
      int dir_offset = *off % DBS;
      while(data_block_index < (int)NUM_DIRECT_INODES)
	{
	  int w_byte = DBS - dir_offset;
	  if(count > DBS)
	    {
	      memcpy(&(fs -> data_root[i -> direct[data_block_index]].data[dir_offset]), buf + done_byte, (size_t)w_byte);
	      done_byte += w_byte;
	      count -= w_byte;
	      dir_offset = 0;
	      
	    }
	  else
	    {
	      memcpy(&(fs -> data_root[i -> direct[data_block_index]].data[dir_offset]), buf + done_byte, count);
	      done_byte += w_byte;
	      count -= count;
	      dir_offset = 0;
	      break;
	    }

	  data_block_index++;
	}

      if(count > 0)
	{
	  data_block indir = fs -> data_root[i -> indirect];
	  int index = 0;
	  while(index < (int)NUM_INDIRECT_INODES)
	    {
	      int w_byte = DBS - dir_offset;
	      int b_index = indir.data[index * 4];
	      if(count > DBS)
		{
		  memcpy(&(fs -> data_root[b_index].data[0]), buf + done_byte, (size_t)w_byte);
		  done_byte += w_byte;
		  count -= w_byte;
		}
	      else
		{
		  memcpy(&(fs -> data_root[b_index].data[0]), buf + done_byte, count);
		  done_byte += count;
		  count -= count;
		  break;
		}
	      
	      index++;
	    }
	}
      
    }

  
  if((*off + o_count) > i -> size)
    {
      i -> size = (*off + o_count);
    }

  if(clock_gettime(CLOCK_REALTIME, &i -> mtim) == -1 || clock_gettime(CLOCK_REALTIME, &i -> atim) == -1)
    {
      return -1;
    }

  *off += done_byte;
  return done_byte;;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);
    // 'ere be treasure!

    inode* i = get_inode(fs, path);
    if(!i)
      {
	errno = ENOENT;
	return -1;
      }

    if(*off > (long)(i -> size))
      {
	return 0;
      }

    int data_block_index = *off / DBS;
    int done_byte = 0;
    size_t r_byte = count;
    if(r_byte > (i -> size - *off))
      {
	r_byte = (i -> size - *off);
      }

    if(data_block_index >= NUM_DIRECT_INODES)
      {
	
	int rm_byte = *off - NUM_DIRECT_INODES * DBS;
	int indir_block_index = rm_byte / DBS;
	int indir_block_offset = rm_byte % DBS;
	data_block indir = fs -> data_root[i -> indirect];

	int index = indir_block_index;
	while(index < (int)NUM_INDIRECT_INODES)
	  {
	    int block_index = indir.data[index * 4];
	    int read_byte = DBS - indir_block_offset;
	    if(r_byte > DBS)
	      {
		memcpy(buf + done_byte, &(fs -> data_root[block_index].data[indir_block_offset]), (size_t)read_byte);
		done_byte += read_byte;
		r_byte -= (size_t)read_byte;
		indir_block_offset = 0;
	   
	      }
	    else
	      {
		memcpy(buf + done_byte, &(fs -> data_root[block_index].data[indir_block_offset]), r_byte);
		done_byte += r_byte;
		r_byte -= r_byte;
		break;
	      }
	    index++;
	  }
      }
    else
      {
	
	int byte_offset = *off % DBS;
	//data_block indir = fs -> data_root[i -> indirect];

	//int index = indir_block_index;
	while(data_block_index < (int)NUM_DIRECT_INODES)
	  {
	    
	    int read_byte = DBS - byte_offset;
	    if(r_byte > DBS)
	      {
		memcpy(buf + done_byte, &(fs -> data_root[i -> direct[data_block_index]].data[byte_offset]), (size_t)read_byte);
		done_byte += read_byte;
		r_byte -= (size_t)read_byte;
		byte_offset = 0;
	      }
	    else
	      {
		memcpy(buf + done_byte, &(fs -> data_root[i -> direct[data_block_index]].data[byte_offset]), r_byte);
		done_byte += r_byte;
		r_byte -= r_byte;
		byte_offset = 0;
		break;
	      }
	    data_block_index++;
	  }
	
	if(r_byte > 0)
	  {
	    data_block indir = fs -> data_root[i -> indirect];
	    int index = 0;
	    while(index < (int)NUM_INDIRECT_INODES)
	      {
		int read_byte = DBS;
		int block_index = indir.data[index * 4];
		if(r_byte > DBS)
		  {
		    memcpy(buf + done_byte, &(fs -> data_root[block_index].data[0]), (size_t)read_byte);
		    done_byte += read_byte;
		    r_byte -= (size_t)read_byte;
		  }
		else
		  {
		    memcpy(buf + done_byte, &(fs -> data_root[block_index].data[0]), r_byte);
		    done_byte += r_byte;
		    r_byte -= r_byte;
		    break;
		  }
		index++;
	      }
	  }
      }

    if(clock_gettime(CLOCK_REALTIME, &i -> atim) == -1)
      {
	return -1;
      }

    *off += done_byte;
    return done_byte;;
}
