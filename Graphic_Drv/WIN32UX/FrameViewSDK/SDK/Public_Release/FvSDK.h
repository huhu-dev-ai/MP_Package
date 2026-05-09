#pragma once

// =========================================================================================
//!  FvSDK Version Definition
//!  Maintain per structure specific version define using the MAKE_FVSDK_VERSION macro.
// =========================================================================================
#define MAKE_FVSDK_VERSION(typeName,ver) (unsigned int)(sizeof(typeName) | ((ver)<<16))
#define GET_FVSDK_VERSION(ver) (unsigned int)((ver)>>16)
#define GET_FVSDK_SIZE(ver) (unsigned int)((ver) & 0xffff)

#define FV_DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name

//! \addtogroup fvsdkhandles
//! FVSDK session handle - opaque type Do not assume this corresponds to any data
FV_DECLARE_HANDLE(FvSession);

// =========================================================================================
//!  FvStatus - error codes
// =========================================================================================

typedef enum FvStatus
{
    FV_SUCCESS                  = 0x00000000L,
    FV_CREATE_SESSION_FAILED    = 0x00000001L,
    FV_MORE_DATA                = 0x00000002L,
    FV_NO_DATA                  = 0x00000003L,
    FV_INVALID_SESSION          = 0x00000004L,
    FV_NOT_IMPLEMENTED          = 0x00000005L,
    FV_LIBRARY_NOT_FOUND        = 0x00000006L,
    FV_SERVICE_ERROR            = 0x00000007L,
    FV_INVALID_METRIC           = 0x00000008L,
    FV_METRIC_NOT_ENABLED       = 0x00000009L,
    FV_BENCHMARK_IN_PROGRESS    = 0x0000000AL,
    FV_BENCHMARK_NOT_STARTED    = 0x0000000BL,
    FV_INSUFFICIENT_BUFFER      = 0x00000010L,
    FV_INVALID_VERSION          = 0x00000011L,
    FV_ERROR_INVALID_ARGS       = 0x00000012L,
    FV_SDK_SESSION_IN_PROGRESS  = 0x00000014L,
    FV_SDK_SESSION_NOT_STARTED  = 0x00000015L,
    FV_INSTALL_IN_PROGRESS      = 0x00000016L,
    FV_INSTALL_FINISHED         = 0x00000017L,
    FV_ERROR                    = 0xFFFFFFFFL,
} FvStatus;
 
#define FVSDK_INTERFACE FvStatus __cdecl
 
//! Used in ReadData() to specify sample type
typedef enum _FvMetricType
{
    eInvalid                    =  0x00000000,  //!< Invalid metrics
    eFrame                      =  0x5BE0DB9E,  //!< Per frame data
    eAvgFPS                     =  0x6C4B1348,  //!< Avg FPS
    eFPS99                      =  0xCA3E085A,  //!< 99 percentile FPS
    eRenderPresentLatency       =  0x92E4D2CA,  //!< Render Present Latency
    eRenderLatency              =  0x4724DF89,  //!< Render Latency
    eGPUFreq                    =  0x567A179A,  //!< GPU clocks in MHz
    eGPUTemp                    =  0x0ADD87E5,  //!< GPU temperature in celsius
    eGPUUtil                    =  0xDFE8B08C,  //!< GPU utilization
    eGPUMemFreq                 =  0x933377A0,  //!< GPU Memory clock in MHz
    eGPUVoltage                 =  0x06BF2EBE,  //!< GPU Voltage in millivolts
    eGPUFanSpeed                =  0x4D8E9794,  //!< Fan Speed in RPM 
    eTGPWatts                   =  0x74BDB13C,  //!< Total board power
    eCPUUtil                    =  0x2BA7949F,  //!< CPU utilization
    eCPUPwr                     =  0xBB272534,  //!< CPU power metrics in watts
    eCPUFreq                    =  0xA2353389,  //!< CPU clocks in MHz
    eCPUCoreTemp                =  0x42AE78F5,  //!< CPU per-core temperature in celsius
    eCPUPkgTemp                 =  0xB0CCC37A,  //!< CPU package temperature in celsius
    eProcessName                =  0xA98734CD,  //!< Process name as metric
    eAvgSWPCLatency             =  0x363AFFA0,  //!< Average SW based PC Latency
    eFPSLow                     =  0x29CCDD99,  //!< FPS 1% Low
} FvMetricType;

