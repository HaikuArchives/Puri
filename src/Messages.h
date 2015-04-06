/*
 * Copyright 2011-2012 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef MESSAGES_H
#define MESSAGES_H

#include <String.h>
#include <Looper.h>

enum{
        MSG_2D3DVIEW                = 'g201'    ,
		MENU_APP_QUIT               = 'ga01'    ,
        MSG_BOARDSTATE              = 'gb01'    ,
        MSG_CANCEL_SELECTION        = 'gc01'    ,
        MSG_ENG_CMD                 = 'ge01'    ,
        MSG_ENG_MOVENOW             = 'ge02'    ,
        MSG_ENG_NEWGAME             = 'ge03'    ,
        MSG_ENG_QUIT                = 'ge04'    ,
        MSG_ENG_START               = 'ge05'    ,
        MSG_ENG_STOP                = 'ge06'    ,
        ENGINE_MADE_MOVE            = 'ge07'    ,
		MENU_ENGINE_SOENGINE        = 'ge08'    ,
        MSG_ENGINEMOVE              = 'ge09'    ,

		MSG_FULLSCREEN              = 'gf01'    ,
		// Game - Menu
		MENU_GAME_NEWGAME           = 'gg01'    ,
        MENU_GAME_MOVENOW           = 'gg02'    ,
        MSG_GOTO_MOVE               = 'gg03'    ,
		// Help - Menu
        MENU_HELP_MOVEASSISTANT     = 'gh01'    ,
		MENU_HELP_HINT              = 'gh02'    ,
		MENU_HELP_HELP              = 'gh03'    ,
		MENU_HELP_ABOUT             = 'gh04'    ,

        ICS_ASKS_PASSWORD           = 'gi01'    ,
        ICS_ASKS_USERNAME           = 'gi02'    ,
        ICS_DISCONNECTED            = 'gi03'    ,
        ICS_FORFEIT_BY_DISCONNECTION= 'gi04'    ,
        ICS_FORFEIT_ON_TIME         = 'gi05'    ,
        ICS_GAME_ABORTED            = 'gi06'    ,
        ICS_GAME_BRACKET            = 'gi07'    ,
        ICS_GAME_DRAWN              = 'gi08'    ,
        ICS_GAME_INFO               = 'gi09'    ,
        ICS_LOGGED_IN               = 'gi10'    ,
        ICS_MADE_MOVE               = 'gi11'    ,
        ICS_NEW_GAME                = 'gi12'    ,
        ICS_RESIGN                  = 'gi13'    ,
        MSG_ICS_MOVE_DONE           = 'gi14'    ,
        ICS_NOTPLAYING              = 'gi15'    ,
        ICS_OFFERED_GAMES           = 'gi16'    ,
        ICS_OFFERED_INFO            = 'gi17'    ,
        ICS_OFFERED_SEEK            = 'gi18'    ,
        ICS_OUTPUT                  = 'gi19'    ,
        ICS_PEND_INFO               = 'gi20'    ,
        ICS_PING_INFO               = 'gi21'    ,
        ICS_PLAYER_ADD              = 'gi22'    ,
        ICS_PLAYER_REMOVE           = 'gi23'    ,
        ICS_SEEK_ADD                = 'gi24'    ,
        ICS_SEEK_CLEAR              = 'gi25'    ,
        ICS_SEEK_REMOVE             = 'gi26'    ,
        MSG_SEND_ICS_CMD            = 'gi27'    ,
        ICS_SHOUT                   = 'gi28'    ,
        ICS_START_GAME              = 'gi29'    ,
        ICS_TELL                    = 'gi30'    ,
        ICS_USER_CMD_RESPONSE       = 'gi31'    ,

        MSG_LOCK                    = 'gl01'    ,

        MSG_MOVEFIRST               = 'gm01'    ,
        MSG_MOVEBACK                = 'gm02'    ,
        MSG_MOVENEXT                = 'gm03'    ,
        MSG_MOVELAST                = 'gm04'    ,

		MSG_NEWGAME                 = 'gn01'    ,

        MSG_PROMOTE                 = 'gp01'    ,

        MSG_QUIT                    = 'gq01'    ,
        //reply - code
        REPLYCODE_ENGINE            = 'gr01'    ,
        REPLYCODE_TIMEVIEW          = 'gr02'    ,
        REPLYCODE_ICS               = 'gr03'    ,

        MENU_SERVER_CONNECT         = 'gs01'    ,
        MSG_SKILLCHANGED            = 'gs02'    ,

        MSG_UNLOCK                  = 'gu01'    ,
		USER_MADE_MOVE              = 'gu02'    ,
		// View-Menu
        MENU_VIEW_ALWAYSONTOP       = 'gv01'    ,
		MENU_VIEW_MININTERFACE      = 'gv02'    ,
        MENU_VIEW_FULLSCREEN        = 'gv03'    ,
		MENU_VIEW_SIDEBAR           = 'gv04'    ,
		MENU_VIEW_AUTOHIDING        = 'gv05'    ,
        MENU_VIEW_SHOWCLOCK         = 'gv06'    ,
		MENU_VIEW_SHOWMENU          = 'gv07'    ,
		MENU_VIEW_SHOWBORDER        = 'gv08'    ,
        MENU_VIEW_FLIPBOARD         = 'gv09'    ,
        MENU_VIEW_3D                = 'gv10'    ,
        MENU_VIEW_SHOW_WINDOW       = 'gv11'    ,

        MSG_WINDOW_CLOSED           = 'gw01'    ,
	};


void	SendICS(BString str, BLooper* looper);

#endif
