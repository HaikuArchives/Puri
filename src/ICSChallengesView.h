/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSCHALLENGESVIEW_H
#define ICSCHALLENGESVIEW_H

#include <Button.h>
#include <GroupView.h>
#include <Locker.h>
#include <Message.h>
#include <ScrollBar.h>
#include <String.h>
#include <TextView.h>
#include <TextControl.h>

#include <ColumnListView.h>

#include "Debug.h"
#include "ToolBar.h"

class ChallengesCLV : public BColumnListView
{
public:
                    ChallengesCLV(void);

    virtual	void    ItemInvoked();

private:

    Debug           out;
};


class ICSChallengesView : public BGroupView
{
public:
                    ICSChallengesView(void);
    virtual void    AttachedToWindow(void);
    virtual void    MessageReceived(BMessage* message);
    virtual void    Pulse(void);
            void    AddSeek(BString const& str);
            void    RemoveSeek(int index);

            void    SetSeeking(bool seeking = true)
            {
                fIsSeeking = seeking;
            }

            bool    IsSeeking(void)
            {
                return fIsSeeking;
            }

    enum{ M_ACCEPT = 'iab1', M_DELETE = 'idb1', M_MODIFY = 'imb1',
          M_SEEK = 'isb1', M_FORMULA = 'ifb1'};

private:
    ChallengesCLV*      fColumnList;
    ToolBar*            fToolBar;

    int                 fPulseRate;
    int                 fPulses;

    bool                fIsSeeking;

    BLocker             fLocker;

    Debug               out;
};


#endif // ICSCHALLENGESVIEW_H
