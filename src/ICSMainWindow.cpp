/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ICSMainWindow.h"

#include <stdlib.h>

#include <Application.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>

#include "BoardWindow.h"
#include "FicsCodes.h"
#include "ImageButton.h"
#include "IntroWindow.h"
#include "Messages.h"
#include "Settings.h"

ICSMainWindow::ICSMainWindow(ICSInfo info)
	:
    BWindow(BRect(), "Chess Server", B_TITLED_WINDOW_LOOK,
        B_NORMAL_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS
        /*    | B_QUIT_ON_WINDOW_CLOSE*/),
    fICSInfo(info),
    fIsConnected(false)
{
    fTarget = NULL;

    fICSGamesView   = new ICSGamesView;
    fICSInfoView    = new ICSInfoView;
    fICSPlayersView = new ICSPlayersView;
    fICSTXTControl  = new ICSTXTControl;

    fTabView1 = new BTabView("tab_view1");
    fTabView1->SetTabWidth(B_WIDTH_FROM_LABEL);
    fTabsVec1.push_back(new BTab());
	fTabView1->AddTab(fICSInfoView, fTabsVec1.back());
	fTabsVec1.back()->SetLabel("Info");
    fTabsVec1.push_back(new BTab());
	fTabView1->AddTab(fICSPlayersView, fTabsVec1.back());
	fTabsVec1.back()->SetLabel("Players");
    fTabsVec1.push_back(new BTab());
	fTabView1->AddTab(fICSGamesView, fTabsVec1.back());
	fTabsVec1.back()->SetLabel("Games");
    fTabsVec1.push_back(new BTab());
	fTabView1->AddTab(fICSTXTControl, fTabsVec1.back());
	fTabsVec1.back()->SetLabel("Txt Cntrl");
    fTabView1->Select(3);

    fICSChatView        = new ICSChatView();
    fICSChannelsView    = new ICSChannelsView();
    fICSInboxView       = new ICSInboxView();
    fICSOutboxView      = new ICSOutboxView();
    fICSPrvChatView     = new ICSChatView();

    fTabView2 = new BTabView("tab_view2");
    fTabView2->SetTabWidth(B_WIDTH_FROM_LABEL);
    fTabsVec2.push_back(new BTab());
	fTabView2->AddTab(fICSChatView, fTabsVec2.back());
	fTabsVec2.back()->SetLabel("Chat");
    fTabsVec2.push_back(new BTab());
	fTabView2->AddTab(fICSChannelsView, fTabsVec2.back());
	fTabsVec2.back()->SetLabel("Channels");
    fTabsVec2.push_back(new BTab());
	fTabView2->AddTab(fICSInboxView, fTabsVec2.back());
	fTabsVec2.back()->SetLabel("Inbox");
    fTabsVec2.push_back(new BTab());
	fTabView2->AddTab(fICSOutboxView, fTabsVec2.back());
	fTabsVec2.back()->SetLabel("Sent");
    //fTabsVec2.push_back(new BTab());
	//fTabView2->AddTab(fICSPrvChatView, fTabsVec2.back());
	//fTabsVec2.back()->SetLabel("Prv Chat");
    fTabView2->Select(0);

    fICSChallengesView      = new ICSChallengesView;
    fICSEventCalendarView   = new ICSEventCalendarView;

    fTabView3 = new BTabView("tab_view3");
    fTabView3->SetTabWidth(B_WIDTH_FROM_LABEL);
    fTabsVec3.push_back(new BTab());
	fTabView3->AddTab(fICSChallengesView, fTabsVec3.back());
	fTabsVec3.back()->SetLabel("Challenges");
    fTabsVec3.push_back(new BTab());
	fTabView3->AddTab(fICSEventCalendarView, fTabsVec3.back());
	fTabsVec3.back()->SetLabel("Event Calendar");
    fTabView3->Select(0);

    fToolBar        = new ICSTabsToolBar();
    fICSRoomsView   = new ICSRoomsView();

    fMainLayout = BLayoutBuilder::Group<>(B_VERTICAL, 0)
        .AddSplit(B_VERTICAL, 0, 1).GetSplitView(&fSplitView1)
            .Add(fToolBar, 1)
            .AddSplit(B_HORIZONTAL, 0, 5).GetSplitView(&fSplitView2)
                .AddSplit(B_VERTICAL, 0, 3).GetSplitView(&fSplitView3)
                    .AddGroup(B_VERTICAL, 0, 3)
                        .Add(fTabView1)
                    .End()
                    .AddGroup(B_VERTICAL, 0, 1)
                        .Add(fTabView3)
                    .End()
                .End()
                .AddSplit(B_VERTICAL, 0, 1).GetSplitView(&fSplitView4)
                    .AddGroup(B_VERTICAL, 0, 2)
                        .Add(fTabView2)
                    .End()
                    .AddGroup(B_VERTICAL, 0, 1)
                        .Add(fICSRoomsView)
                    .End()
                .End()
            .End()
        .End()
    ;

    fCardLayout = new BCardLayout();

    BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
        .Add(fCardLayout)
    ;

    fCardLayout->AddItem(fMainLayout);
    fCardLayout->SetVisibleItem(fMainLayout);
    fICS = new ICS(fICSInfo, REPLYCODE_ICS, this, this);
    _LoadSettings();
    Zoom();
    Show();
  // TEST:  fTarget = new BoardWindow(10, true, "test", "test", this, fICS);
}


