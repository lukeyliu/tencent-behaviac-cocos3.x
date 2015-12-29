/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tencent is pleased to support the open source community by making behaviac available.
//
// Copyright (C) 2015 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at http://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef BEHAVIAC_HTN_PLANNER_H
#define BEHAVIAC_HTN_PLANNER_H
#include "behaviac/base/base.h"
#include "behaviac/behaviortree/behaviortree.h"
#include "behaviac/behaviortree/behaviortree_task.h"
#include "behaviac/property/property.h"

namespace behaviac
{
    class PlannerTask;
    class DecoratorIterator;
    class PlannerTaskComplex;
    class Task;
    class ReferencedBehavior;
    class Planner
    {
        /*public delegate void PlannerCallback(Planner planner);
        public delegate void TaskCallback(Planner planner, PlannerTask task);*/

    public:
        typedef void(*PlannerCallback)(Planner* planner);
        typedef void(*TaskCallback)(Planner* planner, PlannerTask* task);
        /// <summary>
        /// This event is raised whenever the planner is attempting to generate a new task
        /// </summary>
        //public event PlannerCallback PlanningStarted;
        PlannerCallback PlanningStarted;
        PlannerCallback PlanningEnded;

        /// <summary>
        /// This event is raised ng attempt has completed
        /// </summary>
        //public event PlannerCallback PlanningEnded;		/// This event is raised whenever a new task is generated
        /// </summary>
        //public event TaskCallback PlanGenerated;
        TaskCallback PlanGenerated;

        /// <summary>
        /// This event is raised whenever a task is discarded due to being lower priority than the
        /// currently running task
        /// </summary>
        PlannerCallback PlanDiscarded;

        /// <summary>
        /// This event is raised whenever the planner fails to generate a valid task
        /// </summary>
        PlannerCallback PlanningFailed;

        /// <summary>
        /// This event is raised whenever a task starts executing
        /// </summary>
        TaskCallback PlanExecuted;

        /// <summary>
        /// This event is raised whenever a running task is aborted due to a new task being exeuted
        /// </summary>
        TaskCallback PlanAborted;

        /// <summary>
        /// This event is raised whenever a running task has completed successfully
        /// </summary>
        TaskCallback PlanCompleted;

        /// <summary>
        /// This event is raised whenever a running task has returned a failure status code
        /// </summary>
        TaskCallback PlanFailed;

        /// <summary>
        /// This event is raised whenever a new task has begun execution
        /// </summary>
        TaskCallback TaskExecuted;

        /// <summary>
        /// This event is raised whenever a task has successfully completed execution
        /// </summary>
        TaskCallback TaskSucceeded;

        /// <summary>
        /// This event is raised whenever a task fails during execution
        /// </summary>
        TaskCallback TaskFailed;

        /// <summary>
        /// Gets or sets the agent script instance that taskner will be generating plans for
        /// </summary>
    private:
        Agent* agent;

        /// <summary>
        /// Gets or sets whether the planner will automatically perform periodic replanning
        /// </summary>
        bool AutoReplan;

        /// <summary>
        /// Gets or sets the amount of time between replanning attempts
        /// </summary>
        float AutoReplanInterval;
        float timeTillReplan;

        Task* m_rootTaskNode;
        PlannerTask* m_rootTask;

    public:
        void Init(Agent* pAgent, Task* rootTask);

        void Uninit();
        Planner() : agent(0), timeTillReplan(0.0f), m_rootTaskNode(0), m_rootTask(0)
        {
            AutoReplan = true;
            AutoReplanInterval = 0.2f;

            PlanningStarted = 0;
            PlanningEnded = 0;
            PlanGenerated = 0;
            PlanDiscarded = 0;
            PlanningFailed = 0;
            PlanExecuted = 0;
            PlanAborted = 0;
            PlanCompleted = 0;
            PlanFailed = 0;
            TaskExecuted = 0;
            TaskSucceeded = 0;
            TaskFailed = 0;
        }
    private:
        void OnDisable();

    public:
        EBTStatus Update();

        /// <summary>
        /// Generate a new task for the <paramref name="agent"/> based on the current world state as
        /// described by the <paramref name="agentState"/>.
        /// </summary>
        /// <param name="agent">The agent for which the task is being generated. This object instance must be
        /// of the same type as the type for which the Task was developed</param>
        /// <param name="agentState">The current world state required by the planner</param>
        /// <returns></returns>
    private:
        PlannerTask* GeneratePlan();
        void raiseTaskSucceeded(PlannerTask* task);
        void raiseTaskExecuted();

        void raisePlanningStarted();
        void raisePlanningEnded();

        void raisePlanCompleted(PlannerTask* task);

        void raisePlanFailed(PlannerTask* task);

        void raisePlanGenerated(PlannerTask* task);
        void raisePlanDiscarded();

        void raisePlanExecuted(PlannerTask* task);

        void raisePlanAborted(PlannerTask* task);

        void raisePlanningFailed();

        bool canInterruptCurrentPlan();

        void doAutoPlanning();
        behaviac::string GetTickInfo(const behaviac::Agent* pAgent, const behaviac::BehaviorTask* b, const char* action);
    public:
        Agent* GetAgent()
        {
            return this->agent;
        }
        void LogPlanBegin(Agent* a, Task* root);

        void LogPlanEnd(Agent* a, Task* root);

        void LogPlanNodeBegin(Agent* a, BehaviorNode* n);
        void LogPlanNodePreconditionFailed(Agent* a, BehaviorNode* n);
        void LogPlanNodeEnd(Agent* a, BehaviorNode* n, const behaviac::string& result);

        void LogPlanReferenceTreeEnter(Agent* a, ReferencedBehavior* referencedNode);

        void LogPlanReferenceTreeExit(Agent* a, ReferencedBehavior* referencedNode);
        void LogPlanMethodBegin(Agent* a, BehaviorNode* m);

        void LogPlanMethodEnd(Agent* a, BehaviorNode* m, const behaviac::string& result);

        void LogPlanForEachBegin(Agent* a, DecoratorIterator* pForEach, int index, int count);

        void LogPlanForEachEnd(Agent* a, DecoratorIterator* pForEach, int index, int count, const behaviac::string& result);
        PlannerTask* BuildPlan(Task* root);

        PlannerTask* decomposeNode(BehaviorNode* node, int depth);
        bool decomposeComplex(BehaviorNode* node, PlannerTaskComplex* seqTask, int depth);

    public:
        PlannerTask* decomposeTask(Task* task, int depth);
    };
}

#endif
