#include "audiodevice.h"
#include "graphics/canvas.h"

namespace latero::graphics {
        
        AudioDevicePtr AudioDevice::Create()
        {
            return AudioDevicePtr(new AudioDevice());
        }
        
        AudioDevice::AudioDevice()
        {
            
        }
        
        void AudioDevice::Open()
        {
#ifdef ENABLE_AUDIERE
            audioDev_ = audiere::OpenDevice();
#endif
        }
        
        bool AudioDevice::IsOpen()
        {
#ifdef ENABLE_AUDIERE
            return audioDev_;
#else
            return false;
#endif
        }
        
        
    } // namespace
