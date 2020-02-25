/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2019 Barcelona Supercomputing Center (BSC)
*/

#include "Symbol.hpp"
#include "TaskingModel.hpp"


register_polling_service_t *TaskingModel::_registerPollingService = nullptr;
unregister_polling_service_t *TaskingModel::_unregisterPollingService = nullptr;
get_current_blocking_context_t *TaskingModel::_getCurrentBlockingContext = nullptr;
block_current_task_t *TaskingModel::_blockCurrentTask = nullptr;
unblock_task_t *TaskingModel::_unblockTask = nullptr;
get_current_event_counter_t *TaskingModel::_getCurrentEventCounter = nullptr;
increase_current_task_event_counter_t *TaskingModel::_increaseCurrentTaskEventCounter = nullptr;
decrease_task_event_counter_t *TaskingModel::_decreaseTaskEventCounter = nullptr;
notify_task_event_counter_api_t *TaskingModel::_notifyTaskEventCounterAPI = nullptr;