bool
ICSMainWindow::QuitRequested(void)
{
	_SaveSettings();
    if (fICS != NULL)
        fICS->PostMessage(MSG_QUIT);

    if (fTarget != NULL)
        fTarget->PostMessage(MSG_QUIT);

    be_app->PostMessage(B_QUIT_REQUESTED);
    return true;
}


void
ICSMainWindow::MessageReceived(BMessage* message)
{
    if (fIsConnected == false) {
        switch (message->what) {
            case ICS_ASKS_USERNAME:
                _Send(fICSInfo.user);
                return;

            case ICS_ASKS_PASSWORD:
                _Send(fICSInfo.pass);
                return;

            case ICS_LOGGED_IN:
                fIsConnected = true;
                _SetIcsVariables();
                _PStr("*** Connected ***", ICS_SHOUT, fICSChatView);
                return;

            case ICS_DISCONNECTED:
                !out << "***Disconnected" << std::endl;
                break;

            default:
                break;
        }
    }

	switch (message->what) {
        case ICS_DISCONNECTED:
            fIsConnected = false;
            if (fTarget != NULL)
                fTarget->PostMessage(message);
            Hide();
            new IntroWindow();
            Close();
            break;

        case MSG_WINDOW_CLOSED:
            fTarget->PostMessage(MSG_QUIT);
            fTarget = NULL;
            break;

        case MSG_SEND_ICS_CMD:
        {
            BString str = "";
            message->FindString("info", &str);
            _Send(str);
            break;
        }

        case MSG_QUIT:
            QuitRequested();
            break;

        case ICS_GAME_INFO:
        case ICS_MADE_MOVE:
            if (fTarget != NULL)
                fTarget->PostMessage(message);
            break;

        case ICS_OUTPUT:
        {
            BString str;
            message->FindString("info", &str);
            _ParseICSOutput(str);
            break;
        }

		default:
            BWindow::MessageReceived(message);
            break;
	}
}


