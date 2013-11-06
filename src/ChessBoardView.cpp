/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "ChessBoardView.h"

#include <iostream>
#include <algorithm>
#include <array>

#include <IconUtils.h>
#include <String.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <TranslationKit.h>

#include "Messages.h"
#include "ModelPath.h"
#include "Tools.h"
#include "PngLoad.h"
#include "PromotionWindow.h"

using namespace std;

ChessBoardView::ChessBoardView(BRect const& frame)
	:
    BGLView(frame, "ChessBoardView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW |
                   B_NAVIGABLE, BGL_RGB |BGL_DOUBLE | BGL_DEPTH | BGL_ALPHA),
    PI(3.14),
    fDragPiece2dBM(NULL),
    fBackgroundBM(NULL),
    fBoard2dBM(NULL),
    fNullModel(NULL),
    fNoMove(100, 100),
    fHCoord2D(fNoMove),
    fMoveFrom(fNoMove),
    fIR(0, 0, 0, 0),
    fOldMousePos(0, 0),
    fGrabEffect(3, 3),
    fGrabDiff(0, 0),
    fMouseButtons(0),
    fShowValidMoves(false),
    fBoardIsTurned(false),
    fUserCanMove(true),
    fDrawPieces(true),
    fIsDragging(false),
    fPlaySound(true),
    f3DLoaded(false),
    fIs3D(false)
{
    SetMouseEventMask(EventMask(), B_LOCK_WINDOW_FOCUS);

    fDefaultCursor  = new BCursor(B_CURSOR_ID_SYSTEM_DEFAULT);
    fGrabCursor     = new BCursor(B_CURSOR_ID_GRABBING);

    fAppPath = GetAppPath().Append("/");

    BString tempPath = fAppPath;
    tempPath<<"data/Sounds/Board/";


    //BString sounds[6] = {"newgame.ogg", "move.ogg", "castle.ogg",
     //                                "capture.ogg", "illegal.ogg", "gong.ogg"};
                                     
   	array<BString, 6> sounds = {{"newgame.ogg", "move.ogg", "castle.ogg",
                                     "capture.ogg", "illegal.ogg", "gong.ogg"}};

    for (unsigned int i = 0; i < sounds.size(); ++i) {
        fSound.push_back( new BSimpleGameSound(
                                        BString(tempPath).Append(sounds[i]) ) );

        if (fSound.back()->InitCheck() != B_OK)
            fPlaySound = false;
    }

    if (fPlaySound == false)
    	!out << "NO SOUND" << std::endl;
   	else
        fSound[1]->StartPlaying();

    // init the array with NULL pointers;
    std::fill(&fBM[0], &fBM[12], static_cast< BBitmap* >(NULL));
    //TODO: Direct assignment should work, bug of gnu
    BString imageName[12] = {"wK", "wQ", "wR", "wB", "wN", "wP",
                                           "bK", "bQ", "bR", "bB", "bN", "bP"};
    std::copy(&imageName[0], &imageName[12], &fImageName[0]);

	fWidth	= frame.right - frame.left;
	fHeight	= frame.bottom - frame.top;

	fDistance = 30; fTwist = 0; fElevation = 30; fAzimuth = -15;
	fLookToX = 0; fLookToY = 0; fLookToZ = 0;

    fBoard = new BackBoard();

	for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
        _Board(i, j) = NULL;

    for (int i = 0; i < 12; ++i)
        f3DPieces[i]= NULL;

    fBoardModel = NULL;


    SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
  //  NewGame();
    SetViewMode3D(fIs3D);

    fBoard2dBM = BTranslationUtils::GetBitmapFile(
                   GetAppPath().Append("/data/Texture/Board/board_simple.png"));
    fBackgroundBM = NULL;
}


void
ChessBoardView::_Load3DModels(void)
{
    if (f3DLoaded)
        return;

    fBoardModel = new Model3DS(GetAppPath().Append(BOARD_PATH), true);
	fBoardModel->EnableTexture(true);

    int mypieces[12]={K_W, Q_W, R_W, B_W, N_W, P_W, K_B, Q_B, R_B, B_B, N_B,
                                                                           P_B};

    for (int i = 0; i < 12; ++i)
        f3DPieces[i]= new Model3DS(mypieces[i]);

    f3DLoaded = true;
}


ChessBoardView::~ChessBoardView()
{
    for (int i = 0; i < 12; ++i) {
        delete f3DPieces[i];
        delete fBM[i];
    }

    std::vector< BSimpleGameSound* >::const_iterator itr = fSound.begin();

    for ( ;itr != fSound.end(); ++itr)
        delete *itr;

    delete fBoardModel;
    delete fNullModel;
    delete fBoard;
    delete fBoard2dBM;
    delete fDragPiece2dBM;
    delete fBackgroundBM;
    delete fDefaultCursor;
    delete fGrabCursor;
}


