#include "behaviac/fsm/waitstate.h"
#include "behaviac/behaviortree/attachments/effector.h"
#include "behaviac/fsm/startcondition.h"
#include "behaviac/fsm/transitioncondition.h"
#include "behaviac/agent/agent.h"
#include "behaviac/behaviortree/nodes/conditions/condition.h"

namespace behaviac
{
	WaitState::WaitState() : m_ignoreTimeScale(false), m_time_var(0)
	{
	}

	WaitState::~WaitState()
	{
        BEHAVIAC_DELETE(this->m_time_var);
	}

	void WaitState::load(int version, const char* agentType, const properties_t& properties)
	{
		super::load(version, agentType, properties);

		for (propertie_const_iterator_t it = properties.begin(); it != properties.end(); ++it)
		{
			const property_t& p = (*it);

			if (!strcmp(p.name, "IgnoreTimeScale"))
			{
				this->m_ignoreTimeScale = p.value[0] != '\0' && string_icmp(p.value, "true") == 0;

			}
			else if (!strcmp(p.name, "Time"))
			{
				behaviac::string typeName;
				behaviac::string propertyName;
				this->m_time_var = Condition::LoadRight(p.value, typeName);
			}
		}
	}

	float WaitState::GetTime(Agent* pAgent) const
	{
		if (this->m_time_var)
		{
			BEHAVIAC_ASSERT(this->m_time_var);
			TProperty<float>* pP = (TProperty<float>*)this->m_time_var;
			return pP->GetValue(pAgent);
		}

		return 0;
	}

	bool WaitState::IsValid(Agent* pAgent, BehaviorTask* pTask) const
    {
		if (WaitState::DynamicCast(pTask->GetNode()) == 0)
        {
            return false;
        }

        return super::IsValid(pAgent, pTask);
    }

	BehaviorTask* WaitState::createTask() const
	{
		WaitStateTask* pTask = BEHAVIAC_NEW WaitStateTask();

		return pTask;
	}

	WaitStateTask::WaitStateTask() : StateTask(), m_start(0), m_time(0)
	{
	}

	WaitStateTask::~WaitStateTask()
	{

	}

	void WaitStateTask::copyto(BehaviorTask* target) const
	{
		super::copyto(target);

		BEHAVIAC_ASSERT(WaitStateTask::DynamicCast(target));
		WaitStateTask* ttask = (WaitStateTask*)target;

		ttask->m_start = this->m_start;
		ttask->m_time = this->m_time;
	}

	void WaitStateTask::save(ISerializableNode* node) const
	{
		super::save(node);

		if (this->m_status != BT_INVALID)
		{
			CSerializationID  startId("start");
			node->setAttr(startId, this->m_start);

			CSerializationID  timeId("time");
			node->setAttr(timeId, this->m_time);
		}
	}

	void WaitStateTask::load(ISerializableNode* node)
	{
		super::load(node);

		if (this->m_status != BT_INVALID)
		{
			CSerializationID  startId("start");
			behaviac::string attrStr;
			node->getAttr(startId, attrStr);
			StringUtils::FromString(attrStr.c_str(), this->m_start);

			CSerializationID  timeId("time");
			node->getAttr(timeId, attrStr);
			StringUtils::FromString(attrStr.c_str(), this->m_time);
		}
	}

	bool WaitStateTask::GetIgnoreTimeScale() const
	{
		const WaitState* pWaitNode = WaitState::DynamicCast(this->GetNode());

		return pWaitNode ? pWaitNode->m_ignoreTimeScale : false;
	}

	float WaitStateTask::GetTime(Agent* pAgent) const
	{
		const WaitState* pWaitNode = WaitState::DynamicCast(this->GetNode());

		return pWaitNode ? pWaitNode->GetTime(pAgent) : 0;
	}

	bool WaitStateTask::onenter(Agent* pAgent)
	{
		BEHAVIAC_UNUSED_VAR(pAgent);
        
        this->m_nextStateId = -1;

		if (this->GetIgnoreTimeScale())
		{
			this->m_start = Workspace::GetInstance()->GetTimeSinceStartup() * 1000.0f;
		}
		else
		{
			this->m_start = 0;
		}

		this->m_time = this->GetTime(pAgent);

		if (this->m_time <= 0)
		{
			return false;
		}

		return true;
	}

	void WaitStateTask::onexit(Agent* pAgent, EBTStatus s)
	{
		BEHAVIAC_UNUSED_VAR(pAgent);
		BEHAVIAC_UNUSED_VAR(s);
	}

	EBTStatus WaitStateTask::update(Agent* pAgent, EBTStatus childStatus)
	{
		BEHAVIAC_ASSERT(childStatus == BT_RUNNING);
		BEHAVIAC_UNUSED_VAR(childStatus);
		BEHAVIAC_ASSERT(WaitState::DynamicCast(this->GetNode()) != 0, "node is not an WaitState");
		WaitState* pStateNode = (WaitState*)(this->GetNode());

		if (this->GetIgnoreTimeScale())
		{
			if (Workspace::GetInstance()->GetTimeSinceStartup() * 1000.0f - this->m_start >= this->m_time)
			{
				return BT_SUCCESS;
			}
		}
		else
		{
			this->m_start += Workspace::GetInstance()->GetDeltaFrameTime() * 1000.0f;

			if (this->m_start >= this->m_time)
			{
				EBTStatus result = pStateNode->Update(pAgent, this->m_nextStateId);
				BEHAVIAC_UNUSED_VAR(result);
				return BT_SUCCESS;
			}
		}

		return BT_RUNNING;
	}
}


