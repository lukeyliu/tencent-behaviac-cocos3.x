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

#include "behaviac/base/base.h"
#include "behaviac/behaviortree/nodes/composites/referencebehavior.h"
#include "behaviac/agent/agent.h"
#include "behaviac/agent/taskmethod.h"
#include "behaviac/behaviortree/nodes/actions/action.h"

#include "behaviac/htn/planner.h"
#include "behaviac/htn/plannertask.h"
#include "behaviac/htn/task.h"
#include "behaviac/fsm/state.h"
#include "behaviac/fsm/transitioncondition.h"

namespace behaviac
{
	ReferencedBehavior::ReferencedBehavior() : m_taskNode(0), m_taskMethod(0), m_transitions(0)
    {}

    ReferencedBehavior::~ReferencedBehavior()
    {
		BEHAVIAC_DELETE this->m_transitions;
    }

    void ReferencedBehavior::load(int version, const char* agentType, const properties_t& properties)
    {
        super::load(version, agentType, properties);

        for (propertie_const_iterator_t it = properties.begin(); it != properties.end(); ++it)
        {
            const property_t& p = (*it);

            if (strcmp(p.name, "ReferenceFilename") == 0)
            {
                this->m_referencedBehaviorPath = p.value;

                bool bOk = Workspace::GetInstance()->Load(this->m_referencedBehaviorPath.c_str());
                BEHAVIAC_UNUSED_VAR(bOk);

                BEHAVIAC_ASSERT(bOk);

            }
            else if (strcmp(p.name, "Task") == 0)
            {
                BEHAVIAC_ASSERT(!StringUtils::IsNullOrEmpty(p.value));
                CMethodBase* m = Action::LoadMethod(p.value);
                //BEHAVIAC_ASSERT(m is CTaskMethod);

                this->m_taskMethod = (CTaskMethod*)m;

            }
            else
            {
                //BEHAVIAC_ASSERT(0, "unrecognised property %s", p.name);
            }
        }
    }

    bool ReferencedBehavior::decompose(BehaviorNode* node, PlannerTaskComplex* seqTask, int depth, Planner* planner)
    {
        bool bOk = false;
        ReferencedBehavior* taskSubTree = (ReferencedBehavior*)node;// as ReferencedBehavior;
        BEHAVIAC_ASSERT(taskSubTree != 0);
        int depth2 = planner->GetAgent()->m_variables.Depth();
        BEHAVIAC_UNUSED_VAR(depth2);
        {
            AgentState::AgentStateScope scopedState(planner->GetAgent()->m_variables.Push(false));
            //planner.agent.Variables.Log(planner.agent, true);
            taskSubTree->SetTaskParams(planner->GetAgent());

            Task* task = taskSubTree->RootTaskNode();

            if (task != 0)
            {
                planner->LogPlanReferenceTreeEnter(planner->GetAgent(), taskSubTree);

                const BehaviorNode* tree = task->GetParent();
                tree->InstantiatePars(planner->GetAgent());

                PlannerTask* childTask = planner->decomposeNode(task, depth);

                if (childTask != 0)
                {
                    seqTask->AddChild(childTask);
                    bOk = true;
                }

                tree->UnInstantiatePars(planner->GetAgent());
                planner->LogPlanReferenceTreeExit(planner->GetAgent(), taskSubTree);
                BEHAVIAC_ASSERT(true);
            }
        }

        BEHAVIAC_ASSERT(planner->GetAgent()->m_variables.Depth() == depth2);
        return bOk;
    }
	
	void ReferencedBehavior::Attach(BehaviorNode* pAttachment, bool bIsPrecondition, bool bIsEffector, bool bIsTransition)
	{
		if (bIsTransition)
		{
			BEHAVIAC_ASSERT(!bIsEffector && !bIsPrecondition);

			if (this->m_transitions == 0)
			{
				this->m_transitions = BEHAVIAC_NEW behaviac::vector<Transition*>();
			}

			BEHAVIAC_ASSERT(Transition::DynamicCast(pAttachment) != 0);
			Transition* pTransition = (Transition*)pAttachment;
			this->m_transitions->push_back(pTransition);

			return;
		}

		BEHAVIAC_ASSERT(bIsTransition == false);
		super::Attach(pAttachment, bIsPrecondition, bIsEffector, bIsTransition);
	}


