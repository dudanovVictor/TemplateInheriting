#ifndef _FILTER_PROCESS_H_
#define _FILTER_PROCESS_H_

#include "../Core/BindedType.h"
#include "../LogClient/LoggableObject.h"
#include "../Core/ProcessorsData.h"

namespace appcore
{
// prototype
template <typename SettingsType>
class Processor;

/// Interface for filter settings
class ProcessorSettings
{
    // prototype
    template <typename SettingsType> friend class Processor;
public:
    /// Set that setting has been updated
    void Update() {m_New = true;}

protected:
    /// Ctor
    ProcessorSettings() : m_New(true) {}
    /// Dtor
    virtual ~ProcessorSettings() {}
    //virtual bool IsActive() { return true; }
    //virtual void SetActivity(bool isActive) {}
    /// Applies new settings
    void Apply() {m_New = false;}
    /// Checks settings novelty
    bool IsNew() const {return m_New;}

private:
    bool m_New; ///< novelty flag
};

// Interface for the processor - the smallest processing item.
class IProcessor
    : public LoggableObject
    , public Named
{
public:
    /// Starts a calculation
    virtual void Calculate() = 0;
    /// Checks finishing
    virtual bool IsDone() const = 0;
    virtual bool IsParametersChanged() const = 0;
    virtual void MarkToCalculate() = 0;
	virtual bool IsInterruption() const = 0;
    /// Stops calculation
    virtual void Stop() = 0;
protected:
    /// Starts a concrete calculation
    virtual void Start() = 0;
};

/// Class contains settings, calculated data
template<typename T>
class ProcessorContext
{
public:
    ProcessorContext(ProcessorSettings& i_concreteProcessorSettings,
                     ProcessorsData& i_processorsData)
        : m_settings(i_concreteProcessorSettings)
        , m_data(i_processorsData)
    {
    }
    T& GetSettings() {return const_cast<T&>(static_cast<const T&>(m_settings));}
    const T& GetSettings() const {return static_cast<const T&>(m_settings);}
    ImageData& GetData() {return m_data.imageData;}
    const ImageData& GetData() const {return m_data.imageData;}
    DebugData& GetDebug() {return m_data.debugData;}
    const ApplicationInfo& GetAppInfo() const {return m_data.appInfo;}
    ApplicationInfo& GetAppInfo() {return m_data.appInfo;}
    SampleInfo& GetSampleInfo() {return m_data.sampleInfo;}
    StatusInfo& GetStatus() {return m_data.statusInfo;}
    const ProcessorsData& GetProcData() const { return m_data; }
    ProcessorsData& GetProcData() { return m_data; }
private:
    const ProcessorSettings& m_settings; ///< concrete processor's settings
    ProcessorsData& m_data;              ///< processor's data
};

/// Base class for filters
template<typename T>
class Processor
    : public IProcessor
{
public:
    /// Dtor
    virtual ~Processor(){}

    virtual void Calculate() final
    {
        m_done = false;
        m_context.GetSettings().Apply();
        Start();
        m_done = true;
    }
    virtual void Stop() { m_done = true; }
    virtual bool IsDone() const {return m_done;}
    /// Checks parameters changing
    virtual bool IsParametersChanged() const {return m_context.GetSettings().IsNew();};
    virtual void MarkToCalculate() {m_context.GetSettings().Update();}
	virtual bool IsInterruption() const {return false;}
protected:
    /// Ctor
    Processor(ProcessorContext<T>& i_context)
        : m_context(i_context)
        , m_done(false)
    {}
    Processor(ProcessorsData& i_data)
        : m_context(ProcessorContext<T>(ProcessorSettings(), i_data))
        , m_done(false)
    {}

protected:
    const T& GetConstants() const { return m_context.GetSettings();}
    const ImageData& GetImageData() const {return m_context.GetData();}
    ImageData& GetImageData() {return m_context.GetData();}
    DebugData& GetDebugData() {return m_context.GetDebug();}
    SampleInfo& GetSampleInfo() {return m_context.GetSampleInfo();}
    const ApplicationInfo& GetAppConfig() const {return m_context.GetAppInfo();}
    ApplicationInfo& GetAppConfig() {return m_context.GetAppInfo();}
    StatusInfo& GetStatus() {return m_context.GetStatus();}
protected:
    ProcessorContext<T> m_context;
private:
    bool m_done;                   ///< completion flag
};

} /// end of namespace

#endif