void
ChessBoardView::AttachedToWindow(void)
{
	LockGL();
		BGLView::AttachedToWindow();
		_Init();
		_Reshape(fWidth, fHeight);
	UnlockGL();
}


void
ChessBoardView::AllAttached( void )
{
    MakeFocus(true);
    BGLView::AllAttached();
}


void
ChessBoardView::FrameResized(float newWidth, float newHeight)
{
    if (fIs3D) {
        LockGL();
            fWidth  = newWidth;
            fHeight = newHeight;
            BGLView :: FrameResized( fWidth, fHeight );
            _Reshape( fWidth, fHeight );
        UnlockGL();

        Render();
    } else {
        float width = std::min(newWidth, newHeight);
        width = width / 8;

        fIR = BRect(0, 0, width, width);

        if (newHeight < newWidth)
            fBoard2dOffset = BPoint((newWidth-newHeight)/2, 0.0f);
        else
            fBoard2dOffset = BPoint(0, (newHeight-newWidth)/2);

        _LoadImages();
        fArrows.clear();
            //Replace with Recalculation instead of clearing
        Render();
    }
}


void
ChessBoardView::Draw(BRect updateRect)
{
    if (fIs3D) {
        Render();
        return;
    }

    if (fIsDragging) {
        SetDrawingMode(B_OP_COPY);
        DrawBitmapAsync(fBackgroundBM, BPoint(0,0));

        BPoint pos;
        GetMouse(&pos, NULL, false);
        pos -= fGrabDiff;
        SetDrawingMode(B_OP_ALPHA);

        if (fHCoord2D != fNoMove) {
            fIR.OffsetTo(fHCoord2D.x*fIR.Width(), fHCoord2D.y*fIR.Width());
            fIR.OffsetBySelf(fBoard2dOffset);
            SetHighColor(255, 255, 0, 60);
            FillRect(fIR.OffsetBySelf(1,1));
        }

        DrawBitmapAsync(fDragPiece2dBM, pos);
    }
    else {
        _DrawInView(this, updateRect);
    }

    Sync();
}


//Todo: in 2D mode don't redraw all screen while dragging a piece.
void
ChessBoardView::_DrawInView(BView* view, BRect updateRect)
{
    view->SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    view->FillRect(Bounds());
    fIR.OffsetTo(fBoard2dOffset);
    BRect boardbounds = BRect(fBoard2dOffset.x, fBoard2dOffset.y,
                                fIR.Width()*8 + fBoard2dOffset.x,
                                fIR.Width()*8 + fBoard2dOffset.y);
    view->DrawBitmapAsync(fBoard2dBM, boardbounds);
    view->SetHighColor(0, 0, 0, 255);
    view->StrokeRect(boardbounds);
    Tuple<int> coord;

    if (fBoardIsTurned) {
        if (fShowValidMoves) {
            view->SetHighColor(0, 255, 0, 30);
            std::list<Move>::const_iterator itr= fValidMoves.begin();

            for ( ; itr != fValidMoves.end(); ++itr) {
                fIR.OffsetTo( itr->ToX()*fIR.Width() + 1,
                                                itr->ToY()*fIR.Width() + 1);
                fIR.OffsetBySelf(fBoard2dOffset);
                view->FillRect(fIR);
            }
        }

        fIR.OffsetTo(fBoard2dOffset);

        for (coord.y = 0 ; coord.y < 8; ++coord.y) {
            fIR.OffsetBySelf(-fIR.Width() ,0);

            for (coord.x = 0; coord.x < 8; ++coord.x) {
                fIR.OffsetBySelf(fIR.Width() ,0);

                if (coord != fMoveFrom)
                    view->DrawBitmapAsync(_ImageBoard(coord), fIR.LeftTop());
            }

            fIR.OffsetBySelf(0 ,fIR.Width());
            fIR.OffsetTo(fBoard2dOffset.x, fIR.top);
        }
    } else {
        if (fShowValidMoves) {
            view->SetHighColor(0, 255, 0, 50);
            std::list<Move>::const_iterator itr= fValidMoves.begin();

            for ( ; itr != fValidMoves.end(); ++itr) {
                fIR.OffsetTo(itr->ToX()*fIR.Width() + 1,
                                         (7 - itr->ToY())*fIR.Width() + 1);
                fIR.OffsetBySelf(fBoard2dOffset);
                view->FillRect(fIR);
            }
        }

        fIR.OffsetTo(fBoard2dOffset);

        for (coord.y = 7 ; coord.y > -1; --coord.y) {
            fIR.OffsetBySelf(-fIR.Width() ,0);

            for (coord.x = 0; coord.x < 8; ++coord.x) {
                fIR.OffsetBySelf( fIR.Width() ,0);

                if (coord != fMoveFrom)
                    view->DrawBitmapAsync(_ImageBoard(coord), fIR.LeftTop());
            }

            fIR.OffsetBySelf(0 ,fIR.Width());
            fIR.OffsetTo(fBoard2dOffset.x, fIR.top);
        }
    }

    view->SetHighColor(255, 255, 0, 180);
    view->SetPenSize(fIR.Width()/15);
    //DRAW Arrows
    std::vector< std::vector<BPoint> >::const_iterator itr = fArrows.begin();

    for ( ;itr != fArrows.end(); ++itr) {
        view->StrokeLine((*itr)[0], (*itr)[1]);
        view->FillTriangle((*itr)[2], (*itr)[3],(*itr)[4]);
    }


    if (fIsDragging) {
        view->SetHighColor( 0, 0, 0, 100);
        view->SetPenSize(fIR.Width()/9 );
        view->StrokeArc(_CTP(fMoveFrom) + BPoint(fIR.Width()/2,
                fIR.Width()/2), fIR.Width()*0.4, fIR.Width()*0.4, 360, 360);
    }

    view->Sync();
}


