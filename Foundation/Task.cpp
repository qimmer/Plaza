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
#include <mmx/sched.h>
#include <Core/Vector.h>

#undef CreateService
#undef CreateEvent

static void *SchedulerMemory;
static sched_size NeededSchedulerMemory;
static struct scheduler Scheduler;

#define MAX_THREADS 64

static Vector<u64> threadIds;
static int mainThread = 0;

struct Task {
    struct sched_task TaskData;
    bool TaskFinished;
    Entity TaskFunction;
};

struct TaskQueue {
    Vector(QueuedTasks, Entity, 32)
};

static void TaskFunc(void* taskIndexPtr, struct scheduler*, struct sched_task_partition, sched_uint thread_num) {
    auto task = GetComponentEntity(ComponentOf_Task(), (size_t)taskIndexPtr);
    Invoke(task);
}

API_EXPORT void TaskWait(Entity task) {
    auto data = GetTaskData(task);
    scheduler_join(&Scheduler, &data->TaskData);
}

API_EXPORT void TaskSchedule(Entity task) {
    auto data = GetTaskData(task);
    if(!data->TaskFinished) return;

    SetInvocationResult(task, CreateVariant(TypeOf_unknown, 0, 0));

    scheduler_add(&Scheduler, &data->TaskData, TaskFunc, (void*)GetComponentIndex(ComponentOf_Task(), task), 1, 1);
    data->TaskFinished = false;
}

API_EXPORT bool GetTaskRunning(Entity task) {
    return !sched_task_done(&GetTaskData(task)->TaskData);
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

LocalFunction(OnAppLoopFrameChanged, void, Entity appLoop, u64 oldFrame, u64 newFrame) {
    for_entity(task, data, Task) {
        if(GetTaskRunning(task) || GetTaskFinished(task)) continue;

        SetTaskFinished(task, true);
    }
}

LocalFunction(OnCoreModuleInitialized, void, Entity module) {
    memset(threadIds.data(), 0, sizeof(u64) * MAX_THREADS);

    mainThread = GetCurrentThreadIndex();

    scheduler_init(&Scheduler, &NeededSchedulerMemory, SCHED_DEFAULT, 0);
    SchedulerMemory = calloc(NeededSchedulerMemory, 1);
    scheduler_start(&Scheduler, SchedulerMemory);

}

LocalFunction(OnCoreModuleDestroyed, void, Entity module){
    scheduler_stop(&Scheduler, 0);
    free(SchedulerMemory);
}

BeginUnit(Task)
    BeginComponent(Task)
        RegisterBase(Invocation)
        RegisterProperty(bool, TaskFinished)
    EndComponent()

    BeginComponent(TaskQueue)
        RegisterArrayProperty(Task, QueuedTasks)
    EndComponent()

    RegisterSubscription(AppLoopFrameChanged, OnAppLoopFrameChanged, 0)
EndUnit()
