//
// Created by tincho on 26/06/18.
//
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
/*
 *  Prototypes - this would normally go in a .h file
 */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define ERROR -1


static struct file_operations fops = {
    //    .read = device_read,
   .write = device_write,
   .open = device_open,
   .release = device_release
};

struct proc_dir_entry * proc_file;
static char * file_name = "myModuleFile";
static struct timer_list my_timer;
static int Device_Open;

/* ===================================
   ====       TIMER CALLBACK        ====
   =================================== */
void my_timer_callback( unsigned long data )
{
  printk( "my_timer_callback called (%ld).\n", jiffies );
}


/* ===================================
   ====       INIT MODULE        ====
   =================================== */
int init_module(void){
    /*Initialize globals variables and structs */
    Device_Open = 0;

    /* Create /proc file asociated */
    proc_file = proc_create(file_name,0666,NULL,&fops);

    /* registra el timer */
    setup_timer( &my_timer, my_timer_callback, 0 );

    printk(KERN_INFO "timerMod loaded\n");

    return SUCCESS;
}


/* =====================================
   ====       CLEANUP MODULE        ====
   ===================================== */
#define BUF_LEN 10

void cleanup_module(void)
{
    int ret;
    /* try to clear timer */
    ret = del_timer( &my_timer );
    if (ret) printk(KERN_ERR "Error in mod_timer\n");

    /* remove proc file */
    remove_proc_entry(file_name,NULL);

    printk(KERN_INFO "timerMod removed\n");
}


/* ===================================
   ====       DEVICE WRITE        ====
   =================================== */
static ssize_t device_write(struct file *file, const char __user * usr_buffer,
                                    size_t length,loff_t * offset)
{
    char kern_buffer[BUF_LEN]={0,0,0,0,0,0,0,0,0,0};
    int time_ms=0;
    int ret;
    /* prevent buffer overflow */
    if(length > BUF_LEN){
        printk(KERN_ERR "String is too large: ");
        return ERROR;
    }
    
    /* copy user time in ms and convert to int */
    if(copy_from_user(kern_buffer,usr_buffer,length)!=SUCCESS){
        printk(KERN_ERR "Error copying from user");
        return ERROR;
    }
    if (kstrtoint(kern_buffer,10,&time_ms) != SUCCESS){
        printk(KERN_ERR "Error converting to number %s",kern_buffer);
        return ERROR;
    };

    /* try to ativate timer */
    ret = mod_timer( &my_timer, jiffies + msecs_to_jiffies(time_ms) );
    if (ret) printk("Error in mod_timer\n");

    printk(KERN_INFO "timer expires in %d",time_ms);
    return length;

}


/* ===================================
   ====       DEVICE OPEN        =====
   =================================== */
static int device_open(struct inode *inode, struct file *file)
{
    if (Device_Open)
        return -EBUSY;

    Device_Open++;
    try_module_get(THIS_MODULE);

    return SUCCESS;
}


/* =====================================
   ====       DEVICE RELEASE        ====
   ===================================== */
static int device_release(struct inode *inode, struct file *file)
{
    Device_Open=(Device_Open >0)? Device_Open-1 : Device_Open;      /* We're now ready for our next caller */

    /* 
     * Decrement the usage count, or else once you opened the file, you'll
     * never get get rid of the module. 
     */
    module_put(THIS_MODULE);

    return 0;
}