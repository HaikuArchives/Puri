/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "MovesView.h"

#include <Button.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>

#include "Messages.h"
#include "Tools.h"

MovesView::MovesView(void)
    :
    BGroupView(B_VERTICAL)
{

    fMovesList = new MovesListView;

    fScrollMovesList = new ScrollView("test_view", fMovesList, true, false);

	BLayoutBuilder::Group<>(this)
		.AddGroup(B_HORIZONTAL, 0)
			.Add(fScrollMovesList)
		.End()
    .End()
	;
    //Invalidate();
};



MovesListView::MovesListView(int32 replyCode)
    :
    BView("moves_list_view", B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE
    	| B_FRAME_EVENTS),
    fReplyCode(replyCode),
    fTarget(NULL)

{
        // init the array with NULL pointers;
    std::fill(&fBM[0], &fBM[12], static_cast<BBitmap*>(NULL));

    //TODO: Direct assignment should work, bug of gnu
    BString imageName[12] = {"wK", "wQ", "wR", "wB", "wN", "wP", "bK", "bQ",
             	"bR", "bB", "bN", "bP"};
    std::copy(&imageName[0], &imageName[12], &fImageName[0]);
    SetViewColor(B_TRANSPARENT_32_BIT);
    SetDrawingMode(B_OP_ALPHA);
    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);

    fRowSpace = 5;
    fHColor = {255, 255, 100, 255};
    fHCoord = BPoint(2, 0);
    GetFont( &fFont );
    //fFont.SetSize(30);
    //SetFont(&fFont);
    fFont.GetHeight(&fFH);
    fMargin = BRect(2, 2, 2, 2);
    fULSpace = 2;
    fTotalFH = fRowSpace + fFH.ascent + fFH.descent + fFH.leading + fULSpace;
    fVisMin = 1;
    fVisMax = fVisMin + _VisibleRows();
    fIR = BRect(0,0, fTotalFH, fTotalFH);

    _LoadImages();
    Invalidate();
}


void
MovesListView::Clear(void)
{
    fWhiteMoves.clear();
    fBlackMoves.clear();
    fHCoord.x = 2;
    fHCoord.y = 0;

    _NotifyMaxRange();
}


void
MovesListView::AddMove(BString const& move, bool addWhite)
{
    if (move.Length() == 0)
        return;

    switch (static_cast<int>(fHCoord.x)) {
        case 1:
            fWhiteMoves.resize(fHCoord.y);
            fBlackMoves.resize(fHCoord.y - 1);
            fHCoord.x = 2;
            fBlackMoves.push_back(move);
            break;

        case 2:
            fWhiteMoves.resize(fHCoord.y);
            fBlackMoves.resize(fHCoord.y);
            fHCoord.x = 1;
            fHCoord.y += 1;
            fWhiteMoves.push_back(move);
            break;

        default:
            break;
    }

    _NotifyMaxRange();
}


void
MovesListView::Draw(BRect updateRect)
{
    // Calculate dimensions
    BString tempStr;
    float   tempY;
    float   offsetY= fFH.ascent + fFH.leading + fMargin.top + fRowSpace;
    float   offsetX;

    tempStr = "";
    tempStr<< fWhiteMoves.size() << ".";

    fNumWidth = fFont.StringWidth(tempStr.String());

    uint frameWidth = Frame().Width();

    fMovesWidth = (frameWidth - fNumWidth -  3*fMargin.left - fMargin.right )/2;

    // Start Drawing
    SetHighColor(255, 255, 255, 255);
    FillRect(Bounds());

    _Highlight(fHCoord);

    std::pair<BBitmap*, const char*> move;

    //TODO: test: rewrite, and draw only relevant moves, not all
    for (uint i = 0; i < fWhiteMoves.size(); ++i) {
        SetHighColor(0, 0, 0, 255);

        tempStr = "";
        tempStr << i + 1 << ". ";
        tempY = offsetY + i*fTotalFH;
        offsetX = fMargin.left;
        DrawString(tempStr.String(), BPoint(offsetX, tempY));
        offsetX += fMargin.left + fNumWidth;
        move = _SplitMove(fWhiteMoves[i], true);

        if (move.first == NULL) {
            DrawString(move.second, BPoint(offsetX + fIR.Width()*0.3, tempY));
        } else {
            DrawBitmapAsync(move.first, fIR.OffsetToSelf(offsetX,
                                       tempY - fTotalFH + fRowSpace).LeftTop());
            DrawString(move.second, BPoint(offsetX + fIR.Width(), tempY));
        }

        offsetX += fMovesWidth + fMargin.left;

        if (i < fBlackMoves.size()) {
            move = _SplitMove(fBlackMoves[i], false);

            if (move.first == NULL) {
                DrawString(move.second, BPoint(
                                             offsetX + fIR.Width()*0.3, tempY));
            } else {
                DrawBitmapAsync(move.first, fIR.OffsetToSelf(offsetX,
                                       tempY - fTotalFH + fRowSpace).LeftTop());
                DrawString(move.second, BPoint(offsetX + fIR.Width(), tempY));
            }
        }

        tempY += fFH.descent + fULSpace;
        SetHighColor(200, 200, 200, 255);
        StrokeLine(BPoint(fMargin.left,tempY),
                                     BPoint(frameWidth - fMargin.right, tempY));
    }

    Sync();
}


