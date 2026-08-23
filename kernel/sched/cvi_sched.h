#ifdef CONFIG_SCHED_CVITEK
struct cvi_priority_info {
	char comm[TASK_COMM_LEN];
	u32 sched_policy;
	u32 sched_priority;
};
static struct cvi_priority_info priority_list[] = {
	{ "cvitask_isp_pre", SCHED_FIFO, 90 },
	{ "cvitask_isp_pos", SCHED_FIFO, 90 },
	{ "cvitask_vc_sb0", SCHED_RR, 99 },
	{ "cvitask_vc_wt0", SCHED_RR, 95 },
	{ "cvitask_vc_wt1", SCHED_RR, 95 },
	{ "cvitask_vc_bh0", SCHED_RR, 95 },
	{ "cvitask_vpss_0", SCHED_FIFO, 90 },
	{ "cvitask_vpss_1", SCHED_FIFO, 90 },
	{ "cvitask_tpu_wor", SCHED_RR, 85 }
};
#define MAX_OPT_RT_PRIORITY 80
static inline void cvi_checkpriority(struct task_struct *p,
				     struct sched_attr *attr)
{
	int i = 0;

	if (!strncmp(p->comm, "cvitask_", 8)) {
		for (i = 0; i < ARRAY_SIZE(priority_list); i++) {
			if (strcmp(priority_list[i].comm, p->comm))
				continue;
			attr->sched_policy = priority_list[i].sched_policy;
			attr->sched_priority = priority_list[i].sched_priority;
			break;
		}
	} else if (attr->sched_policy == SCHED_FIFO ||
		   attr->sched_policy == SCHED_RR) {
		if (attr->sched_priority > MAX_OPT_RT_PRIORITY)
			attr->sched_priority = MAX_OPT_RT_PRIORITY;
	}
}
#endif
