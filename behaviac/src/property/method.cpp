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

#include "behaviac/agent/agent.h"
#include "behaviac/base/object/method.h"

behaviac::Agent* CMethodBase::GetParentAgent(const behaviac::Agent* pAgent)
{
    behaviac::Agent* pParent = (behaviac::Agent*)pAgent;

    if (!this->m_instanceName.empty() && this->m_instanceName != "Self")
    {
        //pParent = behaviac::Agent::GetInstance(this->m_instanceName.c_str(), pParent->GetContextId());
        pParent = behaviac::Agent::GetInstance(this->GetInstanceNameString(), pParent->GetContextId());
        //pParent=pAgent->GetInstance(this->GetInstanceNameString(), pParent->GetContextId());
        //pParent = behaviac::Agent::GetInstance(this->GetInstanceNameString(), pParent->GetContextId());
        BEHAVIAC_ASSERT(pParent);
    }

    return pParent;
}