void
MovesListView::FrameResized(float newWidth, float newHeight)
{
    _NotifyMaxRange();
    BView::FrameResized( newWidth, newHeight);
}

void
MovesListView::_Highlight(BPoint const& where)
{
    if (where.x < 1 || where.y < 1 || where.x > 2 || where.y > fWhiteMoves.size())
        return;

    BRect rect;

    rect.left = where.x*fMargin.left + fNumWidth + (where.x - 1)*fMovesWidth;
    rect.right = rect.left + fMovesWidth;

    rect.top    = fMargin.top + (where.y  - 1)*fTotalFH;
    rect.bottom = rect.top + fTotalFH;

    PushState();
    SetHighColor(fHColor);
    FillRect(rect);
    PopState();
}


Tuple<uint>
MovesListView::_ToCoord(BPoint const& where)
{
    Tuple<uint> coord;

    coord.y = ceil((where.y - fMargin.top)/fTotalFH);

    if (where.x > fNumWidth + fMovesWidth)
        coord.x = 2;
    else
        coord.x = 1;

    if (coord.y > fBlackMoves.size()) {
        if (coord.y > fWhiteMoves.size() || coord.x == 2) {
            coord.x = 0;
            coord.y = 0;
            return coord;
        } else {
            coord.y = fWhiteMoves.size();
            coord.x = 1;
        }
    }

    return coord;
}


void
MovesListView::MouseDown(BPoint where)
{
    MakeFocus(true);
    Tuple<int> coord = _ToCoord(where);

    if (coord.y == 0)
        return;

    BMessage msg(MSG_GOTO_MOVE);
    msg.AddUInt32("move_idx", (coord.y - 1) *2 + coord.x);
    Window()->PostMessage(&msg);
    BView::MouseDown(where);
}


uint
MovesListView::_VisibleRows(void)
{
    uint32 visibleRows = (Frame().Height() - fMargin.top -
                                                      fMargin.bottom)/fTotalFH ;

    if (visibleRows > fWhiteMoves.size())
        visibleRows = fWhiteMoves.size();

    return (--visibleRows);
}


uint
MovesListView::_HiddenRows(void)
{
    uint hiddenrows = fWhiteMoves.size() - _VisibleRows();

    return hiddenrows;
}


float
MovesListView::_ScrollStep(void)
{
    return fTotalFH;
}


uint32
MovesListView::_MaxRange(void)
{
    int32 maxRange;
    maxRange = fMargin.top + fMargin.bottom +
                                 fWhiteMoves.size()*fTotalFH - Frame().Height();

    if (maxRange < 0 || fWhiteMoves.size() == 0)
        maxRange = 0;

    return maxRange;
}


float
MovesListView::_SizeProportion(void)
{
    return (Frame().Height()/(fMargin.top + fMargin.bottom +
                                                  fWhiteMoves.size()*fTotalFH));
}


void
MovesListView::_NotifyMaxRange(void)
{
    ScrollBar(B_VERTICAL)->SetRange(0, _MaxRange());
    ScrollBar(B_VERTICAL)->SetProportion(_SizeProportion());
    ScrollBar(B_VERTICAL)->SetSteps(_ScrollStep(), _ScrollStep());

    _AdaptScrollBars();
}


void
MovesListView::KeyDown(const char* bytes, int32 numBytes)
{
    if (numBytes == 1) {
		switch (bytes[0]) {
            case B_UP_ARROW:
			case B_LEFT_ARROW:
                Window()->PostMessage(MSG_MOVEBACK);
                break;

            case B_DOWN_ARROW:
            case B_RIGHT_ARROW:
                Window()->PostMessage(MSG_MOVENEXT);
                break;

            case B_SPACE:
                Window()->PostMessage(MENU_GAME_MOVENOW);
                break;
/*
            case B_UP_ARROW:
                if (fHCoord.y > 1)
                    fHCoord.y -= 1;

                break;

            case B_DOWN_ARROW:
                if (fHCoord.y < fWhiteMoves.size())
                    fHCoord.y += 1;

                break;
*/
            default:
                break;
        }
    }

     _AdaptScrollBars();
    BView::KeyDown(bytes, numBytes);
}