bool
ICSMainWindow::_ParseBlock(const char*& p1)
{
    if (p1[0] != BLOCK_START)
        p1 = strchr(p1, BLOCK_START);

    if (p1 == NULL)
        return true;

    ++p1;
    int ident = -1;
    int code  = -1;
    BString str;
    char    temp[65356];
    const char* p2;
    !out << p1 << std::endl;
    for (int i = 0; (p2 = strchr(p1, BLOCK_SEPARATOR)) != NULL; ++i) {
        switch (i) {
            case 0:
                strncpy(temp, p1, p2 - p1);
                temp[p2 - p1] = '\0';
                ident = atoi(temp);
                break;

            case 1:
                strncpy(temp, p1, p2 - p1);
                temp[p2 - p1] = '\0';
                code = atoi(temp);
                break;

            default:
                !out << "\n\n\n***!!! MORE than case 1:\n" << std::endl;
                break;
        }

        p1 = p2 + 1;
    }

    if ((p2 = strchr(p1, BLOCK_END)) != NULL) {
          strncpy(str.LockBuffer(65356), p1, p2 - p1);
          str[p2 - p1] = '\0';
          str.UnlockBuffer();
          p1 = p2;
    } else {
        !out << "Error: No Block_End" << std::endl;
        return false;
    }

    // if (code != ident && ident != 1000 && ident != 2000)
    //  return false;

    if (ident == 2000) {
        // to be empty. The response of setting variables
        // is not important to use.
        return true;
    } else if (ident == 1000) {
        BMessage msg(ICS_USER_CMD_RESPONSE);
        msg.AddString("info", str);
        PostMessage(&msg, fICSTXTControl);
        return true;
    } else {
        switch (code) {
            case BLK_SET:
            {
                std::vector<BString> vec;
                vec = Tools::Split(str, '\t', "<s>", "");
                if (vec.empty())
                    return true;

                PostMessage(ICS_SEEK_CLEAR, fICSChallengesView);

                str = vec[0];
                _PStr(vec[0], ICS_SEEK_ADD, fICSChallengesView);
                return true;
            }

            case BLK_UNSEEK:
            {
                std::vector<BString> vec;
                vec = Tools::Split(str, '\r', "<sr>", "\n");
                if (vec.empty())
                    return true;

                BMessage msg(ICS_SEEK_REMOVE);
                msg.AddString("info", vec[0]);
                PostMessage(&msg, fICSChallengesView);
                return true;
            }

            case BLK_GAME_MOVE:
            {
                !out << "BLK_GAME_MOVE :" << str << std::endl;
                const char* p = str.LockBuffer(65356);
                _ParseAngleBracket(p);
                str.UnlockBuffer();
                return true;
            }

            case BLK_RESIGN:
                _PStr("", ICS_RESIGN, fTarget);
                !out << "BLK_RESIGN :" << str << std::endl;
                return true;

            case BLK_ABORT:
                _PStr("", ICS_GAME_ABORTED, fTarget);
                !out << "BLK_ABORT :" << str << std::endl;
                return true;

            case BLK_DRAW:
                _PStr("", ICS_GAME_DRAWN, fTarget);
                !out << "BLK_DRAW :" << str << std::endl;
                return true;

            case BLK_PLAY:
                !out << "BLK_PLAY :" << str << std::endl;
                return true;

            case BLK_ERROR_NOTPLAYING:
                _PStr("", ICS_NOTPLAYING, fTarget);
                return true;

            case BLK_SEEK:
            {
                std::vector<BString> vec;
                vec = Tools::Split(str, '\r', "<sn>", "\n");
                if (vec.empty())
                    return true;

                _PStr(vec[0], ICS_SEEK_ADD, fICSChallengesView);
                return true;
            }

            case BLK_SOUGHT:
                _PStr(str, ICS_SEEK_ADD, fICSChallengesView);
                return true;

            case BLK_SHOUT: case BLK_XTELL: case BLK_TELL:
            {
                _PStr(str, ICS_OFFERED_INFO, fICSChatView);
                _PStr(str, ICS_OFFERED_INFO, fTarget);
                return true;
            }

            default:
                !out << "\n\n\n*******!!!!! BLOCK-Code : "
                     << code << ": Unhandled" << std::endl;
                return true;
        }
    }

    return false;
}


