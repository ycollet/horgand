/*
  horgand - a organ software

  organMIDIIn.C  -  organ functions
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


void
HOR::jack_process_midievents(jack_midi_event_t *midievent)
{

  int l1;
  int value=0;


  switch (midievent->buffer[0]>>4)
    {
    case 14:
      pitch = (float) (midievent->buffer[1] + 128 * midievent->buffer[2] - 8192) / 8192.0;
      break;

    case 12:

      value = midievent->buffer[1]; 
      if ((value > 0) && (value < 33))
      preset = value;
      break;


    case 11:

      if (midievent->buffer[1] == 1)
	{
         
	a.modulation = (float) midievent->buffer[2] / 128;
        Calc_LFO_Frequency();
        Calc_Chorus_LFO_Frequency();
        }
 
      if (midievent->buffer[1] == 91)
              Reverb_Volume = (float) midievent->buffer[2] / 256.0;

      if (midievent->buffer[1] == 93)
              a.Chorus_Volume = (float) midievent->buffer[2] / 128.0;
            
      if (midievent->buffer[1] == 7)
	Master_Volume = (float) midievent->buffer[2] / 128.0;

      if (midievent->buffer[1] == 64)
	{
          

	  if (midievent->buffer[2] < 64)
	  {
	    pedal = 0;
	    for(l1=0;l1<POLY;l1++) if (gate[l1]==0) env_time[l1]=0; 
	  }  
	  if (midievent->buffer[2] > 63)
	    pedal = 1;
	}



      break;




    case 9:

      if (midievent->buffer[2] != 0)
	{
	  for (l1 = 0; l1 < POLY; l1++)
	    {
	      if (!note_active[l1])
		{
		  rnote[l1] = midievent->buffer[1];
		  note[l1] = rnote[l1];
		  LastMidiInLevel = MidiInLevel;
		  MidiInLevel = midievent->buffer[2];
		  velocity[l1] = midievent->buffer[2] /126.0;
                  if (a.scaling) velocity[l1]=Get_Keyb_Level_Scaling(l1);
                  if (velocity[l1] > 1.0) velocity[l1]=1.0;
		  if ((split) && (rnote[l1] < 60))
		    {
                     
		      note[l1] += 24;
		      velocity[l1] *= .3;
		    }

		  env_time[l1] = 0;
		  gate[l1] = 1;
		  note_active[l1] = 1;
                  if (split) Get_Chord();
                  break;
		}
	    }
	  break;
	}
      else
	{
	  LastMidiInLevel = MidiInLevel;
	  MidiInLevel = 0;
	  for (l1 = 0; l1 < POLY; l1++)
	    {

	      if (gate[l1] && note_active[l1]
		  && (rnote[l1] == midievent->buffer[1]))
		{
                  gate[l1] = 0;
                  if (pedal==0) env_time[l1] = 0;
                  if (split) Get_Chord();
		}

	    }
	}
      break;


    case 8:

      LastMidiInLevel = MidiInLevel;
      MidiInLevel = 0;
      for (l1 = 0; l1 < POLY; l1++)
	{

	  if (gate[l1] && note_active[l1]
	      && (rnote[l1] == midievent->buffer[1]))
	    {
              gate[l1] = 0;
              if (pedal==0) env_time[l1] = 0;
              if (split) Get_Chord();
	    }

	}
      break;
    }

};