//! Used in SampleData()
typedef enum _FvSampleDataFlags {
    FVSDK_SAMPLEDATA_DONOTWAIT = 0,  //!< Do not block in SampleData API
} FvSampleDataFlags;

//! Used in StartBenchmark()
typedef enum _FvBenchmarkFlags {
    FVSDK_BENCHMARKFLAGS_NONE = 0,
    FVSDK_BENCHMARKFLAGS_PER_FRAME_DATA = 1,  //!< Collect per frame data
} FvBenchmarkFlags;

// =========================================================================================
//!  BaseMetric - base of all metrics supported by SDK
// =========================================================================================

struct BaseMetric_v1
{
    unsigned int                  baseversion;        //!< The structure version.
    unsigned long long int        mTimestamp;         //!< QPC timestamp when metric was collected.
    BaseMetric_v1()
    {
        baseversion = MAKE_FVSDK_VERSION(BaseMetric_v1, 1);
        mTimestamp = 0;
    }
};

// =========================================================================================
//!  BaseMetric - base of all metrics supported by SDK - version 2
// =========================================================================================

struct BaseMetric_v2
{
    unsigned int                  baseversion;        //!< The structure version.
    unsigned long long int        mTimestamp;         //!< QPC timestamp when metric was collected.
    double                        minValue;           //!< Advance statistics for the metric
    double                        maxValue;
    double                        avgValue;
    double                        stdDeviation;
    BaseMetric_v2()
    {
        baseversion = MAKE_FVSDK_VERSION(BaseMetric_v2, 2);
        mTimestamp = 0;
        minValue = maxValue = avgValue = stdDeviation = 0;
    }
};
typedef BaseMetric_v2   BaseMetric;

// =========================================================================================
//!  Samples - collection of samples of particular metric
//!  Used in ReadData() call
//!  mData - pointer to array of structures of type metric
// =========================================================================================

struct Samples_v1
{
    unsigned int    version;            //!< The structure version.
    FvMetricType    type;               //!< Type of metric, client needs to set this.
    size_t          mNumSamples;        //!< Number of samples of type metric.
    void* mData;
    Samples_v1()
    {
        version = MAKE_FVSDK_VERSION(Samples_v1, 1);
        mData = 0;
        mNumSamples = 0;
        type = eInvalid;
    }
};
typedef Samples_v1   Samples;

// =========================================================================================
//!  FPS - provides additional information about process
// =========================================================================================

struct FPS_v1
{
    unsigned int              version;
    unsigned long long int    PID;
    unsigned long long int    SwapChain;
    double                    AvgFPS;
    FPS_v1()
    {
        version = MAKE_FVSDK_VERSION(FPS_v1, 1);
        AvgFPS = 0;
        PID = SwapChain = 0;
    }
};
typedef FPS_v1   FPS;

// =========================================================================================
//!  FPS metric - render average
// =========================================================================================

struct AvgFPS_v1 : BaseMetric_v1
{
    unsigned int version;
    FPS mFPS;
    AvgFPS_v1()
    {
        version = MAKE_FVSDK_VERSION(AvgFPS_v1, 1);
    }
};
typedef AvgFPS_v1   AvgFPS;

// =========================================================================================
//!  FPS - render 99 percentile
// =========================================================================================

struct FPS99_v1 : BaseMetric_v1
{
    unsigned int version;
    FPS mFPS;
    FPS99_v1()
    {
        version = MAKE_FVSDK_VERSION(FPS99_v1, 1);
    }
};
typedef FPS99_v1   FPS99;

// =========================================================================================
//!  FPS - 1% Low
// =========================================================================================

struct FPSLow_v1 : BaseMetric_v1
{
    unsigned int version;
    FPS_v1 mFPS;
    FPSLow_v1()
    {
        version = MAKE_FVSDK_VERSION(FPSLow_v1, 1);
    }
};
typedef FPSLow_v1   FPSLow;

// =========================================================================================
//!  FPS - render latency
// =========================================================================================

struct RenderLatency_v1 : BaseMetric_v1
{
    unsigned int version;
    FPS mFPS;
    RenderLatency_v1()
    {
        version = MAKE_FVSDK_VERSION(RenderLatency_v1, 1);
    }
};
typedef RenderLatency_v1   RenderLatency;