void
MovesListView::_AdaptScrollBars(void)
{
    uint minY = _ToCoord(BPoint(0, Bounds().top)).y;
    uint maxY = _ToCoord(BPoint(0, Bounds().bottom)).y;


    if (fHCoord.y <= minY) {
        fVisMin = fHCoord.y;
        ScrollToCoord(fVisMin);
    } else if (fHCoord.y >= maxY) {
        fVisMax = fHCoord.y;
        ScrollToCoord(fVisMax - _VisibleRows());
    } else if (maxY + 1 >= fWhiteMoves.size()) {
        float valuemin, valuemax;

        ScrollBar(B_VERTICAL)->GetRange(&valuemin, &valuemax);
        ScrollBar(B_VERTICAL)->SetValue(valuemax);
    }

    Invalidate();
}


void
MovesListView::ScrollToCoord(uint coordY, uint coordX)
{
    if (coordX < 1 || coordY < 1)
        return;

    ScrollTo(0, fMargin.top + (coordY - 1)*fTotalFH);
}


void
MovesListView::_LoadImages(void)
{
    for (int i = 0; i < 12; ++i) {
        delete fBM[i];
        fBM[i] = LoadBitmap(fImageName[i], fIR.Width());
    }

}


std::pair<BBitmap*, const char*>
MovesListView::_SplitMove(BString moveStr, bool isWhite)
{
    std::pair<BBitmap*, const char*> move;

    BString dest;

    uint idx = 0;

    switch (moveStr[0]) {
        case 'K':   idx = wK;   moveStr.MoveInto(dest, 0, 1);  break;
        case 'Q':   idx = wQ;   moveStr.MoveInto(dest, 0, 1);  break;
        case 'R':   idx = wR;   moveStr.MoveInto(dest, 0, 1);  break;
        case 'B':   idx = wB;   moveStr.MoveInto(dest, 0, 1);  break;
        case 'N':   idx = wN;   moveStr.MoveInto(dest, 0, 1);  break;
        case 'P':   idx = wP;   moveStr.MoveInto(dest, 0, 1);  break;
        case '0':
            move.first  = NULL;
            move.second = moveStr.String();
            return move;

        default:
            idx = wP;
            break;
    }

    if (isWhite == false)
        idx += 6;

    move.first = fBM[idx];

    move.second = moveStr.String();

    return move;
}


void
MovesListView::MoveFirst(void)
{
    fHCoord.x = 2;
    fHCoord.y = 0;
    _AdaptScrollBars();
}


void
MovesListView::MoveBack(void)
{
    if (fHCoord.x > 1 && fHCoord.y != 0) {
        fHCoord.x -= 1;
    } else if (fHCoord.y > 0) {
        fHCoord.x = 2;
        fHCoord.y -= 1;
    }

    _AdaptScrollBars();
}


void
MovesListView::MoveNext(void)
{
    if (fHCoord.x < 2) {
        if (fHCoord.y <= fBlackMoves.size())
            fHCoord.x += 1;
    } else if (fHCoord.y < fWhiteMoves.size()) {
        fHCoord.x  = 1;
        fHCoord.y += 1;
    }

    _AdaptScrollBars();
}


void
MovesListView::MoveLast(void)
{
    fHCoord.y = fWhiteMoves.size();

    if (fWhiteMoves.size() == fBlackMoves.size())
        fHCoord.x = 2;
    else
        fHCoord.x = 1;

    _AdaptScrollBars();
}


void
MovesListView::GotoMove(uint idx)
{
    if (idx > fWhiteMoves.size() + fBlackMoves.size()) {
        GotoMove( fWhiteMoves.size() + fBlackMoves.size() );
        return;
    }

    fHCoord.y = (idx + 1)/2 ;

    if (idx%2 == 1)
        fHCoord.x = 1;
    else
        fHCoord.x = 2;

    _AdaptScrollBars();
}


ScrollView::ScrollView(const char* name, BView* target, bool vertical,
                        bool horizontal, uint32 flags, uint32 resizingMode,
                        border_style border)
    :
    BScrollView(name, target,resizingMode, flags, horizontal,vertical, border)
{
    fTarget = static_cast<MovesListView*>(target);

    ScrollBar(B_VERTICAL)->SetRange(0, 0);
    fTarget->SetReplyTarget(this);
    fTarget->SetReplyCode(M_REPLY);
}


void
ScrollView::MessageReceived(BMessage* message)
{
    switch (message->what) {
		case M_REPLY:
        {
            uint32  maxRange;
			message->FindUInt32("maxRange", &maxRange);
            ScrollBar(B_VERTICAL)->SetRange(0, maxRange);
            ScrollBar(B_VERTICAL)->SetProportion(fTarget->_SizeProportion());

            ScrollBar(B_VERTICAL)->SetSteps(fTarget->_ScrollStep(),
                                                    fTarget->_ScrollStep());
			break;
        }

		default:
			break;
	}
}
