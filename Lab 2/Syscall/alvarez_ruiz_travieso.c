


#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/sched.h>


asmlinkage long sys_alvarez_ruiz_travieso (void)
{
	//current->pid	

	struct task_struct *task;
	
	printk("PID\tState\tParent PID\tCMD\n");

	for_each_process(task)
	{
		char * state;						/* -1 unrunnable, 0 runnable, >0 stopped */
		if(task->state == -1){
			state = "unrunnable";
		}
		else if(task->state == 0){
			state = "runnable";
		}
		else{
			state = "stopped";
		}


   		printk("%d\t%s\t%d\t%s\n", task->pid, state, task->parent->pid, task->comm);
	}
	return 0;
}





