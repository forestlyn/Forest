#pragma once
#include "Engine/Profile/ProfileMacros.h"
#include "Engine/Core/Log.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

namespace Engine::Profile
{
    using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

    struct InstrumentorProfileResult
    {
        std::string Name;

        FloatingPointMicroseconds Start;
        std::chrono::microseconds ElapsedTime;
        std::thread::id ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class Instrumentor
    {
    public:
        Instrumentor(const Instrumentor &) = delete;
        Instrumentor(Instrumentor &&) = delete;

        void BeginSession(const std::string &name, const std::string &filepath = "results.json")
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            if (m_CurrentSession)
            {
                // If there is already a current session, then close it before beginning new one.
                // Subsequent profiling output meant for the original session will end up in the
                // newly opened session instead.  That's better than having badly formatted
                // profiling output.
                if (Engine::Core::Log::GetCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
                {
                    ENGINE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->Name);
                }
                InternalEndSession();
            }
            m_OutputStream.open(filepath);

            if (m_OutputStream.is_open())
            {
                m_CurrentSession = new InstrumentationSession({name});
                WriteHeader();
            }
            else
            {
                if (Engine::Core::Log::GetCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
                {
                    ENGINE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
                }
            }
        }

        void EndSession()
        {
            std::lock_guard lock(m_Mutex);
            // ENGINE_INFO("Ending profiling session.");
            InternalEndSession();
        }

        void WriteProfile(const InstrumentorProfileResult &result)
        {
            std::stringstream json;

            json << std::setprecision(3) << std::fixed;
            json << ",{";
            json << "\"cat\":\"function\",";
            json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
            json << "\"name\":\"" << result.Name << "\",";
            json << "\"ph\":\"X\",";
            json << "\"pid\":0,";
            json << "\"tid\":" << result.ThreadID << ",";
            json << "\"ts\":" << result.Start.count();
            json << "}";

            // ENGINE_INFO("Profile Result: {0} - {1}ms", result.Name, result.ElapsedTime.count() * 0.001);
            std::lock_guard lock(m_Mutex);
            if (m_CurrentSession)
            {
                m_OutputStream << json.str();
                m_OutputStream.flush();
            }
        }

        static Instrumentor &Get()
        {
            static Instrumentor instance;
            return instance;
        }

    private:
        Instrumentor()
            : m_CurrentSession(nullptr)
        {
        }

        ~Instrumentor()
        {
            EndSession();
        }

        void WriteHeader()
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
            m_OutputStream.flush();
        }

        void WriteFooter()
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        // Note: you must already own lock on m_Mutex before
        // calling InternalEndSession()
        void InternalEndSession()
        {
            if (m_CurrentSession)
            {
                WriteFooter();
                m_OutputStream.close();
                delete m_CurrentSession;
                m_CurrentSession = nullptr;
            }
        }

    private:
        std::mutex m_Mutex;
        InstrumentationSession *m_CurrentSession;
        std::ofstream m_OutputStream;
    };

#pragma region InstrumentorTimer
    class InstrumentorTimer
    {
    public:
        InstrumentorTimer(const char *name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
        }
        ~InstrumentorTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();
            auto highResStart = FloatingPointMicroseconds{m_StartTimepoint.time_since_epoch()};
            auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTimepoint - m_StartTimepoint);

            Instrumentor::Get().WriteProfile({m_Name, highResStart, elapsedTime, std::this_thread::get_id()});

            m_Stopped = true;
        }

    private:
        const char *m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        bool m_Stopped;
    };

#pragma endregion InstrumentorTimer
} // namespace Engine::Profile