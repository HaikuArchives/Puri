/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "BoardWindow.h"

#include <Alert.h>
#include <Button.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <GroupView.h>
#include <LayoutBuilder.h>
#include <MenuItem.h>
#include <Screen.h>
#include <SpaceLayoutItem.h>
#include <SplitLayoutBuilder.h>
#include <StringView.h>
#include <View.h>

#include "FicsCodes.h"
#include "ICSMainWindow.h"
#include "Messages.h"
#include "Settings.h"
#include "Tools.h"

// TODO: use Cardlayout to Offline and Online Chess

BoardWindow::BoardWindow(void)
	:
    BWindow(BRect(), "Puri - Master of Chess",
            /*B_DOCUMENT_WINDOW*/ B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS
                | B_QUIT_ON_WINDOW_CLOSE
                | B_AUTO_UPDATE_SIZE_LIMITS | B_OUTLINE_RESIZE),
    fEngineCanWrite(false),
    fIndex(0)
{
    fICSMainWindow = NULL;

    fSkillSlider = new BSlider("slider", "", new BMessage(MSG_SKILLCHANGED),
                                                           0, 20, B_HORIZONTAL);
    fSkillSlider->SetValue(20);
    fSkillSlider->SetToolTip("Engine Skill: 20");
    fSkillSlider->Value();
    fSkillSlider->SetEnabled(true);

	fTabView    = new BTabView("tab_view");
    fTabView->SetTabWidth(B_WIDTH_FROM_LABEL);
	fMovesView  = new MovesView();

	fTabsVec.push_back(new BTab());
	fTabView->AddTab(fMovesView, fTabsVec.back());
	fTabsVec.back()->SetLabel("Moves");

	//fTabsVec.push_back( new BTab() );
	//fTabView->AddTab( new MovesView(), fTabsVec.back() );
	//fTabsVec.back()->SetLabel( "Moves2" );

	// Build the layout

	fChessBoard     = new ChessBoardView(BRect(0, 0, 0, 0));
	fSidebarView    = new SidebarView();
	fTimeView       = new TimeView(REPLYCODE_TIMEVIEW);
    fNavigationView = new NavigationView();
    fMaterialView   = new MaterialView();
    fEngineOutput   = new EngineOutputView(fChessBoard->Board());

	fCard = new BCardLayout();

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
        .Add(fMenuBar = _CreateMenuBar())
		.AddSplit(B_HORIZONTAL).GetSplitView(&fSplitView)
			.AddGroup(B_VERTICAL, 0, 3.5)
				.AddGroup(B_HORIZONTAL, 0, 4)
					.AddGroup(B_VERTICAL, 0, 1).GetLayout(&fLeftColumn)
						.Add(fSidebarView)
					.End()
                    .AddGlue(0)
					.AddGroup(B_VERTICAL, 0, 12).GetLayout(&fMiddleColumn)
						.Add(fChessBoard, 12)
						.Add(fNavigationView)
					.End()
				.End()
			.End()
			.AddGroup(B_VERTICAL, 3, 1).GetLayout(&fRightColumn)
				//	.AddGlue()
                    .AddStrut(3)
					.Add(fTimeView)
                    .AddSplit(B_VERTICAL).GetSplitView(&fMaterialSV)
                        .AddGroup(B_VERTICAL, 0, 20)
                            .Add(fTabView, 20)
                        .End()
                        .AddGroup(B_VERTICAL, 0, 3)
                            .AddStrut(1)
                            .Add(fMaterialView, 3)
                        .End()
                        .AddGroup(B_VERTICAL, 0, 3)
                            .Add(fSkillSlider)
                            .Add(fEngineOutput)
                        .End()
                    .End()
				//	.AddGlue()
			.End()
		.End()

		//.AddGrid()
		//	.Add( new BButton( "left4" , NULL ),0,0 )
			//.Add( new BButton( "left5" , NULL ),2,2 )
	;
/*
	fCard->AddView( new BButton( "test1", "test1",NULL) );
	fCard->AddView( new BButton( "test2", "test2",NULL) );
	fCard->AddView( new BButton( "test3", "test3",NULL) );
	fCard->SetVisibleItem( 0L );
	fCard->SetVisible(true);
*/
	//fMiddleColumn->AlignLayoutWith( fRightColumn, B_VERTICAL);
	fSplitView->SetSplitterSize(9);
    fSplitView->SetSpacing(0);
    fMaterialSV->SetSplitterSize(9);
    fMaterialSV->SetSpacing(0);
	fChessBoard->MakeFocus();

	fEngine = new Engine("stockfish", REPLYCODE_ENGINE,
                            this, this, B_LOWEST_ACTIVE_PRIORITY);
    fEngine->PostMessage(MSG_ENG_START);
    _LoadSettings();
    Show();
    Hide();
	PostMessage(MENU_GAME_NEWGAME);
}


