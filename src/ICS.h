/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICS_H
#define ICS_H

#include <Looper.h>
#include <Message.h>
#include <String.h>
#include <OS.h>

#include "Debug.h"
#include "Messages.h"


class ICSInfo
{
public:
    BString     server;
    int32       port;
    BString     user;
    BString     pass;
};


class ICS : public BLooper
{
public:
                    ICS(ICSInfo const& info, uint32 replyCode = 0,
                            BLooper* targetLooper = NULL,
                            BHandler* targetHandler = NULL,
                            int32 priority = B_NORMAL_PRIORITY);

	virtual void    MessageReceived(BMessage* message);
	virtual void    Quit(void);
    void 	        Send(BString str);
	bool        	IsRunning(void) {	return fIsRunning; }

	enum {BUF_SIZE = 65356};
    enum {M_INIT = 'ii01' };

private:

	static void*    _ReadThread(void* arg);

	void	        _InitCommunication(void);
    void            _ParseICSOutput(BString str);

	//status_t	_ReadThread( void* arg );

	class ThreadData{
    public:
		int         socket_descriptor;
		BLooper*	target_looper;
		BHandler*	target_handler;
	};

    int         fSocketDescriptor;

	pthread_t	fReadThread;

	BLooper*	fTargetLooper;
	BHandler*	fTargetHandler;

	bool		fIsRunning;
    bool        fIsStarted;
    bool        fIsLocked;
    bool        fUserIsWhite;

    ICSInfo     fICSInfo;

    Debug       out;
};

#endif
