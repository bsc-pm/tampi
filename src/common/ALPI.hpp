/*
	This file is part of ALPI and is licensed under the terms contained in the COPYING file.

	Copyright (C) 2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef ALPI_INTERFACE_H
#define ALPI_INTERFACE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Forward declaration of a task
 *
 * This type is not defined in this interface, and thus, it is opaque. All the
 * functions in this interface that are related to tasks work with pointers to
 * this type. These pointers are treated as handles to the tasks. Dereferencing
 * such pointers or accessing memory pointed by them is invalid
 */
struct alpi_task;

/**
 * Forward declaration of a task attributes
 *
 * This type is not defined in this interface, and thus, it is opaque. All the
 * functions in this interface that are related to task attributes work with
 * pointers to this type. These pointers are treated as handles to the task
 * attributes. Dereferencing such pointers or accessing memory pointed by them
 * is invalid
 */
struct alpi_attr;

/**
 * Constant defining the major version of the current interface
 */
#define ALPI_VERSION_MAJOR 1

/**
 * Constant defining the minor version of the current interface
 */
#define ALPI_VERSION_MINOR 0

/**
 * The list of all errors that can be returned by any interface's function
 */
typedef enum {
	ALPI_SUCCESS = 0,         /**< The success code, guaranteed to be zero */
	ALPI_ERR_VERSION,         /**< The version is incompatible */
	ALPI_ERR_NOT_INITIALIZED, /**< The runtime system is not initialized */
	ALPI_ERR_PARAMETER,       /**< At least one parameter is invalid */
	ALPI_ERR_OUT_OF_MEMORY,   /**< Not enough memory to perform operation */
	ALPI_ERR_OUTSIDE_TASK,    /**< Must be executed within a running task */
	ALPI_ERR_UNKNOWN,         /**< Unknown or internal error */
	ALPI_ERR_MAX,             /**< Value only used by the implementation */
} alpi_error_t;

/**
 * Get a string error from an error code
 *
 * The string returned by this function explains the error code. If the code is
 * not recognized, the function returns a string "Error code not recognized".
 * The user should not free the string's memory
 *
 * @param error The error code
 *
 * @return A string explaining the error code
 */
const char *alpi_error_string(int error);

/**
 * Check the compatibility with respect to an expected version
 *
 * The function checks whether the version expected by the user is compatible
 * with the version implemented by the library. The user may call this function
 * passing the ALPI_VERSION_MAJOR and ALPI_VERSION_MINOR as the expected version
 *
 * @param major The major of the expected version
 * @param minor The minor of the expected version
 *
 * @return 0 on success; or ALPI_ERR_VERSION if the expected version is not
 *         compatible with the implemented version
 */
int alpi_version_check(int major, int minor);

/**
 * Get the version provided at run-time by the implementation
 *
 * @param major A pointer to where the major version should be written
 * @param minor A pointer to where the minor version should be written
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if any of the pointer parameters
 *         are invalid
 */
int alpi_version_get(int *major, int *minor);

/**
 * Get the task handle if running within a task
 *
 * This function checks whether the current thread is running within a task and
 * retrieves its handle
 *
 * The function may be called from outside a task. In this case, a NULL task
 * handle is returned in the output parameter
 *
 * @param task A pointer to where the task handle should be written
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the pointer parameter is
 *         invalid
 */
int alpi_task_self(struct alpi_task **task);

/**
 * Block the execution of the current task
 *
 * This function blocks the execution of the calling task until another thread
 * calls the alpi_task_unblock function with the same task handle. Each
 * block function call must have a matching unblock call. This call may return
 * immediately without blocking the task if the matching unblock call was
 * already received
 *
 * The tasking runtime system should reuse the execution resource used by this
 * task to run other ready tasks while this one is blocked to guarantee progress
 *
 * @param task The handle of the calling task
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the task handle is invalid; or
 *         ALPI_ERR_OUTSIDE_TASK if running outside a task
 */
int alpi_task_block(struct alpi_task *task);

/**
 * Unblock a task and resume its execution eventually
 *
 * This function unblocks a task so that it can eventually resume its execution.
 * The task may resume the execution on the same execution resource where it was
 * running or on a different one
 *
 * Each unblock call must have a matching block call. Even so, the unblock call
 * may be issued before the block call. However, only one call to the unblock
 * function may precede its matching call to the block function
 *
 * The return of this function does not guarantee that the blocked task has
 * resumed yet its execution; it only guarantees that it will be resumed
 *
 * @param task The handle of the task to be unblocked
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the task handle is invalid
 */
int alpi_task_unblock(struct alpi_task *task);

/**
 * Block the calling task for an approximate amount of nanoseconds
 *
 * This function blocks the calling task during an approximate amount of
 * nanoseconds. The task will be automatically resumed by the tasking runtime 
 * system after that time. The task cannot be unblocked explicitly
 *
 * The tasking runtime system should reuse the execution resources used by this
 * task to run other ready tasks while this one is blocked to guarantee progress
 *
 * @param target_ns The amount of nanoseconds that should be blocked
 * @param actual_ns Pointer to where the actual number of nanoseconds that the
 *                  task has been blocked should be written
 *
 * @return 0 on success; or ALPI_ERR_OUTSIDE_TASK if running outside a task
 */
int alpi_task_waitfor_ns(uint64_t target_ns, uint64_t *actual_ns);

