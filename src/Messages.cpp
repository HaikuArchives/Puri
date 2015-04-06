/*
 * Copyright 2011-2012 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "Messages.h"

void
SendICS(BString str, BLooper* looper)
{
    BMessage msg(MSG_SEND_ICS_CMD);
    msg.AddString("info", str);
    looper->PostMessage(&msg);
}

