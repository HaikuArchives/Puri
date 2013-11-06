/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef MOVESVIEW_H
#define MOVESVIEW_H

#include <vector>

#include <GroupView.h>
#include <ScrollView.h>
#include <View.h>

#include "Debug.h"
#include "Tuple.h"

class MovesListView : public BView
{
public:
            MovesListView(int32 replyCode = 0);

    void    MoveFirst(void);
    void    MoveBack(void);
    void    MoveNext(void);
    void    MoveLast(void);
    void    GotoMove(uint idx);
    void    Clear(void);
    void    ScrollToCoord(uint coordY, uint coordX = 1);
    void    AddMove(BString const& move, bool addWhite = true);

    void SetReplyCode(int32 replyCode)
    {
        fReplyCode = replyCode;
        _NotifyMaxRange();
    }

    void SetReplyTarget(BHandler* target)
    {
        fTarget = target;
        _NotifyMaxRange();
    }

    virtual void    Draw(BRect updateRect);
    virtual	void    FrameResized(float newWidth, float newHeight);
    virtual	void	MouseDown(BPoint where);
    virtual	void	KeyDown(const char* bytes, int32 numBytes);

    friend class ScrollView;

private:
    std::pair<BBitmap*, const char*>   _SplitMove(BString moveStr,
                                                          bool isWhite = true);
    void                    _LoadImages(void);
    void                    _Highlight(BPoint const& where);
    void                    _NotifyMaxRange(void);
    void                    _AdaptScrollBars(void);
    uint                    _VisibleRows();
    uint                    _HiddenRows();
    uint32                  _MaxRange();
    float                   _ScrollStep(void);
    float                   _SizeProportion(void);

    Tuple<uint>             _ToCoord(BPoint const& where);

    std::vector<BString>    fWhiteMoves;
    std::vector<BString>    fBlackMoves;

    float                   fRowSpace;
    float                   fTotalFH;
    BFont                   fFont;
    BRect                   fMargin;
    uint32                  fMaxRange;
    font_height             fFH;
    float                   fNumWidth;
    float                   fMovesWidth;
    float                   fULSpace;
    rgb_color               fHColor;
    BPoint                  fHCoord;
    int32                   fReplyCode;
    BHandler*               fTarget;
    uint                    fVisMin;
    uint                    fVisMax;
    BBitmap*                fBM[12];
    BString                 fImageName[12];
    BRect                   fIR;

    Debug                   out;

    enum{wK = 0, wQ, wR, wB, wN, wP, bK, bQ, bR, bB, bN, bP };
};


class ScrollView : public BScrollView
{
public:
                    ScrollView(const char* name, BView* target,
                                bool vertical = true, bool horizontal = false,
                                uint32 flags = B_SUPPORTS_LAYOUT
                                    | B_FULL_UPDATE_ON_RESIZE,
                                uint32 resizingMode = B_FOLLOW_ALL_SIDES,
                                border_style border = B_FANCY_BORDER);

    virtual void    MessageReceived( BMessage* message );

    enum {M_REPLY = 'irm1'};

private:
    MovesListView*      fTarget;
};


class MovesView : public BGroupView
{
public:
            MovesView();

    void    Clear(void )	{ fMovesList->Clear(); }
    void    AddMove( BString const& str, bool addWhite = true )
    {
        fMovesList->AddMove( str, addWhite );
    }

    void    MoveFirst(void)    {   fMovesList->MoveFirst();    }
    void    MoveBack(void)     {   fMovesList->MoveBack();     }
    void    MoveNext(void)     {   fMovesList->MoveNext();     }
    void    MoveLast(void)     {   fMovesList->MoveLast();     }
    void    GotoMove(uint idx)
    {
        fMovesList->GotoMove( idx );
    }

private:
    MovesListView*	fMovesList;
    ScrollView*     fScrollMovesList;
};

#endif
