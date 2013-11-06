/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef CHESSBOARDVIEW_H
#define CHESSBOARDVIEW_H

#include <vector>

#include <Cursor.h>
#include <GLView.h>
#include <SimpleGameSound.h>

#include "BackBoard.h"
#include "Debug.h"
#include "Model3DS.h"
#include "Tuple.h"

class ChessBoardView : public BGLView
{
public:
    friend class    PromotionWindow;

                    ChessBoardView(BRect const& frame);
    virtual         ~ChessBoardView();

    virtual void 	AttachedToWindow(void);
    virtual void 	AllAttached(void);
    virtual void 	FrameResized(float newWidth, float newHeight);
    virtual void 	ErrorCallback(GLenum which);

    virtual	void	MakeFocus(bool focusState = true);
    virtual	void	Draw(BRect updateRect);
    virtual void	MessageReceived(BMessage* message);
    virtual	void	KeyDown(const char* bytes, int32 numBytes);
    virtual	void	KeyUp(const char* bytes, int32 numBytes);
    virtual void 	MouseDown(BPoint where);
    virtual	void	MouseUp(BPoint where);
    virtual	void	MouseMoved(BPoint where, uint32 code,
                        const BMessage* dragMessage);

    virtual	bool	HasHeightForWidth();
	virtual	void	GetHeightForWidth(float width, float* min,
                        float* max, float* preferred);

            void	WindowActivated(bool active);
            void	Render(void);
            bool    MoveFromTo(Move move);
            bool    UserMoveFromTo(Tuple<int> kFrom, Tuple<int> kTo);
            bool    EngineMoveFromTo(BString moveStr);
            bool    DoICSMove(BString move);
            void    FlipBoard(void);
            void    SetBoardTurned(bool turned = true);
            void	MoveFirst(void);
            void	MoveBack(void);
            void	MoveNext(void);
            void	MoveLast(void);
            void    GotoMove(uint idx);
            void	NewGame(void);
            void    ChangeViewMode(void);
            void    SetViewMode3D(bool mode);

            void    CancelSelection(void)
            {
                _CancelSelection();
            }

            void    ShowValidMoves(bool showValidMoves)
            {
                fShowValidMoves = showValidMoves;
            }


            bool    IsWhiteTurn(void)
            {
                return fBoard->IsWhiteTurn();
            }

            BString LastMoveICSF(void)
            {
                return fBoard->LastMoveICSF();
            }

            BString MovesEF(void)
            {
                return fBoard->MovesEF();
            }


            void    SetUserCanMove(bool  userCanMove = true)
            {
                fUserCanMove = userCanMove;
            }


            BString GetMaterial(void)
            {
                return fBoard->GetMaterial();
            }

            BackBoard* Board(void)
            {
                return fBoard;
            }


    enum            {wK = 0, wQ, wR, wB, wN, wP, bK, bQ, bR, bB, bN, bP};
    const float     PI;

private:
    BBitmap*    _CreateBitmapBuffer(void);
    BBitmap*    _ImageBoard(uint const& kX, uint const& kY);
    BBitmap*    _ImageBoard(Tuple<uint> const& kTuple)
    {
        return _ImageBoard(kTuple.x, kTuple.y);
    }

    void        _CancelSelection(void);
    void        _NotifyState(void);
    void        _LoadImages(void);
    void        _Load3DModels(void);
    void        _DrawInView(BView* view, BRect updateRect);
    void        _DrawArrow2D(Tuple<int>  from, Tuple<int> to);
    void        _MakeArrowHead(BPoint& p1, BPoint& p2, BPoint& p3, float a,
                    BPoint const& shift = BPoint(0, 0));
    void        _HighlightOnlyCoord(int x, int y);
    void	    _PolarView(void);
    void	    _Init(void);
    void	    _InitBoardFromBackBoard(void);
    void	    _Draw(void);
    void	    _DrawArrow(Tuple<uint> const& kFrom, Tuple<uint> const& kTo);
    void	    _Reshape(int width, int height);
    void	    _ShowPossibleMoves(Tuple<uint> kFrom);
    Move        _StrToMove(BString moveStr);
    Move        _ICSToMove(BString moveStr);
    BPoint      _CTP(Tuple<int> const& coord);
    BPoint      _OGLPos(BPoint const& wehere);

