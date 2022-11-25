#include <algorithm>
#include <cassert>
#include <charconv>
#include <chrono>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#include <CL/opencl.hpp>

class CL
{
public:
    CL();
    void buildProgram(const std::string& kernel);
    cl::Event multMatr(const float *APtr, const float *BPtr, float *CPtr, int AX, int AY, int BY);
private:
    cl::Device m_device;
    cl::Context m_context;
    cl::CommandQueue m_queue;
    cl::Program m_program;
};

struct Params
{
  int AX = 128 * 4;
  int AY = 128 * 4;
  int BY = 128 * 4;
};

using systemCL = std::pair<cl::Platform, cl::Device>;
std::vector<systemCL> getDevices();
cl::Device getDevice(const std::vector<systemCL>& data);
cl::Device getDevice();
void matrixMultCPU(const float *A, const float *B, float *C, int AX, int AY, int BY);
template <typename It>
void initialize(It start, It end);

int main(int argc, char **argv)
{
    CL instance;

    Params Cfg;
    cl::vector<float> A(Cfg.AX * Cfg.AY), B(Cfg.AY * Cfg.BY), C(Cfg.AX * Cfg.BY);

    initialize(A.begin(), A.end());
    initialize(B.begin(), B.end());
    std::string kernel = "\
        __kernel void multMatr(__global float *A, __global float *B, __global float *C, int AX, int AY, int BY) {\
            int row = get_global_id(0);\
            int col = get_global_id(1);\
            float sum = 0;\
            for (int k = 0; k < AY; k++)\
                sum += A[row * AY + k] * B[k * BY + col];\
            C[row * BY + col] = sum;\
        }\
    ";
    instance.buildProgram(kernel);

    std::chrono::high_resolution_clock::time_point TimeStart, TimeFin;
    cl_ulong GPUTimeStart, GPUTimeFin;
    long Dur;
    TimeStart = std::chrono::high_resolution_clock::now();
    instance.multMatr(A.data(), B.data(), C.data(), Cfg.AX, Cfg.AY, Cfg.BY);
    TimeFin = std::chrono::high_resolution_clock::now();
    Dur = std::chrono::duration_cast<std::chrono::milliseconds>(TimeFin - TimeStart).count();
    std::cout << "GPU time: " << Dur << " ms" << std::endl;


    cl::vector<float> CCPU(Cfg.AX * Cfg.BY);
    TimeStart = std::chrono::high_resolution_clock::now();
    matrixMultCPU(A.data(), B.data(), CCPU.data(), Cfg.AX, Cfg.AY, Cfg.BY);
    TimeFin = std::chrono::high_resolution_clock::now();
    Dur = std::chrono::duration_cast<std::chrono::milliseconds>(TimeFin - TimeStart).count();
    std::cout << "CPU time: " << Dur << " ms" << std::endl;
}

template <typename It>
void initialize(It start, It end)
{
  static std::mt19937_64 mt_source;
  std::uniform_int_distribution<int> dist(0, 20);
  for (It cur = start; cur != end; ++cur)
    *cur = dist(mt_source);
}



std::vector<systemCL> getDevices()
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    std::vector<systemCL> ret;

    for(auto&& platform : platforms)
    {
        std::vector<cl::Device> platform_devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &platform_devices);

        for(auto&& device : platform_devices)
        {
            ret.push_back({platform, device});
        }
    }

    return ret;
}

cl::Device getDevice(const std::vector<systemCL>& data)
{
    return data[0].second;
}

cl::Device getDevice()
{
    auto&& data = getDevices();
    return getDevice(data);
}


CL::CL()
{
    m_device = getDevice();
    m_context = cl::Context(m_device),
    m_queue = cl::CommandQueue(m_context, m_device, CL_QUEUE_PROFILING_ENABLE);
}

void CL::buildProgram(const std::string& kernel)
{
    m_program = cl::Program(m_context, kernel);
    m_program.build();

}

cl::Event CL::multMatr(const float *APtr, const  float *BPtr,  float *CPtr, int AX, int AY, int BY)
{
    size_t ASz = AX * AY, ABufSz = ASz * sizeof(float);
    size_t BSz = AY * BY, BBufSz = BSz * sizeof(float);
    size_t CSz = AX * BY, CBufSz = CSz * sizeof(float);

    cl::Buffer A(m_context, CL_MEM_READ_ONLY, ABufSz);
    cl::Buffer B(m_context, CL_MEM_READ_ONLY, BBufSz);
    cl::Buffer C(m_context, CL_MEM_WRITE_ONLY, CBufSz);

    cl::copy(m_queue, APtr, APtr + ASz, A);
    cl::copy(m_queue, BPtr, BPtr + ASz, B);

    cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, int, int, int> kf(m_program, "multMatr");
    cl::NDRange GlobalRange(AX, BY);
    cl::NDRange LocalRange(1, 1);
    cl::EnqueueArgs Args(m_queue, GlobalRange, LocalRange);

    cl::Event Evt = kf(Args, A, B, C, AX, AY, BY);
    cl::copy(m_queue, C, CPtr, CPtr + CSz);
    return Evt;
}

// multiply on CPU, basic version
void matrixMultCPU(const float *A, const float *B, float *C, int AX, int AY, int BY)
{
  assert(A != NULL && B != NULL && C != NULL);
  assert(AX > 0 && AY > 0 && BY > 0);
  int i, j, k;
  for (i = 0; i < AX; i++)
  {
    for (j = 0; j < BY; j++)
    {
      float acc = 0;
      for (k = 0; k < AY; k++)
        acc += A[i * AY + k] * B[k * BY + j];
      C[i * BY + j] = acc;
    }
  }
}
