/*
  horgand - a organ software

  jackoutput.C  -   jack output
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
(version2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#include <jack/jack.h>
#include <jack/midiport.h>
#include "jackoutput.h"
#include "Holrgan.h"


pthread_mutex_t jmutex;


jack_client_t *jackclient;
jack_options_t options;
jack_status_t status;
  

jack_port_t *outport_left,*outport_right, *jack_midi_in;

int jackprocess (jack_nframes_t nframes,void *arg);
void jackaudioprepare();

HOR *JackOUT;



int
JACKstart(HOR *hor_)
{

   
  JackOUT=hor_; 

  jackclient = jack_client_open("Horgand",options,&status,NULL);;
  if (jackclient == NULL)
    {
      fprintf (stderr, "Cannot make a jack client, back to Alsa\n");
      return (2);
    };

  JackOUT->SAMPLE_RATE=DSAMPLE_RATE;
  fprintf (stderr, "Internal SampleRate   = %d\nJack Output SampleRate= %d\n",
           JackOUT->SAMPLE_RATE, jack_get_sample_rate (jackclient));
  if ((unsigned int) jack_get_sample_rate (jackclient) != (unsigned int) JackOUT->SAMPLE_RATE)
    fprintf (stderr, "Adjusting SAMPLE_RATE to jackd.\n");

  JackOUT->SAMPLE_RATE = jack_get_sample_rate(jackclient);
  JackOUT->PERIOD = jack_get_buffer_size (jackclient);
  JackOUT->Put_Period();
  
  jack_set_process_callback (jackclient, jackprocess, 0);

  outport_left = jack_port_register (jackclient, "out_1",
                                     JACK_DEFAULT_AUDIO_TYPE,
                                     JackPortIsOutput | JackPortIsTerminal,
                                     0);
  outport_right =
    jack_port_register (jackclient, "out_2", JACK_DEFAULT_AUDIO_TYPE,
                        JackPortIsOutput | JackPortIsTerminal, 0);


  jack_midi_in =  jack_port_register(jackclient, "in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
   


  if (jack_activate (jackclient))
    {
      fprintf (stderr, "Cannot activate jack client, back to Alsa\n");
      return (2);
    };

  jack_connect (jackclient, jack_port_name (outport_left),
                "alsa_pcm:playback_1");
  jack_connect (jackclient, jack_port_name (outport_right),
                "alsa_pcm:playback_2");


  pthread_mutex_init (&jmutex, NULL);
  
  return 3;

};


int jackprocess(jack_nframes_t nframes,void *arg)

{

 int i,j,count;
 

jack_midi_event_t midievent;

   jack_default_audio_sample_t *outl = (jack_default_audio_sample_t*)
   jack_port_get_buffer(outport_left, JackOUT->PERIOD);
   jack_default_audio_sample_t *outr = (jack_default_audio_sample_t*)
   jack_port_get_buffer(outport_right, JackOUT->PERIOD);
   
   float *data = (float *)jack_port_get_buffer(jack_midi_in, JackOUT->PERIOD);

pthread_mutex_lock(&jmutex);


count = jack_midi_get_event_count(data);

for (int i = 0; i < count; i++)
{                  
  jack_midi_event_get(&midievent, data, i);
  JackOUT->jack_process_midievents(&midievent);
}  

JackOUT->Alg1s(JackOUT->PERIOD,0);

for (i=0; i<JackOUT->PERIOD; i++)
{
 j = i*2;
 outl[i]=JackOUT->buf[j]*JackOUT->Master_Volume;
 outr[i]=JackOUT->buf[j+1]*JackOUT->Master_Volume;
}

pthread_mutex_unlock(&jmutex);

return(0);

};


void JACKfinish()
{
jack_client_close(jackclient);
}
