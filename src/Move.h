/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef MOVE_H
#define MOVE_H

#include <String.h>

#include "Debug.h"
#include "Tools.h"
#include "Tuple.h"

class Move
{
public:
    Move(void)
    {
        fFrom	= 0;
        fTo 	= 0;
        fFlag   = 0;
    }


    Move(uint const& from, uint const& to, int flag = 0)
    {
        fFrom	= from;
        fTo 	= to;
        fFlag   = flag;
    }


    Move(uint const& fromX, uint const& fromY,
                          uint const& toX, uint const& toY, int const& flag = 0)
    {
        fFrom = fromY * 12 + fromX;
        fTo	  = toY   * 12 + toX;
        fFlag = flag;
    }


    Move(Tuple<uint> const& from, Tuple<uint> const& to, int const& flag = 0)
    {
        fFrom = from.y * 12 + from.x;
        fTo	  = to.y   * 12 + to.x;

        fFlag = flag;
    }


    void Print(void)
    {
        !out<<"Move: ("<<FromX()<<","<<FromY()<<")->("
                       <<ToX()  <<","<<ToY()<<")"<<std::endl;
    }


    void Set(uint const& fromX, uint const& fromY,
                                               uint const& toX, uint const& toY)
    {
        fFrom = fromY * 12 + fromX;
        fTo	  = toY   * 12 + toX;
    }


    void Set(uint const& fromX, uint const& fromY,
                              uint const& toX, uint const& toY, int const& flag)
    {
        fFrom = fromY * 12 + fromX;
        fTo	  = toY   * 12 + toX;
        fFlag = flag;
    }


    void Set(Tuple<uint> const& from, Tuple<uint> const& to)
    {
        fFrom = from.y * 12 + from.x;
        fTo	  = to.y   * 12 + to.x;
    }


    void Set(Tuple<uint> const& from, Tuple<uint> const& to, int const& flag)
    {
        fFrom = from.y * 12 + from.x;
        fTo	  = to.y   * 12 + to.x;
        fFlag = flag;
    }


    Tuple<int> From(void) const
    {
        return Tuple<uint>(FromX(), FromY());
    }


    Tuple<int> To(void) const
    {
        return Tuple<uint>(ToX(), ToY());
    }


    inline int FromX(void) const
    {
        return static_cast<int>(fFrom % 12);
    }


    inline int FromY(void) const
    {
        return static_cast<int>(fFrom / 12);
    }


    inline int ToX( void ) const
    {
        return static_cast<int>(fTo % 12);
    }


    inline int ToY(void) const
    {
        return static_cast<int>(fTo / 12);
    }


    inline int DeltaX(void)
    {
        return abs(ToX() - FromX());
    }


    inline int DeltaY(void)
    {
        return abs(ToY() - FromY());
    }


    inline int const& GetFlag(void) const
    {
        return fFlag;
    }


    inline void SetFlag(int const& flag)
    {
        fFlag = flag;
    }


    inline void AddFlag(int const& flag)
    {
        fFlag = fFlag | flag;
    }


    inline bool HasFlag(int const& kFlag) const
    {
        return ((fFlag & kFlag) == kFlag);
    }


    inline bool HasFlag(void) const
    {
        return (fFlag != 0);
    }


    inline Move operator+(int const& kAdd)
    {
        return Move(fFrom + kAdd*13, fTo + kAdd*13, fFlag);
    }


    inline Move operator-(int const& kSub)
    {
        return Move(fFrom - kSub*13, fTo - kSub*13, fFlag);
    }


    inline Move& operator+=(int const& kSummand)
    {
        fFrom = fFrom + kSummand*13;
        fTo	  = fTo   + kSummand*13;

        return *this;
    }


    inline Move& operator-=( int const& kSub )
    {
        fFrom = fFrom - kSub*13;
        fTo	  = fTo   - kSub*13;

        return *this;
    }

// compare

    inline bool operator==( Move const& kMove ) const
    {
        return (kMove.fFrom == fFrom && kMove.fTo == fTo);
    }


    inline bool operator!=( Move const& kMove ) const
    {
        return (kMove.fFrom != fFrom || kMove.fTo != fTo);
    }


    inline bool operator<(int const& value) const
    {
        return ((FromX() < value) && (FromY() < value));
    }


    inline bool operator<=(int const& value) const
    {
        return ((FromX() <= value) && (FromY() <= value));
    }


    inline bool operator>(int const& value) const
    {
        return ((ToX() > value) && (ToY() > value));
    }


    inline bool operator>=(int const& value) const
    {
        return ((ToX() >= value) && (ToY() >= value));
    }
// assigne

    inline Move& operator=(Move const& kMove)
    {
        fFrom 	= kMove.fFrom;
        fTo		= kMove.fTo;

        fFlag = kMove.fFlag;

        return *this;
    }


    inline int Piece(void)  const
    {
        int piece;

        if      (HasFlag(King))     piece = King;
        else if (HasFlag(Queen))    piece = Queen;
        else if (HasFlag(Rook))     piece = Rook;
        else if (HasFlag(Bishop))   piece = Bishop;
        else if (HasFlag(Knight))   piece = Knight;
        else if (HasFlag(Pawn))     piece = Pawn;
        else
            return Empty;

        if (HasFlag(WhiteFlag))     piece = piece|WhiteFlag;
        else                        piece = piece|BlackFlag;

        return piece;
    }


    BString String(void) const
    {
        BString str = "";

        str << (char)('a' + FromX() - 2) << (char)('1' + FromY() - 2);
        str << (char)('a' + ToX()   - 2) << (char)('1' + ToY()   - 2);

        int piece = Piece();
        if (piece == Empty)
            return str;

        str << PieceToStr(piece).ToLower();
        return str;
    }

    BString ICSF(void) const
    {
        BString str = "";

        str << (char)('a' + FromX() - 2) << (char)('1' + FromY() - 2);
        str << (char)('a' + ToX()   - 2) << (char)('1' + ToY()   - 2);

        int piece = Piece();
        if (piece == Empty)
            return str;

        str << "=" << PieceToStr(piece).ToUpper();
        return str;
    }

private:
    int 	fFrom;
    int	    fTo;

    int		fFlag;

    Debug   out;

};

#endif
