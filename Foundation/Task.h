//
// Created by Kim Johannsen on 13-03-2018.
//

#ifndef PLAZA_TASK_H
#define PLAZA_TASK_H

#include <Core/NativeUtils.h>

Unit(Task)
    Component(Task)
        Property(bool, TaskFinished)
    Node(TaskQueue)

void TaskSchedule(Entity task);
void TaskWait(Entity task);
bool GetTaskRunning(Entity task);
int GetCurrentThreadIndex();
int GetMainThreadIndex();
int GetNumThreads();

#ifndef thread_local
#if defined(_MSC_VER)
#define thread_local __declspec(thread)
#else
#define thread_local __thread
#endif
#endif

#endif //PLAZA_TASK_H
