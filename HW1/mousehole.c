#include <linux/syscalls.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/unistd.h>
#include <linux/sched.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");

char filepath[128] = {"0x0,"} ;
int  uid = 100;
int  uid2 = 100;
int  uid3 = 100;
int  uid4 = 100;
int context = 0;
int context2 = 0;
void ** sctable ;
asmlinkage int (*orig_sys_open)(const char __user * filename, int flags, umode_t mode) ;
asmlinkage int (*orig_sys_kill)(pid_t pid, int sig) ;

asmlinkage int mousehole_sys_open(const char __user * filename, int flags, umode_t mode)
{	printk("%d/%d",context,context2);
	if(context2==1)
	{
		char fname[256] ;
		copy_from_user(fname,filename,256) ;
		if (filepath[0] != 0x0 && strstr(fname, filepath) != NULL) {
			printk("No authorize to read the file.");
			return orig_sys_open(NULL,flags,mode);
		}
		return orig_sys_open(filename, flags, mode) ;
	}
	else{
		return orig_sys_open(filename, flags, mode) ;
	}
}

asmlinkage int mousehole_sys_kill(pid_t pid, int sig)
{
  struct task_struct * t;
	if(context == 1) {
    	for_each_process(t) {
        if(pid == t->pid){
					printk("comm / pid : %s %d\n",t->comm, t->pid);
					kuid_t tempid = t->cred->uid;
					int tempidvalue = tempid.val;
					tempid  = current->cred->uid;
					int currentid   = tempid.val;
					if((tempidvalue == uid2) & (uid2 != currentid))
					{
						printk("no authority to quit the program");
						return orig_sys_kill(0,sig);
					}
				}
			}
	}
    return orig_sys_kill(pid,sig) ;
}



static
int mousehole_proc_open(struct inode *inode, struct file *file) {
	return 0 ;
}

static
int mousehole_proc_release(struct inode *inode, struct file *file) {
	return 0 ;
}

static
ssize_t mousehole_proc_read(struct file *file, char __user *ubuf, size_t size, loff_t *offset)
{
	return 0 ;
}

static
ssize_t mousehole_proc_write(struct file *file, const char __user *ubuf, size_t size, loff_t *offset)
{
	char buf[128] ;

	if (*offset != 0 || size > 128)
		return -EFAULT ;

	if (copy_from_user(buf, ubuf, size))
		return -EFAULT ;

  if(buf[0]!=' ')
  {
	printk("buf[0]:%c",buf[0]);
		if(buf[0]=='!')
		{
			kuid_t tempid = current->cred->uid;
			int currentid = tempid.val;
			printk("%d------%d",uid3,currentid);
			if(currentid==uid3)
			{printk("fuck");
				context2 = 0;
			}
			else{
				printk("No permission!");
			}
		}
		else if(context2==0)
		{
    	sscanf(buf,"%d %s",&uid,filepath);
    	printk("uid, filepath : %d %s",uid,filepath);
			printk("block reading mode\n");
			kuid_t tempid = current->cred->uid;
			uid3 = tempid.val;
			printk("uid3,%d",uid3);
	context2 = 1;

		}
  }
  else {
		if(buf[1]=='e') {
			if(context==1)
			{
				kuid_t tempid = current->cred->uid;
				int currentid = tempid.val;
				if(uid4 == currentid) context = 0;
				else {
					printk("No permission to terminate");
				}
			}
			else{
				context = 0; // exit killing block mode
				printk("prevent killing enable!");
			}
		}
		else
		{
			if(context==0)
			{
				sscanf(buf," %d",&uid2);
				printk("uid2 set in %d\n",uid2);
				context = 1;
				kuid_t tempid2 = current->cred->uid;
				uid4 = tempid2.val;
				printk("prevent killing able");
			}
			else{
				printk("Already working\n");
			}
		}
	}
	*offset = strlen(buf) ;
	return *offset ;
}

static const struct file_operations mousehole_fops = {
	.owner = 	THIS_MODULE,
	.open = 	mousehole_proc_open,
	.read = 	mousehole_proc_read,
	.write = 	mousehole_proc_write,
	.llseek = 	seq_lseek,
	.release = 	mousehole_proc_release,
} ;

static
int __init mousehole_init(void) {
	unsigned int level ;
	pte_t * pte ;

	proc_create("mousehole", S_IRUGO | S_IWUGO, NULL, &mousehole_fops) ;

	sctable = (void *) kallsyms_lookup_name("sys_call_table") ;
	orig_sys_open = sctable[__NR_open] ;
    	orig_sys_kill = sctable[__NR_kill] ;
	pte = lookup_address((unsigned long) sctable, &level) ;
    		kuid_t rootUid = current->cred->uid;
        	uid2 = rootUid.val;
		printk("uid2 set in %d\n",uid2);
	if (pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW ;
    	sctable[__NR_kill] = mousehole_sys_kill ;
	sctable[__NR_open] = mousehole_sys_open ;
	return 0;
}

static
void __exit mousehole_exit(void) {
	unsigned int level ;
	pte_t * pte ;
	remove_proc_entry("mousehole", NULL) ;
  sctable[__NR_kill] = orig_sys_kill ;
	sctable[__NR_open] = orig_sys_open ;
	pte = lookup_address((unsigned long) sctable, &level) ;
	pte->pte = pte->pte &~ _PAGE_RW ;
}

module_init(mousehole_init);
module_exit(mousehole_exit);