bool
ICSMainWindow::_ParseAngleBracket(const char*& p)
{
    if (p[0] != '<')
        p = strchr(p, '<');

    if (p == NULL)
        return false;

    switch (p[1]) {
        case 's':
            switch (p[2]) {
                case '>':
                    _PStr(p + 3, ICS_SEEK_ADD, fICSChallengesView);
                    return true;

                case 'r':
                    if (p[3] == '>') {
                        _PStr(p + 4, ICS_SEEK_REMOVE, fICSChallengesView);
                        return true;
                    } else return false;

                case 'n':
                    if (p[3] == '>') {
                        _PStr(p + 4, ICS_SEEK_ADD, fICSChallengesView);
                        return true;
                    } else return false;

                case 'c':
                    if (p[3] == '>') {
                        _PStr("", ICS_SEEK_CLEAR, fICSChallengesView);
                        return true;
                    } else return false;

                default:
                    return false;
            }
            return false;

        case 'd':
            if (p[2] == '1' && p[3] == '>') {
                _PStr(p + 4, ICS_MADE_MOVE, fTarget);
                return true;
            } else return false;

        case 'g':
            if (p[2] == '1' && p[3] == '>') {
                _PStr(p + 4, ICS_GAME_INFO, fTarget);
                return true;
            } else return false;

        case 'p':
            switch (p[2]) {
                case '>':
                    //ICS_PING_INFO;
                    return true;

                case 'f':
                    if (p[3] == '>') {
                    // what does <pf> ?? TODO
                        return true;
                    } else return false;

                case 't':
                    if (p[3] == '>') {
                        //ICS_PEND_INFO;
                        return true;
                    } else return false;

                case 'r':
                    if (p[3] == '>') {
                        //ICS_REMOVE_SOMETHING_ERROR;
                        return true;
                    } else return false;

                default:
                    return false;
            }

            return false;

        case 'w':
            switch (p[2]) {
                case 'a':
                    if (p[3] == '>') {
                        //ICS_PLAYER_ADD;
                        return true;
                    } else return false;

                case 'd':
                    if (p[3] == '>') {
                        //ICS_PLAYER_REMOVE;
                        return true;
                    } else return false;

                default:
                    return false;
            }
            return false;

        default:
            return false;
    }

    return false;
}


bool
ICSMainWindow::_ParseSquareBracket(const char*& p)
{
    if (p[0] != '[')
        p = strchr(p, '[');

    if (p == NULL)
        return false;

    std::vector<BString> vec = Tools::Split(p, ' ', "[", "]");

    if (vec.empty())
        return false;

    return true;
}


bool
ICSMainWindow::_ParseCurlyBracket(const char*& p)
{
    if (p[0] != '{')
        p = strchr(p, '{');

    if (p == NULL)
        return false;

    BString str(p);
    str.RemoveSet("()");
    std::vector<BString> v = Tools::Split(str, ' ', "{", "}");

    if (v.empty())
        return false;

    int index = atoi(v[1]);
    BMessage msg;

    if (v[5] == "Creating") {
        if (v[2] == fICSInfo.user) {
            !out <<"Creating: \n*****" << str << "######" << std::endl;
            fTarget = new BoardWindow(index, true, v[2], v[4], this, fICS);
            _ParseAngleBracket(p);
        } else if (v[4] == fICSInfo.user) {
            !out <<"Creating: \n*****" << str << "######" << std::endl;
            fTarget = new BoardWindow(index, false, v[2], v[4], this, fICS);
            _ParseAngleBracket(p);
        }
    }


    if (fTarget == NULL)
        return true;

    if (((BoardWindow*)fTarget)->Index() == index) {
        if (v[6] == "forfeits") {
            if (v[8] == "time")
                _PStr(v[5], ICS_FORFEIT_ON_TIME, fTarget);
            else if (v[8] == "disconnection")
                _PStr(v[5], ICS_FORFEIT_BY_DISCONNECTION , fTarget);
        }
    }

    return true;
}


void
ICSMainWindow::_ParseICSOutput(BString & str)
{
    const char* p = str.LockBuffer(65356);

    for ( ; p[0] != '\0'; ++p) {
        switch ((int)p[0]) {
            case BLOCK_END:
                // if there was an unparsed Block_start, because of a
                // missing block_end, then append the string to the rest
                // of the unparsed string and parse now the whole.
                if (fParseRest.Length() != 0) {
                    str.UnlockBuffer();
                    fParseRest << str;
                    const char* pr = fParseRest.LockBuffer(65356);
                    _ParseBlock(pr);
                    fParseRest.UnlockBuffer();
                    fParseRest = "";
                }

                return;

            case BLOCK_START:
                if(_ParseBlock(p) == false) {
                    str.UnlockBuffer();
                    fParseRest << str;
                }

                return;

            case '<':
                if (_ParseAngleBracket(p))
                    return;
                else break;

            case '[':
                if (_ParseSquareBracket(p))
                    return;
                else break;

            case '{':
                if (_ParseCurlyBracket(p))
                    return;
                else break;

            default:
                break;
        } // End of Switch
    } // End of For

    str.UnlockBuffer();

    _PStr(str, ICS_OFFERED_INFO, fICSChatView);
    _PStr(str, ICS_OFFERED_INFO, fTarget);
} // End of ParseICSOutput


