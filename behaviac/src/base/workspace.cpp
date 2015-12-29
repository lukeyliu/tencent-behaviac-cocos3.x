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

#ifndef BEHAVIAC_SHARED_H_
#define BEHAVIAC_SHARED_H_

#include "behaviac/base/base.h"
#include "behaviac/base/workspace.h"

#include "behaviac/behaviortree/behaviortree.h"
#include "behaviac/behaviortree/behaviortree_task.h"

#include "behaviac/behaviortree/nodes/conditions/condition.h"
#include "behaviac/agent/agent.h"

#include "behaviac/base/file/filemanager.h"
#include "behaviac/base/file/file.h"

#include "behaviac/base/core/profiler/profiler.h"
#include "behaviac/htn/agentproperties.h"

namespace behaviac
{
    bool TryStart();
    void BaseStop();

    bool IsStarted();

#if BEHAVIAC_COMPILER_MSVC || BEHAVIAC_COMPILER_GCC_CYGWIN || BEHAVIAC_COMPILER_GCC_LINUX
    const  bool Config::ms_bIsDesktopPlatform = true;
#else
    const bool Config::ms_bIsDesktopPlatform = false;
#endif

    bool Config::ms_bIsLogging =
#if !defined(BEHAVIAC_RELEASE)
        false;
#else
        false;
#endif//BEHAVIAC_RELEASE

    bool Config::ms_bIsSocketing =
#if !defined(BEHAVIAC_RELEASE)
        true;
#else
        false;
#endif//BEHAVIAC_RELEASE

    bool Config::ms_bProfiling =
#if !defined(BEHAVIAC_RELEASE)
        false;
#else
        false;
#endif//BEHAVIAC_RELEASE

    bool Config::IsProfiling()
    {
        return ms_bProfiling;
    }

    void Config::SetProfiling(bool bEnabled)
    {
        ms_bProfiling = bEnabled;
    }

    bool Config::IsDesktopPlatform()
    {
        return ms_bIsDesktopPlatform;
    }

    bool Config::IsLogging()
    {
        //logging is only enabled on pc platform, it is disabled on android, ios, etc.
        return ms_bIsDesktopPlatform && ms_bIsLogging;
    }

    void Config::SetLogging(bool bLogging)
    {
        ms_bIsLogging = bLogging;
    }

    /**
    by default, the log file is flushed every logging.
    */
    bool Config::ms_bLoggingFlush = true;

    bool Config::IsLoggingFlush()
    {
        return ms_bLoggingFlush;
    }

    void Config::SetLoggingFlush(bool bFlush)
    {
        ms_bLoggingFlush = bFlush;
    }

    //it is enabled on pc by default
    bool Config::IsSocketing()
    {
        return ms_bIsSocketing;
    }

    void Config::SetSocketing(bool bSocketing)
    {
        BEHAVIAC_ASSERT(!IsStarted(), "please call Config::SetSocketing at the very beginning before behaviac::Start!");
        ms_bIsSocketing = bSocketing;
    }

    bool Config::ms_bSocketIsBlocking = false;
    bool Config::IsSocketBlocking()
    {
        return ms_bSocketIsBlocking;
    }

    void Config::SetSocketBlocking(bool bBlocking)
    {
        BEHAVIAC_ASSERT(!IsStarted(), "please call Config::SetSocketBlocking at the very beginning before behaviac::Start!");

        ms_bSocketIsBlocking = bBlocking;
    }

    unsigned short Config::ms_socketPort = 60636;
    void Config::SetSocketPort(unsigned short port)
    {
        BEHAVIAC_ASSERT(!IsStarted(), "please call Config::SetSocketPort at the very beginning before behaviac::Start!");
        ms_socketPort = port;
    }

    unsigned short Config::GetSocketPort()
    {
        return ms_socketPort;
    }

    GenerationManager* GenerationManager::ms_generationManager = NULL;

    void GenerationManager::RegisterBehaviors()
    {
        if (ms_generationManager)
        {
            ms_generationManager->RegisterBehaviorsImplement();
        }
    }

    void GenerationManager::SetInstance(GenerationManager* generationManager)
    {
        BEHAVIAC_ASSERT(ms_generationManager == NULL, "The GenerationManager instance has already been set!");
        ms_generationManager = generationManager;
    }

