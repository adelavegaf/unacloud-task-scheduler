

#include <linux/latencytop.h>
#include <linux/sched.h>
#include <linux/cpumask.h>
#include <linux/cpuidle.h>
#include <linux/slab.h>
#include <linux/profile.h>
#include <linux/interrupt.h>
#include <linux/mempolicy.h>
#include <linux/migrate.h>
#include <linux/task_work.h>
#include <trace/events/sched.h>

#include "sched.h"

static void update_curr_unacloud(struct rq *rq)
{
	//printk("unacloud: update_curr_unacloud RQ: %d\n", rq->cpu);
	struct task_struct *curr = rq->curr;
	u64 delta_exec;

	if (curr->sched_class != &unacloud_sched_class)
		return;

	delta_exec = rq_clock_task(rq) - curr->se.exec_start;
	if (unlikely((s64)delta_exec <= 0))
		return;

	schedstat_set(curr->se.statistics.exec_max,
		max(curr->se.statistics.exec_max, delta_exec));

	curr->se.sum_exec_runtime += delta_exec;

	curr->se.exec_start = rq_clock_task(rq);
}

static struct task_struct * pick_next_task_unacloud(struct rq *rq, struct task_struct *prev)
{
	if (rq->unacloud_rq.unacloud_task != NULL)
	{
		//printk("unacloud: pick_next_task_unacloud: hay tarea. PID: %d RQ: %d\n", rq->unacloud_rq.unacloud_task->pid, rq->cpu);
		
		if (prev->sched_class == &unacloud_sched_class)
			update_curr_unacloud(rq);
		
		put_prev_task(rq, prev);
		rq->unacloud_rq.unacloud_task->se.exec_start = rq_clock_task(rq);
		return rq->unacloud_rq.unacloud_task;
	}
	else
	{
		//printk("unacloud: pick_next_task_unacloud: No hay tareas PID: %d\n", prev->pid);
		return NULL;
	}
}

static void enqueue_task_unacloud(struct rq *rq, struct task_struct *p, int flags)
{
	if (rq->unacloud_rq.unacloud_task == NULL)
	{
		//printk("unacloud: enqueue_task_unacloud: Puse tarea PID: %d RQ: %d\n", p->pid, rq->cpu);
		rq->unacloud_rq.unacloud_task = p;
		p->se.unacloud_time = 25;
	}
	else
	{
		//printk("unacloud: enqueue_task_unacloud: Esta intentando encolar una tarea adicional PID: %d RQ: %d\n", p->pid, rq->cpu);
	}
}

/*
 * The dequeue_task method is called before nr_running is
 * decreased. We remove the task from the rbtree and
 * update the fair scheduling stats:
 */
static void dequeue_task_unacloud(struct rq *rq, struct task_struct *p, int flags)
{
	if (rq->unacloud_rq.unacloud_task == p)
	{
		update_curr_unacloud(rq);
		//printk("unacloud: dequeue_task_unacloud: va a quitar la tarea PID: %d RQ: %d\n", p->pid, rq->cpu);
		rq->unacloud_rq.unacloud_task = NULL;
	}
	else
	{
		//printk("unacloud: dequeue_task_unacloud: esta intentando desencolar una tarea que no es PID: %d RQ: %d \n", p->pid, rq->cpu);
	}
}

/*
 * sched_yield() is very simple
 *
 * The magic of dealing with the ->skip buddy is in pick_next_entity.
 */
static void yield_task_unacloud(struct rq *rq)
{
	//printk("unacloud: yield_to_task_unacloud RQ: %d\n", rq->cpu);
}

static bool yield_to_task_unacloud(struct rq *rq, struct task_struct *p, bool preempt)
{
	//printk("unacloud: yield_to_task_unacloud PID: %d RQ: %d\n", p->pid, rq->cpu);
	return true;
}

/*
 * Preempt the current task with a newly woken task if needed:
 */
static void check_preempt_wakeup(struct rq *rq, struct task_struct *p, int wake_flags)
{
	//printk("check_preempt_wakeup_unacloud PID: %d RQ: %d\n", p->pid, rq->cpu);
}

/*
 * Account for a descheduled task:
 */
static void put_prev_task_unacloud(struct rq *rq, struct task_struct *prev)
{
	update_curr_unacloud(rq);
	//printk("put_prev_task_unacloud PID: %d RQ: %d\n", prev->pid, rq->cpu);
}


/* 
 * Account for a task changing its policy or group.
 *
 * This routine is mostly called to set cfs_rq->curr field when a task
 * migrates between groups/classes.
 */
static void set_curr_task_unacloud(struct rq *rq)
{
	struct task_struct *p = rq->curr;
	
	//printk("unacloud: set_curr_task_rt START PID: %d on_rt: %d RQ: %d\n", p->pid, p == rq->unacloud_rq.unacloud_task ? 1 : 0, rq->cpu);
	
	p->se.exec_start = rq_clock_task(rq);
	
	//printk("unacloud: set_curr_task_rt END PID: %d on_rt: %d RQ: %d\n", p->pid, p == rq->unacloud_rq.unacloud_task ? 1 : 0, rq->cpu);
}

/*
 * scheduler tick hitting a task of our scheduling class:
 */