BoardWindow::BoardWindow(int index, bool userIsWhite, BString whiteName,
    BString blackName, ICSMainWindow* icswindow, ICS* ics)
    :
    BWindow(BRect(), " ",
            B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS
               /* | B_QUIT_ON_WINDOW_CLOSE*/
                | B_AUTO_UPDATE_SIZE_LIMITS | B_OUTLINE_RESIZE),
    fICSMainWindow(icswindow),
    fICS(ics),
    fSavedFrame(BRect()),
    fEngineCanWrite(true),
    fIndex(index)
{
    fPlayerName[0] = whiteName;
    fPlayerName[1] = blackName;

    fMenuBar = _CreateMenuBar();
    fMenuBar->Hide();
    fICSConnectMI->SetMarked(true);

    fSkillSlider = new BSlider("slider", "", new BMessage(MSG_SKILLCHANGED),
                                                           0, 20, B_HORIZONTAL);
    fSkillSlider->SetValue(20);
    fSkillSlider->SetToolTip("Engine Skill: 20");
    fSkillSlider->Value();
    fSkillSlider->SetEnabled(true);

	fTabView    = new BTabView("tab_view");
    fTabView->SetTabWidth(B_WIDTH_FROM_LABEL);
	fMovesView  = new MovesView();

	fTabsVec.push_back(new BTab());
	fTabView->AddTab(fMovesView, fTabsVec.back());
	fTabsVec.back()->SetLabel("Moves");

	//fTabsVec.push_back( new BTab() );
	//fTabView->AddTab( new MovesView(), fTabsVec.back() );
	//fTabsVec.back()->SetLabel( "Moves2" );

	// Build the layout

	fChessBoard     = new ChessBoardView(BRect(0, 0, 0, 0));
	fSidebarView    = new SidebarView;
	fTimeView       = new TimeView(REPLYCODE_TIMEVIEW);
    fNavigationView = new NavigationView();
    fMaterialView   = new MaterialView;
    fChatView       = new ICSChatView;

	BSplitView* fSplitView;
    BSplitView* fMaterialSV;

	fCard = new BCardLayout();

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.AddSplit(B_HORIZONTAL).GetSplitView(&fSplitView)
			.AddGroup(B_VERTICAL, 0, 3.5)
				.Add(fMenuBar)
				.AddGroup(B_HORIZONTAL, 0, 4)
					//.AddGroup(B_VERTICAL, 0, 1).GetLayout(&fLeftColumn)
						//.Add(fSidebarView)
					//.End()
                    //.AddGlue(0)
					.AddGroup(B_VERTICAL, 0, 12).GetLayout(&fMiddleColumn)
						.Add(fChessBoard, 12)
						//.Add(fNavigationView)
					.End()
				.End()
			.End()
			.AddGroup(B_VERTICAL, 5).GetLayout(&fRightColumn)
				//	.AddGlue()
					.Add(fTimeView)
                    .AddSplit(B_VERTICAL).GetSplitView(&fMaterialSV)
                        .AddGroup(B_VERTICAL, 0, 12)
                            .Add(fTabView, 20)
                            //.Add(fSkillSlider)
                        .End()
                        .Add(fChatView, 8)
                        .AddGroup(B_VERTICAL, 0, 3)
                            .Add(fMaterialView, 3)
                        .End()
                    .End()
				//	.AddGlue()
			.End()
		.End()

		//.AddGrid()
		//	.Add( new BButton( "left4" , NULL ),0,0 )
			//.Add( new BButton( "left5" , NULL ),2,2 )
	;
/*
	fCard->AddView( new BButton( "test1", "test1",NULL) );
	fCard->AddView( new BButton( "test2", "test2",NULL) );
	fCard->AddView( new BButton( "test3", "test3",NULL) );
	fCard->SetVisibleItem( 0L );
	fCard->SetVisible(true);
*/
	//fMiddleColumn->AlignLayoutWith( fRightColumn, B_VERTICAL);
	fSplitView->SetSplitterSize(9);
    fSplitView->SetSpacing(0);
    fSplitView->SetCollapsible(true);
    fMaterialSV->SetSplitterSize(9);
    fMaterialSV->SetSpacing(0);
    fMaterialSV->SetCollapsible(true);
	fChessBoard->MakeFocus();
    //fMenuBar->Hide();

	fEngine = NULL;/*new Engine("stockfish", REPLYCODE_ENGINE,
                            this, this, B_LOWEST_ACTIVE_PRIORITY);
    //fEngine->PostMessage(MSG_ENG_START);
	//PostMessage(MENU_GAME_NEWGAME);
*/
    fChessBoard->SetBoardTurned(!userIsWhite);


    if (userIsWhite)
        fChatView->AddTellTarget(fPlayerName[1]);
    else
        fChatView->AddTellTarget(fPlayerName[0]);

    fTimeView->Init(0);
    Zoom();
    Show();
}


bool
BoardWindow::QuitRequested()
{
    Hide();
    if (fICSMainWindow != NULL) {
        fICSMainWindow->PostMessage(MSG_WINDOW_CLOSED);
        if (fTimeView->IsStarted()) {
            BString str;
            str << BLK_RESIGN << " resign";
            SendICS(str, fICSMainWindow);
        }

        return false;
    } else {
        if (fEngine != NULL)
            fEngine->PostMessage(MSG_QUIT);

        if (fICSConnectMI->IsMarked() == false)
            _SaveSettings();

        return true;
    }
}


bool
BoardWindow::_MessageFilter(BMessage* message)
{
    if (fICSConnectMI->IsMarked()) {
        switch (message->what) {
            case MSG_MOVEFIRST: case MSG_MOVEBACK : case MSG_MOVENEXT:
            case MSG_MOVELAST : case MSG_GOTO_MOVE: case MENU_GAME_MOVENOW:
            case MENU_ENGINE_SOENGINE: case MENU_GAME_NEWGAME:
            case MENU_VIEW_FULLSCREEN:
                return true;
            default:
                break;
        }
    }

    return false;
}


