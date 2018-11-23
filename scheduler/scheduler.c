
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/desc.h>
#include <asm/pgtable.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/highmem.h>
#include <linux/preempt.h>
#include <asm/topology.h>
#include <linux/kthread.h>
#include <linux/kernel_stat.h>
#include <linux/percpu-defs.h>
#include <linux/smp.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

struct OurTask
{
	int (*codesection)(void);
	int flags;
	long long starttime;
	long long rantime;
	struct OurTask *next;
};
struct OurTask TaskForce;

EXPORT_SYMBOL(TaskForce);



int thread1(void *data)
{
	struct OurTask *temp;
	int res=0;
	int (*fp)(void);
	
	TaskForce.next = NULL;
	TaskForce.codesection =NULL;	
	current->flags = 0x10000000;
	printk("#scheduer : scheduler is running...\n");

	while(1)
	{	
		
		if(TaskForce.next == NULL)
		{
			schedule();
			continue;
		}
		
		msleep(1000);
		printk("#scheduler : new task is detected\n");
		

		fp = TaskForce.next->codesection;
		fp();

		temp = TaskForce.next;
		TaskForce.next = TaskForce.next->next;	
		
		vfree(temp->codesection);
		temp->codesection = NULL;	
		kfree(temp);
		temp = NULL;

	}
	return 0;
}

static int hello_init(void)
{
	struct task_struct *task1;

	task1 = kthread_create(thread1,NULL,"TaskForceScheduler");
	wake_up_process(task1);

	return 0;
}


static void hello_exit(void)
{

	printk("end module\n=========================================================\n");

}


module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");



