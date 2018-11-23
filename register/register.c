
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
#include <linux/slab.h>>
#include <linux/spinlock.h>

struct OurTask
{
	int (*codesection)(void);
	int flags;
	long long starttime;
	long long rantime;
	struct OurTask *next;
};

extern struct OurTask TaskForce;



int test(void)
{

	char message[] = "1st task is running\n";

	 __asm__ volatile(
	   
		"push %%ebp ;"
		"mov %%esp, %%ebp ;"
		
		"mov $100,%%esi ;"
	   
		//you have to back up the function address
		"push %0 ; "
		"push %1 ; "
		
	"mystart:;"

		"mov -8(%%ebp), %%eax ;" //get message address which I saved in stack 
      		"push %%eax ;" //pass argument of printk
	      
	        "call call_printk; " 
		"jmp addtional_work;"

	   "call_printk: ;"
 
		"mov -4(%%ebp) ,%%eax ;" //get function address which I saved in stack
	        "jmp *%%eax ;"
	    
	   "addtional_work: ;"
		"add $4, %%esp ;" //remove printk argument 

	      	//loop update
		"dec %%esi;"
		"cmp $0,%%esi;"
		"jne mystart;"

		"leave ;"

		 : 
		 : "r" (printk), "r"(message)
		 : "eax", "esi"
	      );

}



static int hello_init(void)
{
	struct OurTask *header = &TaskForce;
	int (*p)(void);


	
	while(header->next != NULL)
		header = header->next;
		
	header->next = kmalloc(sizeof(struct OurTask),GFP_KERNEL);
		
	printk("before in register: %x\n",header->next->codesection);
	p = __vmalloc(4096,GFP_KERNEL,PAGE_KERNEL_EXEC);
	memcpy(p,test,4096);

	header->next->codesection = (int (*)(void))p;
	header->next->next = NULL;	



	printk("after in register: %x\n",header->next->codesection);
	
	return 0;
}


static void hello_exit(void)
{

	printk("end module\n=========================================================\n");

}


module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");



