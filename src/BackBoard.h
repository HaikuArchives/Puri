/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef	BACKBOARD_H
#define BACKBOARD_H

#include <algorithm>
#include <list>
#include <vector>

#include <Point.h>
#include <String.h>

#include "BackPiece.h"
#include "Debug.h"
#include "Move.h"

class BackBoard
{
public:
    friend class        ChessBoardView;

                        BackBoard(void);

    // This method to be deleted, it's a temporary solution
    BString             GetMaterial(void);
    void                Promote(int value);
    void                DoMove(Move move);
    void                Print(void);
    void                PrintMoves(std::list<Move> const& kMoves);
    void                MoveFirst(void);
    void                MoveBack(void);
    void                MoveNext(void);
    void                MoveLast(void);
    void                GotoMove(uint idx);
    void                NewGame(void);
    int                 State(void);

    // returns the Moves in Engine Format,
    // which is suitable for uci engines
    BString             MovesEF(void);
    std::list<Move>     GetValidMoves(Tuple<uint> const& from) const;


    BString const&      LastMoveString(void)
    {
        return fLastMoveStr;
    }

    BString             LastMoveICSF(void)
    {

        return fPlayedMoves.back().ICSF();
    }

    inline  bool        IsLegal(Move const& move) const
    {
        return (find(fMoves.begin(), fMoves.end(),move) != fMoves.end());
    }


    inline void         GetBoard(int * vBoard)
    {
        for (uint j = 0; j < 8; ++j)
        for (uint i = 0; i < 8; ++i)
            *(vBoard + j + i*8) = fvBoard[i + 2][j + 2];

        return;
    }


    bool                IsWhiteTurn(void)
    {
        if (fTurn == White)      return true;
        else                    return false;
    }


private:
    void            _InitBoard(void);
    void            _DoMove(Move const& kMove);
    void            _DoMoves(uint const& kMoves);
    bool            _IsCheck(void);
    bool            _IsLegal(Tuple<uint> const& kFrom, Tuple<uint> const& kTo);
    int             _ToPiece(char character);
    BString         _MoveToString(Move const& move);
    std::list<Move> _GenMoves(void);

    inline bool     _IsSlider(int const& kPiece)
    {
        return ((kPiece & Queen) != 0);
    }

    inline bool     _IsFlag(int const& kPiece, int const& kFlag)
    {
        return ((kPiece & kFlag) == kFlag);
    }


    inline bool     _FindMove(Tuple<uint> const& from,
                                                    Tuple<uint> const& to) const
    {
        return (find(fMoves.begin(), fMoves.end(),
                                            Move( from, to ) ) != fMoves.end());
    }


    inline bool     _FindMove(Move const& kMove,
                                         std::list<Move> const& kMoveList) const
    {
        return (find(kMoveList.begin(), kMoveList.end(), kMove)
                                                           != kMoveList.end());
    }


    inline bool     _FindMove( Tuple<uint> const& from, Tuple<uint> const& to,
                                             std::list< Move > const& kMoveList ) const
    {
        return (_FindMove(Move(from, to), kMoveList));
    }


    inline int const _GetColor(int const& kPiece)
    {
        return( 0 );
    }


    // returns true if the color of piece is not equal
    // to the color of the side that has to move
    inline bool     _IsCapture(int const& kPiece)
    {
        if (kPiece == Edge || kPiece == Empty)
            return false;
        else
            return ((kPiece & 1) == fTurn);
    }


    inline int&     _Board(Tuple<uint> const& kPosition)
    {
        return fvBoard[kPosition.x][kPosition.y];
    }


    inline int&     _Board(uint const& x, uint const& y)
    {
        return fvBoard[x][y];
    }


    inline void     _Turn( void )
    {
        fTurn = !fTurn;
    }




    int		            fTurn;
    int                 fState;
    int		            fvBoard[12][12];
    uint	            fCurrentMove;
    Tuple<uint>		    fPosK_W;
    Tuple<uint>		    fPosK_B;
    Tuple<int>*         fvInc[256];

    std::list<Move>     fMoves;
    std::list<Move>     fPreMoves;
    std::vector<Move>   fPlayedMoves;

    BString             fLastMoveStr;
    Debug               out;
};

#endif
