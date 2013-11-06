/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "Engine.h"

#include <iostream>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include <pthread.h>

#include "Tools.h"

// TODO: Make more use of the UCI protocol,
// http://wbec-ridderkerk.nl/html/UCIProtocol.html

Engine::Engine(BString name, uint32 replyCode, BLooper* targetLooper,
                                        BHandler* targetHandler, int32 priority)
    :
    BLooper(name, priority),
    fIsRunning(false),
    fIsStarted(false),
    fIsLocked(false),
    fGoDepth(4),
    fGoTime( 10 )
{
	fReplyCode     = replyCode;
	fTargetLooper  = targetLooper;
	fTargetHandler = targetHandler;

	fEngineName = name;
	fEnginePath = GetAppPath().Append("/Engines/").Append( name );

    !out << "ENGINE-PATH:   " << fEnginePath << std::endl;

	Run();
}


bool
Engine::QuitRequested(void)
{
    Send("quit");

	close(fOutPipe[0]);
	close(fOutPipe[1]);
	close(fInPipe[1]);
	close(fInPipe[0]);

	kill(fPidEngine, SIGTERM);
	int status;
	waitpid(fPidEngine, &status, 0);

    return true;
}


void*
Engine::_ReadThread(void* arg)
{
	ThreadData	data(*((ThreadData*)arg));

	char buf[BUF_SIZE];

	int n;

	for ( ; ; ) {
		// read Engine Output, Get Pipes over  arg
		n = read(data.outpipe, buf, BUF_SIZE);
  		buf[n] = 0;

        if (n > 0) {
            BMessage msg(data.replyCode);
            msg.AddString("info", buf);
            data.target_looper->PostMessage(&msg, data.target_handler);
        }
    }

	return NULL;
}


void
Engine::_InitEngineCommunication(void)
{
  /* In a pipe, xx[0] is for reading, xx[1] is for writing */
	if (pipe(fInPipe) < 0) {
		!out << "Engine::pipe in" << std::endl;
	}

	if (pipe(fOutPipe) < 0) {
		!out << "Engine::pipe out" << std::endl;
	}

	if ((fPidEngine = fork()) == 0) {
	  	/* This is the child process */

		/* Close stdin, stdout, stderr */
		close(0);
		close(1);
		close(2);
		/* make our pipes, our new stdin,stdout and stderr */
		dup2(fInPipe[0],  0);
		dup2(fOutPipe[1], 1);
		dup2(fOutPipe[1], 2);

		/* Close the other ends of the pipes that the parent will use, because if
		* we leave these open in the child, the child/parent will not get an EOF
		* when the parent/child closes their end of the pipe.
 		*/
		close(fInPipe[1]);
		close(fOutPipe[0]);

        execl(fEnginePath.String(),fEngineName.String(), (char *)NULL);
    }

	/* This is the parent process */
	/* Close the pipe ends that the child uses to read from / write to so
	* the when we close the others, an EOF will be transmitted properly.
	*/
	close(fInPipe[0]);
	close(fOutPipe[1]);

	ThreadData*	data= new ThreadData;
	data->outpipe = fOutPipe[0];
	data->replyCode = fReplyCode;
	data->target_looper = fTargetLooper;
	data->target_handler = fTargetHandler;

	// thread, for reading.the engineOutput
	pthread_create(&fReadThread, NULL, _ReadThread,(void*)(data));
    delete data;
}


void
Engine::Send(BString str)
{
    str << "\n";
	write(fInPipe[1], str.String(), str.Length());
        // Write some data to the childs input
    !out << "\n***Send-Engine\n" << str << "####" << std::endl;
}


void
Engine::MessageReceived(BMessage* message)
{
    switch (message->what) {
        case MSG_LOCK  :    fIsLocked = true;   return;
        case MSG_UNLOCK:    fIsLocked = false;  return;

        case MSG_SKILLCHANGED:
        {
            int32 value = 0;
            message->FindInt32("val", &value);

            BString str = "";
            str << "setoption name Skill Level value " << value << "\n";
            Send(str);
            return;
        }

        case MSG_QUIT:
            QuitRequested();
            return;

        default:
            break;
    }

    if (fIsLocked == false) {
        switch(message->what) {
        case MSG_ENG_START:
        {
            if (fIsStarted)
                break;
            else
                fIsStarted= true;

            _InitEngineCommunication();
            Send("uci");
            Send("setoption name Book File value book.bin");
            Send("ucinewgame");
            break;
        }

        case MSG_ENG_STOP:
            Send("stop");
            break;

        case MSG_ENG_CMD:
        {
            !out << "Engine-Looper-msg: CMD  " << std::endl;

            BString str;
            message->FindString("cmd", &str);
            Send(str);
            break;
        }

        case MSG_ENG_NEWGAME:
            //Send("stop");
            Send("ucinewgame");
            break;

        case MSG_ENG_MOVENOW:
        {
            BString moves;
            message->FindString("moves", &moves);
            Send(moves);
            break;
        }

        default:
            break;
        }
    }

    BLooper::MessageReceived(message);
}