void BoardWindow::MessageReceived(BMessage* message)
{
    if (_MessageFilter(message) == true)
        return;

	switch (message->what) {
		case REPLYCODE_ENGINE:
		{
            if (fEngineCanWrite == false)
                break;

            PostMessage(message, fEngineOutput);
			break;
		}

        case MSG_ENGINEMOVE:
            fEngineIsRunning = false;
            PostMessage(message, fChessBoard);
            break;

        case REPLYCODE_ICS:
		{
			break;
		}


        case REPLYCODE_TIMEVIEW:
            int32  value;
            message->FindInt32("time", &value);
            break;

        case MSG_SKILLCHANGED:
        {
            BString str = "Engine Skill: ";
            str << fSkillSlider->Value();
            fSkillSlider->SetToolTip(str);

            BMessage msg(MSG_SKILLCHANGED);
            msg.AddInt32("val", fSkillSlider->Value());

            if (fEngine != NULL)
                fEngine->PostMessage(&msg);

            break;
        }

		//case 10:
		//	be_roster->Launch( "application/x-vnd.cipri.Puri" );
		//	break;

        case MENU_VIEW_3D:
            f3DMI->SetMarked(!f3DMI->IsMarked());
            fChessBoard->SetViewMode3D(f3DMI->IsMarked());
            break;

		case MSG_MOVEFIRST:
            fEngineCanWrite = false;
            _EngineStop();
            fEngineIsRunning = false;
            fEngineOutput->SetOn(false);
            fTimeView->Stop();
			fChessBoard->MoveFirst();
            fMovesView ->MoveFirst();
            fMaterialView->SetMaterial(fChessBoard->GetMaterial());
            fChessBoard->SetUserCanMove();
			break;

		case MSG_MOVEBACK:

			fEngineCanWrite = false;
            _EngineStop();
            fEngineIsRunning = false;
            fEngineOutput->SetOn(false);
            fTimeView->Stop();
			fChessBoard->MoveBack();
            fMovesView ->MoveBack();
            fMaterialView->SetMaterial(fChessBoard->GetMaterial());
            fChessBoard->SetUserCanMove();
			break;

		case MSG_MOVENEXT:
            fEngineCanWrite = false;
            _EngineStop();
            fEngineIsRunning = false;
            fEngineOutput->SetOn(false);
            fTimeView->Stop();
			fChessBoard->MoveNext();
            fMovesView ->MoveNext();
            fMaterialView->SetMaterial(fChessBoard->GetMaterial());
            fChessBoard->SetUserCanMove();
			break;

		case MSG_MOVELAST:
            fEngineCanWrite = false;
            _EngineStop();
            fEngineIsRunning = false;
            fEngineOutput->SetOn(false);
            fTimeView->Stop();
			fChessBoard->MoveLast();
            fMovesView ->MoveLast();
            fMaterialView->SetMaterial(fChessBoard->GetMaterial());
            fChessBoard->SetUserCanMove();
			break;

        case MSG_GOTO_MOVE:
        {
            fEngineCanWrite = false;
            _EngineStop();
            fEngineIsRunning = false;
            fEngineOutput->SetOn(false);
            uint32  idx = 0;
            message->FindUInt32("move_idx", &idx);
            fTimeView->Stop();
            fChessBoard->GotoMove(idx);
            fMovesView ->GotoMove(idx);
            fMaterialView->SetMaterial(fChessBoard->GetMaterial());
            break;
        }

		case MENU_APP_QUIT:
			be_app->PostMessage(B_QUIT_REQUESTED);
			break;

        case MSG_QUIT:
            fICSMainWindow = NULL;

            QuitRequested();
            break;

        case MENU_VIEW_SHOW_WINDOW:
        {
            Show();
            break;
        }

        case MENU_VIEW_FLIPBOARD:
        {
            if (fFlipBoardMI->IsMarked())
                fFlipBoardMI->SetMarked(false);
            else
                fFlipBoardMI->SetMarked(true);

            fChessBoard->FlipBoard();
            break;
        }

		case MENU_VIEW_MININTERFACE:
			if (fMinIfMenuItem->IsMarked()) {
				if (!fShowborderMenuItem->IsMarked())
					PostMessage( MENU_VIEW_SHOWBORDER);

				if (!fSidebarMenuItem->IsMarked())
					PostMessage(MENU_VIEW_SIDEBAR);

				if (!fShowMBarMenuItem->IsMarked())
					PostMessage(MENU_VIEW_SHOWMENU);

                if (fAlwaysOnTopMenuItem->IsMarked())
					PostMessage(MENU_VIEW_ALWAYSONTOP);

				fMinIfMenuItem->SetMarked(false);
			} else {
				if (fShowborderMenuItem->IsMarked())
					PostMessage (MENU_VIEW_SHOWBORDER);

				if (fSidebarMenuItem->IsMarked())
					PostMessage (MENU_VIEW_SIDEBAR);

				if (fShowMBarMenuItem->IsMarked())
					PostMessage(MENU_VIEW_SHOWMENU);

                if (!fAlwaysOnTopMenuItem->IsMarked())
					PostMessage(MENU_VIEW_ALWAYSONTOP);

				fMinIfMenuItem->SetMarked(true);
			}

			break;

        case MENU_VIEW_FULLSCREEN:
        {
            if (fFullScrMI->IsMarked()) {
                fFullScrMI->SetMarked(false);
                Hide();
                MoveTo(fSavedFrame.left, fSavedFrame.top);
                ResizeTo(fSavedFrame.Width(), fSavedFrame.Height());
                Show();
            } else {
                fFullScrMI->SetMarked(true);
                fSavedFrame = Frame();
                BScreen screen(this);
                BRect rect(screen.Frame());
                Hide();
                MoveTo(rect.left, rect.top);
                ResizeTo(rect.Width(), rect.Height());
                Show();
            }

            break;
        }

		case MENU_VIEW_SIDEBAR:
        	if (fSidebarMenuItem->IsMarked() == false) {
        		fSidebarMenuItem->SetMarked(true);
                fSidebarView->Show();
        	} else {
				fSidebarMenuItem->SetMarked(false);
                fSidebarView->Hide();
			}

			break;

		case MENU_VIEW_SHOWBORDER:
			if (fShowborderMenuItem->IsMarked()) {
				SetLook(B_BORDERED_WINDOW_LOOK);
				fShowborderMenuItem->SetMarked(false);
			} else {
				SetLook(B_DOCUMENT_WINDOW_LOOK);
				fShowborderMenuItem->SetMarked(true);
			}

			break;

		case MENU_VIEW_AUTOHIDING:
        	if (fAutohidingMenuItem->IsMarked()) {
        		fAutohidingMenuItem->SetMarked(false);
        		fSidebarView->SetAutohiding(false);
                fNavigationView->SetAutohiding(false);
        	} else {
				fAutohidingMenuItem->SetMarked(true);
                fSidebarView->SetAutohiding(true);
                fNavigationView->SetAutohiding(true);
			}

			break;

		case MENU_VIEW_SHOWCLOCK:
			if (fShowClockMenuItem->IsMarked() == false) {
				fTimeView->Show();
				fShowClockMenuItem->SetMarked(true);
			} else {
				fTimeView->Hide();
				fShowClockMenuItem->SetMarked(false);
			}

			break;

        case MENU_VIEW_SHOWMENU:
			if (fShowMBarMenuItem->IsMarked() == false) {
				fMenuBar->Show();
				fShowMBarMenuItem->SetMarked(true);
			} else {
				fMenuBar->Hide();
				fShowMBarMenuItem->SetMarked(false);
			}

			break;

        case MENU_VIEW_ALWAYSONTOP:
            fAlwaysOnTopMenuItem->SetMarked(!fAlwaysOnTopMenuItem->IsMarked());
            SetFeel(fAlwaysOnTopMenuItem->IsMarked() ?
                             B_FLOATING_ALL_WINDOW_FEEL : B_NORMAL_WINDOW_FEEL);
            break;

        // TODO: after the game, ask for isready,
        // and wait till the engine is ready.
		case MENU_GAME_NEWGAME:
            fEngineCanWrite = false;
            fChessBoard->NewGame();
			fTimeView->Init(300);
            fMovesView->Clear();
            fMaterialView->Clear();
            fChessBoard->SetUserCanMove(true);

            if (fEngine != NULL) {
                fEngine->PostMessage(MSG_UNLOCK);
                fEngine->PostMessage(MSG_ENG_NEWGAME);
			}
            fEngineCanWrite = true;
            fEngineIsRunning = false;
			break;

        case MENU_GAME_MOVENOW:
            if (fEngineOffMenuItem->IsMarked())
                fEngineOffMenuItem->SetMarked(false);

            if (fTimeView->IsStarted() == false)
                fTimeView->Start();

            if (fEngineIsRunning)
                _EngineStop();
            else
                _EngineGo();

            break;

		case MENU_ENGINE_SOENGINE:
            if (fEngineOffMenuItem->IsMarked()) {
                fEngineOffMenuItem->SetMarked(false);
            } else {
                fEngineCanWrite = false;
                _EngineStop();
                fEngineOffMenuItem->SetMarked(true);
                fChessBoard->SetUserCanMove(true);
            }

			break;

        case ICS_DISCONNECTED:
          //  fICSConnectMI->SetMarked(false);
           // fChessBoard->SetUserCanMove(false);
            break;

        case MSG_SEND_ICS_CMD:
            if (fICSMainWindow != NULL) {
                fICSMainWindow->PostMessage(message);
            }
            break;

        case MENU_HELP_MOVEASSISTANT:
            fMoveAssistantMenuItem->SetMarked(
                                           !fMoveAssistantMenuItem->IsMarked());
            fChessBoard->ShowValidMoves(fMoveAssistantMenuItem->IsMarked());
            break;

		case MENU_HELP_HINT:
            break;

		case MENU_HELP_HELP:
        {
            BString str = "file://";
            str << Tools::AppPath() << "/data/Help/Help.html";
            char const* args[] = {str.String(), 0};
            be_roster->Launch("text/html", 1, const_cast<char **>(args));
			break;
        }

		case MENU_HELP_ABOUT:
        {
            BString str;
            str << "Puri is a Chess GUI\n\twritten by\n";
            str << "Ciprian Nedisan\n\n\n";
            str << "Version: " << (float)VERSION << "\n";
            str << "License: MIT\n\n";
            str << "Many thanks to:\n";
            str << "\talexd\n";
            str << "\tingo\n";
            str << "\tstippi\n";
            str << "\tZuMi\n\n";

            fChessBoard->CancelSelection();
			(new BAlert("About Puri", str, "Ok"))->Go();
			break;
        }

        case MSG_BOARDSTATE:
        {
            int32 state;
            message->FindInt32("val", &state);

            if (state > 1) {
                fTimeView->Stop();
                if (fEngineOutput != NULL)
                	fEngineOutput->SetOn(false);
            }

            break;
        }

        case ICS_GAME_INFO:
        {
            BString str;
            message->FindString("info", &str);
            // The ics protocol is wrongly  implemented. it= , i=
            vector<BString> timeW = Tools::Split(Tools::Find(str, "it="), ',');
            vector<BString> timeB = Tools::Split(Tools::Find(str, "i="), ',');
            fTimeView->SetTimeWhite(atoi(timeW[0]));
            fTimeView->SetTimeBlack(atoi(timeB[0]));
            fTimeView->Stop();

            vector<BString> elo = Tools::Split(Tools::Find(str, "rt=").RemoveSet("PE "), ',');
            BString title;
            title << fPlayerName[0] << " (" << elo[0] << ")  -  ";
            title << fPlayerName[1] << " (" << elo[1] << ")    [ ";
            title << atoi(timeW[0]) / 60 << " +" << atoi(timeW[1]);

            if (Tools::Find(str, "r=") == "1")
                title << "  rated ]";
            else
                title << "  unrated ]";

            SetTitle(title);

            break;
        }

        case ICS_MADE_MOVE:
        {
            BString move;
            message->FindString("info", &move);
            std::vector<BString> vec = Tools::Split(move);
            fTimeView->SetTimeMS(atoi(vec[5]), atoi(vec[1])%2 == 1);
            fChessBoard->DoICSMove(vec[3]);
            break;
        }

        case ICS_FORFEIT_ON_TIME:
        {
            fChessBoard->CancelSelection();
            fTimeView->Stop();
            fChessBoard->SetUserCanMove(true);
            BString str;
            message->FindString("info", &str);
            str << "\nforfeits on time";
            (new BAlert("Time", str, "OK"))->Go();

            break;
        }

        case ICS_FORFEIT_BY_DISCONNECTION:
        {
            fChessBoard->CancelSelection();
            fTimeView->Stop();
            fChessBoard->SetUserCanMove(true);
            BString str;
            message->FindString("info", &str);
            str << "\nforfeits by disconnection";

            (new BAlert("Disconnection", str, "OK"))->Go();

            break;
        }

        case ICS_RESIGN:
        {
            fChessBoard->CancelSelection();
            fTimeView->Stop();
            fChessBoard->SetUserCanMove(true);
            (new BAlert("Resign", "Game was resigned", "OK"))->Go();

            break;
        }

        case ICS_NOTPLAYING:
        {
            fTimeView->Stop();
            fChessBoard->SetUserCanMove(true);
         //   (new BAlert("Not Playing", "Game not active", "OK"))->Go();

            break;
        }

        case ICS_GAME_DRAWN:
        {
            fChessBoard->CancelSelection();
            fTimeView->Stop();
            fChessBoard->SetUserCanMove(true);
            (new BAlert("Draw", "Game was drawn", "OK"))->Go();

            break;
        }

        case ICS_OFFERED_INFO:
        case ICS_TELL:
        case ICS_SHOUT:
            PostMessage(message, fChatView);
            break;


        case ICS_GAME_ABORTED:
        {
            fChessBoard->CancelSelection();
            fTimeView->Stop();
            fChessBoard->SetUserCanMove(true);
            (new BAlert("Abort", "Game was aborted", "OK"))->Go();

            break;
        }

        case ICS_NEW_GAME:
        {
            fChessBoard->NewGame();
			fTimeView->Init(0);
            fMovesView->Clear();
            fMaterialView->Clear();

            BString wuser, buser;
            bool isWhite;
            message->FindString("wuser", &wuser);
            message->FindString("buser", &buser);
            message->FindBool("iswhite", & isWhite);

            fChessBoard->SetBoardTurned(!isWhite);

            if (isWhite)
                fChessBoard->SetUserCanMove(true);
            else
                fChessBoard->SetUserCanMove(false);

            Activate();
            break;
        }

        case MSG_ICS_MOVE_DONE:
        {
            BString move;
            message->FindString("move", &move);
            fMovesView->AddMove(move, !fChessBoard->IsWhiteTurn());
            fMaterialView->SetMaterial(fChessBoard->GetMaterial());
			fTimeView->Switch();
            fChessBoard->SetUserCanMove(true);
            break;
        }

		case USER_MADE_MOVE:
        {
            BString move;
            message->FindString("move", &move);
            fMovesView->AddMove(move, !fChessBoard->IsWhiteTurn());
			fTimeView->Switch();
            fMaterialView->SetMaterial(fChessBoard->GetMaterial());

            if (fICSConnectMI->IsMarked()) {
                BMessage msg(USER_MADE_MOVE);
                BString str;
                str << BLK_GAME_MOVE << " " << fChessBoard->LastMoveICSF();
                SendICS(str, fICSMainWindow);
                fChessBoard->SetUserCanMove(false);
            } else if (fEngineOffMenuItem->IsMarked() == false)
            {
                _EngineGo();
            }

			break;
        }

        case ENGINE_MADE_MOVE:
        {
            BString move;
            message->FindString("move", &move);
            fMovesView->AddMove(move, !fChessBoard->IsWhiteTurn());
            fMaterialView->SetMaterial(fChessBoard->GetMaterial());
			fTimeView->Switch();
            fChessBoard->SetUserCanMove(true);
			break;
        }

		default:
			BWindow::MessageReceived(message);
			break;
	}
}


