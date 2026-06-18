#pragma once

#include <memory>
#ifdef ENABLE_AUDIERE
#include <audiere.h>
#endif


namespace latero::graphics {

typedef std::shared_ptr<class AudioDevice> AudioDevicePtr;

        class AudioDevice
        {
        public:
            static AudioDevicePtr Create();
            virtual ~AudioDevice() {};
            void Open();
            bool IsOpen();

#ifdef ENABLE_AUDIERE
            audiere::AudioDevicePtr Dev() { return audioDev_; }
#endif

            
        protected:
            /** constructor */
            AudioDevice();
            
        private:
#ifdef ENABLE_AUDIERE
            audiere::AudioDevicePtr audioDev_;
#endif
        };

} // namespace

