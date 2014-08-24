/*
  horgand - a organ software

  main.C  -  Main file of the organ
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


#include <getopt.h>
#include <sched.h>
#include <sys/mman.h>
#include <pthread.h>
#include <math.h>
#include "Holrgan.h"
#include "HORGAN.h"
#include "jackoutput.h"

HOR hor;
pthread_t thr1, thr2;


// Put Kernel RT priority to a thread

void pon_realtime()

{
    sched_param scprior;
    scprior.sched_priority=50;
    int prior=sched_setscheduler(0,SCHED_FIFO,&scprior);
    if (prior==0) printf("SCHED_FIFO\n");
};


// MIDI Input thread ( Read MIDI incoming messages)

void *
thread1 (void *arg)
{

while (Pexitprogram ==0)
   {
   pthread_mutex_lock(&m_mutex);
   hor.midievents(1);
   pthread_mutex_unlock(&m_mutex);
   }
  return (0);
};


// Audio thread

void *
thread2 (void *arg)
{
    pon_realtime();
    while (Pexitprogram ==0)  hor.Alg1s(hor.PERIOD,0);
    return(0);

};



int main(int argc, char *argv[])

{

// Read command Line

  fprintf (stderr,
	   "\n%s %s - Copyright (c) 2003-2009 Josep Andreu (Holborn)\n",PACKAGE,VERSION);
  if (argc == 1)
    fprintf (stderr, "Try 'horgand --help' for command-line options.\n");


  struct option opts[] = {
    {"load", 1, NULL, 'l'},
    {"bank", 1, NULL, 'b'},
    {"rhyt", 1, NULL, 'r'},
    {"no-gui", 0, NULL, 'n'},
    {"help", 0, NULL, 'h'},
    {0, 0, 0, 0}
  };

  
  Pexitprogram = 0;
  commandline = 0;
  gui=1;
  opterr = 0;
  int option_index = 0, opt;
  exitwithhelp = 0;
  
  
  while (1)
    {
      opt = getopt_long (argc, argv, "l:b:r:nh", opts, &option_index);
      char *optarguments = optarg;
      
      if (opt == -1)
	break;


      switch (opt)
	{
	case 'h':
	  exitwithhelp = 1;
	  break;
        case 'n':
          gui=0;
          break;                                  
	case 'l':
	  if (optarguments != NULL)
	    {
              commandline = 1;
	      hor.loadfile (optarguments);
	      break;
	    }
        case 'r' :
            if (optarguments != NULL)
            {
              hor.loadrhyt (optarguments);
              break;
            }

	case 'b':
	  if (optarguments != NULL)
	    {
              commandline = 2;
	      hor.loadbank (optarguments);

	      break;
	    }


	};
    };

  if (exitwithhelp != 0)
    {
      fprintf (stderr, "Usage: horgand [OPTION]\n\n");
      fprintf (stderr, "  -h ,     --help \t\t\t display command-line help and exit\n");
      fprintf (stderr, "  -n ,     --no-gui \t\t\t disable GUI\n");
      fprintf (stderr, "  -l File, --load=File \t\t\t loads sound\n");
      fprintf (stderr, "  -b File, --bank=File \t\t\t loads bank\n");
      fprintf (stderr, "  -r File, --rhyt=File \t\t\t loads rhythm\n\n");
      fprintf (stderr, "FLTK options are:\n\n");
      fprintf (stderr, "  -bg2 color\n");
      fprintf (stderr, "  -bg color\n");
      fprintf (stderr, "  -di[splay] host:n.n\n");
      fprintf (stderr, "  -dn[d]\n");
      fprintf (stderr, "  -fg color\n");
      fprintf (stderr, "  -g[eometry] WxH+X+Y\n");
      fprintf (stderr, "  -i[conic]\n");
      fprintf (stderr, "  -k[bd]\n");
      fprintf (stderr, "  -na[me] classname\n");
      fprintf (stderr, "  -nod[nd]\n");
      fprintf (stderr, "  -nok[bd]\n");
      fprintf (stderr, "  -not[ooltips]\n");
      fprintf (stderr, "  -s[cheme] scheme (plastic,none,gtk+)\n");
      fprintf (stderr, "  -ti[tle] windowtitle\n");
      fprintf (stderr, "  -to[oltips]\n");
      fprintf (stderr, "\n");
      return (0);
    };
    
    
  //Locks memory


   mlockall(MCL_CURRENT | MCL_FUTURE);
  

  hor.init_hor();
  
  if (hor.Salida < 3) hor.Adjust_Audio();

  // Launch GUI


  if(gui) new HORGAN(argc,argv,&hor);
  
  
  // Launch MIDI thread

  pthread_mutex_init (&mutex, NULL);
  pthread_mutex_init (&m_mutex, NULL);

  pthread_create (&thr1, NULL, thread1, NULL);

  // Launch AUDIO thread for ALSA and OSS, not for JACK
  

  if (hor.Salida < 3)  pthread_create (&thr2, NULL, thread2, NULL); 

  if (hor.Salida==3)
  {
     hor.Salida=JACKstart(&hor);
     if (hor.Salida==2)
        {
          hor.alsaaudioprepare();
          pthread_create (&thr2, NULL, thread2, NULL);
          hor.cambiaDriver=1;
        }       

     hor.Adjust_Audio();
  }   
  
  
  // Main Loop 

  while (Pexitprogram == 0)
 { 

      // Refresh GUI
  if (gui) Fl::wait();
  else
  { 
   usleep(1500);
   if (preset !=0)
    {
      hor.Put_Combi_t(preset);
      preset=0;
    } 
   }

}


    // Exit  Close Audio devices

if (hor.Salida < 3 )  hor.CloseAudio(hor.Salida);
else JACKfinish();
    // free memory etc.

   
  free(hor.lsin);    
  free(hor.msin);
  free(hor.nsin);
  free(hor.psin);
  free(hor.qsin);
  free(hor.rsin);
  free(hor.tsin);
  free(hor.ssin);
  free(hor.usin);
  free(hor.history);
  free(hor.buf);
  free(hor.wbuf);
  pthread_mutex_destroy (&mutex);
  exit(0);
};












