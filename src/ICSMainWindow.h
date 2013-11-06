/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef ICSMAINWINDOW_H
#define ICSMAINWINDOW_H

#include <vector>

#include <InterfaceKit.h>
#include <CardLayout.h>
#include <GroupLayout.h>
#include <SplitView.h>

#include "ICS.h"
#include "ICSChallengesView.h"
#include "ICSChannelsView.h"
#include "ICSChatView.h"
#include "ICSEventCalendarView.h"
#include "ICSGamesView.h"
#include "ICSInboxView.h"
#include "ICSInfoView.h"
#include "ICSOutboxView.h"
#include "ICSPlayersView.h"
#include "ICSRoomsView.h"
#include "ICSTabsToolBar.h"
#include "ICSTXTControl.h"
#include "ImageButton.h"

#include "Debug.h"

class ICSMainWindow : public BWindow
{
public:
                    ICSMainWindow(ICSInfo info);
    virtual void    MessageReceived(BMessage* message);
    virtual bool    QuitRequested(void);

private:
            void    _PStr(BString str, int code, BHandler* target);
            void    _PStr(BString str, int code, BLooper*  target);
            bool    _ParseBlock(const char*& p);
            bool    _ParseAngleBracket(const char*& p);
            bool    _ParseSquareBracket(const char*& p);
            bool    _ParseCurlyBracket(const char*& p);
            void    _ParseICSOutput(BString & str);
            void    _SetIcsVariables(void);
            void    _LoadSettings(void);
            void    _SaveSettings(void);
    inline  void    _Send(BString const& str)
    {
        fICS->Send(str);
    }

    BCardLayout*            fCardLayout;
    BGroupLayout*           fMainLayout;

    BTabView*               fTabView1;
    BTabView*               fTabView2;
    BTabView*               fTabView3;

    std::vector<BTab*>      fTabsVec1;
    std::vector<BTab*>      fTabsVec2;
    std::vector<BTab*>      fTabsVec3;

    BWindow*                 fTarget;

    BSplitView*             fSplitView1;
    BSplitView*             fSplitView2;
    BSplitView*             fSplitView3;
    BSplitView*             fSplitView4;

    ICS*                    fICS;
    ICSInfo                 fICSInfo;

    ICSTabsToolBar*         fToolBar;

    ICSInfoView*            fICSInfoView;
    ICSPlayersView*         fICSPlayersView;
    ICSGamesView*           fICSGamesView;
    ICSTXTControl*          fICSTXTControl;

    ICSChatView*            fICSChatView;
    ICSChannelsView*        fICSChannelsView;
    ICSInboxView*           fICSInboxView;
    ICSOutboxView*          fICSOutboxView;
    ICSChatView*            fICSPrvChatView;

    ICSRoomsView*           fICSRoomsView;

    ICSChallengesView*      fICSChallengesView;
    ICSEventCalendarView*   fICSEventCalendarView;

    bool                    fIsConnected;

    BString                 fParseRest;

    Debug                   out;
};


#endif