void
ChessBoardView::WindowActivated(bool active)
{
	BGLView::WindowActivated( active );
}


void
ChessBoardView::ErrorCallback(GLenum which)
{
	!out << "ChessBoardView::ErrorCallback: " << which << std::endl;
}


inline void
ChessBoardView::Render(void)
{
    if (fIs3D) {
        LockGL();
            _Draw();
            SwapBuffers();
        UnlockGL();
    } else {
        if (fIsDragging) {
            delete fBackgroundBM;
            fBackgroundBM = _CreateBitmapBuffer();
            Invalidate();
        }

        Invalidate();
    }
}


void
ChessBoardView::_Init(void)
{
    SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	// Enable lighting and set the position of the light
	GLfloat light_position[] 	= {2, 2, 10, 0};
	//GLfloat	white_light[]		= { 0.4, 0.4, 0.4, 1.0 };
    GLfloat	white_light[]		= {0.99, 0.99, 0.99, 1.0};
	GLfloat	lmodel_ambient[]	= {0.4, 0.4, 0.4, 1.0};

	glClearColor(0.85, 0.85, 0.85, 1.0);
	//glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);

	//glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 999.999 );

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_RESCALE_NORMAL);
	glutInitDisplayMode(GLUT_DEPTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glEnable( GL_BLEND );
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE );

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_POLYGON_SMOOTH_HINT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}


inline void
ChessBoardView::_Draw(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
   _PolarView();
    glTranslated(-fLookToX, -fLookToY, -fLookToZ);

    fBoardModel->Draw();

	if (fDrawPieces)
		for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j)
			if (_Board(i, j) != NULL ) {
                _Board(i, j)->SetVisible(true);
                _Board(i, j)->Draw();
			}

	glFlush();
}


inline void
ChessBoardView::_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,	 							//camera angle
                   (double)width / (double)height,		//width-to-height ratio
                   1.0f,								//near clipping coordinate
                   200.0f); 							//far clipping coordinate

    //additional
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void
ChessBoardView::_DrawArrow(Tuple<uint> const& kFrom, Tuple<uint> const& kTo)
{

}


void
ChessBoardView::_NotifyState(void)
{
    int32 state = fBoard->State();

    if (state != 0) {
        BMessage msg(MSG_BOARDSTATE);
        msg.AddInt32("val", state);
        Window()->PostMessage(&msg);
    }
}


bool
ChessBoardView::UserMoveFromTo(Tuple<int> kFrom, Tuple<int> kTo)
{
    if (fUserCanMove == false) {
        Render();
        return false;
    }

    fValidMoves = _GetValidMoves(kFrom);

    std::list<Move>::iterator itr= fValidMoves.begin();

    for ( ; itr != fValidMoves.end(); ++itr)
        if (itr->From() == kFrom && itr->To() == kTo)
            break;

    if (itr == fValidMoves.end())
        return false;

	MoveFromTo(*itr);

    if (itr->HasFlag(PROM))
        new PromotionWindow(this);
    else {
        BMessage msg(USER_MADE_MOVE);
        msg.AddString("move", fBoard->LastMoveString());
        Window()->PostMessage(&msg);
    }

    Render();
    _NotifyState();
    return true;
}


