#pragma once

#include <fstream>
#include <mutex>

#include <libdevcore/Worker.h>
#include <libethcore/EthashAux.h>
#include <libethcore/Miner.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#if __GNUC__ >= 6
#pragma GCC diagnostic ignored "-Wignored-attributes"
#endif
#pragma GCC diagnostic ignored "-Wmissing-braces"
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS false
#define CL_HPP_ENABLE_EXCEPTIONS true
#define CL_HPP_CL_1_2_DEFAULT_BUILD true
#define CL_HPP_TARGET_OPENCL_VERSION 120
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include "CL/opencl.hpp"
#pragma GCC diagnostic pop

// macOS OpenCL fix:
#ifndef CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV
#define CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV 0x4000
#endif

#ifndef CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV
#define CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV 0x4001
#endif

#define CL_TARGET_BATCH_TIME 0.3F  // seconds

namespace dev
{
namespace eth
{
class CLMiner : public Miner
{
public:
    CLMiner(unsigned _index, DeviceDescriptor& _device);
    ~CLMiner() override;

    static void enumDevices(std::map<string, DeviceDescriptor>& _DevicesCollection);

protected:
    bool initDevice() override;

    void initEpoch() override;

    void kick_miner() override;

private:
    
    void workLoop() override;

    cl::Context* m_context = nullptr;
    cl::CommandQueue* m_queue = nullptr;
    cl::CommandQueue* m_abortqueue = nullptr;
    cl::Kernel m_searchKernel;
    cl::Kernel m_dagKernel;
    cl::Device m_device;

    cl::Buffer* m_dag[2] = {nullptr, nullptr};
    cl::Buffer* m_light = nullptr;
    cl::Buffer* m_header = nullptr;
    cl::Buffer* m_searchBuffer = nullptr;

    void clear_buffers()
    {
        m_abortMutex.lock();
        if (m_dag[0])
        {
            delete m_dag[0];
            m_dag[0] = nullptr;
        }
        if (m_dag[1])
        {
            delete m_dag[1];
            m_dag[1] = nullptr;
        }
        if (m_light)
        {
            delete m_light;
            m_light = nullptr;
        }
        if (m_header)
        {
            delete m_header;
            m_header = nullptr;
        }
        if (m_searchBuffer)
        {
            delete m_searchBuffer;
            m_searchBuffer = nullptr;
        }
        if (m_queue)
        {
            delete m_queue;
            m_queue = nullptr;
        }
        if (m_abortqueue)
        {
            delete m_abortqueue;
            m_abortqueue = nullptr;
        }
        if (m_context)
        {
            delete m_context;
            m_context = nullptr;
        }
    }

    unsigned m_dagItems = 0;
    uint64_t m_lastNonce = 0;
    std::mutex m_abortMutex;
};

}  // namespace eth
}  // namespace dev
