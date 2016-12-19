# Unacloud Task Scheduler

Process scheduler for the linux kernel (4.2.0 generic version) that focuses on managing either Desktop Grid or volunteered computing tasks. It that guarantees the tasks it manages won't increase the execution times of user tasks by more than a predefined threshold (percentage).

The policy used by the scheduler was defined in the research publication: Harvesting idle CPU resources for desktop grid computing while limiting the slowdown generated to end-users. Cluster Computing, Springer, Volume 18: 1331-1350.