/**
 * Increase the calling task's external events to prevent its completion
 *
 * This function increases the number of external events and prevents the
 * completion of the calling task. The task may finish the execution of its
 * body, but the task will not complete until all its external events are
 * fulfilled through the alpi_task_events_decrease function
 *
 * The completion of the task occurs when the task has finished the execution
 * of its body and all its external events have been fulfilled. The completion
 * also implies the release of the task's data dependencies, which may mark its
 * successor tasks as ready to execute
 *
 * The increase and decrease operations are thread-safe; concurrent calls to
 * these functions using the same task handle is safe
 *
 * @param task The handle of the calling task
 * @param increment The amount of events to increase
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the task handle is invalid; or
 *         ALPI_ERR_OUTSIDE_TASK if running outside a task
 */
int alpi_task_events_increase(struct alpi_task *task, uint64_t increment);

/**
 * Decrease the external events of a task and complete it if required
 *
 * This function decreases the number of external events of a task. If the task
 * finished its body execution and there are no pending events left after the
 * decrease operation, the task is considered completed. This implies that its
 * data dependencies will be released eventually and its successor task will be
 * marked as ready to execute
 *
 * An event of a task can be decreased (fulfilled) only after the task has
 * increased it using the alpi_task_events_increase function. At any point
 * in time, a task may only have zero or a positive number of events
 *
 * The increase and decrease operations are thread-safe; concurrent calls to
 * these functions using the same task handle is safe
 *
 * @param task The handle of the task to decrease its events
 * @param decrement The amount of events to decrease
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the task handle is invalid
 */
int alpi_task_events_decrease(struct alpi_task *task, uint64_t decrement);

/**
 * Allocate a task attributes and initialize with default values
 *
 * @param attr A pointer to where the attributes handle should be written
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the pointer parameter is
 *         invalid; or ALPI_ERR_OUT_OF_MEMORY if there is no enough memory
 */
int alpi_attr_create(struct alpi_attr **attr);

/**
 * Destroy and deallocate a task attributes
 *
 * This function should only be called when the task attributes was allocated
 * using the alpi_attr_create function
 *
 * @param attr The handle to the task attributes to destroy
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the handle is invalid
 */
int alpi_attr_destroy(struct alpi_attr *attr);

/**
 * Initialize or reset a task attributes to the default values
 *
 * @param attr The handle to the task attributes to initialize
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the handle is invalid
 */
int alpi_attr_init(struct alpi_attr *attr);

/**
 * Get the maximum size a task attributes structure may need
 *
 * This function returns the number of bytes that any task attributes should
 * need. The user may use this function to allocate its own memory for a task
 * attributes instead of using the alpi_attr_create function. Such task
 * attributes should be initialized using the alpi_attr_init function and
 * may not be destroyed with alpi_attr_destroy
 *
 * @param attr_size A pointer to where the maximum size of task attributes
 *                  should be written
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the pointer parameter is
 *         invalid
 */
int alpi_attr_size(uint64_t *attr_size);

/**
 * Spawn an independent task to asynchronously execute a function
 *
 * This function spawns an independent task that will be eventually scheduled on
 * an execution resource to run the specified function. The spawned task is a
 * regular task, but has an independent namespace of data dependencies and has
 * no relationship with any other existing task
 *
 * Once the task is completed, the completion callback is called by the tasking
 * runtime system to notify the user regarding the spawned task's completion.
 * The user could leverage the callback as a synchronization point with the
 * spawned task
 *
 * @param body The function to execute. The function must have a void return
 *             type and a void pointer parameter
 * @param body_args The arguments which will be passed to the body function; may
 *                  be NULL
 * @param completion_callback The completion callback to execute when the task
 *                            has completed. The function must have a void
 *                            return type and a void pointer parameter
 * @param completion_args The arguments which will be passed to the completion
 *                        callback function; may be NULL
 * @param label The label string of the spawned task; may be NULL
 * @param attr The attributes handle for the spawned task; or NULL to use the
 *             default attributes
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if any of the functions is
 *         invalid; or ALPI_ERR_OUT_OF_MEMORY if there is no enough memory
 */
int alpi_task_spawn(
	void (*body)(void *),
	void *body_args,
	void (*completion_callback)(void *),
	void *completion_args,
	const char *label,
	const struct alpi_attr *attr);

/**
 * Get the number of execution resources or CPUs available
 *
 * This function returns the number of CPUs which the tasking runtime system has
 * available to execute tasks
 *
 * @param count A pointer to where the number of CPUs should be written
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the pointer parameter is
 *         invalid
 */
int alpi_cpu_count(uint64_t *count);

/**
 * Get the logical id of the CPU where the calling task is running
 *
 * The logical ids are the unique integer identifiers for the set of available
 * CPUs and which are arbitrarily decided by the tasking runtime system. The
 * logical id returned by this function is within the range [0, cpu_count),
 * where cpu_count is the value retrieved by the alpi_cpu_count function
 *
 * This function can only be called within a running task
 *
 * @param logical_id A pointer to where the logical id should be written
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the pointer parameter is
 *         invalid; or ALPI_ERR_OUTSIDE_TASK if running outside a task
 */
int alpi_cpu_logical_id(uint64_t *logical_id);

/**
 * Get the system id of the CPU where the calling task is running
 *
 * The system ids are usually decided by the operating system and they do not
 * have any guarantees on their values
 *
 * This function can only be called within a running task
 *
 * @param system_id A pointer to where the system id should be written
 *
 * @return 0 on success; or ALPI_ERR_PARAMETER if the pointer parameter is
 *         invalid; or ALPI_ERR_OUTSIDE_TASK if running outside a task
 */
int alpi_cpu_system_id(uint64_t *system_id);

#ifdef __cplusplus
}
#endif

#endif // ALPI_INTERFACE_H