    bool ReferencedBehavior::IsValid(Agent* pAgent, BehaviorTask* pTask) const
    {
        if (!ReferencedBehavior::DynamicCast(pTask->GetNode()))
        {
            return false;
        }

        return super::IsValid(pAgent, pTask);
    }

    BehaviorTask* ReferencedBehavior::createTask() const
    {
        ReferencedBehaviorTask* pTask = BEHAVIAC_NEW ReferencedBehaviorTask();

        return pTask;
    }

    void ReferencedBehavior::SetTaskParams(Agent* pAgent)
    {
        if (this->m_taskMethod != 0)
        {
            this->m_taskMethod->SetTaskParams(pAgent);
        }
    }

    Task* ReferencedBehavior::RootTaskNode()
    {
        if (this->m_taskNode == 0)
        {
            BehaviorTree* bt = Workspace::GetInstance()->LoadBehaviorTree(this->m_referencedBehaviorPath.c_str());

            if (bt != 0 && bt->GetChildrenCount() == 1)
            {
                BehaviorNode* root = (BehaviorNode*)bt->GetChild(0);
                this->m_taskNode = (Task*)root;
            }
        }

        return this->m_taskNode;
    }

    const char* ReferencedBehavior::GetReferencedTree() const
    {
        return this->m_referencedBehaviorPath.c_str();
    }

	ReferencedBehaviorTask::ReferencedBehaviorTask() : SingeChildTask(), m_nextStateId(-1), m_subTree(0)
    {
    }

    ReferencedBehaviorTask::~ReferencedBehaviorTask()
    {
    }

    void ReferencedBehaviorTask::copyto(BehaviorTask* target) const
    {
        super::copyto(target);

        // BEHAVIAC_ASSERT(ReferencedBehaviorTask::DynamicCast(target));
        // ReferencedBehaviorTask* ttask = (ReferencedBehaviorTask*)target;
    }

    void ReferencedBehaviorTask::save(ISerializableNode* node) const
    {
        super::save(node);
    }

    void ReferencedBehaviorTask::load(ISerializableNode* node)
    {
        super::load(node);
    }

    void ReferencedBehaviorTask::Init(const BehaviorNode* node)
    {
        super::Init(node);
    }

    bool ReferencedBehaviorTask::onenter(Agent* pAgent)
    {
        BEHAVIAC_UNUSED_VAR(pAgent);
		BEHAVIAC_ASSERT(ReferencedBehavior::DynamicCast(this->m_node) != 0);
		ReferencedBehavior* pNode = (ReferencedBehavior*)this->m_node;

		BEHAVIAC_ASSERT(pNode != 0);

		this->m_nextStateId = -1;

		pNode->SetTaskParams(pAgent);

		this->m_subTree = Workspace::GetInstance()->CreateBehaviorTreeTask(pNode->m_referencedBehaviorPath.c_str());

		{
			const char* pThisTree = pAgent->btgetcurrent()->GetName().c_str();
			const char* pReferencedTree = pNode->m_referencedBehaviorPath.c_str();

			behaviac::string msg = FormatString("%s[%d] %s", pThisTree, pNode->GetId(), pReferencedTree);

			LogManager::GetInstance()->Log(pAgent, msg.c_str(), EAR_none, ELM_jump);
		}

        return true;
    }

    void ReferencedBehaviorTask::onexit(Agent* pAgent, EBTStatus s)
    {
        BEHAVIAC_UNUSED_VAR(pAgent);
        BEHAVIAC_UNUSED_VAR(s);
    }

	int ReferencedBehaviorTask::GetNextStateId() const
	{
		return this->m_nextStateId;
	}

    EBTStatus ReferencedBehaviorTask::update(Agent* pAgent, EBTStatus childStatus)
    {
        BEHAVIAC_UNUSED_VAR(childStatus);
		BEHAVIAC_ASSERT(ReferencedBehavior::DynamicCast(this->GetNode()));
		const ReferencedBehavior* pNode = (const ReferencedBehavior*)this->m_node;
		BEHAVIAC_ASSERT(pNode);

		EBTStatus result = this->m_subTree->exec(pAgent);

		bool bTransitioned = State::UpdateTransitions(pAgent, pNode, pNode->m_transitions, this->m_nextStateId);

		if (bTransitioned)
		{
			result = BT_SUCCESS;
		}

		return result;
    }
}//namespace behaviac
