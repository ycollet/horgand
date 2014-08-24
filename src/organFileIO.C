/*
  horgand - a organ software

  organFileIO.C  -  File Input/Output functions
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
#include <dirent.h>

#define BANK_EXTENSION ".horeb"

void
HOR::savefile (char *filename)
{

  int i;
  FILE *fn;
  char buf[2048];
  fn = fopen (filename, "w");

  bzero(buf,sizeof(buf));
  sprintf(buf,"%s\n",VERSION);
  fputs(buf,fn);

  for (i = 1; i <= 10; i++)
    {
      bzero (buf, sizeof (buf));
      sprintf (buf, "%d,%f,%f,%d\n", a.Operator[i].harmonic,
	       a.Operator[i].harmonic_fine,a.Operator[i].volumen,a.Operator[i].marimba);
      fputs (buf, fn);
    }

  bzero (buf, sizeof (buf));
  sprintf (buf, "%f,%d,%f,%f,%f,%d,%f,%f\n",
	   a.Organ_Master_Volume, a.organ_transpose, a.Pitch_LFO_Speed, a.Pitch_LFO_Delay, a.Rotary_LFO_Speed, a.E_Rotary_On, a.LFOpitch,
	   a.modulation);
  fputs (buf, fn);
  bzero (buf, sizeof (buf));
  sprintf (buf, "%f,%d,%f,%d,%f,%f\n", a.attack, a.E_Reverb_On, a.detune, a.E_Delay_On,
	   a.Delay_Delay, a.Delay_Volume);
  fputs (buf, fn);
  bzero (buf, sizeof (buf));
  sprintf (buf, "%d,%d,%f,%d,%f,%f,%f\n", a.scaling, a.E_Chorus_On, a.Chorus_Delay, a.Reverb_Preset,
	   a.Chorus_LFO_Speed, a.Chorus_LFO_Amplitude, a.Chorus_Volume);
  fputs (buf, fn);
  bzero (buf, sizeof (buf));
  sprintf (buf, "%f,%f,%f,%f,%f\n", a.decay, a.sustain, a.p_attack, a.p_decay,
             a.Rotary_LFO_Amplitude);
  fputs (buf, fn);
  for (i = 1; i <= 22; i++)
  {           
  bzero (buf, sizeof (buf));
  sprintf (buf, "%f\n",a.Normalize[i]);
  fputs (buf, fn);
  }

  bzero (buf, sizeof (buf));
  sprintf (buf, "%d,%d,%f,%f,%f,%f,%f,%f\n", a.Speed_Sync,a.Click,a.Click_Vol,a.Click2_Vol,a.Click_Freq,a.Click_Freq2,a.Click_Vol1,a.Click_Vol2);
  fputs (buf, fn);
 
  for (i = 1; i <= 10; i++)
      {
            bzero (buf, sizeof (buf));
            sprintf (buf, "%d\n",a.Operator[i].wave);
            fputs(buf,fn);
      }

  bzero (buf, sizeof (buf));
  sprintf (buf, "%d,%d,%d\n",a.LFO_Wave,a.Chorus_Wave,a.Rotary_Wave);
  fputs (buf, fn);

  bzero (buf, sizeof (buf));
  fputs (a.nombre, fn);
  fputs ("\n", fn);
  fclose (fn);

};

void
HOR::loadfile (char *filename)
{

  int i;
  FILE *fn;
  char buf[2048];
  float Data_Version;

  if ((fn = fopen (filename, "r")) == NULL) return;

  
  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf(buf, "%f\n",&Data_Version);          
  
  

  if (Data_Version < 1.11)
     {
      printf("old file format not supported\n");     
      fclose(fn);
      return;
     } 

  New();

  for (i = 1; i <= 10; i++)
    {
      bzero (buf, sizeof (buf));
      fgets (buf, sizeof buf, fn);
      sscanf (buf, "%d,%f,%f,%d", &a.Operator[i].harmonic,
	      &a.Operator[i].harmonic_fine,&a.Operator[i].volumen,&a.Operator[i].marimba);
    }

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf,
	  "%f,%d,%f,%f,%f,%d,%f,%f", &a.Organ_Master_Volume, &a.organ_transpose, &a.Pitch_LFO_Speed,
	  &a.Pitch_LFO_Delay, &a.Rotary_LFO_Speed, &a.E_Rotary_On, &a.LFOpitch, &a.modulation);

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%f,%d,%f,%d,%f,%f", &a.attack, &a.E_Reverb_On, &a.detune, &a.E_Delay_On,
	  &a.Delay_Delay, &a.Delay_Volume);

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf
    (buf, "%d,%d,%f,%d,%f,%f,%f\n", &a.scaling, &a.E_Chorus_On, &a.Chorus_Delay, &a.Reverb_Preset,
     &a.Chorus_LFO_Speed, &a.Chorus_LFO_Amplitude, &a.Chorus_Volume);


  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%f,%f,%f,%f,%f\n", &a.decay, &a.sustain, &a.p_attack, &a.p_decay,
             &a.Rotary_LFO_Amplitude);
  
  for (i = 1; i <= 22; i++)
  {           
  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%f\n",&a.Normalize[i]);
  
  }

  if (Data_Version >= 1.12)
  {
   bzero (buf, sizeof (buf));
   fgets (buf, sizeof buf, fn);
   sscanf (buf, "%d %d,%f,%f,%f,%f,%f,%f\n", &a.Speed_Sync,&a.Click,&a.Click_Vol,&a.Click2_Vol,&a.Click_Freq,&a.Click_Freq2,&a.Click_Vol1,&a.Click_Vol2);


    for (i = 1; i <= 10; i++)
     {
       bzero (buf, sizeof (buf));
       fgets (buf, sizeof buf, fn);
       sscanf (buf, "%d\n",&a.Operator[i].wave);
      }
  }

 if (Data_Version>=1.13)
   {
   
  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%d,%d,%d\n",&a.LFO_Wave,&a.Chorus_Wave,&a.Rotary_Wave);
  
   }


  bzero (buf, sizeof (buf));
  bzero (a.Name, sizeof (a.Name));
  fgets (buf, sizeof buf, fn);
  for (i = 0; i <= 24; i++)
    if (buf[i] > 20)
      a.Name[i] = buf[i];
  a.nombre = a.Name;
  fclose (fn);

};

void
HOR::savebank (char *filename)
{
  int i,j;
  FILE *fn;
  char buf[2048];
  strcpy(SavedBank,filename); 
  fn = fopen (filename, "w");

  bzero(buf,sizeof(buf));
  sprintf(buf,"%s\n",VERSION);
  fputs(buf,fn);

  for (j = 1; j <= 32; j++)
    {
      for (i = 1; i <= 10; i++)
	{
	  bzero (buf, sizeof (buf));
	  sprintf (buf, "%d,%f,%f,%d\n",
		   Banco[j].Operator[i].harmonic,
		   Banco[j].Operator[i].harmonic_fine,
		   Banco[j].Operator[i].volumen,Banco[j].Operator[i].marimba);
	  fputs (buf, fn);
	}

      bzero (buf, sizeof (buf));
      sprintf (buf, "%f,%d,%f,%f,%f,%d,%f,%f\n",
	       Banco[j].Organ_Master_Volume, Banco[j].organ_transpose, Banco[j].Pitch_LFO_Speed,
	       Banco[j].Pitch_LFO_Delay, Banco[j].Rotary_LFO_Speed, Banco[j].E_Rotary_On,
	       Banco[j].LFOpitch, Banco[j].modulation);


      fputs (buf, fn);
      bzero (buf, sizeof (buf));
      sprintf (buf, "%f,%d,%f,%d,%f,%f\n", Banco[j].attack, Banco[j].E_Reverb_On,
	       Banco[j].detune, Banco[j].E_Delay_On, Banco[j].Delay_Delay,
	       Banco[j].Delay_Volume);
      fputs (buf, fn);
      bzero (buf, sizeof (buf));
      sprintf
	(buf, "%d,%d,%f,%d,%f,%f,%f\n", Banco[j].scaling, Banco[j].E_Chorus_On,
	 Banco[j].Chorus_Delay, Banco[j].Reverb_Preset, Banco[j].Chorus_LFO_Speed,
	 Banco[j].Chorus_LFO_Amplitude, Banco[j].Chorus_Volume);
      fputs (buf, fn);

     bzero (buf, sizeof (buf));
     sprintf (buf, "%f,%f,%f,%f,%f\n", Banco[j].decay, Banco[j].sustain, Banco[j].p_attack, Banco[j].p_decay,
             Banco[j].Rotary_LFO_Amplitude);
     fputs (buf, fn);
     for (i = 1; i <= 22; i++)
        {           
          bzero (buf, sizeof (buf));
          sprintf (buf, "%f\n",Banco[j].Normalize[i]);
          fputs (buf, fn);
        }

     bzero (buf, sizeof (buf));
     sprintf (buf, "%d,%d,%f,%f,%f,%f,%f,%f\n", Banco[j].Speed_Sync,Banco[j].Click, Banco[j].Click_Vol, Banco[j].Click2_Vol, Banco[j].Click_Freq,
             Banco[j].Click_Freq2,Banco[j].Click_Vol1,Banco[j].Click_Vol2);
     fputs (buf, fn);
 
   for (i = 1; i <= 10; i++)
      {
          bzero (buf, sizeof (buf));
          sprintf (buf, "%d\n",Banco[j].Operator[i].wave);
          fputs(buf,fn);
      }
   


   bzero (buf, sizeof (buf));
   sprintf (buf, "%d,%d,%d\n",Banco[j].LFO_Wave,Banco[j].Chorus_Wave,Banco[j].Rotary_Wave);
   fputs (buf, fn);
   

      bzero (buf, sizeof (buf));
      for (i = 0; i <= 24; i++)
	buf[i] = Banco[j].Name[i];
      fputs (buf, fn);
      fputs ("\n", fn);
      fputs ("---\n", fn);
    }
  fclose (fn);

};


void
HOR::loadbank (char *filename)
{

  int i,j;
  FILE *fn;
  char buf[2048];
  float Data_Version;
  
  
  if ((fn = fopen (filename, "r")) == NULL) return;

  strcpy(SavedBank,filename);
  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf(buf, "%f\n",&Data_Version);
            
  if ( Data_Version<1.11)
     {
      printf("old file format, please install the default bank file\n");     
      fclose(fn);
      return;
     } 
  New_Bank();

  for (j = 1; j <= 32; j++)
    {
      for (i = 1; i <= 10; i++)
	{
	  bzero (buf, sizeof (buf));
	  fgets (buf, sizeof buf, fn);

	  sscanf (buf, "%d,%f,%f,%d",
		  &Banco[j].Operator[i].harmonic,
		  &Banco[j].Operator[i].harmonic_fine,
		  &Banco[j].Operator[i].volumen,&Banco[j].Operator[i].marimba);
	}

      bzero (buf, sizeof (buf));
      fgets (buf, sizeof buf, fn);
      sscanf (buf,
	      "%f,%d,%f,%f,%f,%d,%f,%f", &Banco[j].Organ_Master_Volume,
	      &Banco[j].organ_transpose, &Banco[j].Pitch_LFO_Speed, &Banco[j].Pitch_LFO_Delay,
	      &Banco[j].Rotary_LFO_Speed, &Banco[j].E_Rotary_On, &Banco[j].LFOpitch,
	      &Banco[j].modulation);

      bzero (buf, sizeof (buf));
      fgets (buf, sizeof buf, fn);
      sscanf (buf,
	      "%f,%d,%f,%d,%f,%f", &Banco[j].attack, &Banco[j].E_Reverb_On,
	      &Banco[j].detune, &Banco[j].E_Delay_On, &Banco[j].Delay_Delay,
	      &Banco[j].Delay_Volume);

      bzero (buf, sizeof (buf));
      fgets (buf, sizeof buf, fn);
      sscanf
	(buf, "%d,%d,%f,%d,%f,%f,%f\n", &Banco[j].scaling, &Banco[j].E_Chorus_On,
	 &Banco[j].Chorus_Delay, &Banco[j].Reverb_Preset, &Banco[j].Chorus_LFO_Speed,
	 &Banco[j].Chorus_LFO_Amplitude, &Banco[j].Chorus_Volume);


     bzero (buf, sizeof (buf));
     fgets (buf, sizeof buf, fn);
     sscanf (buf, "%f,%f,%f,%f,%f\n", &Banco[j].decay, &Banco[j].sustain, &Banco[j].p_attack, &Banco[j].p_decay,
             &Banco[j].Rotary_LFO_Amplitude);
     for (i = 1; i <= 22; i++)
        {           
          bzero (buf, sizeof (buf));
          fgets (buf, sizeof buf, fn);
          sscanf (buf, "%f\n",&Banco[j].Normalize[i]);
        }

      if (Data_Version>=1.12)
      {
       bzero (buf, sizeof (buf));
       fgets (buf, sizeof buf, fn);
       sscanf (buf, "%d,%d,%f,%f,%f,%f,%f,%f\n", &Banco[j].Speed_Sync,&Banco[j].Click, &Banco[j].Click_Vol, &Banco[j].Click2_Vol, &Banco[j].Click_Freq,
             &Banco[j].Click_Freq2,&Banco[j].Click_Vol1,&Banco[j].Click_Vol2);

       
    for (i = 1; i <= 10; i++)
     {
       bzero (buf, sizeof (buf));
       fgets (buf, sizeof buf, fn);
       sscanf (buf, "%d\n",&Banco[j].Operator[i].wave);
      }

       } 

    if (Data_Version>=1.13)
       {
         bzero (buf, sizeof (buf));
         fgets (buf, sizeof buf, fn);
         sscanf (buf, "%d,%d,%d\n",&Banco[j].LFO_Wave,&Banco[j].Chorus_Wave,&Banco[j].Rotary_Wave);
           
        }
                



      bzero (buf, sizeof (buf));
      bzero (Banco[j].Name, sizeof (Banco[j].Name));
      fgets (buf, sizeof buf, fn);
      for (i = 0; i <= 24; i++)
	if (buf[i] > 20)
	  Banco[j].Name[i] = buf[i];
          Banco[j].nombre=Banco[j].Name;
      bzero (buf, sizeof (buf));
      fgets (buf, sizeof buf, fn);

    }

  fclose (fn);
};


void
HOR::loadrhyt(char *filename)
{
   int i;
   FILE *fs;
   char temp[1024];
   
   bzero(temp,sizeof(temp));
 
  if ((fs = fopen (filename, "r")) != NULL)
   {
         int linea = 0;
         Num_Rhythm= 1;
         int w;    
      while (fgets(temp, sizeof temp, fs) != NULL)
          {
             linea++;
             switch (linea)
           { 
             case 1:
              bzero(Rt[Num_Rhythm].Nom,sizeof(Rt[Num_Rhythm].Nom));
              for (i = 0; i <= (int) strlen(temp) - 2; i++) Rt[Num_Rhythm].Nom[i] = temp[i];                   
              break; 
             case 2:
              bzero(Rt[Num_Rhythm].Nfile,sizeof(Rt[Num_Rhythm].Nfile));
              for (i = 0; i <= (int) strlen(temp) - 2; i++) Rt[Num_Rhythm].Nfile[i] = temp[i];
              break;
             case 3:
              sscanf (temp,"%d", &Rt[Num_Rhythm].bars);
              break;
             case 4:
              sscanf (temp,"%d", &Rt[Num_Rhythm].quarter_note);
              break;
             case 5: 
             sscanf(temp,"%d,%d,%d,%d",&Rt[Num_Rhythm].Line_Bass_Note[1],&Rt[Num_Rhythm].Line_Bass_Note[2],&Rt[Num_Rhythm].Line_Bass_Note[3],&Rt[Num_Rhythm].Line_Bass_Note[4]);
             for (i=1; i<Rt[Num_Rhythm].bars*Rt[Num_Rhythm].quarter_note; i++)
             {
              w = i*4;
              bzero(temp,sizeof(temp));
              fgets(temp, sizeof temp, fs);
              sscanf(temp,"%d,%d,%d,%d",&Rt[Num_Rhythm].Line_Bass_Note[w+1],&Rt[Num_Rhythm].Line_Bass_Note[w+2],&Rt[Num_Rhythm].Line_Bass_Note[w+3],&Rt[Num_Rhythm].Line_Bass_Note[w+4]);
             } 
             bzero(temp,sizeof(temp));
             fgets(temp, sizeof temp, fs);
             sscanf(temp,"%d,%d,%d,%d",&Rt[Num_Rhythm].Line_Bass_Velocity[1],&Rt[Num_Rhythm].Line_Bass_Velocity[2],&Rt[Num_Rhythm].Line_Bass_Velocity[3],&Rt[Num_Rhythm].Line_Bass_Velocity[4]);
             for (i=1; i<Rt[Num_Rhythm].bars*Rt[Num_Rhythm].quarter_note; i++)
             {
              w = i*4;
              bzero(temp,sizeof(temp));
              fgets(temp, sizeof temp, fs);
              sscanf(temp,"%d,%d,%d,%d",&Rt[Num_Rhythm].Line_Bass_Velocity[w+1],&Rt[Num_Rhythm].Line_Bass_Velocity[w+2],&Rt[Num_Rhythm].Line_Bass_Velocity[w+3],&Rt[Num_Rhythm].Line_Bass_Velocity[w+4]);
             }
              break;
              case 6:
              break;
              case 7:
              break;
              case 8:
              break;
              case 9:
              break;
              case 10:
              break;
              case 11:
              linea = 0;
              Num_Rhythm++;
              break;
          }
 


         }
       }




};


void
HOR::saverhyt(char *filename)
{
   int i,j;
   FILE *fs;
   char temp[1024];
   int w=0;   
   bzero(temp,sizeof(temp));
 
  if ((fs = fopen (filename, "w")) != NULL)
   {     
     for(Num_Rhythm=1; Num_Rhythm<=20; Num_Rhythm++)
       {    
         bzero(temp,sizeof(temp));
         sprintf(temp,"%s\n",Rt[Num_Rhythm].Nom);
         fputs (temp, fs);        
         bzero(temp,sizeof(temp));           
         sprintf(temp,"%s\n",Rt[Num_Rhythm].Nfile);
         fputs (temp, fs);
         bzero(temp,sizeof(temp));
         sprintf (temp,"%d\n", Rt[Num_Rhythm].bars);
         fputs (temp, fs);
         bzero(temp,sizeof(temp));
         sprintf (temp,"%d\n", Rt[Num_Rhythm].quarter_note);
         fputs (temp, fs);
         bzero(temp,sizeof(temp));
         sprintf(temp,"%d,%d,%d,%d\n",Rt[Num_Rhythm].Line_Bass_Note[1],Rt[Num_Rhythm].Line_Bass_Note[2],Rt[Num_Rhythm].Line_Bass_Note[3],Rt[Num_Rhythm].Line_Bass_Note[4]);
         fputs (temp, fs);
         for (i=1; i<Rt[Num_Rhythm].bars*Rt[Num_Rhythm].quarter_note; i++)
             {
              w = i*4;
              bzero(temp,sizeof(temp));
              sprintf(temp,"%d,%d,%d,%d\n",Rt[Num_Rhythm].Line_Bass_Note[w+1],Rt[Num_Rhythm].Line_Bass_Note[w+2],Rt[Num_Rhythm].Line_Bass_Note[w+3],Rt[Num_Rhythm].Line_Bass_Note[w+4]);
              fputs (temp, fs);
             } 
             bzero(temp,sizeof(temp));
             sprintf(temp,"%d,%d,%d,%d\n",Rt[Num_Rhythm].Line_Bass_Velocity[1],Rt[Num_Rhythm].Line_Bass_Velocity[2],Rt[Num_Rhythm].Line_Bass_Velocity[3],Rt[Num_Rhythm].Line_Bass_Velocity[4]);
             fputs (temp, fs);

             for (i=1; i<Rt[Num_Rhythm].bars*Rt[Num_Rhythm].quarter_note; i++)
             {
              w = i*4;
              bzero(temp,sizeof(temp));
              sprintf(temp,"%d,%d,%d,%d\n",Rt[Num_Rhythm].Line_Bass_Velocity[w+1],Rt[Num_Rhythm].Line_Bass_Velocity[w+2],Rt[Num_Rhythm].Line_Bass_Velocity[w+3],Rt[Num_Rhythm].Line_Bass_Velocity[w+4]);
              fputs (temp, fs);
             }
              for (j=1; j<=6; j++)
              {
              bzero(temp, sizeof(temp));
              sprintf(temp, "0\n");
              fputs(temp,fs);
              }
          }
     fclose(fs);
  }
};

void
HOR::ScanDir()
{

char nomdir[256];
char nombank[256];

Nums=0;

bzero(nomdir,sizeof(nomdir));
sprintf (nomdir, "%s%s", getenv ("HOME"), "/.horgand");

DIR *dir=opendir(nomdir);
if (dir==NULL) return;


struct dirent *fs;

while ((fs=readdir(dir)))
{

if (strstr(fs->d_name,BANK_EXTENSION)!=NULL)
  { 
    bzero(nombank,sizeof(nombank));
    sprintf(nombank,"%s/%s",nomdir,fs->d_name);
    ReadNames(nombank,fs->d_name);
    if (Nums > 999) return;  
   }

   
}


closedir(dir);

};


void
HOR::ReadNames(char *filename,char *bankname)
{

  int i,j,k;
  FILE *fn;
  char buf[2048];
  float Data_Version;

  if ((fn = fopen (filename, "r")) == NULL) return;

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf(buf,"%f\n",&Data_Version);          
  if ( Data_Version<1.11)     {
      printf("old file format, please install the default bank file\n");     
      fclose(fn);
      return;
     } 

  k=36;
  if (Data_Version>=1.12) k=47;
  if (Data_Version>=1.13) k=48;
 
  
  for (j = 1; j <= 32; j++)
    {
      for (i = 1; i <= k; i++)
	{
	  bzero (buf, sizeof (buf));
	  fgets (buf, sizeof buf, fn);
	}


      bzero (buf, sizeof (buf));
      bzero (NBP[Nums].Name, sizeof (NBP[Nums].Name));
      fgets (buf, sizeof buf, fn);
      for (i = 0; i <= 24; i++)
	if (buf[i] > 20)
	  NBP[Nums].Name[i] = buf[i];
      bzero (NBP[Nums].Bank, sizeof (NBP[Nums].Bank)); 	  
          strcpy(NBP[Nums].Bank,bankname);
          NBP[Nums].pos=j;
          Nums++;
          if (Nums > 999)
           { 
             printf("Sounds full\n");
             fclose(fn);
             return;
           }
           

      bzero (buf, sizeof (buf));
      fgets (buf, sizeof buf, fn);

    }

  fclose (fn);
};


void
HOR::LoadSoundBank(int Num)
{


  float Data_Version;
  int i,j,k;
  FILE *fn;
  char buf[2048];
  char nomfile[256];
  bzero(nomfile,sizeof(nomfile));
  sprintf (nomfile, "%s%s%s", getenv ("HOME"), "/.horgand/",NBP[Num].Bank);


  if ((fn = fopen (nomfile, "r")) == NULL) return;

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf(buf,"%f\n",&Data_Version);          
  if (Data_Version<1.11)
     {
      printf("old file format, please install the default bank file\n");     
      fclose(fn);
      return;
     } 

  j=1;
  k=38;
  if (Data_Version>=1.12) k=49;
  if (Data_Version>=1.13) k=50;
  
  while (j < NBP[Num].pos)
    {
    
     for (i = 1; i <= k; i++)
	{
	  bzero (buf, sizeof (buf));
	  fgets (buf, sizeof buf, fn);
	}
     j++;	
    }

  New();
  
  for (i = 1; i <= 10; i++)
    {
      bzero (buf, sizeof (buf));
      fgets (buf, sizeof buf, fn);
      sscanf (buf, "%d,%f,%f,%d", &a.Operator[i].harmonic,
	      &a.Operator[i].harmonic_fine,&a.Operator[i].volumen,&a.Operator[i].marimba);
    }

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf,
	  "%f,%d,%f,%f,%f,%d,%f,%f", &a.Organ_Master_Volume, &a.organ_transpose, &a.Pitch_LFO_Speed,
	  &a.Pitch_LFO_Delay, &a.Rotary_LFO_Speed, &a.E_Rotary_On, &a.LFOpitch, &a.modulation);

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%f,%d,%f,%d,%f,%f", &a.attack, &a.E_Reverb_On, &a.detune, &a.E_Delay_On,
	  &a.Delay_Delay, &a.Delay_Volume);

  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf
    (buf, "%d,%d,%f,%d,%f,%f,%f\n", &a.scaling, &a.E_Chorus_On, &a.Chorus_Delay, &a.Reverb_Preset,
     &a.Chorus_LFO_Speed, &a.Chorus_LFO_Amplitude, &a.Chorus_Volume);


  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%f,%f,%f,%f,%f\n", &a.decay, &a.sustain, &a.p_attack, &a.p_decay,
             &a.Rotary_LFO_Amplitude);
  
  for (i = 1; i <= 22; i++)
  {           
  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%f\n",&a.Normalize[i]);
  }

  if (Data_Version >= 1.12)
  {
  bzero (buf, sizeof (buf));
  fgets (buf, sizeof buf, fn);
  sscanf (buf, "%d,%d,%f,%f,%f,%f,%f,%f\n", &a.Speed_Sync,&a.Click,&a.Click_Vol,&a.Click2_Vol,&a.Click_Freq,&a.Click_Freq2,&a.Click_Vol1,&a.Click_Vol2);

  
   for (i = 1; i <= 10; i++)
     {
       bzero (buf, sizeof (buf));
       fgets (buf, sizeof buf, fn);
       sscanf (buf, "%d\n",&a.Operator[i].wave);
      }

  }

 if (Data_Version>=1.13)
    {
      bzero (buf, sizeof (buf));
      fgets (buf, sizeof buf, fn);
      sscanf (buf, "%d,%d,%d\n",&a.LFO_Wave,&a.Chorus_Wave,&a.Rotary_Wave);
     }
             


  bzero (buf, sizeof (buf));
  bzero (a.Name, sizeof (a.Name));
  fgets (buf, sizeof buf, fn);
  for (i = 0; i <= 24; i++)
    if (buf[i] > 20)
      a.Name[i] = buf[i];
  a.nombre = a.Name;
  fclose (fn);
  
};