void
ICSMainWindow::_SaveSettings(void)
{
    Settings settings("ics_wnd");
    settings
        << "Split10" << (float)fSplitView1->ItemWeight(0L)
        << "Split11" << (float)fSplitView1->ItemWeight(1L)
        << "Split20" << (float)fSplitView2->ItemWeight(0L)
        << "Split21" << (float)fSplitView2->ItemWeight(1L)
        << "Split30" << (float)fSplitView3->ItemWeight(0L)
        << "Split31" << (float)fSplitView3->ItemWeight(1L)
        << "Split40" << (float)fSplitView4->ItemWeight(0L)
        << "Split41" << (float)fSplitView4->ItemWeight(1L)
        << std::endl;
}


void
ICSMainWindow::_LoadSettings(void)
{
    float value;
    Settings settings("ics_wnd");

    value = 1;
    settings << "Split10" >> value;
    fSplitView1->SetItemWeight(0, value, true);
    if (value == 0)
    	fSplitView1->SetItemCollapsed(0, false);
    	
    value = 5;
    settings << "Split11" >> value;
    fSplitView1->SetItemWeight(1, value, true);
    if (value == 0)
    	fSplitView1->SetItemCollapsed(1, false);

    value = 3;
    settings << "Split20" >> value;
    fSplitView2->SetItemWeight(0, value, true);
    if (value == 0)
    	fSplitView2->SetItemCollapsed(0, false);
    	
    value = 1;
    settings << "Split21" >> value;
    fSplitView2->SetItemWeight(1, value, true);
    if (value == 0)
    	fSplitView2->SetItemCollapsed(1, false);

    value = 3;
    settings << "Split30" >> value;
    fSplitView3->SetItemWeight(0, value, true);
    if (value == 0)
    	fSplitView3->SetItemCollapsed(0, false);
    	
    value = 1;
    settings << "Split31" >> value;
    fSplitView3->SetItemWeight(1, value, true);
    if (value == 0)
    	fSplitView3->SetItemCollapsed(1, false);

    value = 2;
    settings << "Split40" >> value;
    fSplitView4->SetItemWeight(0, value, true);
    if (value == 0)
    	fSplitView4->SetItemCollapsed(0, false);
    	
    value = 1;
    settings << "Split41" >> value;
    fSplitView4->SetItemWeight(1, value, true);
    if (value == 0)
    	fSplitView4->SetItemCollapsed(1, false);
}


void
ICSMainWindow::_SetIcsVariables(void)
{
    _Send("iset block 1");
    _Send("2000 set echo 0");
    _Send("2000 iset nowrap 1");
    _Send("2000 iset pendinfo 1");
    _Send("2000 iset pinginfo 1");
        // Notifies you about your ping
        // e.g  <p> N   (your ping is normal)
    _Send("2000 iset compressmove 1");
    _Send("2000 iset gameinfo 1");
    _Send("2000 iset seekinfo 1");
    _Send("2000 iset seekremove 1");
    _Send("2000 set seek 0");
    _Send("2000 iset premove 1");
    _Send("2000 set style 1");
    _Send("2000 iset showownseek 1");
    _Send("2000 set showownseek 1");
    _Send("2000 iset smartmove 1");
    _Send("2000 iset movecase 1");
    _Send("2000 set gin 1"); // Show games that are started (not the own)
    _Send("2000 set open 1");
    _Send("2000 set pgn 1");
    _Send("2000 iset pin 1");     //notify when users connect and disconnect
    _Send("2000 set notifiedby 1");
    _Send("2000 set noescape 1");
    _Send("2000 iset lock 1");
}


void
ICSMainWindow::_PStr(BString str, int code, BLooper* target)
{
    if (target == NULL)
        return;

    BMessage msg(code);
    if (str.Length() > 0)
        msg.AddString("info", str);

    target->PostMessage(&msg);
}


void
ICSMainWindow::_PStr(BString str, int code, BHandler* target)
{
    if (target == NULL)
        return;

    BMessage msg(code);
    if (str.Length() > 0)
        msg.AddString("info", str);

    PostMessage(&msg, target);
}