// =========================================================================================
//!  FPS - render present latency
// =========================================================================================

struct RenderPresentLatency_v1 : BaseMetric_v1
{
    unsigned int version;
    FPS mFPS;
    RenderPresentLatency_v1()
    {
        version = MAKE_FVSDK_VERSION(RenderPresentLatency_v1, 1);
    }
};
typedef RenderPresentLatency_v1   RenderPresentLatency;

// =========================================================================================
//!  Average Software based PC latency
// =========================================================================================

struct AvgSWPCLatency_v1 : BaseMetric_v1
{
    unsigned int version;
    FPS_v1 mFPS;
    AvgSWPCLatency_v1()
    {
        version = MAKE_FVSDK_VERSION(AvgSWPCLatency_v1, 1);
    }
};
typedef AvgSWPCLatency_v1   AvgSWPCLatency;

// =========================================================================================
//!  ProcessName_v1
// =========================================================================================

#define FVSDK_PROCESS_NAME_SIZE 256

struct ProcessName_v1 : BaseMetric_v1
{
    unsigned int                version;
    unsigned long long int      PID;
    char                        name[FVSDK_PROCESS_NAME_SIZE];
    ProcessName_v1()
    {
        version = MAKE_FVSDK_VERSION(ProcessName_v1, 1);
        PID = 0;
        name[0] = '\0';
    }
};
typedef ProcessName_v1   ProcessName;

// =========================================================================================
//!  GPUTempC
// =========================================================================================

struct GPUTemp_v1 : BaseMetric_v1
{
    unsigned int    version;
    unsigned int    mVendorId;
    size_t          mGPUIndex;
    double          mGPUTemp;
    GPUTemp_v1()
    {
        version = MAKE_FVSDK_VERSION(GPUTemp_v1, 1);
        mVendorId = 0;
        mGPUIndex = 0;
        mGPUTemp = 0;
    }
};
typedef GPUTemp_v1   GPUTemp;

// =========================================================================================
//!  GPUUtil
// =========================================================================================

struct GPUUtil_v1 : BaseMetric_v1
{
    unsigned int    version;
    unsigned int    mVendorId;
    size_t          mGPUIndex;
    double          mGPUUtil;
    GPUUtil_v1()
    {
        version = MAKE_FVSDK_VERSION(GPUUtil_v1, 1);
        mVendorId = 0;
        mGPUIndex = 0;
        mGPUUtil = 0;
    }
};
typedef GPUUtil_v1   GPUUtil;

// =========================================================================================
//!  GPUFreq
// =========================================================================================

struct GPUFreq_v1 : BaseMetric_v1
{
    unsigned int    version;
    unsigned int    mVendorId;
    size_t          mGPUIndex;
    double          mGPUFreq;
    GPUFreq_v1()
    {
        version = MAKE_FVSDK_VERSION(GPUFreq_v1, 1);
        mVendorId = 0;
        mGPUIndex = 0;
        mGPUFreq = 0;
    }
};
typedef GPUFreq_v1   GPUFreq;

// =========================================================================================
//!  GPUFanSpeed
// =========================================================================================

struct GPUFanSpeed_v1 : BaseMetric_v1
{
    unsigned int    version;
    unsigned int    mVendorId;
    size_t          mGPUIndex;
    size_t          mFanIndex;
    double          mFanSpeed;
    GPUFanSpeed_v1()
    {
        version = MAKE_FVSDK_VERSION(GPUFanSpeed_v1, 1);
        mVendorId = 0;
        mGPUIndex = 0;
        mFanIndex = 0;
        mFanSpeed = 0;
    }
};
typedef GPUFanSpeed_v1   GPUFanSpeed;


// =========================================================================================
//!  GPUVoltage
// =========================================================================================

struct GPUVoltage_v1 : BaseMetric_v1
{
    unsigned int    version;
    unsigned int    mVendorId;
    size_t          mGPUIndex;
    double          mGPUVoltage;
    GPUVoltage_v1()
    {
        version = MAKE_FVSDK_VERSION(GPUVoltage_v1, 1);
        mVendorId = 0;
        mGPUIndex = 0;
        mGPUVoltage = 0;
    }
};
typedef GPUVoltage_v1   GPUVoltage;