Move
ChessBoardView::_StrToMove(BString moveStr)
{
    Move move;

    if (moveStr.Length() < 4)
        return move;

    move.Set(moveStr[0] - 'a', moveStr[1] - '1',
                                          moveStr[2] - 'a', moveStr[3] - '1');

    if ((_BBoard(move.From()) & King) == King && move.DeltaX() == 2)
        move.SetFlag(CASTLE);
    else if((_BBoard(move.From()) & Pawn) == Pawn && move.DeltaX() != 0
        && _BBoard(move.To()) == Empty)
        move.SetFlag(ENP);
    else if (moveStr.Length() > 4) {
        int flag;
        switch (moveStr[4]) {
            case 'q': case 'Q': flag = Queen ; break;
            case 'r': case 'R': flag = Rook  ; break;
            case 'b' : case 'B': flag = Bishop; break;
            case 'n': case 'N': flag = Knight; break;
            default: return move;
        }

        move.SetFlag(PROM);

        if (fBoard->IsWhiteTurn())
            move.AddFlag((flag|WhiteFlag));
        else
            move.AddFlag((flag|BlackFlag));
    }

    return move;
}


bool
ChessBoardView::DoICSMove(BString moveStr)
{
    moveStr.RemoveSet("nrqkpCE ");
    moveStr.ReplaceAll("b", "", 4);
    moveStr.ReplaceAll("c", "", 4);

    moveStr.ToLower();

    Move move = _StrToMove(moveStr);

    if (MoveFromTo(move) == false)
        return false;

    BMessage msg(MSG_ICS_MOVE_DONE);
    msg.AddString("move", fBoard->LastMoveString());
    Window()->PostMessage(&msg);

    if (fIs3D) {
        if (fMouseButtons == 0)
            Render();
    } else {
        fArrows.clear();
        fArrows.push_back(_CreateArrow2D(move.From(), move.To()));
        Render();
    }

    _NotifyState();
    return true;
}


bool
ChessBoardView::EngineMoveFromTo(BString moveStr)
{
    if (moveStr == "(none") {
        _NotifyState();
        return false;
    }

    Move move = _StrToMove(moveStr);

    if (MoveFromTo(move) == false) {
        return false;
    }

    BMessage msg(ENGINE_MADE_MOVE);
    msg.AddString("move", fBoard->LastMoveString());
    Window()->PostMessage(&msg);

    if (fIs3D) {
        if (fMouseButtons == 0)
            Render();
    } else {
        fArrows.clear();
        fArrows.push_back(_CreateArrow2D(move.From(), move.To()));
        Render();
    }

    _NotifyState();
    return true;
}


bool
ChessBoardView::MoveFromTo(Move move)
{
    if (fBoard->IsLegal(move+2) == false)
        return false;

    if (fIs3D) {
        delete _Board(move.To());

        _Board(move.To()) = _Board(move.From());
        _Board(move.To())-> SetCoordinate(move.To());
        _Board(move.From()) = NULL;
    }

	fBoard->DoMove(move + 2);

    if (move.HasFlag())
        _InitBoardFromBackBoard();

	return true;
}


void
ChessBoardView::_HighlightOnlyCoord(int x, int y)
{
    for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
        if (_Board(i, j) != NULL)
            _Board(i, j)->SetHighlight(false);

    if (_Board(x, y) != NULL)
        _Board(x, y)->SetHighlight(true);
}


void
ChessBoardView::MouseDown(BPoint where)
{
    MakeFocus(true);
    BPoint mousePos;
    uint32 mouseButtons;

    GetMouse(&mousePos, &mouseButtons);

    if (fIs3D) {
        be_app->SetCursor(fGrabCursor);

        if (/*mouseButtons == B_PRIMARY_MOUSE_BUTTON*/ true) {
            Tuple<int> coord = _ToCoord( _OGLPos( where ) );

            if (fMoveFrom != fNoMove) {
                _HighlightCoord(fMoveFrom, false);
                UserMoveFromTo(fMoveFrom, coord);
                fMoveFrom = fNoMove;
            } else if (_Board(coord) == NULL || _IsValidCoord(coord) == false) {
                GetMouse(&fOldMousePos, &fMouseButtons);
                fDrawPieces = false;
                return;
            } else {
                _HighlightOnlyCoord(coord);
                fMoveFrom = coord;
            }

            fValidMoves = _GetValidMoves(fMoveFrom);
        } else {
            GetMouse(&fOldMousePos, &fMouseButtons);
            fDrawPieces = false;
        }
    } else {
        mousePos -= fBoard2dOffset;

        Tuple<int> coord = _To2DCoord(where);

        if (fMoveFrom != fNoMove) {
            UserMoveFromTo(fMoveFrom, coord);
            fMoveFrom = fNoMove;
        } else if (_IsValidCoord(coord) && _BBoard(coord) != Empty) {
            be_app->SetCursor(fGrabCursor);
            fMoveFrom = coord;
            fDragPiece2dBM = _ImageBoard(fMoveFrom);

            if (fBoardIsTurned == false) coord.y = 7 - coord.y;

            fGrabDiff = mousePos - BPoint(coord.x*fIR.Width(),
                                                          coord.y*fIR.Width());
            fGrabDiff += fGrabEffect;

            fHCoord2D =  _To2DCoord(where - fGrabDiff +
                                           BPoint(fIR.Width()/2,fIR.Width()/2));

            if (fBoardIsTurned == false)
                fHCoord2D.y = 7 - fHCoord2D.y;

            fValidMoves = _GetValidMoves(fMoveFrom);
            delete fBackgroundBM;
            fBackgroundBM = _CreateBitmapBuffer();
            fIsDragging = true;
        }

        Render();
    }

	BGLView :: MouseDown(where);
}


