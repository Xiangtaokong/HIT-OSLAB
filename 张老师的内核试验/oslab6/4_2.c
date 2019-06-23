#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
static int  __init helloinit(void)
{
printk(KERN INFO "hello world!\n"); 
return 0;
}
static void  __exit helloexit(void)
{
printk(KERN_INFO "Goodbye world\n");
}
moduleinti(hettoinit); 
moduleexit(helloexit);
MODULELICENSE("GPL");