// =========================================================================================
//!  GPUMemFreq
// =========================================================================================

struct GPUMemFreq_v1 : BaseMetric_v1
{
    unsigned int    version;
    unsigned int    mVendorId;
    size_t          mGPUIndex;
    double          mGPUMemFreq;
    GPUMemFreq_v1()
    {
        version = MAKE_FVSDK_VERSION(GPUMemFreq_v1, 1);
        mVendorId = 0;
        mGPUIndex = 0;
        mGPUMemFreq = 0;
    }
};
typedef GPUMemFreq_v1   GPUMemFreq;


// =========================================================================================
//!  CPUUtil
// =========================================================================================

struct CPUUtil_v1 : BaseMetric_v1
{
    unsigned int version;
    double mCPUUtil;
    CPUUtil_v1()
    {
        version = MAKE_FVSDK_VERSION(CPUUtil_v1, 1);
        mCPUUtil = 0;
    }
};
typedef CPUUtil_v1   CPUUtil;

// =========================================================================================
//!  CPUFreq - CPU clocks in MHz
// =========================================================================================

struct CPUFreq_v1 : BaseMetric_v1
{
    unsigned int version;
    unsigned int mCPUFreq;
    CPUFreq_v1()
    {
        version = MAKE_FVSDK_VERSION(CPUFreq_v1, 1);
        mCPUFreq = 0;
    }
};
typedef CPUFreq_v1   CPUFreq;

// =========================================================================================
//!  CPUPwr - CPU power metrics
//!  mPkgPwr is on desktop only, otherwise value is zero.
//!  mECorePwr and mPCorePwr is on WoA only, otherwise values are zero.
// =========================================================================================

struct CPUPwr_v1 : BaseMetric_v1
{
    unsigned int version;
    double mPkgPwr;          //!< Package power (desktop only)
    double mECorePwr;        //!< E-Core power (WoA only)
    double mPCorePwr;        //!< P-Core power (WoA only)
    CPUPwr_v1()
    {
        version = MAKE_FVSDK_VERSION(CPUPwr_v1, 1);
        mPkgPwr = 0;
        mECorePwr = 0;
        mPCorePwr = 0;
    }
};
typedef CPUPwr_v1   CPUPwr;

// =========================================================================================
//!  CPUPkgTemp - CPU package temperature in celsius
// =========================================================================================

struct CPUPkgTemp_v1 : BaseMetric_v1
{
    unsigned int version;
    double mCPUPkgTemp;
    CPUPkgTemp_v1()
    {
        version = MAKE_FVSDK_VERSION(CPUPkgTemp_v1, 1);
        mCPUPkgTemp = 0;
    }
};
typedef CPUPkgTemp_v1   CPUPkgTemp;

// =========================================================================================
//!  CPUCoreTemp - CPU per-core temperature in celsius
// =========================================================================================

struct CPUCoreTemp_v1 : BaseMetric_v1
{
    unsigned int version;
    unsigned int coreIndex;      //!< Zero-based core index
    double mCoreTemp;           //!< Core temperature in celsius
    CPUCoreTemp_v1()
    {
        version = MAKE_FVSDK_VERSION(CPUCoreTemp_v1, 1);
        coreIndex = 0;
        mCoreTemp = 0.0;
    }
};
typedef CPUCoreTemp_v1 CPUCoreTemp;


// =========================================================================================
//!  TGPWatts
// =========================================================================================

struct TGPWatts_v1 : BaseMetric_v1
{
    unsigned int version;
    double mTGPWatts;
    TGPWatts_v1()
    {
        version = MAKE_FVSDK_VERSION(TGPWatts_v1, 1);
        mTGPWatts = 0;
    }
};

// =========================================================================================
//!  Individual TGPWatts 
// =========================================================================================

struct TGPWatts_v2 : BaseMetric_v2
{
    unsigned int    version;
    double          mTGPWatts;
    unsigned int    mVendorId;
    size_t          mGPUIndex;

    TGPWatts_v2()
    {
        version = MAKE_FVSDK_VERSION(TGPWatts_v2, 2);
        mVendorId = 0;
        mGPUIndex = 0;
        mTGPWatts = 0;
    }
};
typedef TGPWatts_v2   TGPWatts;

