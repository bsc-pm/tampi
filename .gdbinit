set breakpoint pending on
source /home/jbellon/ompss/src/nanox-master/src
#break do_slave
#break nanos::Scheduler::waitOnCondition
rbreak nanos::ext::PollCondBF
condition 1 (*cond_it)->check()
