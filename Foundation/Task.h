//
// Created by Kim Johannsen on 13-03-2018.
//

#ifndef PLAZA_TASK_H
#define PLAZA_TASK_H

#include <Core/Entity.h>

typedef u32(*TaskFunction)(Entity task);

DeclareComponent(Task)
DeclareComponentProperty(Task, TaskFunction, TaskFunction)
DeclareComponentProperty(Task, bool, TaskFinished)
DeclareComponentProperty(Task, u32, TaskResult)

DeclareEvent(TaskFinished, Entity entity, u32 result)

DeclareService(TaskScheduler)

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
