/*
  horgand - a organ software

 organ.C  -  organ functions
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


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <FL/Fl_Preferences.H>
#include "Holrgan.h"

pthread_mutex_t mutex, m_mutex;
int i,j,l1,k,Pexitprogram,MidiInLevel, LastMidiInLevel,UndoCount,preset,Signal_for_Cb_Sliders,BarLead,changeNameChord;
char NameChord[16];
int Selected_Rhythm,commandline;
const char *FilePreset;
int exitwithhelp,gui;
Pixmap p,mask;
XWMHints *hints;


HOR::HOR()
{
  //Init de vars
  cambiaDriver=0;
  Nums=0;
  commandline=0;
  Bass_Type=0;
  TypeRecChord=0;
  Selected_Rhythm=0;
  rperhis=65100;
  capsg=0;
  cl_counter=0;
  Master_Volume=0.70;
  pattern_bars=1;
  ae=1;
  split=0;
  file_ok=0;  
  cpreset=1;
  IsCoIn=0;
  Cyoin=0;
  Pyoin=0;
  Ccin=0;
  Pcin=0;

// FM Operator frequencys

  lasfreq[1] = 0.5; 
  lasfreq[2] = 0.75;
  lasfreq[3] = 1.0;
  lasfreq[4] = 1.5;
  lasfreq[5] = 2.0;
  lasfreq[6] = 2.5;
  lasfreq[7] = 3.0;
  lasfreq[8] = 4.0;
  lasfreq[9] = 4.5;
  lasfreq[10] = 5.0;
  lasfreq[11] = 6.0;
  lasfreq[12] = 7.0;
  lasfreq[13] = 7.5;
  lasfreq[14] = 8.0;
  lasfreq[15] = 9.0;
  lasfreq[16] = 10.0;
  lasfreq[17] = 10.5;
  lasfreq[18] = 11.0;
  lasfreq[19] = 12.0;
  lasfreq[20] = 13.0;
  lasfreq[21] = 14.0;
  lasfreq[22] = 16.0;

  New();


  mastertune=1;
  release=0.6;
  u_release=1.0/release;
  transpose = 0;
  pitch =0.0f;
  pedal=0;
  UndoCount=0;
  Stereo_Side=0;
  To_Stereo_Side=0;
  Rotary_X=0.0f;
  Chorus_X_L=0.0f;
  Chorus_X_R=0.5;
  Reverb_Time=10;
  Reverb_Diffussion=0.1;
  Reverb_Volume=0.20;
  tempo=1;
  Rhythm_Volume = 0.5;
  Bass_Volume = 0.5;
  basspending = 0;
  lpos = 100;
  length_bass_note = 2.0;
  changeNameChord = 0;
  bzero(SavedBank,sizeof(SavedBank));

// Init Reverb Taps 

int tcombl[16]= {10586, 12340, 6400, 13100, 8004, 7200,5130,9037,12045,11237,9456,7634,5389, 8056,10120,11432};
int tcombr[16]= {10518, 11340, 8450, 11100, 9644, 7560,9536,11507,12600,11111,8056,6048,7690,5978,8845,10056};
int tapsg[16]= {36,33,29,27,24,21,17,15,13,16,21,24,27,31,33,36};

for (i=0; i<16; i++)

{
  a_combl[i]=0;
  a_combr[i]=0;
  combl[i] = tcombl[i]/8;
  combr[i] = tcombr[i]/8;
  apsg[i] = tapsg[i];
  apss += apsg[i];
}

for (i=0; i<16; i++)  ready_apsg[i]=(float) apsg[i] / (float) apss;



// Init Chord Names


strcpy(NC[0].Nom,"C");
strcpy(NC[1].Nom,"Db");
strcpy(NC[2].Nom,"D");
strcpy(NC[3].Nom,"Eb");
strcpy(NC[4].Nom,"E");
strcpy(NC[5].Nom,"F");
strcpy(NC[6].Nom,"Gb");
strcpy(NC[7].Nom,"G");
strcpy(NC[8].Nom,"Ab");
strcpy(NC[9].Nom,"A");
strcpy(NC[10].Nom,"Bb");
strcpy(NC[11].Nom,"B");


//Init bass offsets to tune sample notes

AB[0].tune = 1.0;
AB[1].tune = 1.0595;
AB[2].tune = 1.1225;
AB[3].tune = 1.18925;
AB[4].tune = 1.2599385;
AB[5].tune = 1.334862;
AB[6].tune = 2.414373;
AB[7].tune = 0.749235;
AB[8].tune = 0.7938835;
AB[9].tune = 0.8409785;
AB[10].tune = 0.891131;
AB[11].tune = 0.9440365;



// Init Chord Types



//Maj7

TCh[1].third = 0;
TCh[1].five_th = 0;
TCh[1].seven_th = 0;

//7

TCh[2].third = 0;
TCh[2].five_th = 0;
TCh[2].seven_th = -1;

//-7

TCh[3].third = -1;
TCh[3].five_th = 0;
TCh[3].seven_th = -1;

//-7(b5)

TCh[4].third = -1;
TCh[4].five_th = -1;
TCh[4].seven_th = -1;

//dis

TCh[5].third = -1;
TCh[5].five_th = -1;
TCh[5].seven_th = -2;

//aug7

TCh[6].third = 0;
TCh[6].five_th = +1;
TCh[6].seven_th = -1;


//7Sus4

TCh[7].third = +1;
TCh[7].five_th = 0;
TCh[7].seven_th = -1;



// Init Chords (Distances between notes, type, Names)

// 3 Notes Chords

Chord3[1].type = 1;
Chord3[1].fund = 1;
Chord3[1].dist1  = 4;
Chord3[1].dist2  = 3;
strcpy(Chord3[1].Nom,"");

Chord3[2].type = 1;
Chord3[2].fund = 3;
Chord3[2].dist1  = 3;
Chord3[2].dist2  = 5;
strcpy(Chord3[2].Nom,"");

Chord3[3].type = 1;
Chord3[3].fund = 2;
Chord3[3].dist1  = 5;
Chord3[3].dist2  = 4;
strcpy(Chord3[3].Nom,"");

Chord3[4].type = 3;
Chord3[4].fund = 1;
Chord3[4].dist1  = 3;
Chord3[4].dist2  = 4;
strcpy(Chord3[4].Nom,"m");

Chord3[5].type = 3;
Chord3[5].fund = 3;
Chord3[5].dist1  = 4;
Chord3[5].dist2  = 5;
strcpy(Chord3[5].Nom,"m");

Chord3[6].type = 3;
Chord3[6].fund = 2;
Chord3[6].dist1  = 5;
Chord3[6].dist2  = 3;
strcpy(Chord3[6].Nom,"m");

Chord3[7].type = 5;
Chord3[7].fund = 1;
Chord3[7].dist1  = 3;
Chord3[7].dist2  = 3;
strcpy(Chord3[7].Nom,"°");

Chord3[8].type = 6;
Chord3[8].fund = 1;
Chord3[8].dist1  = 4;
Chord3[8].dist2  = 4;
strcpy(Chord3[8].Nom,"+");

Chord3[9].type = 1;
Chord3[9].fund = 1;
Chord3[9].dist1  = 2;
Chord3[9].dist2  = 5;
strcpy(Chord3[9].Nom,"2");

Chord3[10].type = 1;
Chord3[10].fund = 3;
Chord3[10].dist1  = 3;
Chord3[10].dist2  = 2;
strcpy(Chord3[10].Nom,"");

Chord3[11].type = 4;
Chord3[11].fund = 2;
Chord3[11].dist1  = 6;
Chord3[11].dist2  = 3;
strcpy(Chord3[11].Nom,"m(b5)");


Chord3[12].type = 2;
Chord3[12].fund = 1;
Chord3[12].dist1  = 4;
Chord3[12].dist2  = 6;
strcpy(Chord3[12].Nom,"7");

Chord3[13].type = 3;
Chord3[13].fund = 1;
Chord3[13].dist1  = 3;
Chord3[13].dist2  = 7;
strcpy(Chord3[13].Nom,"m7");

Chord3[14].type = 3;
Chord3[14].fund = 1;
Chord3[14].dist1  = 3;
Chord3[14].dist2  = 6;
strcpy(Chord3[14].Nom,"m6");



// 4 Notes Chords

Chord4[1].type = 1;
Chord4[1].fund = 1;
Chord4[1].dist1  = 4;
Chord4[1].dist2  = 3;
Chord4[1].dist3  = 4;
strcpy(Chord4[1].Nom,"Maj7");

Chord4[2].type = 3;
Chord4[2].fund = 1;
Chord4[2].dist1  = 3;
Chord4[2].dist2  = 4;
Chord4[2].dist3  = 3;
strcpy(Chord4[2].Nom,"m7");

Chord4[3].type = 2;
Chord4[3].fund = 1;
Chord4[3].dist1  = 4;
Chord4[3].dist2  = 3;
Chord4[3].dist3  = 3;
strcpy(Chord4[3].Nom,"7");

Chord4[4].type = 4;
Chord4[4].fund = 1;
Chord4[4].dist1  = 3;
Chord4[4].dist2  = 3;
Chord4[4].dist3  = 4;
strcpy(Chord4[4].Nom,"m7(b5)");

Chord4[5].type = 5;
Chord4[5].fund = 1;
Chord4[5].dist1  = 3;
Chord4[5].dist2  = 3;
Chord4[5].dist3  = 3;
strcpy(Chord4[5].Nom,"°7");

Chord4[6].type = 6;
Chord4[6].fund = 1;
Chord4[6].dist1  = 4;
Chord4[6].dist2  = 4;
Chord4[6].dist3  = 2;
strcpy(Chord4[6].Nom,"+7");

Chord4[7].type = 1;
Chord4[7].fund = 1;
Chord4[7].dist1  = 4;
Chord4[7].dist2  = 3;
Chord4[7].dist3  = 5;
strcpy(Chord4[7].Nom,"");

Chord4[8].type = 1;
Chord4[8].fund = 1;
Chord4[8].dist1  = 4;
Chord4[8].dist2  = 3;
Chord4[8].dist3  = 2;
strcpy(Chord4[8].Nom,"6");

Chord4[9].type = 7;
Chord4[9].fund = 1;
Chord4[9].dist1  = 5;
Chord4[9].dist2  = 2;
Chord4[9].dist3  = 3;
strcpy(Chord4[9].Nom,"7(Sus4)");

Chord4[10].type = 3;
Chord4[10].fund = 1;
Chord4[10].dist1  = 3;
Chord4[10].dist2  = 4;
Chord4[10].dist3  = 4;
strcpy(Chord4[10].Nom,"m(Maj7)");

Chord4[11].type = 3;
Chord4[11].fund = 1;
Chord4[11].dist1  = 3;
Chord4[11].dist2  = 4;
Chord4[11].dist3  = 2;
strcpy(Chord4[11].Nom,"m6");

Chord4[12].type = 6;
Chord4[12].fund = 1;
Chord4[12].dist1  = 4;
Chord4[12].dist2  = 4;
Chord4[12].dist3  = 4;
strcpy(Chord4[12].Nom,"+(Maj7)");

Chord4[13].type = 1;
Chord4[13].fund = 1;
Chord4[13].dist1  = 2;
Chord4[13].dist2  = 4;
Chord4[13].dist3  = 3;
strcpy(Chord4[13].Nom,"lyd");

Chord4[14].type = 1;
Chord4[14].fund = 4;
Chord4[14].dist1  = 3;
Chord4[14].dist2  = 4;
Chord4[14].dist3  = 1;
strcpy(Chord4[14].Nom,"Maj7");

Chord4[15].type = 1;
Chord4[15].fund = 4;
Chord4[15].dist1  = 3;
Chord4[15].dist2  = 3;
Chord4[15].dist3  = 2;
strcpy(Chord4[15].Nom,"7");

Chord4[16].type = 6;
Chord4[16].fund = 4;
Chord4[16].dist1  = 4;
Chord4[16].dist2  = 2;
Chord4[16].dist3  = 2;
strcpy(Chord4[16].Nom,"+7");

Chord4[17].type = 1;
Chord4[17].fund = 3;
Chord4[17].dist1  = 3;
Chord4[17].dist2  = 5;
Chord4[17].dist3  = 4;
strcpy(Chord4[17].Nom,"");

Chord4[18].type = 7;
Chord4[18].fund = 4;
Chord4[18].dist1  = 2;
Chord4[18].dist2  = 3;
Chord4[18].dist3  = 2;
strcpy(Chord4[18].Nom,"7(Sus4)");


Chord4[19].type = 3;
Chord4[19].fund = 4;
Chord4[19].dist1  = 4;
Chord4[19].dist2  = 4;
Chord4[19].dist3  = 1;
strcpy(Chord4[19].Nom,"m(Maj7)");

Chord4[20].type = 6;
Chord4[20].fund = 4;
Chord4[20].dist1  = 4;
Chord4[20].dist2  = 4;
Chord4[20].dist3  = 1;
strcpy(Chord4[20].Nom,"+(Maj7)");

Chord4[21].type = 1;
Chord4[21].fund = 3;
Chord4[21].dist1  = 4;
Chord4[21].dist2  = 1;
Chord4[21].dist3  = 4;
strcpy(Chord4[21].Nom,"Maj7");

Chord4[22].type = 3;
Chord4[22].fund = 3;
Chord4[22].dist1  = 3;
Chord4[22].dist2  = 2;
Chord4[22].dist3  = 3;
strcpy(Chord4[22].Nom,"m7");

Chord4[23].type = 2;
Chord4[23].fund = 3;
Chord4[23].dist1  = 3;
Chord4[23].dist2  = 2;
Chord4[23].dist3  = 4;
strcpy(Chord4[23].Nom,"7");

Chord4[24].type = 4;
Chord4[24].fund = 3;
Chord4[24].dist1  = 4;
Chord4[24].dist2  = 2;
Chord4[24].dist3  = 3;
strcpy(Chord4[24].Nom,"m7(b5)");

Chord4[26].type = 6;
Chord4[25].fund = 3;
Chord4[25].dist1  = 2;
Chord4[25].dist2  = 2;
Chord4[25].dist3  = 4;
strcpy(Chord4[25].Nom,"+7");

Chord4[26].type = 1;
Chord4[26].fund = 2;
Chord4[26].dist1  = 5;
Chord4[26].dist2  = 4;
Chord4[26].dist3  = 3;
strcpy(Chord4[26].Nom,"");

Chord4[27].type = 7;
Chord4[27].fund = 3;
Chord4[27].dist1  = 3;
Chord4[27].dist2  = 2;
Chord4[27].dist3  = 5;
strcpy(Chord4[27].Nom,"7(Sus4)");

Chord4[28].type = 2;
Chord4[28].fund = 3;
Chord4[28].dist1  = 4;
Chord4[28].dist2  = 2;
Chord4[28].dist3  = 3;
strcpy(Chord4[28].Nom,"m(Maj7)");

Chord4[29].type = 6;
Chord4[29].fund = 3;
Chord4[29].dist1  = 3;
Chord4[29].dist2  = 1;
Chord4[29].dist3  = 4;
strcpy(Chord4[29].Nom,"+(Maj7)");

Chord4[30].type = 1;
Chord4[30].fund = 2;
Chord4[30].dist1  = 1;
Chord4[30].dist2  = 4;
Chord4[30].dist3  = 3;
strcpy(Chord4[30].Nom,"Maj7");

Chord4[31].type = 3;
Chord4[31].fund = 2;
Chord4[31].dist1  = 2;
Chord4[31].dist2  = 3;
Chord4[31].dist3  = 4;
strcpy(Chord4[31].Nom,"m7");

Chord4[32].type = 4;
Chord4[32].fund = 2;
Chord4[32].dist1  = 2;
Chord4[32].dist2  = 3;
Chord4[32].dist3  = 3;
strcpy(Chord4[32].Nom,"m7(b5)");

Chord4[33].type = 6;
Chord4[33].fund = 2;
Chord4[33].dist1  = 2;
Chord4[33].dist2  = 4;
Chord4[33].dist3  = 4;
strcpy(Chord4[33].Nom,"+7");

Chord4[34].type = 2;
Chord4[34].fund = 4;
Chord4[34].dist1  = 6;
Chord4[34].dist2  = 3;
Chord4[34].dist3  = 5;
strcpy(Chord4[34].Nom,"7");

Chord4[35].type = 7;
Chord4[35].fund = 2;
Chord4[35].dist1  = 2;
Chord4[35].dist2  = 5;
Chord4[35].dist3  = 2;
strcpy(Chord4[35].Nom,"7(Sus4");

Chord4[36].type = 3;
Chord4[36].fund = 2;
Chord4[36].dist1  = 1;
Chord4[36].dist2  = 3;
Chord4[36].dist3  = 4;
strcpy(Chord4[36].Nom,"m(Maj7)");

Chord4[37].type = 6;
Chord4[37].fund = 2;
Chord4[37].dist1  = 1;
Chord4[37].dist2  = 4;
Chord4[37].dist3  = 4;
strcpy(Chord4[37].Nom,"+(Maj7)");

Chord4[38].type = 3;
Chord4[38].fund = 1;
Chord4[38].dist1  = 3;
Chord4[38].dist2  = 4;
Chord4[38].dist3  = 5;
strcpy(Chord4[38].Nom,"m");

Chord4[39].type = 3;
Chord4[39].fund = 3;
Chord4[39].dist1  = 4;
Chord4[39].dist2  = 5;
Chord4[39].dist3  = 3;
strcpy(Chord4[39].Nom,"m");

Chord4[40].type = 3;
Chord4[40].fund = 2;
Chord4[40].dist1  = 5;
Chord4[40].dist2  = 3;
Chord4[40].dist3  = 4;
strcpy(Chord4[40].Nom,"m");

Chord4[41].type = 1;
Chord4[41].fund = 1;
Chord4[41].dist1  = 2;
Chord4[41].dist2  = 2;
Chord4[41].dist3  = 3;
strcpy(Chord4[41].Nom,"9");

Chord4[42].type = 1;
Chord4[42].fund = 4;
Chord4[42].dist1  = 2;
Chord4[42].dist2  = 3;
Chord4[42].dist3  = 5;
strcpy(Chord4[42].Nom,"9");

Chord4[43].type = 1;
Chord4[43].fund = 3;
Chord4[43].dist1  = 3;
Chord4[43].dist2  = 5;
Chord4[43].dist3  = 2;
strcpy(Chord4[43].Nom,"9");


Chord4[44].type = 1;
Chord4[44].fund = 2;
Chord4[44].dist1  = 5;
Chord4[44].dist2  = 2;
Chord4[44].dist3  = 2;
strcpy(Chord4[44].Nom,"9");

Chord4[45].type = 4;
Chord4[45].fund = 1;
Chord4[45].dist1  = 3;
Chord4[45].dist2  = 3;
Chord4[45].dist3  = 5;
strcpy(Chord4[45].Nom,"m(Maj7)(b5)");

Chord4[46].type = 3;
Chord4[46].fund = 1;
Chord4[46].dist1  = 2;
Chord4[46].dist2  = 1;
Chord4[46].dist3  = 4;
strcpy(Chord4[46].Nom,"m7");



// 5 Notes Chords

Chord5[1].type = 1;
Chord5[1].fund = 1;
Chord5[1].dist1  = 2;
Chord5[1].dist2  = 2;
Chord5[1].dist3  = 3;
Chord5[1].dist4   = 4;
strcpy(Chord5[1].Nom,"Maj7/9");

Chord5[2].type = 7;
Chord5[2].fund = 3;
Chord5[2].dist1  = 3;
Chord5[2].dist2  = 2;
Chord5[2].dist3  = 2;
Chord5[2].dist4   = 3;
strcpy(Chord5[2].Nom,"7(Sus4)");

Chord5[3].type = 7;
Chord5[3].fund = 1;
Chord5[3].dist1  = 5;
Chord5[3].dist2  = 2;
Chord5[3].dist3  = 3;
Chord5[3].dist4   = 2;
strcpy(Chord5[3].Nom,"7(Sus4)");

Chord5[4].type = 2;
Chord5[4].fund = 3;
Chord5[4].dist1  = 3;
Chord5[4].dist2  = 2;
Chord5[4].dist3  = 2;
Chord5[4].dist4   = 2;
strcpy(Chord5[4].Nom,"7");

Chord5[5].type = 1;
Chord5[5].fund = 3;
Chord5[5].dist1  = 4;
Chord5[5].dist2  = 1;
Chord5[5].dist3  = 2;
Chord5[5].dist4   = 2;
strcpy(Chord5[5].Nom,"Maj7");

Chord5[6].type = 7;
Chord5[6].fund = 1;
Chord5[6].dist1  = 2;
Chord5[6].dist2  = 3;
Chord5[6].dist3  = 2;
Chord5[6].dist4   = 3;
strcpy(Chord5[6].Nom,"7(Sus4)");

Chord5[7].type = 7;
Chord5[7].fund = 1;
Chord5[7].dist1  = 2;
Chord5[7].dist2  = 3;
Chord5[7].dist3  = 2;
Chord5[7].dist4   = 5;
strcpy(Chord5[7].Nom,"7(Sus4)");

Chord5[8].type = 4;
Chord5[8].fund = 1;
Chord5[8].dist1  = 3;
Chord5[8].dist2  = 2;
Chord5[8].dist3  = 1;
Chord5[8].dist4   = 4;
strcpy(Chord5[8].Nom,"m7(b5)");

Chord5[9].type = 3;
Chord5[9].fund = 1;
Chord5[9].dist1  = 2;
Chord5[9].dist2  = 1;
Chord5[9].dist3  = 4;
Chord5[9].dist4   = 3;
strcpy(Chord5[9].Nom,"m7");






//Init Rhythm


for (j = 1; j<= 20; j++)

{ 
   strcpy(Rt[j].Nom,"Empty");
   strcpy(Rt[j].Nfile,"Empty");
   Rt[j].bars = 1;
   Rt[j].quarter_note = 4;
   for (i=0; i<=64; i++)
   {
    Rt[j].Line_Bass_Note[i] = 0;
    Rt[j].Line_Bass_Velocity[i]= 0;
   }
}


// Init Bank

  New_Bank();
 
//  Init Undo/Redo

  for (j = 0; j <= 100; j++) Undo[j]=a;


// Init Initial state of Undo/Redo buffer

  for (j = 0; j <= 3; j++) Prim[j]=a;


  // Init gated notes

  for (i = 0; i < POLY; i++)
    {
      note_active[i] = 0;
      env_time[i] = 0;
      dcphi[i]=0.0; 
    }

  // Init frequency Notes 


  for (i = 1; i <= 192; i++)
    {
      h[i].f1 = 8.1757989156 * exp ((float) (i - 2) * log (2.0) / 12.0);
      h[i].f2 = 8.1757989156 * exp ((float) (i) * log (2.0) / 12.0);
      h[i].f3 = 8.1757989156 * exp ((float) (i + 2) * log (2.0) / 12.0);
    }


  // Allocate memory for calculated sins

  size_t sizesin = (size_t) (D_PI * 1000)+2; 

  lsin = (float *) malloc (sizeof (float) * (sizesin + 4));
  nsin = (float *) malloc (sizeof (float) * (sizesin + 4));
  msin = (float *) malloc (sizeof (float) * (sizesin + 4));
  psin = (float *) malloc (sizeof (float) * (sizesin + 4));
  qsin = (float *) malloc (sizeof (float) * (sizesin + 4));
  rsin = (float *) malloc (sizeof (float) * (sizesin + 4));
  tsin = (float *) malloc (sizeof (float) * (sizesin + 4));
  ssin = (float *) malloc (sizeof (float) * (sizesin + 4));
  usin = (float *) malloc (sizeof (float) * (sizesin + 4));
  
  
  memset (lsin, 0, sizesin*4);
  memset (nsin, 0, sizesin*4);
  memset (msin, 0, sizesin*4);
  memset (psin, 0, sizesin*4);
  memset (qsin, 0, sizesin*4);
  memset (rsin, 0, sizesin*4);
  memset (tsin, 0, sizesin*4);
  memset (ssin, 0, sizesin*4);
  memset (usin, 0, sizesin*4);
  

   float x_sin;
   for (i = 0; i < (int) sizesin; i++)

    {

      x_sin = (float) ( (float)i * (float) D_PI / (float) sizesin);
      lsin[i] =sin (x_sin);

      if( i > 0) lsin[i-1] = (lsin[i-1] *  ( 1.0 +  lsin[i] - lsin[i-1]));
      if( i > 1) lsin[i-2] = (lsin[i-2] *  ( 1.0 +  lsin[i-1] - lsin[i-2]));
      if( i > 2) lsin[i-3] = (lsin[i-3] *  ( 1.0 +  lsin[i-2] - lsin[i-3]));
      if( i > 3) lsin[i-4] = (lsin[i-4] *  ( 1.0 +  lsin[i-3] - lsin[i-4]));
      if( i > 4) lsin[i-5] = (lsin[i-5] *  ( 1.0 +  lsin[i-4] - lsin[i-5]));
      if( i > 5) lsin[i-6] = (lsin[i-6] *  ( 1.0 +  lsin[i-5] - lsin[i-6]));
      if( i > 6) lsin[i-7] = (lsin[i-7] *  ( 1.0 +  lsin[i-6] - lsin[i-7]));
      if( i > 7) lsin[i-8] = (lsin[i-8] *  ( 1.0 +  lsin[i-7] - lsin[i-8]));


    }

  for (i = 0; i < (int) sizesin; i++)
  {
    
    x_sin = (float) ( i * D_PI / sizesin);

    psin[i]=sin(x_sin+sin(2.0*x_sin));
    nsin[i]=sin(x_sin+lsin[i]);
    msin[i]=sin(x_sin+sin(1.5*x_sin));
      if( i > 0) msin[i-1] = (msin[i-1] *  ( 1.0 +  msin[i] - msin[i-1]));
      if( i > 1) msin[i-2] = (msin[i-2] *  ( 1.0 +  msin[i-1] - msin[i-2]));
      if( i > 2) msin[i-3] = (msin[i-3] *  ( 1.0 +  msin[i-2] - msin[i-3]));
      if( i > 3) msin[i-4] = (msin[i-4] *  ( 1.0 +  msin[i-3] - msin[i-4]));
      if( i > 4) msin[i-5] = (msin[i-5] *  ( 1.0 +  msin[i-4] - msin[i-5]));
      if( i > 5) msin[i-6] = (msin[i-6] *  ( 1.0 +  msin[i-5] - msin[i-6]));
      if( i > 6) msin[i-7] = (msin[i-7] *  ( 1.0 +  msin[i-6] - msin[i-7]));
      if( i > 7) msin[i-8] = (msin[i-8] *  ( 1.0 +  msin[i-7] - msin[i-8]));

    qsin[i]=sin(nsin[i]+lsin[i]+psin[i]); 
    tsin[i]=sin(lsin[i]);
    rsin[i]=sin(lsin[i]+sin(msin[i]));

     ssin[i]=(lsin[i]+nsin[i]+msin[i]+psin[i]+qsin[i]+rsin[i]+tsin[i])/7.0;
      if( i > 0) ssin[i-1] = (ssin[i-1] *  ( 1.0 +  ssin[i] - ssin[i-1]));
      if( i > 1) ssin[i-2] = (ssin[i-2] *  ( 1.0 +  ssin[i-1] - ssin[i-2]));
      if( i > 2) ssin[i-3] = (ssin[i-3] *  ( 1.0 +  ssin[i-2] - ssin[i-3]));
      if( i > 3) ssin[i-4] = (ssin[i-4] *  ( 1.0 +  ssin[i-3] - ssin[i-4]));
      if( i > 4) ssin[i-5] = (ssin[i-5] *  ( 1.0 +  ssin[i-4] - ssin[i-5]));
      if( i > 5) ssin[i-6] = (ssin[i-6] *  ( 1.0 +  ssin[i-5] - ssin[i-6]));
      if( i > 6) ssin[i-7] = (ssin[i-7] *  ( 1.0 +  ssin[i-6] - ssin[i-7]));
      if( i > 7) ssin[i-8] = (ssin[i-8] *  ( 1.0 +  ssin[i-7] - ssin[i-8]));

     usin[i]=sin(ssin[i]);  
 }
 
   for (i = 0; i < (int) sizesin; i++)
   {
   x_sin = (float) ( i * D_PI / sizesin);
   ssin[i]=sin(x_sin+usin[i]); 
   }
  
   

  // Init Sound and effect buffers


  wbuf = (short *) malloc (2 * sizeof (short) * BUFSIZE);
  rbuf = (float *) malloc (2 * sizeof (float) * BUFSIZE);
  bbuf = (float *) malloc (2 * sizeof (float) * BUFSIZE);
  buf = (float *) malloc (2 * sizeof (float) * BUFSIZE);

  history = (float *) malloc (sizeof (float) * 131200);
  cldelay = (float *) malloc (sizeof  (float) * 8192);
  crdelay = (float *) malloc (sizeof (float)  * 8192); 

// Init Buffers


  memset (history, 0, sizeof (float)*131200);
  memset (cldelay, 0, sizeof (float)*8192);
  memset (crdelay, 0, sizeof (float)*8192);

  
  memset (buf, 0, PERIOD4);
  memset (wbuf, 0, PERIOD4);
  memset (rbuf, 0, PERIOD4);  
  memset (bbuf, 0, PERIOD4);

   
};


HOR::~HOR ()
{
if (exitwithhelp==0)  snd_seq_close (midi_in);

};

void
HOR::init_hor()
{

int val=0;
char nomfile[128];
char orden[256];

  //ALSA init
  
  // Open Alsa Seq 
 
  snd_seq_open (&midi_in, "default", SND_SEQ_OPEN_INPUT, 0);
  snd_seq_set_client_name(midi_in, "horgand");
  int alsaport = 0;
  char portname[50];

  // Create Alsa Seq Client
  
  sprintf (portname, "Horgand IN");
  alsaport = snd_seq_create_simple_port (midi_in, portname,
					 SND_SEQ_PORT_CAP_WRITE |
					 SND_SEQ_PORT_CAP_SUBS_WRITE,
					 SND_SEQ_PORT_TYPE_SYNTH);



Fl_Preferences horgand (Fl_Preferences::USER, WEBSITE , PACKAGE);

horgand.get("FirstTime",val,0);

if (val==0)
{

 bzero(nomfile,sizeof(nomfile));
 bzero(orden,sizeof(orden));
 sprintf (nomfile, "%s%s", getenv ("HOME"), "/.horgand");
 sprintf (orden, "mkdir %s",nomfile);
 system(orden);
 bzero(orden,sizeof(orden));
 sprintf(orden, "cp %s/* %s",DATADIR,nomfile);
 system(orden);
 horgand.set("FirstTime",1);
 sprintf(BankFilename,"%s/Default.horeb",nomfile);
 sprintf(RhythmFilename,"%s/Rhythm_List.txt",nomfile);
 horgand.set("Bank Filename", BankFilename);
 horgand.set("Rhythm Filename", RhythmFilename);
 bzero(orden,sizeof(orden));
 sprintf(orden,"%s","Not Connected");
 horgand.set("MIDI IN Device",orden);
 bzero(orden,sizeof(orden));
 sprintf(orden,"%s","Jack");
 horgand.set("Audio Out device",orden);
 horgand.set("Preset",1);                    
}


// Get config settings and init settings 
// Get MIDI IN Setting      

    horgand.get("MIDI IN Device", MID,"Not Connected",40);
    Conecta();          

// Get Audio Out Setting      
            
      char temp[512];

      horgand.get("Audio Out device",temp,"Alsa",127);


      if (strstr (temp, "OSS") != NULL)  Salida = 1;
      if (strstr (temp, "Alsa") != NULL) Salida = 2;
      if (strstr (temp, "Jack") != NULL) Salida = 3;
 
      if (Salida==1) ossaudioprepare();      
      if (Salida==2) alsaaudioprepare();
   
// Load Preset Bank File
    
       horgand.get("Bank Filename",BankFilename,"",127);                     
       if (commandline==0)loadbank(BankFilename);
       ScanDir();                

// Load Rhythm File

       horgand.get("Rhythm Filename",RhythmFilename,"",127);
       loadrhyt(RhythmFilename);


};


void
HOR::Adjust_Audio()
{

      increment = .25 / (float) SAMPLE_RATE;
      D_PI_to_SAMPLE_RATE = D_PI / SAMPLE_RATE;
      
}

// Keyboard Level Scaling
float
HOR::Get_Keyb_Level_Scaling(int nota)
{
return(1.5*velocity[nota]*sin((120-note[nota])/120.0));
};




// Returns the FM Operator Pitch (Frequency + LFO)

float
HOR::pitch_Operator (int i, int note)
{
return (lasfreq[a.Operator[i].harmonic]+a.Operator[i].harmonic_fine);
}


float
HOR::pitch_Operator2 (int i, int note)
{
return (lasfreq[a.Operator[i].harmonic] - a.Operator[i].harmonic_fine);
}




// Returns The FM Operator Volume 

void
HOR::volume_Operator (int i, int l2)
{
 a.Operator[i].con1=a.Operator[i].volumen * velocity[l2] * a.Normalize[a.Operator[i].harmonic];
  
};



// Turn Off all the sound


void
HOR::panic()
{
  int i;
  for (i = 0; i < POLY; i++)
    {
      note_active[i] = 0;
      env_time[i] = 0.0f;
      gate[i]=0;
    }

  memset ( f, 0 ,sizeof f);  
};



float
HOR::Cenvelope (int *note_active,int gate,float t,int nota)
{

float val=0;
       
       if (t > a.c_attack + a.c_decay )  return 0.0f;
       if (t > a.c_attack) val=1.0 - (t - a.c_attack) * u_c_decay;
       else
       val=t * u_c_attack;
       
       if (gate) return (a.Click_Vol*val);  
       return(a.Click2_Vol*val);
};




float
HOR::Penvelope (int *note_active,int gate,float t,int nota)
{

       if ((gate) || (pedal))
       {    
       if (t > a.p_attack + a.p_decay )  return 0.0f;
       if (t > a.p_attack) return(1.0 - (t - a.p_attack) * u_p_decay);
       return(t * u_p_attack);
       }
       else 
       return Perc_Volume[nota] * (1.0-t*u_p_release);




};



float
HOR::Jenvelope (int *note_active, int gate, float t, int nota)
{

  float Env = 0.0f;
  
    if (gate)
    {
       if (t > a.attack + a.decay )  return (a.sustain);
       if (t > a.attack) return(1.0 - (1.0 - a.sustain) * (t - a.attack) * u_decay);
       
       return(t * u_attack);
    }
  else
    {

      if (pedal == 0)
      {  
          if (release>t)
          {
          Env = Envelope_Volume[nota] * (1.0 - t * u_release);
          if (Env < 0.0015)
             {
               if (*note_active) *note_active=0;
                env_time[nota] = 0.0f;
                return(0.0f);
              }
              
             else                           
             return (Env);
          }
          else
          {
          if (*note_active) *note_active=0;
          env_time[nota] = 0.0f;
          return(0.0f);
          }
       }
       
      else
        {
        if (a.sustain != 0) return(a.sustain);
        else  return(1.0 -  (t - a.attack) * u_decay);
        }
     }     

     return(Env);
};



// Returns Pitch LFO 


float
HOR::Pitch_LFO (float t)
{

  float x,out;

  if (t * 20 < a.Pitch_LFO_Delay)
    return (0.0f);

  x=fmod(a.Pitch_LFO_Speed*t,1.0); 

  out = NFsin(a.LFO_Wave,x*D_PI)*LFO_Frequency;

  return(out);     

}


// Return Played Note Frequency

float
HOR::Get_Partial (int nota)
{
  int l;
  float partial=0;
  float freq_note=0; 
  
  l = note[nota] + transpose + a.organ_transpose;
  freq_note=(pitch >0) ? h[l].f2 + (h[l].f3 - h[l].f2) * pitch : h[l].f2 + (h[l].f2 - h[l].f1) * pitch;
  partial = mastertune * freq_note * D_PI_to_SAMPLE_RATE;
  if(partial>D_PI) partial= fmod(partial,D_PI);
  return(partial);
  

};


void
HOR::Calc_LFO_Frequency()
{
LFO_Frequency =  a.modulation * a.LFOpitch * D_PI_to_SAMPLE_RATE;

};  




float 
HOR:: NFsin(int i,float x)
{
   int k;
   
   k=lrintf(x*1000.0);

   if (k>6283) k %= 6283; 
     
   switch(i)
   {
      case 1:
      return(lsin[k]);
      break;
      case 2:
      return(nsin[k]);
      break;
      case 3:
      return(msin[k]);
      break;
      case 4:
      return(psin[k]);
      break;
      case 5:
      return(qsin[k]);
      break;
      case 6:
      return(rsin[k]);
      break;
      case 7:
      return(tsin[k]);
      break;
      case 8:
      return(ssin[k]);
      break;
      case 9:
      return(usin[k]);
      break;
   } 
   return 0.0;
};




// Main Audio thread

void
HOR::Alg1s (int nframes, void *)
{

 pthread_mutex_lock(&mutex);
  int l1, l2, i;
  float sound,sound2;
  float Env_Vol=0.0f;
  float Am_Click=0.0f;
  float Click_TVol=0.0f;
  float Click_Env=0.0f;
  float m_partial;
  float LFO_Volume;
    
  memset (buf, 0, PERIOD8);

    for (l2 = 0; l2 < POLY; l2++)
    {

      if (note_active[l2])
	{
	  m_partial=Get_Partial(l2);
          for(i=1;i<=10;i++) volume_Operator(i,l2);
           
                  
          for (l1 = 0; l1< PERIOD2; l1 +=2)
          {
     	    sound=0.0f;
     	    sound2=0.0f;
            Envelope_Volume[l2] = Jenvelope(&note_active[l2], gate[l2], env_time[l2], l2);        
            Perc_Volume[l2] = Penvelope (&note_active[l2], gate[l2], env_time[l2], l2);        
     	    LFO_Volume=Pitch_LFO(env_time[l2]);
     	         	   
            if (a.Click)
               {
      	       Click_Env=Cenvelope(&note_active[l2], gate[l2], env_time[l2], l2);                     
               if (Click_Env>0.0f)
                  {
                    dcphi[l2] +=Click_sFreq;
                    dcphi2[l2] +=Click_2sFreq;
                    if(dcphi[l2]>D_PI) dcphi[l2] -= D_PI;
                    if(dcphi2[l2]>D_PI) dcphi2[l2] -= D_PI;
                    Click_TVol=Click_Env*velocity[l2]*organ_master;
                    Am_Click=a.Click_Vol1*Click_TVol*NFsin(3,dcphi[l2]);
                    Am_Click+=a.Click_Vol2*Click_TVol*NFsin(3,dcphi2[l2]);
                    buf[l1] +=Am_Click;
                    buf[l1+1] +=Am_Click;
                   }
               }
             for(i = 1; i<=10; i++)
	      {
                  
                  if (a.Operator[i].marimba==0)
	            Env_Vol=Envelope_Volume[l2]*a.Operator[i].con1;
                  else
                    Env_Vol=Perc_Volume[l2]*a.Operator[i].con1; 
                    
                  if(Env_Vol > 0.0f)
                     {                 
                   
                     f[i].dphi = m_partial * (p_op[i] + LFO_Volume);
                     if(f[i].dphi>D_PI) f[i].dphi -= D_PI;
                     f[i].phi[l2] += f[i].dphi;
                     if(f[i].phi[l2]>D_PI) f[i].phi[l2] -= D_PI;

                     f[i].dphi2 = m_partial * (p_op2[i] + LFO_Volume);
                     if(f[i].dphi2>D_PI) f[i].dphi2 -= D_PI; 
                     f[i].phi2[l2] += f[i].dphi2;
                     if(f[i].phi2[l2]>D_PI) f[i].phi2[l2] -= D_PI;                    

                     sound += Env_Vol*NFsin(a.Operator[i].wave,f[i].phi[l2]);
                     sound2 += Env_Vol*NFsin(a.Operator[i].wave,f[i].phi2[l2]);                  
                   }
                                   
              }  
              
                buf[l1] += sound  * organ_master;
                buf[l1+1] += sound2 * organ_master;
                env_time[l2] +=increment;                
           }
             
	}

    }

    

if (a.E_Chorus_On) Effect_Chorus();
if (a.E_Rotary_On) Effect_Rotary();
if (a.E_Delay_On)  Effect_Delay();
if (a.E_Reverb_On) Effect_Reverb();
Write_Buffer_Effects();
if (Rhythm_On) Get_Rhythm();
if (Salida < 3) Final_Output(Salida);
pthread_mutex_unlock(&mutex);
return;

};

  
