#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>


namespace Uneye
{
    struct ProfileResult
    {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class Instrumentor
    {
    public:
        Instrumentor()
            : m_CurrentSession(nullptr), m_ProfileCount(0)
        {
        }

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")
        {
            m_OutputStream.open(filepath);
            WriteHeader();
            m_CurrentSession = new InstrumentationSession{ name };
        }

        void EndSession()
        {
            WriteFooter();
            m_OutputStream.close();
            delete m_CurrentSession;
            m_CurrentSession = nullptr;
            m_ProfileCount = 0;
        }

        void WriteProfile(const ProfileResult& result)
        {
            if (m_ProfileCount++ > 0)
                m_OutputStream << ",\n";

            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            m_OutputStream << "\n{";
            m_OutputStream << "\n\"cat\":\"function\",";
            m_OutputStream << "\n\"dur\":" << (result.End - result.Start) << ',';
            m_OutputStream << "\n\"name\":\"" << name << "\",";
            m_OutputStream << "\n\"ph\":\"X\",\n";
            m_OutputStream << "\n\"pid\":0,\n";
            m_OutputStream << "\n\"tid\":" << result.ThreadID << ",";
            m_OutputStream << "\n\"ts\":" << result.Start;
            m_OutputStream << "\n}";

            m_OutputStream.flush();
        }

        void WriteHeader()
        {
            m_OutputStream << "\n{\n\"otherData\": {},\n\"traceEvents\":[";
            m_OutputStream.flush();
        }

        void WriteFooter()
        {
            m_OutputStream << "\n]}\n";
            m_OutputStream.flush();
        }

        static Instrumentor& Get()
        {
            static Instrumentor instance;
            return instance;
        }

    private:
        InstrumentationSession* m_CurrentSession;
        std::ofstream m_OutputStream;
        int m_ProfileCount;
    };

    class InstrumentationTimer
    {
    public:
        InstrumentationTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            uint32_t threadID = static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });

            m_Stopped = true;
        }

    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        bool m_Stopped;
    };
}

#define UNEYE_PROFILE false
#if UNEYE_PROFILE
    #define UNEYE_PROFILE_BEGIN_SESSION(name, filepath)  ::Uneye::Instrumentor::Get().BeginSession(name, filepath)
    #define UNEYE_PROFILE_END_SESSION()                  ::Uneye::Instrumentor::Get().EndSession()   
    #define UNEYE_PROFILE_SCOPE(name)                    ::Uneye::InstrumentationTimer timer##__LINE__(name);
    #define UNEYE_PROFILE_FUNCTION()                       UNEYE_PROFILE_SCOPE(__FUNCSIG__);
#else
    #define UNEYE_PROFILE_BEGIN_SESSION(name, filepath)
    #define UNEYE_PROFILE_END_SESSION()
    #define UNEYE_PROFILE_SCOPE(name)
    #define UNEYE_PROFILE_FUNCTION()
#endif
