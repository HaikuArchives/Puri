/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ENGINE_H
#define ENGINE_H

#include <Looper.h>
#include <Message.h>
#include <String.h>
#include <OS.h>

#include "Debug.h"
#include "Messages.h"

class Engine : public BLooper
{
public:
                    Engine(BString name, uint32 replyCode = 0,
                            BLooper* targetLooper = NULL,
                            BHandler* targetHandler = NULL,
                            int32 priority = B_NORMAL_PRIORITY);

	virtual void    MessageReceived(BMessage* message);
    virtual bool    QuitRequested(void);
	void 	        Send(BString str);
	bool        	IsRunning(void) {	return fIsRunning; }

	enum {BUF_SIZE = 65356};

    enum {FIXEDDEPTH = 0, FIXEDTIME};

private:
	static void*    _ReadThread(void* arg);

	void	        _InitEngineCommunication(void);

	//status_t	_ReadThread( void* arg );

	class ThreadData{
		public:
		int outpipe;
		uint32		replyCode;
		BLooper*	target_looper;
		BHandler*	target_handler;
	};

	int		    fInPipe[2];
	int	    	fOutPipe[2];
	int		    fPidEngine;

	pthread_t	fReadThread;

	BString		fEnginePath;
	BString		fEngineName;

	BLooper*	fTargetLooper;
	BHandler*	fTargetHandler;

	bool		fIsRunning;
    bool        fIsStarted;
    bool        fIsLocked;

	uint32		fReplyCode;


    BString     fGoMsg;

    uint        fGoDepth;
    uint        fGoTime;


    Debug       out;
};

#endif
