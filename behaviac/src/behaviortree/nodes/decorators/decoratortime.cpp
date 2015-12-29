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
#include "behaviac/behaviortree/nodes/decorators/decoratortime.h"
#include "behaviac/agent/agent.h"
#include "behaviac/behaviortree/nodes/conditions/condition.h"

namespace behaviac
{
    DecoratorTime::DecoratorTime() : m_time_var(0)
    {}

    DecoratorTime::~DecoratorTime()
    {
    }

    //Property* LoadRight(const char* value, const behaviac::string& propertyName, behaviac::string& typeName);

    void DecoratorTime::load(int version, const char* agentType, const properties_t& properties)
    {
        super::load(version, agentType, properties);

        for (propertie_const_iterator_t it = properties.begin(); it != properties.end(); ++it)
        {
            const property_t& p = (*it);

            if (!strcmp(p.name, "Time"))
            {
                behaviac::string typeName;
                behaviac::string propertyName;
                this->m_time_var = Condition::LoadRight(p.value, typeName);
            }
        }
    }

    int DecoratorTime::GetTime(Agent* pAgent) const
    {
        if (this->m_time_var)
        {
            BEHAVIAC_ASSERT(this->m_time_var);
            TProperty<int>* pP = (TProperty<int>*)this->m_time_var;
            uint64_t time = pP->GetValue(pAgent);

            return (time == ((uint64_t) - 1) ? -1 : (int)time);
        }

        return 0;
    }

    BehaviorTask* DecoratorTime::createTask() const
    {
        DecoratorTimeTask* pTask = BEHAVIAC_NEW DecoratorTimeTask();

        return pTask;
    }

    DecoratorTimeTask::DecoratorTimeTask() : DecoratorTask(), m_start(0), m_time(0)
    {
    }

    DecoratorTimeTask::~DecoratorTimeTask()
    {
    }

    int DecoratorTimeTask::GetTime(Agent* pAgent) const
    {
        BEHAVIAC_ASSERT(DecoratorTime::DynamicCast(this->GetNode()));
        const DecoratorTime* pNode = (const DecoratorTime*)(this->GetNode());

        return pNode ? pNode->GetTime(pAgent) : 0;
    }

    void DecoratorTimeTask::copyto(BehaviorTask* target) const
    {
        super::copyto(target);

        BEHAVIAC_ASSERT(DecoratorTimeTask::DynamicCast(target));
        DecoratorTimeTask* ttask = (DecoratorTimeTask*)target;

        ttask->m_start = this->m_start;
        ttask->m_time = this->m_time;
    }

    void DecoratorTimeTask::save(ISerializableNode* node) const
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

    void DecoratorTimeTask::load(ISerializableNode* node)
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

    bool DecoratorTimeTask::onenter(Agent* pAgent)
    {
        super::onenter(pAgent);

        this->m_start = 0;
        this->m_time = this->GetTime(pAgent);

        if (this->m_time <= 0)
        {
            return false;
        }

        return true;
    }

    EBTStatus DecoratorTimeTask::decorate(EBTStatus status)
    {
        BEHAVIAC_UNUSED_VAR(status);

        this->m_start += (int)(Workspace::GetInstance()->GetDeltaFrameTime() * 1000.0f);

        if (this->m_start >= this->m_time)
        {
            return BT_SUCCESS;
        }

        return BT_RUNNING;
    }
}//namespace behaviac
