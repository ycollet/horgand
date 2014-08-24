/*
  horgand - a organ software

 organAccompaniment.C  -  organ accompaniment functions
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
#include "math.h"


// Calculates the 1/16 position of the drumploop sampleplayed  to play the bass line

void 
HOR::Get_Tempo()


{

int i;

pos = lrintf(tempo * Samples_Readed)/fractional_position+1;
if (pos == 1)  BarLead = 127; else BarLead = 0;
for (i=1; i<=bars; i++) if (pos== 4*quarter_note) BarLead = 127;

};


// Reads the Bass Line and play if found notes


void 

HOR:: Get_Bass_Line()
{

int readcounts,lanota;

  
  if ((Line_Bass_Note[pos] == 0) && (basspending == 0)) return; 

  if ( pos != lpos)
	{
        lpos = pos;
        if (Line_Bass_Note[pos] != 0)
        {
        if (split) Get_Chord();
        lanota = Line_Bass_Note[pos];
        if (lanota == 5 ) lanota += TCh[chord_type].third;
        if (lanota == 8 ) lanota += TCh[chord_type].five_th;
        if (lanota == 12 ) lanota += TCh[chord_type].seven_th;
 
	bass_note = bass_transpose + lanota - 1 + fundamental;
        bass_velocity = Line_Bass_Velocity[pos] / 100.0;
        if (bass_note > 11) bass_note -= 12;
        if (bass_note < 0 ) bass_note += 12;
        length_bass_note = (((AB[bass_note].tune+AB[bass_note].bmt)*44100.0)/SAMPLE_RATE);
        readcounts = sf_seek (infileb, 0, SEEK_SET);
        basspending = 1;
	Get_Bass();
        }
	}
  if (basspending == 1 ) Get_Bass();

};




// Open DrumLoop File


int
HOR::Select_Rhythm(char *nomrit)
{

   
   if (! (infile = sf_open (nomrit, SFM_READ, &sfinfo)))
{
        printf ("Not able to open input file %s.\n", nomrit);
        file_ok = 0;
        return(1);
}
file_ok = 1;       
return(0);
};


// Open Bass Sampled Instrument

void
HOR::Select_Bass(char *nombass)
{
   if (! (infileb = sf_open (nombass, SFM_READ, &sfinfob)))
        printf ("Not able to open input file %s.\n", nombass) ;
        
};


// Put the Sampled Bass on the Audio Buffer


void
HOR::Get_Bass()
{
float l, r, rl, rr;
int i,j,readcounts,readcountr;
int longi;

memset (bbuf, 0, PERIOD8);

longi  = (int) (length_bass_note * PERIOD);

readcounts = sf_seek (infileb, 0, SEEK_CUR); 
readcountr = sf_readf_float (infileb, bbuf, longi);

if ((readcounts + longi ) < frames_bass) basspending = 1; 

else basspending = 0; 


 for (i = 0; i < PERIOD2; i +=2)
    {

      j = (int) (i * length_bass_note);

    
     if (j % 2 != 0) j--;

      l = buf[i];
      r = buf[i+1];

       if (l < -1.0)
        l = -1.0;
      else if (l > 1.0)
        l = 1.0;
      if (r < -1.0)
        r = -1.0;
      else if (r > 1.0)
        r = 1.0;
     

      rl = bbuf[j];
      rr = bbuf[j+1];     
 
      if (rl < -1.0)
        rl = -1.0;
      else if (rl > 1.0)
        rl = 1.0;
 
       if (rl < -1.0)
        rl = -1.0;
      else if (rl > 1.0)
        rl = 1.0;
     

     buf[i] =  l  + (rl * Bass_Volume * bass_velocity);
     buf[i+1] =  r  + (rr * Bass_Volume * bass_velocity);


      }


};


// Put the Sampled Drumloop on the Audio buffer


void HOR::Get_Rhythm() { float l, r, rl, rr; 
int i,j,readcountr,falta; 
int ftempo;

memset (rbuf, 0, PERIOD8);


ftempo  = (int) (tempo * PERIOD);

Get_Tempo();
if (Bass_On == 1 ) Get_Bass_Line();

Samples_Readed  = sf_seek (infile, 0, SEEK_CUR);
readcountr = sf_readf_float (infile, rbuf, ftempo);

if ((readcountr) < ftempo)

{

falta = ftempo - readcountr;
Samples_Readed = sf_seek (infile, 0, SEEK_SET);
readcountr = sf_readf_float (infile, rbuf, falta);

}




 for (i = 0; i < PERIOD2; i += 2)
    {

      j = (int) (i * tempo);
      if (j % 2 != 0) j++; 
     
      l = buf[i];
      r = buf[i+1];
      
       if (l < -1.0)
        l = -1.0;
      else if (l > 1.0)
        l = 1.0;
      if (r < -1.0)
        r = -1.0;
      else if (r > 1.0)
        r = 1.0;

 
      rl = rbuf[j];
      rr = rbuf[j+1];     
 
      if (rl < -1.0)
        rl = -1.0;
      else if (rl > 1.0)
        rl = 1.0;
      if (rr < -1.0)
        rr = -1.0;
      else if (rr > 1.0)
        rr = 1.0;
     

     buf[i] =    ((l * Rhythm_Volume) + (rl * Rhythm_Volume ) * .5);
     buf[i+1] =  ((r * Rhythm_Volume) + (rr * Rhythm_Volume ) * .5);

      }

};
