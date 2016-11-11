
#include <linux/sched/prio.h>


#define UNACLOUD_PRIO		141




static inline int unacloud_prio(int prio)
{
	if (prio == UNACLOUD_PRIO)
	{
		return 1;
	}
	return 0;
}




static inline int unacloud_task(struct task_struct *p)
{
	return unacloud_prio(p->prio);
}



