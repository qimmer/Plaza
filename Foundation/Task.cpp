//
// Created by Kim Johannsen on 13-03-2018.
//

#include "Task.h"
#include "AppLoop.h"
#include <stdint.h>

#define SCHED_UINT_PTR uintptr_t
#define SCHED_IMPLEMENTATION
#define SCHED_STATIC
#undef SCHED_MIN
#include <mmx/sched.h>

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
    TaskFunction TaskFunction;
    bool TaskFinished;
    u32 TaskResult;
};

DefineEvent(TaskFinished)

DefineComponent(Task)
EndComponent()

DefineComponentProperty(Task, TaskFunction, TaskFunction)
DefineComponentProperty(Task, bool, TaskFinished)
DefineComponentProperty(Task, u32, TaskResult)

static void TaskFunc(void* taskIndexPtr, struct scheduler*, struct sched_task_partition, sched_uint thread_num) {
    auto task = GetTaskEntity((size_t)taskIndexPtr);

    GetTaskFunction(task)(task);
}

API_EXPORT void TaskWait(Entity task) {
    auto data = GetTask(task);
    scheduler_join(&Scheduler, &data->TaskData);
}

API_EXPORT void TaskSchedule(Entity task) {
    auto data = GetTask(task);
    if(!data->TaskFinished) return;

    data->TaskResult = 0;

    scheduler_add(&Scheduler, &data->TaskData, TaskFunc, (void*)GetTaskIndex(task), 1, 1);
    data->TaskFinished = false;
}

API_EXPORT bool GetTaskRunning(Entity task) {
    return !sched_task_done(&GetTask(task)->TaskData);
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

static void OnUpdate(double deltaTime) {
    for_entity(task, Task) {
        if(GetTaskRunning(task) || GetTaskFinished(task)) continue;

        FireNativeEvent(TaskFinished, task, GetTaskResult(task));

        SetTaskFinished(task, true);
    }
}

static void OnServiceStart(Service service) {
    memset(threadIds.data(), 0, sizeof(u64) * MAX_THREADS);

    mainThread = GetCurrentThreadIndex();

    scheduler_init(&Scheduler, &NeededSchedulerMemory, SCHED_DEFAULT, 0);
    SchedulerMemory = calloc(NeededSchedulerMemory, 1);
    scheduler_start(&Scheduler, SchedulerMemory);

}

static void OnServiceStop(Service service){
    scheduler_stop(&Scheduler, 0);
    free(SchedulerMemory);
}

DefineService(TaskScheduler)
    Subscribe(TaskSchedulerStarted, OnServiceStart)
    Subscribe(TaskSchedulerStopped, OnServiceStop)
    Subscribe(AppUpdate, OnUpdate)
EndService()