BMenuBar*
BoardWindow::_CreateMenuBar(void)
{
	BMenuBar* menuBar = new BMenuBar("BoardWindowMenuBar");
	BMenuItem*	menuItem;

	BMenu*	applicationMenu = new BMenu("Application");
	menuBar->AddItem(applicationMenu);
	BMenu*  newSubMenu = new BMenu("New");
	applicationMenu->AddItem(newSubMenu);
        menuItem = new BMenuItem("Blitz Game" B_UTF8_ELLIPSIS, NULL, 'B');
		newSubMenu->AddItem(menuItem);
		menuItem->SetEnabled(false);
        menuItem = new BMenuItem("Long Game" B_UTF8_ELLIPSIS, NULL, 'L');
		newSubMenu->AddItem(menuItem);
		menuItem->SetEnabled(false);
		newSubMenu->AddSeparatorItem();
        menuItem = new BMenuItem("Position Setup" B_UTF8_ELLIPSIS, NULL, 'P');
		newSubMenu->AddItem(menuItem);
		menuItem->SetEnabled(false);
        menuItem = new BMenuItem("Engine Match" B_UTF8_ELLIPSIS, NULL, 'E');
		newSubMenu->AddItem(menuItem);
		menuItem->SetEnabled(false);
		BMenu*  openSubMenu = new BMenu("Open");
		applicationMenu->AddItem(openSubMenu);
        menuItem = new BMenuItem("PGN File" B_UTF8_ELLIPSIS, NULL, 'O');
		openSubMenu->AddItem(menuItem);
		menuItem->SetEnabled(false);
        menuItem = new BMenuItem("Database" B_UTF8_ELLIPSIS, NULL, 'D');
		openSubMenu->AddItem(menuItem);
		menuItem->SetEnabled(false);

	applicationMenu->AddSeparatorItem();
	menuItem = new BMenuItem("Save" B_UTF8_ELLIPSIS, NULL, 'S');
    applicationMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);
	applicationMenu->AddSeparatorItem();
    menuItem = new BMenuItem("Quit", new BMessage(MENU_APP_QUIT), 'Q');
	applicationMenu->AddItem(menuItem);

	BMenu*	editMenu = new BMenu("Edit");
	menuBar->AddItem(editMenu);

    menuItem = new BMenuItem("Copy Position", NULL, 'C');
	editMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);
    menuItem = new BMenuItem("Paste Position", NULL, 'V');
	editMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);

    editMenu->AddSeparatorItem();
    menuItem = new BMenuItem("Edit Game Data" B_UTF8_ELLIPSIS, NULL, 'E');
	editMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);

	BMenu*	viewMenu = new BMenu("View");
	menuBar->AddItem(viewMenu);
    fFlipBoardMI = new BMenuItem("Flip Board", new BMessage(MENU_VIEW_FLIPBOARD));
    viewMenu->AddItem(fFlipBoardMI);
    fFlipBoardMI->SetShortcut('R', B_COMMAND_KEY);
    fFlipBoardMI->SetMarked(false);

	viewMenu->AddSeparatorItem();
    fMinIfMenuItem = new BMenuItem("Minimal Interface",
        new BMessage(MENU_VIEW_MININTERFACE), '1');
	f3DMI = new BMenuItem("3D", new BMessage(MENU_VIEW_3D));
    viewMenu->AddItem(f3DMI);
    f3DMI->SetShortcut('3', B_COMMAND_KEY);
    f3DMI->SetMarked(false);

    fFullScrMI = new BMenuItem("Fullscreen",
        new BMessage(MENU_VIEW_FULLSCREEN), 'F');
    viewMenu->AddItem(fFullScrMI);
    fFullScrMI->SetMarked(false);
    fFullScrMI->SetEnabled(true);
	viewMenu->AddSeparatorItem();
    fShowClockMenuItem = new BMenuItem("Show Clock",
        new BMessage(MENU_VIEW_SHOWCLOCK));
    viewMenu->AddItem(fShowClockMenuItem);
    fShowClockMenuItem->SetShortcut('T', B_COMMAND_KEY);
    fShowClockMenuItem->SetMarked(true);
    fShowMBarMenuItem = new BMenuItem("Show Menubar",
        new BMessage(MENU_VIEW_SHOWMENU));
	viewMenu->AddItem(fShowMBarMenuItem);
    fShowMBarMenuItem->SetShortcut('M', B_COMMAND_KEY);
    fShowMBarMenuItem->SetMarked(true);
    fSidebarMenuItem = new BMenuItem("Show Sidebar",
        new BMessage(MENU_VIEW_SIDEBAR));
    viewMenu->AddItem(fSidebarMenuItem);
    fSidebarMenuItem->SetShortcut('4', B_COMMAND_KEY);
    fSidebarMenuItem->SetMarked(true);
    fShowborderMenuItem = new BMenuItem("Show Window Frame",
        new BMessage(MENU_VIEW_SHOWBORDER));
    viewMenu->AddItem(fShowborderMenuItem);
    fShowborderMenuItem->SetShortcut('Y', B_COMMAND_KEY);
    fShowborderMenuItem->SetMarked(true);
    fAutohidingMenuItem = new BMenuItem("Autohiding",
        new BMessage(MENU_VIEW_AUTOHIDING));
    viewMenu->AddItem(fAutohidingMenuItem);
    fAutohidingMenuItem->SetShortcut('X', B_COMMAND_KEY);
    fAutohidingMenuItem->SetMarked(false);
    viewMenu->AddSeparatorItem();
    fAlwaysOnTopMenuItem = new BMenuItem("Always on top",
        new BMessage(MENU_VIEW_ALWAYSONTOP));
    viewMenu->AddItem(fAlwaysOnTopMenuItem);
    fAlwaysOnTopMenuItem->SetShortcut('A', B_COMMAND_KEY);
    fAlwaysOnTopMenuItem->SetMarked(false);
    //PostMessage(new BMessage(MENU_VIEW_AUTOHIDING));

	BMenu*	gameMenu = new BMenu("Game");
	menuBar->AddItem(gameMenu);
	gameMenu->AddItem(new BMenuItem("New Game",
        new BMessage(MENU_GAME_NEWGAME), 'N'));
	gameMenu->AddSeparatorItem();
    menuItem = new BMenuItem("Move Now", new BMessage(MENU_GAME_MOVENOW), ' ');
	gameMenu->AddItem(menuItem);
	menuItem->SetEnabled(true);
    menuItem = new BMenuItem("Repley" B_UTF8_ELLIPSIS, NULL);
	gameMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);
	gameMenu->AddSeparatorItem();
		BMenu*  levelsSubMenu = new BMenu("Levels");
		levelsSubMenu->SetRadioMode(true);
		gameMenu->AddItem(levelsSubMenu);
        menuItem = new BMenuItem("Fixed depth" B_UTF8_ELLIPSIS, NULL);
		levelsSubMenu->AddItem(menuItem);
		menuItem->SetEnabled(false);
        menuItem = new BMenuItem("Fixed time" B_UTF8_ELLIPSIS, NULL);
		levelsSubMenu->AddItem(menuItem);
		menuItem->SetEnabled(false);
	gameMenu->AddSeparatorItem();
    fPauseMenuItem = new BMenuItem("Pause", NULL);
	gameMenu->AddItem(fPauseMenuItem);
	fPauseMenuItem->SetEnabled(false);
    menuItem = new BMenuItem("Offer Draw", NULL);
	gameMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);
    menuItem = new BMenuItem("Resign", NULL, '0');
	gameMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);

	BMenu*	engineMenu = new BMenu("Engine");
	menuBar->AddItem(engineMenu);
    menuItem = new BMenuItem("Change Engine" B_UTF8_ELLIPSIS, NULL);
	engineMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);
    fEngineOffMenuItem = new BMenuItem("Switch off Engine",
        new BMessage(MENU_ENGINE_SOENGINE));
	engineMenu->AddItem(fEngineOffMenuItem);
	fEngineOffMenuItem->SetShortcut('P', B_COMMAND_KEY);
    fEngineOffMenuItem->SetEnabled(true);
    fEngineOffMenuItem->SetMarked(false);
	engineMenu->AddSeparatorItem();
    menuItem = new BMenuItem("Engine Management" B_UTF8_ELLIPSIS, NULL);
	engineMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);

	BMenu*	toolsMenu = new BMenu("Tools");
	menuBar->AddItem(toolsMenu);
    menuItem = new BMenuItem("User Info" B_UTF8_ELLIPSIS, NULL);
	toolsMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);
	toolsMenu->AddSeparatorItem();
    menuItem = new BMenuItem("Options" B_UTF8_ELLIPSIS, NULL);
	toolsMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);
	toolsMenu->AddSeparatorItem();
    menuItem = new BMenuItem("Factory Settings" B_UTF8_ELLIPSIS, NULL);
	toolsMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);

    //BMenu*  serverMenu = new BMenu("Server");
    //menuBar->AddItem(serverMenu);
    fICSConnectMI = new BMenuItem("Connect" B_UTF8_ELLIPSIS,
        new BMessage(MENU_SERVER_CONNECT));
   // serverMenu->AddItem(fICSConnectMI);
   // fICSConnectMI->SetShortcut('S', B_CONTROL_KEY);

	BMenu*	helpMenu = new BMenu("Help");
	menuBar->AddItem(helpMenu);

    fMoveAssistantMenuItem = new BMenuItem("Move Assistant",
        new BMessage(MENU_HELP_MOVEASSISTANT));
    helpMenu->AddItem(fMoveAssistantMenuItem);
    fMoveAssistantMenuItem->SetShortcut('J', B_COMMAND_KEY);
    fMoveAssistantMenuItem->SetMarked(false);
    menuItem = new BMenuItem("Hint" B_UTF8_ELLIPSIS,
        new BMessage(MENU_HELP_HINT), 'H');
	helpMenu->AddItem(menuItem);
	menuItem->SetEnabled(false);
	helpMenu->AddSeparatorItem();
    menuItem = new BMenuItem("Help" B_UTF8_ELLIPSIS,
        new BMessage(MENU_HELP_HELP));
	helpMenu->AddItem(menuItem);
	menuItem->SetEnabled(true);
	helpMenu->AddSeparatorItem();
    menuItem = new BMenuItem("About Puri" B_UTF8_ELLIPSIS,
        new BMessage(MENU_HELP_ABOUT));
	helpMenu->AddItem(menuItem);

	return menuBar;
}