static void task_tick_unacloud(struct rq *rq, struct task_struct *curr, int queued)
{
	//printk("unacloud: task_tick_unacloud PID: %d RQ: %d\n", curr->pid, rq->cpu);
	update_curr_unacloud(rq);
	if (curr->policy != SCHED_UNACLOUD)
		return;
	if (--curr->se.unacloud_time)
		return;
	curr->se.unacloud_time = 25;
	resched_curr(rq);
}

/*
 * called on fork with the child task as argument from the parent's context
 *  - child not yet on the tasklist
 *  - preemption disabled
 */
static void task_fork_unacloud(struct task_struct *p)
{
	//printk("unacloud: task_fork_unacloud PID: %d\n", p->pid);
}

/*
 * Priority of the task has changed. Check to see if we preempt
 * the current task.
 */
static void prio_changed_unacloud(struct rq *rq, struct task_struct *p, int oldprio)
{
	//printk("unacloud: prio_changed_unacloud PID: %d RQ: %d\n", p->pid, rq->cpu);
}

static void switched_from_unacloud(struct rq *rq, struct task_struct *p)
{
	//printk("unacloud: switched_from_unacloud PID: %d RQ: %d\n", p->pid, rq->cpu);
}



/*
 * We switched to the sched_unacloud class.
 */
static void switched_to_unacloud(struct rq *rq, struct task_struct *p)
{
	//printk("unacloud: switched_to_unacloud PID: %d RQ: %d\n", p->pid, rq->cpu);	
	if (rq->curr == p)
		resched_curr(rq);
	else
		check_preempt_curr(rq, p, 0);
}



static unsigned int get_rr_interval_unacloud(struct rq *rq, struct task_struct *task)
{
	//printk("unacloud: get_rr_interval_unacloud: 25 PID: %d RQ: %d\n", task->pid, rq->cpu);
	return 25;
}

__init void init_sched_unacloud_class(void)
{
}

//COSAS SMP
static void record_wakee(struct task_struct *p)
{
	/*
	 * Rough decay (wiping) for cost saving, don't worry
	 * about the boundary, really active task won't care
	 * about the loss.
	 */
	if (time_after(jiffies, current->wakee_flip_decay_ts + HZ)) {
		current->wakee_flips >>= 1;
		current->wakee_flip_decay_ts = jiffies;
	}

	if (current->last_wakee != p) {
		current->last_wakee = p;
		current->wakee_flips++;
	}
}
static void task_waking_unacloud(struct task_struct *p)
{
	//printk("unacloud: task_waking_unacloud, PID: %d\n", p->pid);
	record_wakee(p);
}
static void rq_online_unacloud(struct rq *rq)
{
	//printk("unacloud: rq_online_unacloud RQ: %d\n", rq->cpu);
	//update_sysctl();

	//update_runtime_enabled(rq);
}

static void rq_offline_unacloud(struct rq *rq)
{
	//printk("unacloud: rq_offline_unacloud RQ: %d\n", rq->cpu);
	//update_sysctl();

	/* Ensure any throttled groups are reachable by pick_next_task */
	//unthrottle_offline_cfs_rqs(rq);
}
static int select_task_rq_unacloud(struct task_struct *p, int prev_cpu, int sd_flag, int wake_flags)
{
	//printk("unacloud: select_task_rq_unacloud: RQ: %d\n", prev_cpu);
	return prev_cpu;
}

static void migrate_task_rq_unacloud(struct task_struct *p, int next_cpu)
{
	//printk("unacloud: migrate_task_rq_unacloud: PID: %d\n", p->pid);
	struct sched_entity *se = &p->se;
	se->exec_start = 0;
}

static void task_woken_unacloud(struct rq *rq, struct task_struct *p)
{
	//printk("unacloud: task_woken_unacloud PID: %d RQ: %d\n", p->pid, rq->cpu);
}

static void set_cpus_allowed_unacloud(struct task_struct *p,
	const struct cpumask *new_mask)
{
	//printk("unacloud: set_cpus_allowed_unacloud PID: %d\n", p->pid);
}

static void task_dead_unacloud(struct task_struct *p)
{
	//printk("unacloud: task_dead_unacloud PID: %d\n", p->pid);
} 

const struct sched_class unacloud_sched_class = {		
	.next = &idle_sched_class,
	.enqueue_task = enqueue_task_unacloud,
	.dequeue_task = dequeue_task_unacloud,
	.yield_task = yield_task_unacloud,
	.yield_to_task = yield_to_task_unacloud,

	.check_preempt_curr = check_preempt_wakeup,

	.pick_next_task = pick_next_task_unacloud,
	.put_prev_task = put_prev_task_unacloud,

	.set_curr_task = set_curr_task_unacloud,
	.task_tick = task_tick_unacloud,
	.task_fork = task_fork_unacloud,
	.task_dead = task_dead_unacloud,
	
	
	
#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_unacloud,
	.migrate_task_rq	= migrate_task_rq_unacloud,

	.rq_online		= rq_online_unacloud,
	.rq_offline		= rq_offline_unacloud,
	.set_cpus_allowed = set_cpus_allowed_unacloud,

	.task_waking		= task_waking_unacloud,
	.task_woken			= task_woken_unacloud,
#endif

	.prio_changed = prio_changed_unacloud,
	.switched_from = switched_from_unacloud,
	.switched_to = switched_to_unacloud,

	.get_rr_interval = get_rr_interval_unacloud,
 
	.update_curr = update_curr_unacloud,
};

void init_unacloud_rq(struct unacloud_rq *rq)
{
	rq->unacloud_task = NULL;
}

