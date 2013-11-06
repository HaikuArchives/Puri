/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef TIMEVIEW_H
#define TIMEVIEW_H

#include <Box.h>
#include <GroupView.h>
#include <StringView.h>

#include "Debug.h"

class TimeView : public BGroupView
{
public:
                    TimeView(int32 const& replyCode = 0);
    virtual void    Pulse(void);
            void    AllAttached(void);

    void            Init(int time);
    void            Start(void);
    void            Stop(void);
    void            Switch(void);
    void            SetTimeMS(int time, bool white);
    void            SetTimeWhite(int const& time);
    void            SetTimeWhite(BString time);
    void            SetTimeBlack(int const& time);
    void            SetTimeBlack(BString time);
    void            SetTime(int const& time);

    bool            IsStarted(void)         {   return(fIsRunning);         }
    int             WTime(void)             {   return(fTimeWhite*1000);    }
    int             BTime(void)             {   return(fTimeBlack*1000);    }
    int const&      GetTimeWhite( void )    {   return( fTimeWhite );       }
    int const&      GetTimeBlack( void )    {   return( fTimeBlack );       }

    virtual	void SetViewColor(rgb_color c)
    {
        BView::SetViewColor(0, 0, 0);
    }

    void SetViewColor(uchar r, uchar g, uchar b, uchar a = 255)
    {
            BView::SetViewColor(0, 0, 0);
    }

private:
	void            _Init(void);
    void            _DisplayTimeWhite(void);
    void            _DisplayTimeBlack(void);
    void            _NotifyTime(void);
    BString         _TimeToString(int time);
    int             _StringToTime(BString time);

    BBox*           fBox;
    BStringView*	fWhiteClock;
    BStringView*	fBlackClock;

    bool            fIsRunning;
    bool            fIsTurnWhite;
    int             fTimeWhite;
    int             fTimeBlack;
	int             fPulseRate;
    int             fPulses;
    int32           fReplyCode;

    Debug           out;
};

#endif