    Tuple<int>  _ToCoord(BPoint const& where)
    {
        return Tuple<int>(floor((where.x + 8)/2) , floor((where.y + 8)/2));
    }

    Tuple<int>  _To2DCoord(BPoint  where)
    {
        where -= fBoard2dOffset;

        if (where.x < 0 || where.y < 0)
            return fNoMove;

        Tuple<int> coord(where.x/fIR.Width(), where.y/fIR.Width());

        if (coord.x > 7 || coord.y > 7)
            return fNoMove;

        if (fBoardIsTurned == false)
            coord.y = 7 - coord.y;

        return coord;
    }

    inline bool _IsValidCoord(Tuple<int> kPos)
    {
        if (kPos.x < 0 || kPos.y < 0 || kPos.x > 7 || kPos.y >7)
            return false;
        else
            return true;
    }

    inline void _HighlightOnlyCoord(Tuple<uint> const& kPos)
    {
        _HighlightOnlyCoord(kPos.x, kPos.y);
    }

    void        _HighlightCoord(Tuple<int> const& kPos, bool state)
    {
        if (_Board(kPos) == NULL)
            return;

        _Board(kPos)->SetHighlight(state);
    }

    inline void _AdjustAzimuth(void)
    {
        if      (fAzimuth >  360)   fAzimuth =  0;
        else if (fAzimuth < -360)	fAzimuth =  0;
    }

    inline void _AdjustElevation(void)
    {
        if      (fElevation > 90)	fElevation = 90;
        else if (fElevation <  0)	fElevation =  0;
    }

    inline void _AdjustDistance( void )
    {
        if (fDistance <  5)	    fDistance =  5;
        if (fDistance > 50)	    fDistance = 50;
    }


    std::list<Move>         _GetValidMoves(Tuple<uint> kFrom);
    std::vector<BPoint>     _CreateArrow2D(Tuple<int> const& from,
                                                          Tuple<int> const& to);

            Model3DS* &     _3DModel(uint const& x, uint const& y);


    inline  Model3DS* &     _Board(Tuple<uint> const& kPos)
    {
        return fvPieceModel[kPos.x][kPos.y];
    }

    inline Model3DS* &      _Board(uint const& kX, uint const& kY)
    {
        if (kX < 0 || kX > 7 || kY < 0 || kY > 7) {
            return fNullModel;
        }

        return fvPieceModel[kX][kY];
    }

    inline const int&       _BBoard(uint const& x, uint const& y) const
    {
        return fBoard->_Board(x + 2, y + 2);
    }


    inline const int&       _BBoard(Tuple<uint>  kCoord) const
    {
        return fBoard->_Board(kCoord + 2);
    }


    std::list<Move>                     fValidMoves;
    std::vector<std::vector<BPoint>>    fArrows;
    std::vector<BSimpleGameSound*>      fSound;

    BBitmap*        fDragPiece2dBM;
    BBitmap*        fBackgroundBM;
    BBitmap*        fBoard2dBM;
    BBitmap*        fBM[12];

    BCursor*        fDefaultCursor;
    BCursor*        fGrabCursor;

    Model3DS*	    fvPieceModel[8][8];
    Model3DS*	    fBoardModel;
    Model3DS*       fNullModel;
    Model3DS*       f3DPieces[12];

    Tuple<uint>     fNoMove;
    Tuple<uint>     fHCoord2D;
    Tuple<uint>	    fMoveFrom;

    BackBoard*	    fBoard;

    BRect           fIR;
    BPoint          fBoard2dOffset;
    BPoint	        fOldMousePos;
    BPoint          fGrabEffect;
    BPoint          fGrabDiff;
    BString         fImageName[12];
    BString         fAppPath;

    float	        fWidth;
    float	        fHeight;
    float	        fLookToX;
    float	        fLookToY;
    float	        fLookToZ;
    GLfloat	        fDistance;
    GLfloat	        fTwist;
    GLfloat	        fElevation;
    GLfloat	        fAzimuth;
    uint32	        fMouseButtons;
    uint            fvCoordToPiece[8][8];
    bool            fShowValidMoves;
    bool            fBoardIsTurned;
    bool            fUserCanMove;
    bool            fDrawPieces;
    bool            fIsDragging;
    bool            fPlaySound;
    bool            f3DLoaded;
    bool            fIs3D;

    Debug           out;

    float           fHeightForWidth;
};

#endif
