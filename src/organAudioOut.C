/*
  horgand - a organ software

 organAudioOUT.C  -  Audio OUT functions
  Copyright (C) 2003-2008 Josep Andreu (Holborn)
  Author: Josep Andreu

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/


#include "Holrgan.h"
#include <sys/soundcard.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <math.h>



void
HOR::CloseAudio(int i)
{


       switch(i)
         
         { 
           case 1:
                 close(snd_handle);
                 break;
           case 2:
                 snd_pcm_close (playback_handle);
                 break;
         }
     
};



void
HOR::Final_Output (int S_Output)
{
  int i,j,kk;
  short sl,sr;
  
  
  memset (wbuf, 0, PERIOD8);
  
  for (i = 0; i < PERIOD2; i +=2)
    { 
      sl = (short) (buf[i]*Master_Volume*32767.0);
      sr = (short) (buf[i+1]*Master_Volume*32767.0);
      wbuf[i] = sl;
      wbuf[i+1]=sr;
    }


    switch (S_Output)
    {
    case 1:
      write (snd_handle, wbuf, PERIOD4);
      break;
    case 2:
      kk = snd_pcm_writei (playback_handle, wbuf, PERIOD);
      if (kk < PERIOD)
        {
        printf("xrun!\n");
	snd_pcm_prepare (playback_handle);
	}
      break;
    
    }





};


// OSS AUDIO OUT Check and prepare

void
HOR::ossaudioprepare ()
{

  int snd_bitsize = 16;
  SAMPLE_RATE = DSAMPLE_RATE;
  snd_fragment = 0x00080009;
  snd_stereo = 1;
  snd_format = AFMT_S16_LE;
  snd_samplerate = SAMPLE_RATE;
  PERIOD = MPERIOD;
  Put_Period ();
  snd_handle = open ("/dev/dsp", O_WRONLY, 0);
  if (snd_handle == -1)
    {
      fprintf (stderr, "ERROR - I can't open  /dev/dsp \n");
      return;
    };
  ioctl (snd_handle, SNDCTL_DSP_RESET, NULL);

  ioctl (snd_handle, SNDCTL_DSP_SETFMT, &snd_format);
  ioctl (snd_handle, SNDCTL_DSP_STEREO, &snd_stereo);
  ioctl (snd_handle, SNDCTL_DSP_SPEED, &snd_samplerate);
  ioctl (snd_handle, SNDCTL_DSP_SAMPLESIZE, &snd_bitsize);
  ioctl (snd_handle, SNDCTL_DSP_SETFRAGMENT, &snd_fragment);


};

// ALSA AUDIO OUT Check and prepare

void
HOR::alsaaudioprepare ()
{
  char pcm_name[50];
  sprintf (pcm_name, "plughw:0,0");


  if (snd_pcm_open (&playback_handle, pcm_name, SND_PCM_STREAM_PLAYBACK, 0) <
      0)
    {
      fprintf (stderr, "cannot open audio device %s\n", pcm_name);
      exit (1);
    }
  PERIOD = MPERIOD;
  SAMPLE_RATE=DSAMPLE_RATE;
  Put_Period ();
  snd_pcm_hw_params_alloca (&hw_params);
  snd_pcm_hw_params_any (playback_handle, hw_params);
  snd_pcm_hw_params_set_access (playback_handle, hw_params,
                                SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format (playback_handle, hw_params,
                                SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_rate (playback_handle, hw_params, SAMPLE_RATE, 0);
  snd_pcm_hw_params_set_channels (playback_handle, hw_params, 2);

  snd_pcm_hw_params_set_periods (playback_handle, hw_params, 2, 0);
  snd_pcm_hw_params_set_period_size (playback_handle, hw_params, PERIOD2, 0);
  snd_pcm_hw_params (playback_handle, hw_params);
  snd_pcm_sw_params_alloca (&sw_params);
  snd_pcm_sw_params_current (playback_handle, sw_params);
  snd_pcm_sw_params_set_avail_min (playback_handle, sw_params, PERIOD2);
  snd_pcm_sw_params (playback_handle, sw_params);


};

