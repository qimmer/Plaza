//
// Created by Kim Johannsen on 13-03-2018.
//

#include "Task.h"
#include "AppLoop.h"
#include <stdint.h>

#define SCHED_UINT_PTR uintptr_t
#define SCHED_IMPLEMENTATION
#define SCHED_STATIC
#include <mmx/sched.h>

#undef CreateService

static void *SchedulerMemory;
static sched_size NeededSchedulerMemory;
static struct scheduler Scheduler;

#define MAX_THREADS 64

static Vector<u64> threadIds;
static int mainThread = 0;

struct Task {
    struct sched_task TaskData;
    TaskFunction TaskFunction;
    bool TaskFinished;
    u32 TaskResult;
};

DefineEvent(TaskFinished, TaskHandler)

DefineComponent(Task)
EndComponent()

DefineComponentProperty(Task, TaskFunction, TaskFunction)
DefineComponentProperty(Task, bool, TaskFinished)
DefineComponentProperty(Task, u32, TaskResult)

DefineService(TaskScheduler)
EndService()

static void TaskFunc(void* taskIndexPtr, struct scheduler*, struct sched_task_partition, sched_uint thread_num) {
    auto task = GetTaskEntity((size_t)taskIndexPtr);

    GetTaskFunction(task)(task);
}

void TaskWait(Entity task) {
    auto data = GetTask(task);
    scheduler_join(&Scheduler, &data->TaskData);
}

void TaskSchedule(Entity task) {
    auto data = GetTask(task);
    if(!data->TaskFinished) return;

    data->TaskResult = 0;

    scheduler_add(&Scheduler, &data->TaskData, TaskFunc, (void*)GetTaskIndex(task), 1, 1);
    data->TaskFinished = false;
}

bool GetTaskRunning(Entity task) {
    return !sched_task_done(&GetTask(task)->TaskData);
}

int GetCurrentThreadIndex() {
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

int GetMainThreadIndex() {
    return mainThread;
}

int GetNumThreads() {
    return MAX_THREADS;
}

static void OnUpdate(double deltaTime) {
    for_entity(task, Task) {
        if(GetTaskRunning(task) || GetTaskFinished(task)) continue;

        FireEvent(TaskFinished, task, GetTaskResult(task));

        SetTaskFinished(task, true);
    }
}

static bool ServiceStart() {
    memset(threadIds.data(), 0, sizeof(u64) * MAX_THREADS);

    mainThread = GetCurrentThreadIndex();

    scheduler_init(&Scheduler, &NeededSchedulerMemory, SCHED_DEFAULT, 0);
    SchedulerMemory = calloc(NeededSchedulerMemory, 1);
    scheduler_start(&Scheduler, SchedulerMemory);

    SubscribeAppUpdate(OnUpdate);

    return true;
}

static bool ServiceStop() {
    UnsubscribeAppUpdate(OnUpdate);

    scheduler_stop(&Scheduler, 0);
    free(SchedulerMemory);

    return true;
}