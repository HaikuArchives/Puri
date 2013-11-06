/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef BOARDWINDOW_H
#define BOARDWINDOW_H

#include <vector>

#include <CardLayout.h>
#include <MenuBar.h>
#include <Slider.h>
#include <SplitView.h>
#include <StringView.h>
#include <TabView.h>
#include <Window.h>

#include "ChessBoardView.h"
#include "Debug.h"
#include "Engine.h"
#include "EngineOutputView.h"
#include "ICS.h"
#include "ICSChatView.h"
#include "ICSMainWindow.h"
#include "MaterialView.h"
#include "MovesView.h"
#include "NavigationView.h"
#include "TimeView.h"
#include "SidebarView.h"


class BoardWindow : public BWindow
{
public:
                        BoardWindow(void);
                        BoardWindow(int index, bool userIsWhite,
                            BString whiteName, BString blackName,
                            ICSMainWindow* icswindow, ICS* ics);
    virtual void        MessageReceived(BMessage* message);
    virtual bool        QuitRequested(void);

            int         Index(void) { return fIndex; }

private:
            void        _SaveSettings(void);
            void        _LoadSettings(void);
    inline  void        _EngineGo();
    inline  void        _EngineStop();
    bool                _MessageFilter(BMessage* message);
    BMenuBar*		    _CreateMenuBar(void);

    BCardLayout*	        fCard;
    BMenuBar*		        fMenuBar;
    BMenuItem*		        fPauseMenuItem;
    BMenuItem*		        fEngineOffMenuItem;
    BMenuItem*	            fFlipBoardMI;
    BMenuItem*	            f3DMI;
    BMenuItem*		        fSidebarMenuItem;
    BMenuItem*		        fMinIfMenuItem;
    BMenuItem*		        fAutohidingMenuItem;
    BMenuItem*              fShowClockMenuItem;
    BMenuItem*		        fShowMBarMenuItem;
    BMenuItem*		        fShowborderMenuItem;
    BMenuItem*		        fMoveAssistantMenuItem;
    BMenuItem*              fAlwaysOnTopMenuItem;
    BMenuItem*              fFullScrMI;
    BMenuItem*              fICSConnectMI;

    BTabView*		        fTabView;
    std::vector<BTab*>      fTabsVec;

    BGroupLayout*	        fLeftColumn;
    BGroupLayout*	        fMiddleColumn;
    BGroupLayout*	        fRightColumn;

    ICSChatView*            fChatView;
    ChessBoardView*		    fChessBoard;
    MovesView*			    fMovesView;
    NavigationView*		    fNavigationView;
    SidebarView*            fSidebarView;
    TimeView*			    fTimeView;
    MaterialView*           fMaterialView;
    BSlider*                fSkillSlider;
    BSplitView*             fSplitView;
    BSplitView*             fMaterialSV;

    EngineOutputView*       fEngineOutput;
    Engine*			        fEngine;
    ICSMainWindow*    		fICSMainWindow;
    ICS*                    fICS;

    BRect                   fSavedFrame;
    bool                    fEngineCanWrite;
    bool                    fEngineIsRunning;

    int                     fIndex;

    BString                 fPlayerName[2];

    Debug                   out;
};


#endif