// =========================================================================================
//!  Per Frame data: Frame metrics and latency
// =========================================================================================

struct Frame_v1 : BaseMetric_v1
{
    unsigned int           version;
    unsigned long long int PID;
    unsigned long long int SwapChain;
    double                 mMsBetweenPresents;
    double                 mMsBetweenDisplayChange;
    double                 mMsPCLatency;
    long long int          FrameID;

    Frame_v1()
    {
        version = MAKE_FVSDK_VERSION(Frame_v1, 1);
        PID = SwapChain = 0;
        mMsBetweenPresents = mMsBetweenDisplayChange = mMsPCLatency = 0;
        FrameID = 0;
    }
};
typedef Frame_v1   Frame;

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_Initialize
//
//!   DESCRIPTION: Loads FvSDK library
//!
//! \retval    FV_SUCCESS
//! \retval    FV_LIBRARY_NOT_FOUND
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_Initialize();

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_CreateSession
//
//!   DESCRIPTION: Allocates data needed for session. FvSession handle can be 
//!                     used until call to FvSDK_DestroySession
//!
//!
//! \param [out] FvSession*
//!
//! \retval    FV_SUCCESS
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_CreateSession(FvSession*);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_StartSession
//
//!   DESCRIPTION: Starts data collection for FvSession
//!
//!
//! \param [in] FvSession
//!
//! \retval    FV_SUCCESS
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_StartSession(FvSession);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_EnableMetrics
//
//!   DESCRIPTION: Enables collection of metrics. Use FvSDK_ReadData() to read
//!                 enabled collection of metrics.
//!
//! \param [in] FvSession
//! \param [in] FvMetricType*
//! \param [in] unsigned int
//!
//! \retval    FV_SUCCESS
//! \retval    FV_NOT_IMPLEMENTED
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_EnableMetrics(FvSession, FvMetricType*, unsigned int);


// Metric specific params
// If client needs to modify these params later, they can just call FvSDK_EnableMetrics 
// with new values.
struct FvMetricParams
{
    unsigned int numMetrics;   // number of metrics
    FvMetricType* metrics;     // metrics to enable
    int* samplingWindowSize;   // sampling window size
    bool* enableAdvanceStats;  // like min,max,stddev 
};

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_EnableMetrics2
//
//!   DESCRIPTION: Enables collection of metrics. Use FvSDK_ReadData() to read
//!                 enabled collection of metrics.
//!
//! \param [in] FvSession
//! \param [in] FvMetricParams
//!
//! \retval    FV_SUCCESS
//! \retval    FV_NOT_IMPLEMENTED
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_EnableMetrics2(FvSession, FvMetricParams);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_SampleData
//
//!   DESCRIPTION: Snapshots data available since last call to FvSDK_SampleData()
//!
//! \param [in] FvSession
//!
//! \retval    FV_SUCCESS
//! \retval    FV_INVALID_SESSION
//! \retval    FV_SERVICE_ERROR
//! \retval    FV_NOT_IMPLEMENTED
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_SampleData(FvSession, FvSampleDataFlags);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_ResetSampling
//
//!   DESCRIPTION: Discard the old samples
//!
//! \param [in] FvSession
//!
//! \retval    FV_SUCCESS
//! \retval    FV_INVALID_SESSION
//! \retval    FV_SERVICE_ERROR
//! \retval    FV_NOT_IMPLEMENTED
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_ResetSampling(FvSession, FvMetricType*, unsigned int);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_ReadData
//
//!   DESCRIPTION: Provides copy of data collected when FvSDK_SampleData was 
//!       called. Populate metric type field of Samples to retrieve information
//!       about metric type sample. Information provided is number of Samples
//!       mData member of Samples if not NULL should point to valid memory that
//!       can accomodate N metric type structures.
//!
//! HOW TO USE: 1. First call
//!                  a) session - a valid FvSession handle
//!                  b) pSamples - address pointing to sampleCount of Samples structure
//!                         set type member to metric type of interest
//!                         set mData member to NULL
//!                  c) sampleCount - number of Samples elements in pSamples
//!                     If all parameters are correct and this call is successful, 
//!                     this call will return FV_INSUFFICIENT_BUFFER.
//!             2. Second call
//!                  a) session - a valid FvSession handle
//!                  b) pSamples - address pointing to sampleCount of Samples structure
//!                         set type member to metric type of interest
//!                         set mData member to array of structure of metric type
//!                  c) sampleCount - number of Samples elements in pSamples
//!                     If all parameters are correct and this call is successful, 
//!                     this call will return FVSDK_SUCCESS.

