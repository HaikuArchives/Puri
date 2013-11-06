/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICS.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>

#include "FicsCodes.h"
#include "Tools.h"

using namespace std;


ICS::ICS(ICSInfo const& info, uint32 replyCode, BLooper* targetLooper,
    BHandler* targetHandler, int32 priority)
    :
    BLooper(info.server, priority),
    fSocketDescriptor(0),
    fTargetLooper(targetLooper),
    fTargetHandler(targetHandler),
    fIsRunning(false),
    fIsStarted(false),
    fIsLocked(false),
    fUserIsWhite(false),
    fICSInfo(info)
{
	Run();
    PostMessage(M_INIT);
}


void
ICS::Quit(void)
{
    BString str;
    str << BLK_QUIT << " quit";
    !out << "SEND QUIT" << std::endl;
    Send(str);
    BLooper::Quit();
}


void*
ICS::_ReadThread(void* arg)
{
	ThreadData	data(*((ThreadData*)arg));

	int     n = 0;
    bool    isLoggedIn = false;
	char    buf[BUF_SIZE / 2];
    char    buffer[BUF_SIZE];
    char    temp[BUF_SIZE];

    const char*   p1;
    const char*   p2;

    Debug   debug;

	for ( ; ; ) {
		// read ICS Output, Get Pipes over  arg
		n = read(data.socket_descriptor, buf, BUF_SIZE);
  		buf[n] = 0;

     //   !debug << "DEBUG" << std::endl << buf << ":\n---End" <<std::endl;
        if (n > 0) {
            if (isLoggedIn == false) {
                if (strstr(buf, "login:") != NULL)
                    data.target_looper->
                        PostMessage(ICS_ASKS_USERNAME, data.target_handler);
                else if (strstr(buf, "password:") != NULL)
                    data.target_looper->
                        PostMessage(ICS_ASKS_PASSWORD, data.target_handler);
                else if (strstr(buf, "**** Starting") != NULL) {
                    isLoggedIn = true;
                    data.target_looper->
                        PostMessage(ICS_LOGGED_IN, data.target_handler);
                }
                else if (strstr(buf, "**** Invalid password") != NULL)
                    data.target_looper->
                        PostMessage(ICS_DISCONNECTED, data.target_handler);
                else if (strstr(buf, "not a registered name") != NULL)
                    data.target_looper->
                        PostMessage(ICS_DISCONNECTED, data.target_handler);

                continue;
            }

            strncat(buffer, buf, n);
            p1 = buffer;
            for( ;; ) {
                if ((p2 = strstr(p1, "fics% ")) != NULL) {
                    temp[0] = '\0';
                    strncpy(temp, p1, p2 - p1);
                    temp[p2 - p1] = '\0';
                    BMessage msg(ICS_OUTPUT);
                    msg.AddString("info", temp);
                    data.target_looper->PostMessage(&msg, data.target_handler);
                    p2 += 6;
                    p1 = p2;
                    buffer[0] = '\0';
                } else {
                    strcpy(buffer, p1);
                    buffer[n - (p1 - buf)] = '\0';
                    break;
                }
            }
        } else if (n == 0) {
            BMessage msg(ICS_DISCONNECTED);
            data.target_looper->PostMessage(&msg, data.target_handler);
            break;
        }
    }

    pthread_exit(NULL);
	return NULL;
}


void
ICS::_InitCommunication(void)
{
    // Force the socket to be greater than 2, because in normal case
    // 0,1,2 are the standard in,out, error.
    // socket() can return 0, if _InitCommunication is called before
    // the standardsockets are set. For example if _InitCommunication
    // is called directly in the constructor. In the present case, the
    // while loop is not needed, but it doesnt harm anything, so we let it.

    while (fSocketDescriptor < 3) {
        fSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);

        if (fSocketDescriptor == -1) {
            !out << "Socket could not be created" << std::endl;
            return;
        }
    }

    struct sockaddr_in sin;
    struct hostent *host = gethostbyname(fICSInfo.server.String());

    if (host == NULL) {
        !out << "gethostbyname failed" << std::endl;
        BMessage msg(ICS_DISCONNECTED);
        fTargetLooper->PostMessage(&msg, fTargetHandler);
        return;
    }

    memcpy(&sin.sin_addr.s_addr, host->h_addr, host->h_length);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(fICSInfo.port);

    if (connect(fSocketDescriptor, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        !out << "Error connecting to ICS" << std::endl;

        BMessage msg(ICS_DISCONNECTED);
        fTargetLooper->PostMessage(&msg, fTargetHandler);
        return;
    }

	ThreadData*	data= new ThreadData;
	data->socket_descriptor = fSocketDescriptor;
	data->target_looper = fTargetLooper;
	data->target_handler = fTargetHandler;

	pthread_create(&fReadThread, NULL, _ReadThread,(void*)(data));
    delete data;
}


void
ICS::Send(BString str)
{
    if (str.Length() < 1)
        return;

    str << "\n";
	write(fSocketDescriptor, str.String(), str.Length());
    !out << "ICS::Send :" << str << std::endl;
}


void
ICS::MessageReceived(BMessage* message)
{
    switch (message->what) {
        case M_INIT:
            _InitCommunication();
            break;

        case MSG_QUIT:
        {
            Quit();
            break;
        }

        default:
           BLooper::MessageReceived(message);
            break;
    }
}
