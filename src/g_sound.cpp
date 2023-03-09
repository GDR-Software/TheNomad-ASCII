#include "n_shared.h"
#include "scf.h"
#include "g_zone.h"
#include "g_items.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_scripted.h"
#include "s_world.h"
#include "g_playr.h"
#include "g_game.h"
#include "g_animation.h"
#include "g_rng.h"
#include "g_sound.h"

void Snd_Init(Game* const gptr)
{
}

void P_PlaySFX(const std::string& name)
{
}
void G_RunSound()
{
}


#if 0

#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <string>

class ALSAPCMDevice {
    protected:
    snd_pcm_t* handle;
    std::string device_name;
    unsigned int sample_rate, channels;             // Quality of the recorded audio.
    snd_pcm_uframes_t frames_per_period;            // Latency - lower numbers will decrease latency and increase CPU usage.
    snd_pcm_format_t format;                        // Bit depth - Quality.
    enum _snd_pcm_stream type;                      // SND_PCM_STREAM_CAPTURE | SND_PCM_STREAM_PLAYBACK

    void set_hw_params();
    public:
    ALSAPCMDevice(
        std::string device_name,
        unsigned int sample_rate,
        unsigned int channels,
        unsigned int frames_per_period,
        snd_pcm_format_t format,
        enum _snd_pcm_stream type
    ) : 
        device_name(device_name),
        sample_rate(sample_rate),
        channels(channels),
        frames_per_period(frames_per_period),
        format(format),
        type(type)
    {}

    bool open();
    void close();
    char* allocate_buffer();
    unsigned int get_frames_per_period();
    unsigned int get_bytes_per_frame();
};


class ALSACaptureDevice : public ALSAPCMDevice {
    public:
    ALSACaptureDevice(
        std::string device_name,
        unsigned int sample_rate,
        unsigned int channels,
        unsigned int frames_per_period,
        snd_pcm_format_t format
    ) : ALSAPCMDevice( // Calling super constructor
        device_name,
        sample_rate, 
        channels, 
        frames_per_period, 
        format, 
        SND_PCM_STREAM_CAPTURE) // This is the param that makes this a readable - CAPTURE - device.
    {}

    unsigned int capture_into_buffer(char* buffer, snd_pcm_uframes_t frames_to_capture);
};


class ALSAPlaybackDevice : public ALSAPCMDevice {
    public:
    ALSAPlaybackDevice(
        std::string device_name,
        unsigned int sample_rate,
        unsigned int channels,
        unsigned int frames_per_period,
        snd_pcm_format_t format
    ) : ALSAPCMDevice( // Calling super constructor
        device_name,
        sample_rate, 
        channels, 
        frames_per_period, 
        format, 
        SND_PCM_STREAM_PLAYBACK) // This is the param that makes this a readable - CAPTURE - device.
    {}

    unsigned int play_from_buffer(char* buffer, snd_pcm_uframes_t frames_to_play);
};

void P_PlaySFX(const std::string& name)
{

}

void G_RunSound()
{

}


const snd_pcm_format_t FORMAT = SND_PCM_FORMAT_S16_BE;
#define SAMPLING_RATE 16000
#define CHANNELS 1
#define FRAMES_PER_PERIOD 64

ALSAPlaybackDevice speaker("default", SAMPLING_RATE, CHANNELS, FRAMES_PER_PERIOD, FORMAT);


void Snd_Init(Game* const gptr)
{
    speaker.open();
    char* buffer = microphone.allocate_buffer();
    unsigned int frames_captured, frames_played;

    do{
        frames_played = speaker.play_from_buffer(buffer, FRAMES_PER_PERIOD);
        std::cout << "Played ---> " << "," << frames_played << std::endl;
    }while(1);

    microphone.close();
    speaker.close();
}


bool ALSAPCMDevice::open() {
    snd_pcm_hw_params_t *params;
    /* Open PCM device. */
    int rc = snd_pcm_open(&handle, device_name.c_str(), type, 0);
    if (rc < 0) {
        fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
        return false;
    }

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* Set the desired hardware parameters. */

    /* Interleaved mode */
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    /* Signed 16-bit little-endian format */
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

    /* Two channels (stereo) */
    snd_pcm_hw_params_set_channels(handle, params, 1);

    /* 44100 bits/second sampling rate (CD quality) */
    snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, NULL);

    /* Set period size to 32 frames. */
    snd_pcm_hw_params_set_period_size_near(handle,
                                params, &frames_per_period, NULL);

    /* Write the parameters to the driver */
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
        return false;
    }

    /* Use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(params, &frames_per_period, NULL);
    return true;
}

void ALSAPCMDevice::close() {
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
}

char* ALSAPCMDevice::allocate_buffer() {
    unsigned int size_of_one_frame = (snd_pcm_format_width(format)/8) * channels;
    return (char*) calloc(frames_per_period, size_of_one_frame);
}

unsigned int ALSAPCMDevice::get_frames_per_period() {
    return frames_per_period;
}

unsigned int ALSAPCMDevice::get_bytes_per_frame() {
    unsigned int size_of_one_frame = (snd_pcm_format_width(format)/8) * channels;
    return size_of_one_frame;
}

unsigned int ALSACaptureDevice::capture_into_buffer(char* buffer, snd_pcm_uframes_t frames_to_capture) {
    if(frames_to_capture != frames_per_period) {
        fprintf(stderr, "frames_to_read must equal frames in period <%lu>\n", frames_per_period);
        return 0;
    }

    snd_pcm_sframes_t frames_read = snd_pcm_readi(handle, buffer, frames_to_capture);

    if(frames_read == 0) {
        fprintf(stderr, "End of file.\n");
        return 0;
    }

    if(frames_read != frames_per_period) {
        fprintf(stderr, "Short read: we read <%ld> frames\n", frames_read);
        // A -ve return value means an error.
        if(frames_read < 0) {
            fprintf(stderr, "error from readi: %s\n", snd_strerror(frames_read));
            return 0;
        }
        return frames_read;
    }
    return 0;
}

unsigned int ALSAPlaybackDevice::play_from_buffer(char* buffer, snd_pcm_uframes_t frames_to_play) {
    if(frames_to_play != frames_per_period) {
        fprintf(stderr, "frames_to_play must equal frames in period <%lu>\n", frames_per_period);
        return 0;
    }

    bool done = false;

    snd_pcm_sframes_t frames_written = snd_pcm_writei(handle, buffer, frames_to_play);

    if (frames_written == -EPIPE) {
        /* EPIPE means underrun */
        fprintf(stderr, "underrun occurred\n");
        snd_pcm_prepare(handle);
    } else if (frames_written < 0) {
        fprintf(stderr, "error from writei: %s\n", snd_strerror(frames_written));
    }  else if (frames_written != frames_per_period) {
        fprintf(stderr, "short write, write %ld frames\n", frames_written);
    }

    return frames_written;
}

#endif