    void GenerationManager::RegisterBehaviorsImplement()
    {
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    namespace Socket
    {
        void SendWorkspaceSettings();
    }

    const wchar_t* Workspace::GetWorkspaceAbsolutePath()
    {
        this->TryInit();

        return m_workspace_file;
    }

    Workspace* Workspace::ms_instance = 0;

    Workspace::Workspace() : m_bInited(false), m_bExecAgents(true), m_fileFormat(Workspace::EFF_xml), m_frame(0),
        m_pBehaviorNodeLoader(0), m_behaviortreeCreators(0),
        m_fileBuffer(0), m_fileBufferTop(0), m_timeSinceStartup(0),
        m_deltaTime(0.0167f), m_deltaFrames(1)
    {
#if BEHAVIAC_ENABLE_HOTRELOAD
        m_AutoHotReload = true;
        m_allBehaviorTreeTasks = 0;
#endif//BEHAVIAC_ENABLE_HOTRELOAD
        memset(this->m_fileBufferOffset, 0, sizeof(m_fileBufferOffset));
        memset(this->m_workspace_file, 0, sizeof(m_workspace_file));
        //memset(m_szWorkspaceExportPath, 0, sizeof(m_szWorkspaceExportPath));
        string_cpy(m_szWorkspaceExportPath, "./behaviac/workspace/exported/");
		this->m_fileBufferLength = 0;

        BEHAVIAC_ASSERT(ms_instance == 0);
        ms_instance = this;
    }

    Workspace::~Workspace()
    {
        ms_instance = 0;
    }

    Workspace* Workspace::GetInstance()
    {
        if (ms_instance == NULL)
        {
            //if new  an Workspace class ,then the staict variable will be set value
            Workspace* _workspace = BEHAVIAC_NEW Workspace();
            BEHAVIAC_UNUSED_VAR(_workspace);
            BEHAVIAC_ASSERT(ms_instance != NULL);
        }

        return ms_instance;
    }

    bool Workspace::LoadWorkspaceSetting(const char* file, behaviac::string& workspaceRootPath)
    {
        char* pBuffer = Workspace::ReadFileToBuffer(file);

        if (pBuffer)
        {
            rapidxml::xml_document<> doc;

            doc.parse<0>(pBuffer);

            rapidxml::xml_node<>* nodeWorkspace = doc.first_node("workspace");

            if (!nodeWorkspace)
            {
                BEHAVIAC_DELETE(pBuffer);
                return false;
            }

            if (rapidxml::xml_attribute<>* attrName = nodeWorkspace->first_attribute("path"))
            {
                workspaceRootPath = attrName->value();
            }

            Workspace::PopFileFromBuffer(pBuffer);

            return true;
        }

        return false;
    }

    const char* Workspace::GetFilePath() const
    {
        return  m_szWorkspaceExportPath;// "./behaviac/workspace/exported/";
    }

    void Workspace::SetFilePath(const char* szExportPath)
    {
        string_ncpy(this->m_szWorkspaceExportPath, szExportPath, kMaxPath);
		BEHAVIAC_LOGINFO(m_szWorkspaceExportPath);
    }

    Workspace::EFileFormat Workspace::GetFileFormat()
    {
        return m_fileFormat;
    }

    void Workspace::SetFileFormat(EFileFormat ff)
    {
        this->m_fileFormat = ff;
    }

    void Workspace::BehaviorNodeLoaded(const char* nodeType, const properties_t& properties)
    {
        if (this->m_pBehaviorNodeLoader)
        {
            this->m_pBehaviorNodeLoader(nodeType, properties);
        }
    }

    void Workspace::HandleFileFormat(const behaviac::string& fullPath, behaviac::string& ext, Workspace::EFileFormat& f)
    {
        if (f == EFF_default)
        {
            // try to load the behavior in xml
            ext = ".xml";

            if (behaviac::CFileManager::GetInstance()->FileExists(fullPath, ext))
            {
                f = EFF_xml;
            }
            else
            {
                // try to load the behavior in bson
                ext = ".bson";

                if (behaviac::CFileManager::GetInstance()->FileExists(fullPath, ext))
                {
                    f = EFF_bson;
                }
                else
                {
                    // try to load the behavior in cs
                    f = EFF_cpp;
                }
            }
        }
        else if (f == EFF_xml || f == EFF_cpp)
        {
            ext = ".xml";

        }
        else if (f == EFF_bson)
        {
            ext = ".bson.bytes";
        }
    }

    bool Workspace::TryInit()
    {
        if (this->m_bInited)
        {
            return true;
        }

        this->m_bInited = true;

        bool bOk = TryStart();

        if (!bOk)
        {
            return false;
        }

        if (this->GetFileFormat() == EFF_cpp || this->GetFileFormat() == EFF_default)
        {
            GenerationManager::RegisterBehaviors();
        }

        const char* szWorkspaceExportPath = this->GetFilePath();

        if (StringUtils::IsNullOrEmpty(szWorkspaceExportPath))
        {
            LogManager::GetInstance()->Error("No 'WorkspaceExportPath' is specified!");
            BEHAVIAC_ASSERT(false);

            return false;
        }

        BEHAVIAC_LOGINFO("'WorkspaceExportPath' is '%s'\n", szWorkspaceExportPath);

        //BEHAVIAC_ASSERT(!StringUtils::EndsWith(szWorkspaceExportPath, "\\"), "use '/' instead of '\\'");

        LoadWorkspaceAbsolutePath();

		m_deltaTime = 0.0167f;
        m_deltaFrames = 1;

#if BEHAVIAC_ENABLE_HOTRELOAD
        behaviac::wstring dir = behaviac::StringUtils::Char2Wide(szWorkspaceExportPath);
        CFileSystem::StartMonitoringDirectory(dir.c_str());
#endif//BEHAVIAC_ENABLE_HOTRELOAD

        //////////////////////////////////////////////////////////
        //this->RegisterStuff();
        AgentProperties::RegisterCustomizedTypes();

        AgentProperties::Load();

#if !BEHAVIAC_RELEASE
#if BEHAVIAC_HOTRELOAD

        // set the file watcher
        if (Config.IsDesktop)
        {
            if (this->GetFileFormat() != EFF_cs)
            {
                if (m_DirectoryMonitor == null)
                {
                    m_DirectoryMonitor = new DirectoryMonitor();
                    m_DirectoryMonitor.Changed += new DirectoryMonitor.FileSystemEvent(OnFileChanged);
                }

                string filter = "*.*";

                if (this->GetFileFormat() == EFF_xml)
                {
                    filter = "*.xml";

                }
                else if (this->GetFileFormat() == EFF_bson)
                {
                    filter = "*.bson.bytes";
                }

                m_DirectoryMonitor.Start(this->WorkspaceExportPath, filter);
            }
        }

#endif//BEHAVIAC_HOTRELOAD

        LogWorkspaceInfo();
#endif

        return true;
    }
    /*
    Log the information of workspace info
    */
    void Workspace::LogWorkspaceInfo()
    {
        const wchar_t* wksAbsPath = this->GetWorkspaceAbsolutePath();

        if (wksAbsPath)
        {
            Workspace::EFileFormat format = this->GetFileFormat();
            const char* formatString = (format == EFF_xml ? "xml" : "bson");

            char msg[1024] = { 0 };
            sprintf(msg, "[workspace] %s \"%S\"\n", formatString, wksAbsPath);
            LogManager::GetInstance()->LogWorkspace(false, msg);
        }
    }

    void Workspace::LoadWorkspaceAbsolutePath()
    {
#if !BEHAVIAC_RELEASE

        if (Config::IsLoggingOrSocketing())
        {
            //relative to exe's current path
            const char* workspaceExportPath = this->GetFilePath();

            //workspaceExportPath is the path to the export:
            //like: ..\example\spaceship\data\bt\exported
            behaviac::string fullPath = StringUtils::CombineDir(workspaceExportPath, "behaviors.dbg.xml");
            behaviac::string workspaceFilePathRelative;
            bool bOk = LoadWorkspaceSetting(fullPath.c_str(), workspaceFilePathRelative);

            if (bOk)
            {
                //workspaceFilePathRelative stored in behaviors.dbg.xml is the path relative to export
                //convert it to the full path
                wchar_t* workspaceRootPath = this->m_workspace_file;
                workspaceRootPath[0] = '\0';

                if (behaviac::CFileManager::GetInstance()->PathIsRelative(workspaceExportPath))
                {
                    const behaviac::wstring currentWD = behaviac::CFileManager::GetInstance()->GetCurrentWorkingDirectory();
                    int len = currentWD.size();
                    BEHAVIAC_ASSERT(len + 1 < kMaxPath, "path is too long !");
                    wcscpy(workspaceRootPath, currentWD.c_str());
                    wchar_t last = workspaceRootPath[len - 1];

                    if (last != L'/' && last != L'\\')
                    {
                        workspaceRootPath[len] = L'/';
                        workspaceRootPath[len + 1] = L'\0';
                    }

                    m_workspaceExportPathAbs = workspaceRootPath;

                    //{
                    //    uint32_t p = m_workspaceExportPathAbs.find_last_of(STRING2WSTRING("Assets"));

                    //    if (p != (uint32_t)-1) {
                    //        m_workspaceExportPathAbs = m_workspaceExportPathAbs.substr(0, p);
                    //    }
                    //}

                    m_workspaceExportPathAbs = StringUtils::CombineDir(m_workspaceExportPathAbs.c_str(), STRING2WSTRING(workspaceExportPath).c_str());
                }
                else
                {
                    m_workspaceExportPathAbs = STRING2WSTRING(workspaceExportPath);
                }

                m_workspaceExportPathAbs = StringUtils::CombineDir(m_workspaceExportPathAbs.c_str(), STRING2WSTRING(workspaceFilePathRelative).c_str());

                //ms_workspace_file = m_workspaceExportPathAbs+workspaceFilePathRelative;
                wcscpy(m_workspace_file, m_workspaceExportPathAbs.c_str());
            }
            else
            {
            }
        }

#endif
    }

    void Workspace::SetTimeSinceStartup(float timeSinceStartup)
    {
        //BEHAVIAC_ASSERT(timeSinceStartup >= 0.0f);
        m_timeSinceStartup = timeSinceStartup;
    }

    float Workspace::GetTimeSinceStartup()
    {
        return m_timeSinceStartup;
    }

    void Workspace::SetDeltaFrameTime(float deltaTime)
    {
        //BEHAVIAC_ASSERT(deltaTime > 0.0f);
        m_deltaTime = deltaTime;
    }

    float Workspace::GetDeltaFrameTime()
    {
        return m_deltaTime;
    }

    void Workspace::SetDeltaFrames(int deltaFrames)
    {
        //BEHAVIAC_ASSERT(deltaFrames >= 0);
        m_deltaFrames = deltaFrames;
    }

    int Workspace::GetDeltaFrames()
    {
        return m_deltaFrames;
    }

    bool Workspace::ExportMetas(const char* xmlMetaFilePath)
    {
        bool result = Agent::ExportMetas(xmlMetaFilePath);

        return result;
    }

    void Workspace::Cleanup()
    {
#if BEHAVIAC_ENABLE_HOTRELOAD

        if (m_allBehaviorTreeTasks)
        {
			//BehaviorTreeTasks will be freed by Agent
			//for (AllBehaviorTreeTasks_t::iterator it = m_allBehaviorTreeTasks->begin(); it != m_allBehaviorTreeTasks->end(); ++it)
			//{
			//	BTItem_t& btItems = it->second;

			//	for (behaviac::vector<BehaviorTreeTask*>::iterator it1 = btItems.bts.begin(); it1 != btItems.bts.end(); ++it1)
			//	{
			//		BehaviorTreeTask* bt = *it1;

			//		BehaviorTask::DestroyTask(bt);
			//	}
			//}

            m_allBehaviorTreeTasks->clear();
            BEHAVIAC_DELETE m_allBehaviorTreeTasks;
            m_allBehaviorTreeTasks = NULL;
        }

        CFileSystem::StopMonitoringDirectory();
#endif//BEHAVIAC_ENABLE_HOTRELOAD
        AgentProperties::UnRegisterCustomizedTypes();

        UnRegisterBehaviorTreeCreators();

        Agent::Cleanup();

        this->UnLoadAll();
        this->UnRegisterBasicNodes();

        Workspace::FreeFileBuffer();
        BaseStop();

        this->m_bInited = false;
    }

    bool Workspace::RegisterBehaviorTreeCreator(const char* relativePath, BehaviorTreeCreator_t creator)
    {
        if (relativePath != NULL)
        {
            if (m_behaviortreeCreators == NULL)
            {
                m_behaviortreeCreators = BEHAVIAC_NEW BehaviorTreeCreators_t();
            }

            (*m_behaviortreeCreators)[relativePath] = creator;

            return true;
        }

        return false;
    }

    void Workspace::UnRegisterBehaviorTreeCreators()
    {
        if (m_behaviortreeCreators)
        {
            m_behaviortreeCreators->clear();
            BEHAVIAC_DELETE m_behaviortreeCreators;
            m_behaviortreeCreators = NULL;
        }
    }

    const Workspace::BehaviorTrees_t& Workspace::GetBehaviorTrees()
    {
        return m_behaviortrees;
    }

    void Workspace::FreeFileBuffer()
    {
        if (m_fileBuffer)
        {
            BEHAVIAC_FREE(m_fileBuffer);
            m_fileBuffer = 0;
            m_fileBufferLength = 0;
        }

        for (int i = 0; i < kFileBufferDepth; ++i)
        {
            m_fileBufferOffset[i] = 0;
        }

        m_fileBufferTop = 0;
    }

    char* Workspace::ReadFileToBuffer(const char* file, const char* ext)
    {
        char path[1024];
        sprintf(path, "%s%s", file, ext);
        return ReadFileToBuffer(path);
    }

    char* Workspace::ReadFileToBuffer(const char* file)
    {
        IFile* fp = behaviac::CFileManager::GetInstance()->FileOpen(file, CFileSystem::EOpenAccess_Read);

        if (!fp)
        {
            return 0;
        }

        //fp->Seek(0, CFileSystem::ESeekMoveMode_End);
        uint32_t fileSize = (uint32_t)fp->GetSize();

		BEHAVIAC_ASSERT(m_fileBufferTop < kFileBufferDepth - 1, "please increase kFileBufferDepth");
        uint32_t offset = m_fileBufferOffset[m_fileBufferTop++];
        uint32_t offsetNew = offset + fileSize + 1;
        BEHAVIAC_ASSERT(m_fileBufferTop < kFileBufferDepth - 1, "please increase kFileBufferDepth");
        m_fileBufferOffset[m_fileBufferTop] = offsetNew;

        if (m_fileBuffer == 0 || offsetNew > m_fileBufferLength)
        {
            //to allocate extra 10k
            m_fileBufferLength = offsetNew + 10 * 1024;

            if (m_fileBufferLength < 50 * 1024)
            {
                m_fileBufferLength = 50 * 1024;
            }

            m_fileBuffer = (char*)BEHAVIAC_REALLOC(m_fileBuffer, m_fileBufferLength);
        }

        BEHAVIAC_ASSERT(m_fileBuffer);
        BEHAVIAC_ASSERT(offsetNew < m_fileBufferLength);

        char* pBuffer = m_fileBuffer + offset;

        fp->Read(pBuffer, sizeof(char) * fileSize);
        pBuffer[fileSize] = 0;

        behaviac::CFileManager::GetInstance()->FileClose(fp);

        return pBuffer;
    }

    bool Workspace::PopFileFromBuffer(const char* file, const char* str, char* pBuffer)
    {
        BEHAVIAC_UNUSED_VAR(file);
        BEHAVIAC_UNUSED_VAR(str);
        BEHAVIAC_UNUSED_VAR(pBuffer);
        //BEHAVIAC_ASSERT(0, "the code in c-sharp not implement.");
		this->PopFileFromBuffer(pBuffer);

        return false;
    }

    bool Workspace::IsExecAgents() const
    {
        return this->m_bExecAgents;
    }

    void Workspace::SetIsExecAgents(bool bExecAgents)
    {
        this->m_bExecAgents = bExecAgents;
    }


	void Workspace::DebugUpdate()
	{
		this->LogFrames();
		this->HandleRequests();

		if (this->GetAutoHotReload())
		{
			this->HotReload();
		}
	}

    void Workspace::Update()
    {
		this->DebugUpdate();

        if (this->m_bExecAgents)
        {
            int contextId = -1;

            Context::execAgents(contextId);
        }
    }

    void Workspace::LogCurrentStates()
    {
        int contextId = -1;
        Context::LogCurrentStates(contextId);
    }

    void Workspace::PopFileFromBuffer(char* pBuffer)
    {
        BEHAVIAC_UNUSED_VAR(pBuffer);
        BEHAVIAC_ASSERT(m_fileBufferTop < kFileBufferDepth - 1 && m_fileBufferTop > 0);
        BEHAVIAC_DEBUGCODE(uint32_t offset = pBuffer - m_fileBuffer);

        BEHAVIAC_DEBUGCODE(uint32_t offset_recorded = m_fileBufferOffset[m_fileBufferTop - 1]);
        BEHAVIAC_DEBUGCODE(BEHAVIAC_ASSERT(offset == offset_recorded));

        m_fileBufferOffset[m_fileBufferTop] = 0;
        m_fileBufferTop--;
    }

    bool IsValidPath(const char* relativePath)
    {
        BEHAVIAC_ASSERT(relativePath);

        if (relativePath[0] == '.' && (relativePath[1] == '/' || relativePath[1] == '\\'))
        {
            // ./dummy_bt
            return false;

        }
        else if (relativePath[0] == '/' || relativePath[0] == '\\')
        {
            // /dummy_bt
            return false;
        }

        return true;
    }

    bool Workspace::Load(const char* relativePath, bool bForce)
    {
        this->TryInit();

        //BEHAVIAC_ASSERT(behaviac::StringUtils::FindExtension(relativePath) == 0, "no extention to specify");
        BEHAVIAC_ASSERT(IsValidPath(relativePath));

        BehaviorTree* pBT = 0;
        BehaviorTrees_t::iterator it = m_behaviortrees.find(relativePath);

        if (it != m_behaviortrees.end())
        {
            if (!bForce)
            {
                return true;
            }

            pBT = it->second;
        }

        behaviac::string fullPath = StringUtils::CombineDir(this->GetFilePath(), relativePath);

        Workspace::EFileFormat f = this->GetFileFormat();

        switch (f)
        {
            case EFF_default:
            {
                // try to load the behavior in xml
                behaviac::string path = fullPath + ".xml";

                if (behaviac::CFileManager::GetInstance()->FileExists(path.c_str()))
                {
                    f = EFF_xml;
                    fullPath = path;
                }
                else
                {
                    // try to load the behavior in bson
                    path = fullPath + ".bson.bytes";

                    if (behaviac::CFileManager::GetInstance()->FileExists(path.c_str()))
                    {
                        f = EFF_bson;
                        fullPath = path;
                    }
                    // try to load the behavior in cpp
                    else
                    {
                        f = EFF_cpp;
                    }
                }
            }
            break;

            case EFF_xml:
                fullPath += ".xml";
                break;

            case EFF_bson:
                fullPath += ".bson.bytes";
                break;

            case EFF_cpp:
                break;

            default:
                BEHAVIAC_ASSERT(0);
                break;
        }

        bool bLoadResult = false;
        bool bNewly = false;

        if (!pBT)
        {
            //in case of circular referencebehavior
            bNewly = true;
            pBT = BEHAVIAC_NEW BehaviorTree();
            m_behaviortrees[relativePath] = pBT;
        }

        BEHAVIAC_ASSERT(pBT);

        bool bCleared = false;

        if (f == EFF_xml || f == EFF_bson)
        {
            char* pBuffer = ReadFileToBuffer(fullPath.c_str());

            if (pBuffer)
            {
                //if forced to reload
                if (!bNewly)
                {
                    bCleared = true;
                    pBT->Clear();
                }

                if (f == EFF_xml)
                {
                    bLoadResult = pBT->load_xml(pBuffer);
                }
                else
                {
                    bLoadResult = pBT->load_bson(pBuffer);
                }

                PopFileFromBuffer(pBuffer);
            }
            else
            {
                BEHAVIAC_LOGERROR("'%s' doesn't exist!, Please check the file name or override Workspace and its GetFilePath()\n", fullPath.c_str());
                BEHAVIAC_ASSERT(false);
            }
        }
        else if (f == EFF_cpp)
        {
            if (!bNewly)
            {
                bCleared = true;
                pBT->Clear();
            }

            if (m_behaviortreeCreators && m_behaviortreeCreators->find(relativePath) != m_behaviortreeCreators->end())
            {
                BehaviorTreeCreator_t btCreator = (*m_behaviortreeCreators)[relativePath];
                bLoadResult = (*btCreator)(pBT);
            }
            else
            {
                BEHAVIAC_ASSERT(0);
                BEHAVIAC_LOGWARNING("The behaviac_generated/behaviors/generated_behaviors.h should be included by one of your apps.");
            }
        }
        else
        {
            BEHAVIAC_ASSERT(0);
        }

        if (bLoadResult)
        {
            BEHAVIAC_ASSERT(pBT->GetName() == relativePath);

            if (!bNewly)
            {
                BEHAVIAC_ASSERT(m_behaviortrees[pBT->GetName()] == pBT);
            }
        }
        else
        {
            if (bNewly)
            {
                //if it is forced to reload
                m_behaviortrees.erase(relativePath);

                BEHAVIAC_DELETE(pBT);
            }
            else if (bCleared)
            {
                //it has been cleared but failed to load, to remove it
                m_behaviortrees.erase(relativePath);
            }

            BEHAVIAC_LOGWARNING("'%s' is not loaded!\n", fullPath.c_str());
        }

        return bLoadResult;
    }

    BehaviorTree* Workspace::LoadBehaviorTree(const char* relativePath)
    {
        behaviac::string strRelativePath(relativePath);

        if (m_behaviortrees[strRelativePath] != NULL)
        {
            return m_behaviortrees[strRelativePath];
        }
        else
        {
            bool bOk = this->Load(relativePath, true);

            if (bOk)
            {
                return m_behaviortrees[strRelativePath];
            }
        }

        return NULL;
    }
    BehaviorTreeTask* Workspace::CreateBehaviorTreeTask(const char* relativePath)
    {
        BEHAVIAC_ASSERT(behaviac::StringUtils::FindExtension(relativePath) == 0, "no extention to specify");
        BEHAVIAC_ASSERT(IsValidPath(relativePath));

        BehaviorTrees_t::iterator it = m_behaviortrees.find(relativePath);
        const BehaviorTree* bt = 0;

        if (it != m_behaviortrees.end())
        {
            bt = it->second;
        }
        else
        {
            bool bOk = (Workspace::Load(relativePath));

            if (bOk)
            {
                BehaviorTrees_t::iterator it = m_behaviortrees.find(relativePath);

                if (it != m_behaviortrees.end())
                {
                    bt = it->second;
                }
            }
        }

        if (bt)
        {
            BehaviorTask* task = bt->CreateAndInitTask();

            BEHAVIAC_ASSERT(BehaviorTreeTask::DynamicCast(task));
            BehaviorTreeTask* behaviorTreeTask = (BehaviorTreeTask*)task;

#if BEHAVIAC_ENABLE_HOTRELOAD

            if (!m_allBehaviorTreeTasks)
            {
                m_allBehaviorTreeTasks = BEHAVIAC_NEW AllBehaviorTreeTasks_t;
            }

            AllBehaviorTreeTasks_t::iterator it = m_allBehaviorTreeTasks->find(relativePath);

            if (it == m_allBehaviorTreeTasks->end())
            {
                (*m_allBehaviorTreeTasks)[relativePath] = BTItem_t();
            }

            BTItem_t& btItems = (*m_allBehaviorTreeTasks)[relativePath];

            bool isAdded = false;

            for (uint32_t i = 0; i < btItems.bts.size(); ++i)
            {
                if (btItems.bts[i] == behaviorTreeTask)
                {
                    isAdded = true;
                    break;
                }
            }

            if (!isAdded)
            {
                btItems.bts.push_back(behaviorTreeTask);
            }

#endif//BEHAVIAC_ENABLE_HOTRELOAD

            return behaviorTreeTask;
        }

        return 0;
    }

    void Workspace::DestroyBehaviorTreeTask(BehaviorTreeTask* behaviorTreeTask, Agent* agent)
    {
        BEHAVIAC_UNUSED_VAR(agent);

        if (behaviorTreeTask)
        {
#if BEHAVIAC_ENABLE_HOTRELOAD

            if (m_allBehaviorTreeTasks)
            {
                const char* relativePath = behaviorTreeTask->GetName().c_str();
                AllBehaviorTreeTasks_t::iterator it = m_allBehaviorTreeTasks->find(relativePath);

                if (it != m_allBehaviorTreeTasks->end())
                {
                    BTItem_t& btItems = (*m_allBehaviorTreeTasks)[relativePath];

                    for (behaviac::vector<BehaviorTreeTask*>::iterator it1 = btItems.bts.begin(); it1 != btItems.bts.end(); ++it1)
                    {
                        BehaviorTreeTask* bt = *it1;

                        if (bt == behaviorTreeTask)
                        {
                            btItems.bts.erase(it1);
                            break;
                        }
                    }

                    if (agent)
                    {
                        for (behaviac::vector<Agent*>::iterator it1 = btItems.agents.begin(); it1 != btItems.agents.end(); ++it1)
                        {
                            Agent* a = (*it1);

                            if (agent == a)
                            {
                                btItems.agents.erase(it1);
                                break;
                            }
                        }
                    }
                }
            }

#endif//BEHAVIAC_ENABLE_HOTRELOAD

            BehaviorTask::DestroyTask(behaviorTreeTask);
        }
    }

    void Workspace::RecordBTAgentMapping(const char* relativePath, Agent* agent)
    {
        BEHAVIAC_UNUSED_VAR(relativePath);
        BEHAVIAC_UNUSED_VAR(agent);
#if BEHAVIAC_ENABLE_HOTRELOAD

        if (!m_allBehaviorTreeTasks)
        {
            m_allBehaviorTreeTasks = BEHAVIAC_NEW AllBehaviorTreeTasks_t;
        }

        AllBehaviorTreeTasks_t::iterator it = m_allBehaviorTreeTasks->find(relativePath);

        if (it == m_allBehaviorTreeTasks->end())
        {
            (*m_allBehaviorTreeTasks)[relativePath] = BTItem_t();
        }

        BTItem_t& btItems = (*m_allBehaviorTreeTasks)[relativePath];
        bool bFound = false;

        for (behaviac::vector<Agent*>::iterator it1 = btItems.agents.begin(); it1 != btItems.agents.end(); ++it1)
        {
            Agent* a = (*it1);

            if (agent == a)
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            btItems.agents.push_back(agent);
        }

#endif//BEHAVIAC_ENABLE_HOTRELOAD
    }

    void Workspace::UnLoad(const char* relativePath)
    {
        BEHAVIAC_ASSERT(behaviac::StringUtils::FindExtension(relativePath) == 0, "no extention to specify");
        BEHAVIAC_ASSERT(IsValidPath(relativePath));

        BehaviorTrees_t::iterator it = m_behaviortrees.find(relativePath);

        if (it != m_behaviortrees.end())
        {
            BehaviorTree* bt = it->second;
            BEHAVIAC_DELETE(bt);
            m_behaviortrees.erase(it);
        }
    }

    void Workspace::UnLoadAll()
    {
        for (BehaviorTrees_t::iterator it = m_behaviortrees.begin();
             it != m_behaviortrees.end(); ++it)
        {
            BehaviorTree* bt = it->second;
            BEHAVIAC_DELETE(bt);
        }

        m_behaviortrees.clear();

        m_workspace_file[0] = '\0';

		AgentProperties::UnloadLocals();
    }

    void Workspace::SetAutoHotReload(bool enable)
    {
        BEHAVIAC_UNUSED_VAR(enable);
#if BEHAVIAC_ENABLE_HOTRELOAD
        m_AutoHotReload = enable;
#endif//BEHAVIAC_ENABLE_HOTRELOAD
    }

    bool Workspace::GetAutoHotReload()
    {
#if BEHAVIAC_ENABLE_HOTRELOAD
        return m_AutoHotReload;
#else
        return false;
#endif//BEHAVIAC_ENABLE_HOTRELOAD
    }

    void Workspace::HotReload()
    {
#if BEHAVIAC_ENABLE_HOTRELOAD

        if (!m_allBehaviorTreeTasks)
        {
            return;
        }

        behaviac::vector<behaviac::string> modifiedFiles;
        CFileSystem::GetModifiedFiles(modifiedFiles);
        uint32_t fileCount = modifiedFiles.size();

        if (fileCount > 0)
        {
            Workspace::EFileFormat f = Workspace::GetFileFormat();

            for (uint32_t i = 0; i < fileCount; ++i)
            {
				behaviac::string relativePath = modifiedFiles[i];

                const char* format = behaviac::StringUtils::FindFullExtension(relativePath.c_str());

                if (format != 0 && (((f & EFF_xml) == EFF_xml && 0 == strcmp(format, "xml")) ||
                                    ((f & EFF_bson) == EFF_bson && 0 == strcmp(format, "bson.bytes"))))
                {
                    behaviac::StringUtils::StripFullFileExtension(relativePath);
                    behaviac::StringUtils::UnifySeparator(relativePath);

                    AllBehaviorTreeTasks_t::iterator it = m_allBehaviorTreeTasks->find(relativePath);

                    if (it != m_allBehaviorTreeTasks->end())
                    {
                        if (Workspace::Load(relativePath.c_str(), true))
                        {
                            //BEHAVIAC_LOGWARNING("HotReload 1:%s\n", relativePath.c_str());

                            BTItem_t& btItems = (*m_allBehaviorTreeTasks)[relativePath];
                            BehaviorTree* behaviorTree = m_behaviortrees[relativePath];

                            uint32_t taskCount = btItems.bts.size();

                            if (taskCount > 0)
                            {
                                for (uint32_t i = 0; i < taskCount; ++i)
                                {
                                    BehaviorTreeTask* behaviorTreeTask = btItems.bts[i];
                                    BEHAVIAC_ASSERT(behaviorTreeTask);

                                    //BEHAVIAC_LOGWARNING("HotReload 2:%s\n", behaviorTreeTask->GetName().c_str());

                                    behaviorTreeTask->reset(0);
                                    behaviorTreeTask->Clear();
                                    behaviorTreeTask->Init(behaviorTree);
                                }
                            }

                            for (behaviac::vector<Agent*>::iterator it1 = btItems.agents.begin(); it1 != btItems.agents.end(); ++it1)
                            {
                                Agent* agent = (*it1);

                                //BEHAVIAC_LOGWARNING("HotReload 3:%s\n", agent->GetName().c_str());
                                agent->bthotreloaded(behaviorTree);
                            }
                        }
                    }
                }
            }
        }

#endif//BEHAVIAC_ENABLE_HOTRELOAD
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    /*uint32_t						Workspace::m_frame;
    behaviac::string				Workspace::m_applogFilter;

    Workspace::BreakpointInfos_t	Workspace::m_breakpoints;

    Workspace::ActionCount_t		Workspace::m_actions_count;

    behaviac::Mutex					Workspace::m_cs;*/

    //[breakpoint] add TestBehaviorGroup\btunittest.xml->Sequence[3]:enter all Hit=1
    //[breakpoint] add TestBehaviorGroup\btunittest.xml->Sequence[3]:exit all Hit=1
    //[breakpoint] add TestBehaviorGroup\btunittest.xml->Sequence[3]:exit success Hit=1
    //[breakpoint] add TestBehaviorGroup\btunittest.xml->Sequence[3]:exit failure Hit=1
    //[breakpoint] remove TestBehaviorGroup\btunittest.x1ml->Sequence[3]:enter all Hit=10
    void Workspace::ParseBreakpoint(const behaviac::vector<behaviac::string>& tokens)
    {
        BreakpointInfo_t bp;

        bool bAdd = false;
        bool bRemove = false;

        if (tokens[1] == "add")
        {
            bAdd = true;

        }
        else if (tokens[1] == "remove")
        {
            bRemove = true;

        }
        else
        {
            BEHAVIAC_ASSERT(0);
        }

        bp.btname = tokens[2];

        if (tokens[3] == "all")
        {
            BEHAVIAC_ASSERT(bp.action_result == EAR_all);

        }
        else if (tokens[3] == "success")
        {
            bp.action_result = EAR_success;

        }
        else if (tokens[3] == "failure")
        {
            bp.action_result = EAR_failure;

        }
        else
        {
            BEHAVIAC_ASSERT(0);
        }

        const char* kHitNumber = "Hit=";
        behaviac::string::size_type posb = tokens[4].find(kHitNumber);

        if (posb != behaviac::string::npos)
        {
            posb = tokens[4].find('=');
            BEHAVIAC_ASSERT(posb != (unsigned int) - 1);

            behaviac::string::size_type size = behaviac::string::npos;
            //tokens[4] is the last one with '\n'
            behaviac::string::size_type pose = tokens[4].find('\n');

            if (pose != behaviac::string::npos)
            {
                size = pose - posb - 1;
            }

            behaviac::string numString = tokens[4].substr(posb + 1, size);
            bp.hit_config = (unsigned short)atoi(numString.c_str());
        }

        uint32_t bpid = MakeVariableId(bp.btname.c_str());

        if (bAdd)
        {
            m_breakpoints[bpid] = bp;

        }
        else if (bRemove)
        {
            m_breakpoints.erase(bpid);
        }
    }

    void Workspace::ParseProfiling(const behaviac::vector<behaviac::string>& tokens)
    {
        if (tokens[1] == "true")
        {
            Config::SetProfiling(true);

        }
        else if (tokens[1] == "false")
        {
            Config::SetProfiling(false);

        }
        else
        {
            BEHAVIAC_ASSERT(0);
        }
    }

    void Workspace::ParseAppLogFilter(const behaviac::vector<behaviac::string>& tokens)
    {
        m_applogFilter = tokens[1];
    }

    //[property] WorldState::WorldState int WorldState::time->185606213
    //[property] Ship::Ship_2_3 long GameObject::age->91291
    //[property] Ship::Ship_2_3 bool par_a->true
    void Workspace::ParseProperty(const behaviac::vector<behaviac::string>& tokens)
    {
        BEHAVIAC_UNUSED_VAR(tokens);
#if !defined(BEHAVIAC_RELEASE)
        const behaviac::string& agentName = tokens[1];
        Agent* pAgent = Agent::GetAgent(agentName.c_str());

        //pAgent could be 0
        if (pAgent && tokens.size() == 4)
        {
            //const behaviac::string& varTypeName = tokens[2];
            const behaviac::string& varNameValue = tokens[3];

            behaviac::string::size_type posb = varNameValue.find("->");
            BEHAVIAC_ASSERT(posb != behaviac::string::npos);

            if (posb != behaviac::string::npos)
            {
                behaviac::string::size_type size = behaviac::string::npos;
                //varNameValue is the last one with '\n'
                behaviac::string::size_type pose = varNameValue.find('\n');

                if (pose != behaviac::string::npos)
                {
                    size = pose - posb - 1;
                }

                behaviac::string varName = varNameValue.substr(0, posb);
                behaviac::string varValue = varNameValue.substr(posb + 2, size);

                if (pAgent)
                {
                    pAgent->SetVariableFromString(varName.c_str(), varValue.c_str());
                }//end of if (pAgent)
            }
        }

#endif
    }

    void Workspace::LogFrames()
    {
        LogManager::GetInstance()->Log("[frame]%d\n", m_frame++);
    }

    void Workspace::WaitforContinue()
    {
#if !defined(BEHAVIAC_RELEASE)

        while (!HandleRequests())
        {
            behaviac::Thread::Sleep(200);
        }

#endif//BEHAVIAC_RELEASE
    }

    bool Workspace::HandleRequests()
    {
        bool bContinue = false;

#if !defined(BEHAVIAC_RELEASE)

        if (Config::IsSocketing())
        {
            behaviac::string command;

            if (Socket::ReadText(command))
            {
                const char* kBreakpoint = "[breakpoint]";
                const char* kProperty = "[property]";
                const char* kProfiling = "[profiling]";
                const char* kStart = "[start]";
                const char* kAppLogFilter = "[applogfilter]";
                const char* kContinue = "[continue]";
                const char* kCloseConnection = "[closeconnection]";

                behaviac::vector<behaviac::string> cs;
                behaviac::StringUtils::SplitIntoArray(command.c_str(), "\n", cs);

                for (behaviac::vector<behaviac::string>::iterator it = cs.begin(); it != cs.end(); ++it)
                {
                    behaviac::string& c = *it;

                    if (c.empty())
                    {
                        continue;
                    }

                    behaviac::vector<behaviac::string> tokens;
                    behaviac::StringUtils::SplitIntoArray(c.c_str(), " ", tokens);

                    if (tokens[0] == kBreakpoint)
                    {
                        ParseBreakpoint(tokens);
                    }
                    else if (tokens[0] == kProperty)
                    {
                        ParseProperty(tokens);
                    }
                    else if (tokens[0] == kProfiling)
                    {
                        ParseProfiling(tokens);
                    }
                    else if (tokens[0] == kStart)
                    {
                        m_breakpoints.clear();
                        bContinue = true;
                    }
                    else if (tokens[0] == kAppLogFilter)
                    {
                        ParseAppLogFilter(tokens);
                    }
                    else if (tokens[0] == kContinue)
                    {
                        bContinue = true;
                    }
                    else if (tokens[0] == kCloseConnection)
                    {
                        m_breakpoints.clear();
                        bContinue = true;
                    }
                    else
                    {
                        BEHAVIAC_ASSERT(0);
                    }
                }//end of for
            }//end of if (Socket::ReadText(command))

        }
        else
        {
            bContinue = true;
        }

#endif//BEHAVIAC_RELEASE

        return bContinue;
    }

#if !defined(BEHAVIAC_RELEASE)
    //behaviac::string GetTickInfo(const behaviac::Agent* pAgent, const behaviac::BehaviorTask* b, const char* action);
#endif//#if !defined(BEHAVIAC_RELEASE)

    bool Workspace::CheckBreakpoint(const behaviac::Agent* pAgent, const behaviac::BehaviorNode* b, const char* action, EActionResult actionResult)
    {
        BEHAVIAC_UNUSED_VAR(pAgent);
        BEHAVIAC_UNUSED_VAR(b);
        BEHAVIAC_UNUSED_VAR(action);
        BEHAVIAC_UNUSED_VAR(actionResult);
#if !defined(BEHAVIAC_RELEASE)

        if (Config::IsSocketing())
        {
#if BEHAVIAC_ENABLE_PROFILING
            BEHAVIAC_PROFILE("Workspace::CheckBreakpoint");
#endif
            behaviac::string bpStr = BehaviorTask::GetTickInfo(pAgent, b, action);

            uint32_t bpid = MakeVariableId(bpStr.c_str());

            BreakpointInfos_t::const_iterator it = m_breakpoints.find(bpid);

            if (it != m_breakpoints.end())
            {
                const BreakpointInfo_t& bp = it->second;

                if (bp.action_result & actionResult)
                {
                    int count = GetActionCount(bpStr.c_str());
                    BEHAVIAC_ASSERT(count > 0);

                    if (bp.hit_config == 0 || bp.hit_config == count)
                    {
                        return true;
                    }
                }
            }
        }

#endif//#if !defined(BEHAVIAC_RELEASE)
        return false;
    }

    bool Workspace::CheckAppLogFilter(const char* filter)
    {
        if (Config::IsSocketing())
        {
            //m_applogFilter is UPPER
            if (!m_applogFilter.empty())
            {
                if (m_applogFilter == "ALL")
                {
                    return true;

                }
                else
                {
                    behaviac::string f = filter;
                    f = make_upper(f);

                    if (m_applogFilter == f)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    int Workspace::UpdateActionCount(const char* actionString)
    {
        behaviac::ScopedLock lock(m_cs);

        int count = 1;
        CStringID actionId(actionString);
        ActionCount_t::iterator it = m_actions_count.find(actionId);

        if (it == m_actions_count.end())
        {
            m_actions_count[actionId] = count;

        }
        else
        {
            count = m_actions_count[actionId];
            count++;
            m_actions_count[actionId] = count;
        }

        return count;
    }

    int Workspace::GetActionCount(const char* actionString)
    {
        behaviac::ScopedLock lock(m_cs);

        int count = 0;
        CStringID actionId(actionString);
        ActionCount_t::iterator it = m_actions_count.find(actionId);

        if (it != m_actions_count.end())
        {
            count = m_actions_count[actionId];
        }

        return count;
    }
}//namespace behaviac

#endif // BEHAVIAC_SHARED_H_
