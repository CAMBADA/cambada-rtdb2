/*
 * This file is part of the CAMBADA project.
 *
 * Copyright (C) 2007 Ricardo Marau <marau@det.ua.pt>
 * Copyright (C) 2007 CAMBADA - Universidade de Aveiro, Portugal
 * (Written by Ricardo Marau <marau@det.ua.pt> for CAMBADA - UA)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


/* A brief introduction over signals...
 *
 * The Linux Signals are used for interprocess comunication. This asynchronous
 * mechanism is used to send events to the running processes. For each process, there is a
 * limited range of signals that can be used. Most of them have a standard meaning, e.g. 
 * Crtr-C (INT signal) stands for a user will to shutdown the process. Most of those signals 
 * can be trapped by the application to perform a set of actions, instead of the standard ones.
 * 
 * To trap the signal the user can either register an handler or wait (sigwait) for the signal.
 * Some issues about Signals:
 * 	- A Signal is held in a single event (like a unique interruption) that must then be polled 
 * 	  in order to determine its type. When simultaneous signals occur, only one of them is handled,
 * 	  even for with different signals.
 * 	  Moreover, special cautions must be followed if the a signal handler is assigned, to 
 * 	  avoid signals losses (the signals reception is some how held until the end of the handler).
 *
 * 	- About the sigwait, this is done by blocking the execution while waiting with a specific 
 * 	  signal mask. In a multi-threaded system, one might think that different sigwait calls waiting 
 * 	  with diferent masks in different threads, would solve the issue of catching diferent signals.
 * 	  This is true, but when more than one sigwait is waiting for a signal, even with non-overlaping
 * 	  masks, only one of the sigwait will catch the signal and then unblock if the signal matches 
 * 	  the mask. The sigwait that gets the signal is unspecified.
 *
 * So, in multi-threading systems, if we want a thread to be held in a signal the only way is
 * to register a handler for that signal and then wake some other synchronization mechanism, like
 * mutexes.
 *
 * BTW, there are also the "pthread_" oriented signals. These signals use a signalization API that has 
 * nothing to do with the process signals, except the namespace analogy. It's a similar implementation
 * of signals for intra process  signalization (inter-threads).
 *
 * */

#include <stdio.h>
#include "SignalsHandler.h"

#include <pthread.h>

#include <signal.h>
#include <iostream>
using namespace std;

#include <semaphore.h>

static sem_t sem_pman;

static void pmansig_handler(int dummy)
{
	(void)dummy;
	sem_post(&sem_pman);
}

SignalsHandler::SignalsHandler()
{
	(void) signal(SIGCONT, pmansig_handler); //< \todo Não deveria ser PMAN_ACTIVATE_SIG ?
}

void SignalsHandler::startup()
{
	sem_init(&sem_pman,0,0);
}

SignalsHandler::~SignalsHandler()
{
}


void SignalsHandler::shutdown()
{
	sem_destroy(&sem_pman);
}

void SignalsHandler::wait_PMAN(void)
{
/*
	pthread_mutex_lock( &pman_c_mutex );
    //fprintf(stderr, "HWcomm: waiting for signal tick\n");
	pthread_cond_wait( &pman_c_cond, &pman_c_mutex );
	pthread_mutex_unlock( &pman_c_mutex );
*/
	//cerr << "wait_PMAN 1"<<endl;	
	sem_wait(&sem_pman);
	//cerr << "wait_PMAN 2"<<endl;	
}

//SignalsHandler signalsHandler; //This is a global var!

