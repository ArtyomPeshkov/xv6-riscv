#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"

static struct inode*
create(char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];
  if((dp = nameiparent(path, name)) == 0)
    return 0;

  ilock(dp);
  if((ip = dirlookup(dp, name, 0)) != 0){
    iunlockput(dp);
    ilock(ip);
    if(type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE))
      return ip;
    if(type == T_SYMLINK)
      return ip;
    iunlockput(ip);
    return 0;
  }
  if((ip = ialloc(dp->dev, type)) == 0){
    iunlockput(dp);
    return 0;
  }
  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);
  if(type == T_DIR){  // Create . and .. entries.
    // No ip->nlink++ for ".": avoid cyclic ref count.
    if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      goto fail;
  }
  if(dirlink(dp, name, ip->inum) < 0)
    goto fail;
  if(type == T_DIR){
    // now that success is guaranteed:
    dp->nlink++;  // for ".."
    iupdate(dp);
  }
  iunlockput(dp);

  return ip;

 fail:
  // something went wrong. de-allocate ip.
  ip->nlink = 0;
  iupdate(ip);
  iunlockput(ip);
  iunlockput(dp);
  return 0;
}

uint64 sys_symlink(void)
{  
  char path[MAXPATH], target[MAXPATH];

  int t_len = argstr(0, target, MAXPATH);
  if (t_len < 0 || t_len >= MAXPATH || argstr(1, path, MAXPATH) < 0)
    return -1;
  
  begin_op();

  struct inode *lnk = create(path, T_SYMLINK, 0, 0);

  if(lnk == 0) {
    end_op();
    return -1;
  }

  int res = writei(lnk, 0, (uint64)target, 0, t_len);
  iunlockput(lnk);
  end_op();
  return res < t_len;
}

uint64 sys_readlink() {

  char linkpath[MAXPATH];
  if (argstr(0, linkpath, MAXPATH) < 0)
    return -1;

  uint64 user_buf;
  argaddr(1, &user_buf);

  begin_op(); 

  struct inode *lnk = namei(linkpath);
  if (lnk == 0) {
    end_op();
    return -1;
  }
  if (lnk->type != T_SYMLINK) {
    iput(lnk);
    end_op();
    return -1;
  }

  ilock(lnk);
  int res = readi(lnk, 1, user_buf, 0, lnk->size) ;
  iunlockput(lnk);

  end_op();
  return res <= 0;
}