void
BoardWindow::_EngineGo(void)
{
    if (fEngine == NULL)
        return;

    fChessBoard->SetUserCanMove(false);
    fEngineCanWrite = true;

    BString str = "position startpos moves";

    str << fChessBoard->MovesEF() << "\n";
    str << "go wtime " << fTimeView->WTime()
        << " btime " << fTimeView->BTime();

    BMessage msg(MSG_ENG_MOVENOW);
    msg.AddString("moves", str);
    fEngine->PostMessage(&msg);
    fEngineIsRunning = true;
}


void
BoardWindow::_EngineStop(void)
{
    if (fEngine != NULL)
        fEngine->PostMessage(MSG_ENG_STOP);
}


void
BoardWindow::_SaveSettings(void)
{
    Settings settings("BoardWindow");
    settings
        << "Split10"        << fSplitView->ItemWeight(0)
        << "Split11"        << fSplitView->ItemWeight(1)
        << "Split20"        << fMaterialSV->ItemWeight(0)
        << "Split21"        << fMaterialSV->ItemWeight(1)
        << "Split22"        << fMaterialSV->ItemWeight(2)
        << "EngineSkill"    << fSkillSlider->Value()
        << "EngineOff"      << (int32)fEngineOffMenuItem->IsMarked()
        << "3DView"         << (int32)f3DMI->IsMarked()
        << "FlipBoard"      << (int32)fFlipBoardMI->IsMarked()
        << "Sidebar"        << (int32)fSidebarMenuItem->IsMarked()
        << "Autohiding"     << (int32)fAutohidingMenuItem->IsMarked()
        << "ShowClock"      << (int32)fShowClockMenuItem->IsMarked()
        << "ShowMBar"       << (int32)fShowMBarMenuItem->IsMarked()
        << "ShowBorder"     << (int32)fShowborderMenuItem->IsMarked()
        << "MoveAssistant"  << (int32)fMoveAssistantMenuItem->IsMarked()
        << "AlwaysOnTop"    << (int32)fAlwaysOnTopMenuItem->IsMarked()
        << "Fullscreen"     << (int32)fFullScrMI->IsMarked();

    if (fFullScrMI->IsMarked())
        settings << fSavedFrame;
    else
        settings << "SavedFrame" << Frame();

    settings << std::endl;
}