BPoint
ChessBoardView::_OGLPos(BPoint const& where)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = where.x;
	winY = (float)viewport[3] - where.y;

	glReadPixels(winX, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	gluUnProject(winX, winY, winZ, modelview, projection, viewport,
                                                           &posX, &posY, &posZ);
	return BPoint(posX, posY);
}


void
ChessBoardView::_CancelSelection(void)
{
    fMouseButtons = 0;

    if (fIs3D)
        return;

    fMoveFrom = fNoMove;
    fValidMoves.clear();
    fIsDragging = false;
    Invalidate();
}


void
ChessBoardView::MakeFocus(bool focusState)
{
    BGLView::MakeFocus(focusState);

    if (fIsDragging == true) {
        be_app->SetCursor(fDefaultCursor);
        _CancelSelection();
    }
}


void
ChessBoardView::MouseUp(BPoint where)
{
    be_app->SetCursor(fDefaultCursor);

    BPoint mousePos;
    uint32 mouseButtons;

    GetMouse(&mousePos, &mouseButtons);

    if (fIs3D) {
        GetMouse(&where, &fMouseButtons);
        fDrawPieces = true;
        Render();
    } else {
        mousePos -= fBoard2dOffset;

        Tuple<int> coord = _To2DCoord(where - fGrabDiff +
                                           BPoint(fIR.Width()/2,fIR.Width()/2));

        if (fMoveFrom != fNoMove) {
            if (UserMoveFromTo(fMoveFrom, coord)) {
                fArrows.clear();
                fArrows.push_back(_CreateArrow2D(fMoveFrom,coord));
            }

            _CancelSelection();
        }
    }

    BGLView::MouseUp(where);
}



void
ChessBoardView::MouseMoved(BPoint where, uint32 code,
									                const BMessage* dragMessage)
{
    BGLView::MouseMoved(where, code, dragMessage);

	switch (code) {
		case B_EXITED_VIEW:
            _CancelSelection();
            break;


		case B_ENTERED_VIEW:
			MakeFocus(true);
			return;

		default:
            break;
	}

    if (fIs3D) {
        switch (fMouseButtons) {
            case B_PRIMARY_MOUSE_BUTTON:
            {
                where       -= fOldMousePos;
                fAzimuth	+= where.x / 50;
                fElevation	-= where.y / 50;

                _AdjustAzimuth();
                _AdjustElevation();
                Render();
                break;
            }

            case B_SECONDARY_MOUSE_BUTTON:
            {
                where     -= fOldMousePos;
                fDistance -= where.y / 100;
                _AdjustDistance();
                Render();
                break;
            }

            case B_PRIMARY_MOUSE_BUTTON | B_SECONDARY_MOUSE_BUTTON:
                // blend if a context-menu with entries like:
                // reset cameras (like pointing it again to the middle of the board)
                break;

            default:
                break;
        }
    } else {
        if (fMoveFrom != fNoMove) {
            fHCoord2D =  _To2DCoord(where - fGrabDiff +
                BPoint(fIR.Width()/2, fIR.Width()/2));

            if (fBoardIsTurned == false)
                fHCoord2D.y = 7 - fHCoord2D.y;

            Invalidate();
        }
    }
}


