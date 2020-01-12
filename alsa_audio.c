/*
Copyright (c) 2020, Peter de Rivaz
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Based on tutorial at https://users.suse.com/~mana/alsa090_howto.html
// sudo apt-get install libasound2-dev
// gcc -o test_sound test_sound.c -lasound

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int16_t *audio_start=NULL;
static int audio_num=0;

 // audio sample rate in Hz
static int samplerate = 11025;
// numnber of audio channels
static int nchannels = 2;
// Number of buffers
static int numbuffers = 3;

#ifndef countof
   #define countof(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

#define BUFFER_SIZE_SAMPLES 512

typedef int int32_t;

static snd_pcm_t *pcm_handle=NULL;      

static unsigned char *data=NULL;

// Open the audio hardware
void audio_init(int dest) {
    /* Handle for the PCM device */ 
    
    /* Playback stream */
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

    /* This structure contains information about    */
    /* the hardware and can be used to specify the  */      
    /* configuration to be used for the PCM stream. */ 
    snd_pcm_hw_params_t *hwparams;
  
    /* Name of the PCM device, like plughw:0,0          */
    /* The first number is the number of the soundcard, */
    /* the second number is the number of the device.   */
    char *pcm_name;
    pcm_name = strdup("plughw:0,0");
  
    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);
  
    /* Open PCM. The last parameter of this function is the mode. */
    /* If this is set to 0, the standard mode is used. Possible   */
    /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */ 
    /* If SND_PCM_NONBLOCK is used, read / write access to the    */
    /* PCM device will return immediately. If SND_PCM_ASYNC is    */
    /* specified, SIGIO will be emitted whenever a period has     */
    /* been completely processed by the soundcard.                */
    if (snd_pcm_open(&pcm_handle, pcm_name, stream, 0) < 0) {
      fprintf(stderr, "Error opening PCM device %s\n", pcm_name);
      pcm_handle=NULL;
      return;
    }
  
    /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Can not configure this PCM device.\n");
      pcm_handle=NULL;
      return;
    }
  
    int rate = samplerate; /* Sample rate */
    unsigned int exact_rate;   /* Sample rate returned by */
                      /* snd_pcm_hw_params_set_rate_near */ 
    int periods = numbuffers;       /* Number of periods */
    snd_pcm_uframes_t periodsize = BUFFER_SIZE_SAMPLES * 4; /* Periodsize (bytes) */
    
    /* Set access type. This can be either    */
    /* SND_PCM_ACCESS_RW_INTERLEAVED or       */
    /* SND_PCM_ACCESS_RW_NONINTERLEAVED.      */
    /* There are also access types for MMAPed */
    /* access, but this is beyond the scope   */
    /* of this introduction.                  */
    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
      fprintf(stderr, "Error setting access.\n");
      pcm_handle=NULL;
      return;
    }
  
    /* Set sample format */
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
      fprintf(stderr, "Error setting format.\n");
      pcm_handle=NULL;
      return;
    }

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */ 
    exact_rate = rate;
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0) {
      fprintf(stderr, "Error setting rate.\n");
      pcm_handle=NULL;
      return;
    }
    if (rate != exact_rate) {
      fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n" 
                       "==> Using %d Hz instead.\n", rate, exact_rate);
    }

    /* Set number of channels */
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, nchannels) < 0) {
      fprintf(stderr, "Error setting channels.\n");
      pcm_handle=NULL;
      return;
    }

    /* Set number of periods. Periods used to be called fragments. */ 
    if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0) < 0) {
      fprintf(stderr, "Error setting periods.\n");
      pcm_handle=NULL;
      return;
    }
  
    // The unit of the buffersize depends on the function. Sometimes it is given in bytes, sometimes the number of frames has to be specified. One frame is the sample data vector for all channels. For 16 Bit stereo data, one frame has a length of four bytes.
    /* Set buffer size (in frames). The resulting latency is given by */
    /* latency = periodsize * periods / (rate * bytes_per_frame)     */
    if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, (periodsize * periods)>>2) < 0) {
      fprintf(stderr, "Error setting buffersize.\n");
      pcm_handle=NULL;
      return;
    }
  
    // If your hardware does not support a buffersize of 2^n, you can use the function snd_pcm_hw_params_set_buffer_size_near. This works similar to snd_pcm_hw_params_set_rate_near. Now we apply the configuration to the PCM device pointed to by pcm_handle. This will also prepare the PCM device.
  
    /* Apply HW parameter settings to */
    /* PCM device and prepare device  */
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Error setting HW params.\n");
      pcm_handle=NULL;
      return;
    }
    
    // Buffer to be used for audio data
    data = (unsigned char *)malloc(periodsize);
    if (!data) {
      fprintf(stderr, "Error allocating memory.\n");
      pcm_handle=NULL;
      return;
    }
}


void audio_exit(void)
{
   if (pcm_handle) {
      /* Stop PCM device and drop pending frames */
      snd_pcm_drop(pcm_handle);
      free(data);
   }
}

// Call this regularly to keep the audio going
void audio_update(void)
{
   while(pcm_handle) {
      int i;
      int pcmreturn;
      int frames = snd_pcm_avail(pcm_handle);
      if (frames<0) return;
      if (frames<BUFFER_SIZE_SAMPLES) return;
      int16_t *p = (int16_t *) data;

      // fill the buffer
      for (i=0; i<BUFFER_SIZE_SAMPLES; i++)
      {
          if (audio_num>0)
          {
            *p++=*audio_start;
            *p++=*audio_start++;
            audio_num--;
          }
          else
          {
            *p++ = 0;
            *p++ = 0;
          }
         
      }
      while ((pcmreturn = snd_pcm_writei(pcm_handle, data, BUFFER_SIZE_SAMPLES)) < 0) {
        snd_pcm_prepare(pcm_handle);
        fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
      }
   }
}

/* Play samples found between the two locations */
void audio_play(char *start, char *end)
{
	audio_start=(int16_t*)start;
	audio_num=((int)(end-start))>>1;
}

