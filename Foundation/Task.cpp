//
// Created by Kim Johannsen on 13-03-2018.
//

#include "Task.h"
#include "AppLoop.h"
#include "Invocation.h"
#include <stdint.h>

#define SCHED_UINT_PTR uintptr_t
#define SCHED_IMPLEMENTATION
#define SCHED_STATIC
#undef SCHED_MIN
#define __SSE__ 1

#include <mmx/sched.h>
#include <Core/Vector.h>

#undef CreateService
#undef CreateEvent

static void *SchedulerMemory;
static sched_size NeededSchedulerMemory;
static struct scheduler Scheduler;

#define MAX_THREADS 64

static eastl::vector<u64> threadIds;
static int mainThread = 0;

struct Task {
    struct sched_task TaskData;
    bool TaskFinished;
    Entity TaskFunction;
};

struct TaskQueue {
    ChildArray QueuedTasks;
};

static void TaskFunc(void* taskIndexPtr, struct scheduler*, struct sched_task_partition, sched_uint thread_num) {
    static auto taskComponent = ComponentOf_Task();
    auto task = GetComponentEntity(taskComponent, (size_t)taskIndexPtr);
    Invoke(task);
}

API_EXPORT void TaskWait(Entity task) {
    auto data = GetTask(task);
    scheduler_join(&Scheduler, &data.TaskData);
}

API_EXPORT void TaskSchedule(Entity task) {
    auto data = GetTask(task);
    if(!data.TaskFinished) return;

    scheduler_add(&Scheduler, &data.TaskData, TaskFunc, (void*)GetComponentIndex(ComponentOf_Task(), task), 1, 1);
    data.TaskFinished = false;
}

API_EXPORT bool GetTaskRunning(Entity task) {
    return !sched_task_done(&GetTask(task).TaskData);
}

API_EXPORT int GetCurrentThreadIndex() {
    u64 threadId;
#if defined(WIN32) && !defined(_POSIX_THREADS)
    threadId = GetCurrentThreadId();
#else
    threadId = (u64)pthread_self();
#endif

    for(auto i = 0; i < threadIds.size(); ++i) {
        if(threadIds[i] == threadId) {
            return i;
        }
    }

    threadIds.push_back(threadId);

    return threadIds.size() - 1;
}

API_EXPORT int GetMainThreadIndex() {
    return mainThread;
}

API_EXPORT int GetNumThreads() {
    return MAX_THREADS;
}

static void OnAppLoopChanged(Entity appLoop, const AppLoop& oldData, const AppLoop& newData) {
    Task taskData;
    for_entity_data(task, ComponentOf_Task(), &taskData) {
        if(GetTaskRunning(task) || GetTask(task).TaskFinished) continue;
        taskData.TaskFinished = true;
        SetTask(task, taskData);
    }
}

BeginUnit(Task)
    BeginComponent(Task)
        RegisterProperty(bool, TaskFinished)
    EndComponent()

    BeginComponent(TaskQueue)
        RegisterArrayProperty(Task, QueuedTasks)
    EndComponent()

    memset(threadIds.data(), 0, sizeof(u64) * MAX_THREADS);

    mainThread = GetCurrentThreadIndex();

    scheduler_init(&Scheduler, &NeededSchedulerMemory, SCHED_DEFAULT, 0);
    SchedulerMemory = calloc(NeededSchedulerMemory, 1);
    scheduler_start(&Scheduler, SchedulerMemory);

    RegisterDeferredSystem(OnAppLoopChanged, ComponentOf_AppLoop(), AppLoopOrder_Update * 0.5f)

    /*
    scheduler_stop(&Scheduler, 0);
    free(SchedulerMemory);
     * */
EndUnit()