void
ChessBoardView::KeyDown(const char* bytes, int32 numBytes)
{
	if (numBytes == 1) {
		switch (bytes[0]) {
			case 'D':
				fAzimuth += 5;
				_AdjustAzimuth();
				break;

			case 'A':
				fAzimuth -= 5;
				_AdjustAzimuth();
				break;

			case 'W':
				fElevation += 5;
				_AdjustElevation();
				break;

			case 'S':
				fElevation -= 5;
				_AdjustElevation();
				break;

			case 'E':
				fDistance -= 0.5;
				_AdjustDistance();
				break;

			case 'Q':
				fDistance += 0.5;
				_AdjustDistance();
				break;

			case 'd':
    	     	fLookToX   += 0.2;
				break;

			case 'a':
       		  	fLookToX   -= 0.2;
				break;

			case 'w':
    	     	fLookToY   += 0.5;
				break;

			case 's':
        	 	fLookToY   -= 0.5;
				break;

			case 'e':
			case B_PAGE_UP:
				fLookToZ   += 0.2;;
				break;

			case 'q':
			case B_PAGE_DOWN:
				fLookToZ -= 0.2;
				break;

			case B_UP_ARROW: case B_LEFT_ARROW:
                Window()->PostMessage(MSG_MOVEBACK);
				break;

			case B_DOWN_ARROW: case B_RIGHT_ARROW:
                Window()->PostMessage(MSG_MOVENEXT);
                break;

            case B_SPACE:
                Window()->PostMessage(MENU_GAME_MOVENOW);
                break;

			case B_ENTER:
                break;

    	  	default:
				BGLView::KeyDown(bytes, numBytes);
       		  	break;
 		}
	}

	Render();
}


std::list<Move>
ChessBoardView::_GetValidMoves(Tuple<uint> kFrom)
{
	std::list<Move> moves = fBoard->GetValidMoves(kFrom + 2);

	for (std::list<Move>::iterator itr = moves.begin(); itr != moves.end(); ++itr)
		*itr -= 2;

    return moves;
}


void
ChessBoardView::_ShowPossibleMoves(Tuple<uint> kFrom)
{
	std::list<Move> moves = fBoard->GetValidMoves(kFrom + 2);
	std::list<Move>::const_iterator itr = moves.begin();

	for (; itr != moves.end(); ++itr)
		!out << itr->FromX() << "-" << itr->FromY()<< "    "
             << itr->ToX()<< "-" << itr->ToY() << std::endl;
}


void
ChessBoardView::KeyUp(const char* bytes, int32 numBytes)
{
	BGLView::KeyUp(bytes, numBytes);
}


void
ChessBoardView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_MOUSE_UP:
			!out<<"BoardView: Mouse Down Message"<< std::endl;
            break;

        case ICS_MADE_MOVE:
        {
            BString move = "";
            message->FindString("info", &move);
            DoICSMove(move);
            break;
        }

        case MSG_CANCEL_SELECTION:
            _CancelSelection();
            break;

        case MSG_ENGINEMOVE:
        {
            BString bestMove;
			message->FindString("info", &bestMove);
            EngineMoveFromTo(bestMove);
            break;
        }

        case MSG_PROMOTE:
        {
            int32 value = 0;
			message->FindInt32("val", &value);
            fBoard->Promote(value);
            BMessage msg(USER_MADE_MOVE);
            msg.AddString("move", fBoard->LastMoveString());
            Window()->PostMessage(&msg);

            if (fIs3D)
                _InitBoardFromBackBoard();

            Render();
            break;
        }

		default:
			BGLView::MessageReceived(message);
			break;
	}
}


inline void
ChessBoardView::_PolarView(void)
{
    glLoadIdentity();
	glTranslated(0.0, 0.0, -fDistance);
	glRotated(-fTwist, 0.0, 0.0, 1.0);
	glRotated(-fElevation, 1.0, 0.0, 0.0);
	glRotated(fAzimuth, 0.0, 0.0, 1.0);
}


void
ChessBoardView::_InitBoardFromBackBoard(void)
{
    if (fIs3D == false) {
        fArrows.clear();
        return;
    }

	for (int j = 0; j < 8; ++j)
    for (int i = 0; i < 8; ++i) {
		delete _Board(i, j);
        _Board(i, j) = NULL;

		if (_BBoard(i,j) != Empty) {
			_Board(i, j) = new Model3DS(*_3DModel(i, j));
			_Board(i, j)->SetCoordinate(i, j);
		}
	}
}


void
ChessBoardView::MoveFirst(void)
{
	fBoard->MoveFirst();

	//rewrite.  Store the board with pointers in a vector...
	//and use it to switch to other positions....,
	//and don't delete the Models, just hide them
	//and store a number, that indicates when the model should be hidden.

	_InitBoardFromBackBoard();
	Render();
}


void
ChessBoardView::MoveBack(void)
{
	fBoard->MoveBack();
	_InitBoardFromBackBoard();
	Render();
}


void
ChessBoardView::MoveNext(void)
{
	fBoard->MoveNext();
	_InitBoardFromBackBoard();
	Render();
}