//!
//! \param [in] session
//! \param [in,out] pSamples
//! \param [in] sampleCount
//!
//! \retval    FV_SUCCESS
//! \retval    FV_MORE_DATA
//! \retval    FV_INVALID_SESSION
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_ReadData(FvSession session, Samples* pSamples, unsigned int sampleCount);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_StartBenchmark
//
//!   DESCRIPTION: TODO
//!
//! \param [in] session
//!
//! \retval    FV_SUCCESS
//! \retval    FV_ERROR
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_StartBenchmark(FvSession session, FvBenchmarkFlags flags);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_StopBenchmark
//
//!   DESCRIPTION: TODO
//!
//! \param [in] session
//!
//! \retval    FV_SUCCESS
//! \retval    FV_ERROR
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_StopBenchmark(FvSession session);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_ReadBenchmarkData
//
//!   DESCRIPTION: TODO
//!
//! \param [in] session
//! \param [in,out] pSamples
//! \param [in] sampleCount
//!
//! \retval    FV_SUCCESS
//! \retval    FV_MORE_DATA
//! \retval    FV_NOT_IMPLEMENTED
//! \retval    FV_INVALID_VERSION
//! \retval    FV_ERROR_INVALID_ARGS
//! \retval    FV_BENCHMARK_IN_PROGRESS
//! \retval    FV_NO_DATA
//! \retval    FV_ERROR
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_ReadBenchmarkData(FvSession session, Samples* pSamples, unsigned int numSamples);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_ReadBenchmarkPerFrameData
//
//!   DESCRIPTION: TODO
//!
//! \param [in] session
//! \param [in,out] pSamples
//! \param [in] sampleCount
//!
//! \retval    FV_SUCCESS
//! \retval    FV_MORE_DATA
//! \retval    FV_NOT_IMPLEMENTED
//! \retval    FV_INVALID_VERSION
//! \retval    FV_ERROR_INVALID_ARGS
//! \retval    FV_BENCHMARK_IN_PROGRESS
//! \retval    FV_NO_DATA
//! \retval    FV_ERROR
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_ReadBenchmarkPerFrameData(FvSession session, Samples* pSamples, unsigned int numSamples);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_GetPerFrameDataPipe
//
//!   DESCRIPTION: Returns a read pipe that outputs per-frame data in below csv format:
//!
//!     <ProcessId>,<SwapChainAddress>,<QpcTime>,<MsBetweenPresents>,<MsBetweenDisplayChange>,<MsPCLatency>"
//!
//!   The very 1st row is a string with below content:
//!
//!     "ProcessId,SwapChainAddress,QpcTime,MsBetweenPresents,MsBetweenDisplayChange,MsPCLatency"
//!
//! \param [in] session
//! \param [in,out] ppPipe
//!
//! \retval    FV_SUCCESS
//! \retval    FV_METRIC_NOT_ENABLED
//! \retval    FV_SERVICE_ERROR
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_GetPerFrameDataPipe(FvSession session, void** ppPipe);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_StopSession
//
//!   DESCRIPTION: Stops metrics capturing session.
//!
//! \param [in] FvSession
//!
//! \retval    FV_SUCCESS
//! \retval    FV_INVALID_SESSION
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_StopSession(FvSession);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_DestroySession
//
//!   DESCRIPTION: Destroy any session specific data. After calling
//!     FvSDK_DestroySession FvSession handle becomes invalid. Do not call this
//!     function while application is still executing FvSDK_SampleData
//!
//! \param [in] FvSession
//!
//! \retval    FV_SUCCESS
//! \retval    FV_INVALID_SESSION
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_DestroySession(FvSession);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  FvSDK_Shutdown
//
//!   DESCRIPTION: Unloads FvSDK library. Do not call this function while 
//!         while application is still executing FvSDK_SampleData
//!
//! \retval    FV_SUCCESS
//!
///////////////////////////////////////////////////////////////////////////////
FVSDK_INTERFACE FvSDK_Shutdown();