void
BoardWindow::_LoadSettings()
{
    Settings settings("BoardWindow");

    BRect frame;
    settings << "SavedFrame" >> fSavedFrame;
    if (fSavedFrame == BRect()) {
        Zoom();
    } else {
        MoveTo(fSavedFrame.left, fSavedFrame.top);
        ResizeTo(fSavedFrame.Width(), fSavedFrame.Height());
    }

    float weight;
    weight = 3.5;
    settings << "Split10" >> weight;
    fSplitView->SetItemWeight(0, weight, true);
    if (weight == 0)
    	fSplitView->SetItemCollapsed(0, false);
    	
    weight = 1.0;
    settings << "Split11" >> weight;
    fSplitView->SetItemWeight(1, weight, true);
    if (weight == 0)
    	fSplitView->SetItemCollapsed(1, false);
    	
    weight = 20.0;
    settings << "Split20" >> weight;
    fMaterialSV->SetItemWeight(0, weight, true);
    if (weight == 0)
    	fMaterialSV->SetItemCollapsed(0, false);
    	
    weight = 3.0;
    settings << "Split21" >> weight;
    fMaterialSV->SetItemWeight(1, weight, true);
    if (weight == 0)
    	fMaterialSV->SetItemCollapsed(1, false);
    	
    weight = 3.0;
    settings << "Split22" >> weight;
    fMaterialSV->SetItemWeight(2, weight, true);
    if (weight == 0)
    	fMaterialSV->SetItemCollapsed(2, false);

    int32 skill = 20;
    settings << "EngineSkill" >> skill;
    if (skill != 20) {
        fSkillSlider->SetValue(skill);
        PostMessage(MSG_SKILLCHANGED);
    }

    int32 value;
    value = 0;
    settings << "3DView" >> value;
    if (value != 0)
        PostMessage(MENU_VIEW_3D);

    value = 0;
    settings << "EngineOff" >> value;
    if (value != 0)
        PostMessage(MENU_ENGINE_SOENGINE);

    value = 0;
    settings << "FlipBoard" >> value;
    if (value != 0)
        PostMessage(MENU_VIEW_FLIPBOARD);

    value = 0;
    settings << "Autohiding" >> value;
    if (value != 0)
        PostMessage(MENU_VIEW_AUTOHIDING);

    value = 1;
    settings << "ShowMBar" >> value;
    if (value != 1) {
        PostMessage(MENU_VIEW_SHOWMENU);
    }

    value = 1;
    settings << "ShowClock" >> value;
    if (value != 1)
        PostMessage(MENU_VIEW_SHOWCLOCK);

    value = 1;
    settings << "ShowBorder" >> value;
    if (value != 1)
        PostMessage(MENU_VIEW_SHOWBORDER);

    value = 0;
    settings << "MoveAssistant" >> value;
    if (value != 0)
        PostMessage(MENU_HELP_MOVEASSISTANT);

    value = 0;
    settings << "AlwaysOnTop" >> value;
    if (value != 0)
        PostMessage(MENU_VIEW_ALWAYSONTOP);

    value = 0;
    settings << "Fullscreen" >> value;
    if (value != 0) {
        PostMessage(MENU_VIEW_FULLSCREEN);
    }

    value = 1;
    settings << "Sidebar" >> value;
    if (value != 1)
        PostMessage(MENU_VIEW_SIDEBAR);


    PostMessage(MENU_VIEW_SHOW_WINDOW);
}