void
ChessBoardView::MoveLast(void)
{
	fBoard->MoveLast();
	_InitBoardFromBackBoard();
	Render();
}


void
ChessBoardView::GotoMove(uint idx)
{
    fBoard->GotoMove(idx);
    _InitBoardFromBackBoard();
	Render();
}


void ChessBoardView::NewGame(void)
{
	fBoard->NewGame();
    fUserCanMove = true;
	_InitBoardFromBackBoard();
	Render();
}


void
ChessBoardView::ChangeViewMode(void)
{
    if (fIs3D)      SetViewMode3D(false);
    else            SetViewMode3D(true);
}


void
ChessBoardView::SetViewMode3D(bool mode)
{
    fIs3D = mode;

    if (fIs3D) {
        SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
        SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
        SetDrawingMode(B_OP_COPY);
        FillRect(Bounds());
        _Load3DModels();
        _InitBoardFromBackBoard();
    } else {
        SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
        SetViewColor(B_TRANSPARENT_32_BIT);
        SetDrawingMode(B_OP_ALPHA);
        SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    }

    FrameResized(Bounds().Width(), Bounds().Height());
}


void
ChessBoardView::_LoadImages(void)
{
    delete fBoard2dBM;
    fBoard2dBM = LoadBitmap("board", fIR.Width()*8);

    for (int i = 0; i < 12; ++i) {
        delete fBM[i];
        fBM[i] = LoadBitmap(fImageName[i], fIR.Width());
    }
}


BBitmap*
ChessBoardView::_ImageBoard(uint const& x, uint const& y)
{
    switch (_BBoard(x, y)) {
        case P_W: return fBM[wP];
        case N_W: return fBM[wN];
        case B_W: return fBM[wB];
        case R_W: return fBM[wR];
        case Q_W: return fBM[wQ];
        case K_W: return fBM[wK];
        case P_B: return fBM[bP];
        case N_B: return fBM[bN];
        case B_B: return fBM[bB];
        case R_B: return fBM[bR];
        case Q_B: return fBM[bQ];
        case K_B: return fBM[bK];
        default:
            return NULL;
    }
}


Model3DS* &
ChessBoardView::_3DModel(uint const& x, uint const& y)
{
    switch (_BBoard(x, y)) {
        case P_W: return f3DPieces[wP];
        case N_W: return f3DPieces[wN];
        case B_W: return f3DPieces[wB];
        case R_W: return f3DPieces[wR];
        case Q_W: return f3DPieces[wQ];
        case K_W: return f3DPieces[wK];
        case P_B: return f3DPieces[bP];
        case N_B: return f3DPieces[bN];
        case B_B: return f3DPieces[bB];
        case R_B: return f3DPieces[bR];
        case Q_B: return f3DPieces[bQ];
        case K_B: return f3DPieces[bK];
        default:
            return f3DPieces[wP];
    }
}


void
ChessBoardView::_DrawArrow2D(Tuple<int> from, Tuple<int> to)
{
    SetHighColor(0, 0, 255, 255);
    StrokeLine(_CTP(from), _CTP(to));
    Sync();
    Flush();
}


// Board-Coordinate to Screen-Position
BPoint
ChessBoardView::_CTP(Tuple<int> const& coord)
{
    if (fBoardIsTurned)
        return BPoint(coord.x*fIR.Width() + fBoard2dOffset.x,
                                    coord.y*fIR.Width() + fBoard2dOffset.y);
    else
        return BPoint(coord.x*fIR.Width() + fBoard2dOffset.x,
                                (7-coord.y)*fIR.Width() + fBoard2dOffset.y);
}


std::vector<BPoint>
ChessBoardView::_CreateArrow2D(Tuple<int> const& from, Tuple<int> const& to)
{
    std::vector<BPoint> vec;
    BPoint pFrom = _CTP(from) + BPoint(fIR.Width()*0.5, fIR.Width()*0.5);
    BPoint pTo   = _CTP(to) + BPoint(fIR.Width()*0.5, fIR.Width()*0.5);
    BPoint tp1, tp2, tp3;

    float angle = 0;

    if (pFrom.x == pTo.x) {
        if (pTo.y < pFrom.y ) {
            pTo += BPoint(0, fIR.Width()*0.45);
            angle = PI/2;
        } else {
            pTo -= BPoint(0, fIR.Width()*0.45);
            angle = -PI/2;
        }
    } else if (pFrom.y == pTo.y) {
        if (pTo.x < pFrom.x) {
            angle = PI;
            pTo += BPoint(fIR.Width()*0.45, 0);
        } else {
            angle = 0;
            pTo -= BPoint(fIR.Width()*0.45, 0);
        }
    } else if (pFrom.y < pTo.y) {
        float c = (pFrom.y - pTo.y) / (pTo.x - pFrom.x);

        if (pFrom.x < pTo.x) {
            angle = atan(c);
            pTo -= BPoint(fIR.Width()*0.45, fIR.Width()*0.45);
        } else {
            angle = PI + atan(c);
            pTo -= BPoint(-fIR.Width()*0.45, fIR.Width()*0.45);
        }
    } else {
        float c = (pFrom.y - pTo.y) / (pTo.x - pFrom.x);

        if (pFrom.x < pTo.x) {
            angle = atan(c);
            pTo += BPoint(-fIR.Width()*0.45, fIR.Width()*0.45);
        } else {
            angle = PI + atan(c);
            pTo += BPoint(fIR.Width()*0.45, fIR.Width()*0.45);
        }
    }

    _MakeArrowHead(tp1, tp2, tp3, angle, pTo);
    vec.push_back(pFrom );
    vec.push_back(pTo);
    vec.push_back(tp1);
    vec.push_back(tp2);
    vec.push_back(tp3);

    return vec;
}


void
ChessBoardView::_MakeArrowHead(BPoint& p1, BPoint& p2, BPoint& p3,
                                                   float a, BPoint const& shift)
{
    p1 = BPoint(0, -fIR.Width()*0.1);
    p2 = BPoint(0, +fIR.Width()*0.1);
    p3 = BPoint(fIR.Width()*0.2, 0);

    BPoint tp;

    tp = p1; p1.x = cos(a)*tp.x + sin(a)*tp.y; p1.y = -sin(a)*tp.x + cos(a)*tp.y;
    tp = p2; p2.x = cos(a)*tp.x + sin(a)*tp.y; p2.y = -sin(a)*tp.x + cos(a)*tp.y;
    tp = p3; p3.x = cos(a)*tp.x + sin(a)*tp.y; p3.y = -sin(a)*tp.x + cos(a)*tp.y;

    p1 += shift; p2 += shift; p3 += shift;
}


void
ChessBoardView::FlipBoard(void)
{
    fBoardIsTurned = !fBoardIsTurned;

    fDistance = 30;
    fTwist = 0;
    fElevation = 30;
    fAzimuth = -15;

    fLookToX = 0;
    fLookToY = 0;
    fLookToZ = 0;

    if (fBoardIsTurned)
        fAzimuth += 180;

    fArrows.clear();
    Render();
}


void
ChessBoardView::SetBoardTurned(bool turned)
{
    if (fBoardIsTurned != turned)
        FlipBoard();

}


// Method written by Stippi, thanks.
BBitmap*
ChessBoardView::_CreateBitmapBuffer(void)
{

    BBitmap* bitmap = new BBitmap(Bounds().OffsetToCopy(B_ORIGIN),
        B_RGBA32, true);
    BView* offscreen = new BView(bitmap->Bounds(), NULL, 0, 0);
    bitmap->Lock();
    bitmap->AddChild(offscreen);

    // Transfer some view parameters. This is only necessary if they
    // are different from the default BView settings.
    BFont font;
    GetFont(&font);
    offscreen->SetFont(&font);
    // offscreen->SetScale(Scale());
    // offscreen->SetOrigin(Origin());
    // ...
    offscreen->SetDrawingMode(B_OP_COPY);
    offscreen->SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    offscreen->SetViewColor(B_TRANSPARENT_32_BIT);
    offscreen->SetDrawingMode(B_OP_ALPHA);
    offscreen->SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
    offscreen->SetLowColor(LowColor());
    // Do what the app_server does for you before Draw() is called:
    if (ViewColor() != B_TRANSPARENT_COLOR) {
        offscreen->SetHighColor(ViewColor());
        offscreen->FillRect(offscreen->Bounds());
    }



    offscreen->SetHighColor(HighColor());
    _DrawInView(offscreen, offscreen->Bounds());

    // Drawing is asynchronous, so it's important to do this:
    offscreen->Sync();
    bitmap->Unlock();

    // bitmap contains now the same contents as are rendered
    // on the screen by YourView itself.

    // If you want to get rid of the extra app_server drawing
    // thread which is spawned for bitmap, you can do this:
    // (Would have to look it up, but there is something similar
    // to a copy constructor)
    BBitmap* regularBitmap = new BBitmap(bitmap);

    // You can use "regularBitmap" from now on, if you keep a
    // reference to it...

    // This would delete the BView as well:
    delete bitmap;

    return regularBitmap;
}


bool
ChessBoardView::HasHeightForWidth()
{
    return false;
    return (!fIs3D);
}


void
ChessBoardView::GetHeightForWidth(float width, float* min, float* max, float* preferred)
{
}
