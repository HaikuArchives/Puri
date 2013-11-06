/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "BackBoard.h"

#include <iostream>

#include "BackPiece.h"


BackBoard::BackBoard(void)
    :
    fTurn(White),
    fState(0),
    fLastMoveStr("")
{
    !out << "BACKBOARD start" << std::endl;

	Tuple<int>* Q_Inc  	= new Tuple<int>[9];
	Tuple<int>* R_Inc  	= new Tuple<int>[5];
	Tuple<int>* B_Inc  	= new Tuple<int>[5];
	Tuple<int>* N_Inc 	= new Tuple<int>[9];
	Tuple<int>* P_Inc	= new Tuple<int>[1];

	Q_Inc[0] = R_Inc[0] = Tuple<int>(1, 0);
	Q_Inc[1] = R_Inc[1] = Tuple<int>(0, 1);
	Q_Inc[2] = R_Inc[2] = Tuple<int>(-1, 0);
	Q_Inc[3] = R_Inc[3] = Tuple<int>(0, -1);
	Q_Inc[4] = B_Inc[0] = Tuple<int>(1, 1);
	Q_Inc[5] = B_Inc[1] = Tuple<int>(-1, 1);
	Q_Inc[6] = B_Inc[2] = Tuple<int>(-1, -1);
	Q_Inc[7] = B_Inc[3] = Tuple<int>(1, -1);

	N_Inc[0] = Tuple<int>(2, 1);
	N_Inc[1] = Tuple<int>(1, 2);
	N_Inc[2] = Tuple<int>(-1, 2);
	N_Inc[3] = Tuple<int>(-2, 1);
	N_Inc[4] = Tuple<int>(-2, -1);
	N_Inc[5] = Tuple<int>(-1, -2);
	N_Inc[6] = Tuple<int>(1, -2);
	N_Inc[7] = Tuple<int>(2, -1);

	Q_Inc[8] = R_Inc[4] = B_Inc[4] = N_Inc[8] = P_Inc[0] = 0;

	fvInc[King  ] = fvInc[K_W] = fvInc[K_B] = Q_Inc;
	fvInc[Queen ] = fvInc[Q_W] = fvInc[Q_B] = Q_Inc;
	fvInc[Rook  ] = fvInc[R_W] = fvInc[R_B] = R_Inc;
	fvInc[Bishop] = fvInc[B_W] = fvInc[B_B] = B_Inc;
	fvInc[Knight] = fvInc[N_W] = fvInc[N_B] = N_Inc;
	fvInc[Pawn  ] = fvInc[P_W] = fvInc[P_B] = P_Inc;

	NewGame();
}


void
BackBoard::PrintMoves(std::list< Move > const& kMoves)
{
    if (out.state() == false)
        return;

	!out << "Moves - begin" << std::endl;

	std::list<Move>::const_iterator itr = kMoves.begin();

	for (int i = 0; itr != kMoves.end(); ++itr, ++i) {
		out << "  " << i + 1 << ":  ";
		out << itr->FromX() << "-" << itr->FromY() << "  ";
		out << itr->ToX()   << "-" << itr->ToY()   << std::endl;
	}

	!out << "Moves - end" << std::endl;
}


void
BackBoard::_InitBoard(void)
{
	fTurn = White;

	int vPiece[8] = {Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook};

	for (int i = 0; i < 12; ++i)
    for (int j = 0; j < 12; ++j) {
        if (i < 2 || i > 9 || j < 2 || j > 9)
            _Board(j, i) = Edge;
        else
            _Board(j, i) = Empty;
    }

	for (int i = 0; i < 8; ++i) {
		_Board(i + 2, 7 + 2) = vPiece[i] | BlackFlag;
		_Board(i + 2, 6 + 2) = P_B;
		_Board(i + 2, 1 + 2) = P_W;
		_Board(i + 2, 0 + 2) = vPiece[i] | WhiteFlag;
	}

	fPosK_W = Tuple<uint>(6, 2);
	fPosK_B = Tuple<uint>(6, 9);

	fPreMoves.clear();
	fMoves.clear();
}


void
BackBoard::_DoMoves(uint const& kMoves)
{
	_InitBoard();

	for (uint i = 0; i < kMoves; ++i)
		_DoMove(fPlayedMoves[i]);

    fMoves = _GenMoves();
}


void
BackBoard::_DoMove(Move const& move)
{
	const int pieceFrom = _Board(move.From());
    _Board(move.To())   = pieceFrom;
	_Board(move.From()) = Empty;

	if (_IsFlag(pieceFrom, Pawn)) {
		if (pieceFrom == P_W) {
            // add en passant
			if (move.FromY() == 3 && move.ToY() == 5) {
				if (_Board(move.FromX() - 1, 5 ) == P_B )
					fPreMoves.push_back(Move(move.FromX() - 1, 5,
                                                         move.FromX(), 4, ENP));
				if (_Board(move.FromX() + 1, 5) == P_B)
					fPreMoves.push_back(Move(move.FromX() + 1, 5,
                                                         move.FromX(), 4, ENP));
			}
            else if (move.HasFlag(PROM))
				_Board(move.To()) = move.Piece();
			else if (move.HasFlag(ENP))
				_Board(move.To() - Tuple<uint>(0, 1)) = Empty;
		} else {
            // add en passant
			if (move.FromY() == 8 && move.ToY() == 6) {
				if (_Board(move.FromX() - 1, 6) == P_W)
					fPreMoves.push_back(Move(move.FromX() - 1, 6,
                                                        move.FromX(), 7, ENP));
				if (_Board(move.FromX() + 1, 6) == P_W)
					fPreMoves.push_back(Move(move.FromX() + 1, 6,
                                                        move.FromX(), 7, ENP));
			}
			else if (move.HasFlag(PROM))
				_Board(move.To()) = move.Piece();
			else if (move.HasFlag(ENP))
				_Board(move.To() + Tuple<uint>(0, 1)) = Empty;
		}
	} else if (_IsFlag(pieceFrom, King)) {
		if (pieceFrom == K_W) {
			fPosK_W = move.To();

            // If castle Move, put rooks on right position
			if ((move.ToX() - move.FromX()) == 2) {
				_Board(9, 2) = Empty;
				_Board(7, 2) = R_W;
			} else if ((move.ToX() - move.FromX()) == -2) {
				_Board(2, 2) = Empty;
				_Board(5, 2) = R_W;
			}
		} else {
			fPosK_B = move.To(); //update king position

			// If castle Move, put rooks on right position
			if ((move.ToX() - move.FromX()) == 2) {
				_Board(9, 9) = Empty;
				_Board(7, 9) = R_B;
			} else if ((move.ToX() - move.FromX()) == -2) {
				_Board(2, 9) = Empty;
				_Board(5, 9) = R_B;
			}
		}
	}

	_Turn();
}


void
BackBoard::DoMove(Move move)
{
	if (move < 2 || move > 9)
		!out<<"ERROR->BackBoard::DoMove"<<std::endl;

    fLastMoveStr = _MoveToString(move);
	_DoMove(move);

	if (fCurrentMove != fPlayedMoves.size())
		fPlayedMoves.resize(fCurrentMove);

	fPlayedMoves.push_back(move);
	fCurrentMove++;

    //ToDo: if flag -> promotion add sign.
    // replace: switch state: check, mate, stale mate

    fMoves = _GenMoves();

    fState = State();

    switch(fState) {
        case 1: fLastMoveStr += "+";    break;
        case 2: fLastMoveStr += "#";    break;
        default:                        break;
    }
}


int
BackBoard::State(void)
{
    if (_IsCheck()) {
        if (fMoves.empty())     return 2;
        else                    return 1;
    } else if (fMoves.empty())
        return 3;

    return 0;
}


void
BackBoard::Promote(int value)
{
    Move move = fPlayedMoves[fCurrentMove - 1];

    if (move.HasFlag(PROM) == false) {
        ++fCurrentMove;
        return;
    }

    move.SetFlag(PROM);
    move.AddFlag(value);
    MoveBack();
    DoMove(move);
}


bool
BackBoard::_IsLegal(Tuple<uint> const& kFrom, Tuple<uint> const& kTo)
{
	Tuple<uint> * kingPos = NULL;
	bool isKing = false;

	// if the king makes the pseudo-move, change also the king-coordinates,
	// and then change them back, to the inital value
	if (_IsFlag(_Board(kFrom), King)) {
		if (_Board(kFrom) == K_W) {
            if ((kTo - fPosK_B) < 2)
                return false;

            kingPos = &fPosK_W;
        } else	if (_Board(kFrom) == K_B) {
            if ((kTo - fPosK_W) < 2)
                return false;

            kingPos = &fPosK_B;
        }

		*kingPos = kTo;
		isKing 	 = true;
	}

	const int kPiece = _Board(kTo);

	// Do  a pseudo-move , check if King is attackend
	// then undo the move
	_Board(kTo)   = _Board(kFrom);
	_Board(kFrom) = Empty;

	if (_IsCheck()) {
		if (isKing)
			*kingPos = kFrom;

		_Board(kFrom) = _Board(kTo);
		_Board(kTo)   = kPiece;

        return false;
	} else {
		if (isKing)
			*kingPos = kFrom;

		_Board(kFrom) = _Board(kTo);
		_Board(kTo)   = kPiece;

		return true;
	}
}


bool
BackBoard::_IsCheck(void)
{
	Tuple<uint>  from, to;

	if (fTurn == White)     from = fPosK_W;
	else					from = fPosK_B;

	Tuple<int> inc;

	// check if the King is attacked by a Queen, Rook or Bishop
	for (int i = 0; (inc = fvInc[ Queen ][i]) != 0 ; ++i) {
		to	= from;

		for ( ; ; ) {
			to += inc ;

			if (_Board(to) == Empty)
				continue;
			else if (_Board(to) == Edge)
				break;
			else if (_IsCapture(_Board(to))) {
            	if (i < 4) {
					if (_IsFlag(_Board(to), Rook))
						return true;
				} else if (_IsFlag(_Board(to), Bishop))
						return true;
			}

			break;
		}
	}

	// check if the King is attacked by a Knight
	for (int i = 0; (inc = fvInc[ Knight ][i]) != 0; ++i) {
		if (_IsFlag(_Board(from + inc), Knight))
			if (_IsCapture(_Board(from + inc)))
				return true;
	}

	// check if the King is attacked by a Pawn

	if (fTurn == White) {
		if (_Board(from + Tuple<int>(-1, 1)) == P_B)
			return true;
		else if (_Board(from + Tuple<int>(1, 1)) == P_B)
			return true;
	} else {
		if (_Board(from + Tuple<int>(-1, -1)) == P_W)
			return true;
		else if (_Board(from + Tuple<int>(1, -1)) == P_W)
			return true;
	}

	return false;
}


std::list<Move>
BackBoard::_GenMoves(void)
{
	std::list< Move > moves = fPreMoves;
        // add preliminar moves, like en passant

	fPreMoves.clear();

	Tuple<uint>     from, to;
	int             piece;

	for (from.y = 2; from.y < 10; ++from.y)
	for (from.x = 2; from.x < 10; ++from.x) {
		piece = _Board( from );

		if (piece == Empty || piece == Edge || _IsCapture(piece))
            continue;

		// add pawn-moves
		if (piece == P_B) {
			to = from + Tuple<int>(-1, -1);

			if (_IsCapture(_Board(to)) && _IsLegal(from, to)) {
                if (to.y == 2)      moves.push_back(Move(from, to, PROM|P_B));
                else                moves.push_back(Move(from, to));
            }

			to = from + Tuple<int>(1, -1);

			if (_IsCapture(_Board(to)) && _IsLegal(from, to)) {
                if (to.y == 2)      moves.push_back(Move(from, to, PROM|P_B));
                else                moves.push_back(Move(from, to));
            }

			to = from + Tuple<int>( 0, -1 );

			if (_Board(to) == Empty && _IsLegal(from, to)) {
                if (to.y == 2)   	moves.push_back(Move(from, to, PROM|P_B));
                else                moves.push_back(Move(from, to));
            }

			if (from.y == 8 && _Board(to) == Empty) {
				to = from + Tuple<int>(0, -2);

				if (_Board( to ) == Empty && _IsLegal(from, to))
                    moves.push_back(Move(from, to));
			}

			continue;
		} else if (piece == P_W) {
			to = from + Tuple<int>(-1, 1);

			if (_IsCapture(_Board(to)) && _IsLegal(from, to)) {
                if (to.y == 9) 		moves.push_back(Move(from, to, PROM|P_W));
                else                moves.push_back(Move(from, to));
            }

			to = from + Tuple<int>(1, 1);

			if (_IsCapture(_Board(to)) && _IsLegal(from, to)) {
                if (to.y == 9)   	moves.push_back(Move(from, to, PROM|P_W));
                else                moves.push_back(Move(from, to));
            }

			to = from + Tuple<int>(0, 1);

			if (_Board(to) == Empty && _IsLegal(from, to)) {
                if (to.y == 9)   	moves.push_back(Move(from, to, PROM|P_W));
                else                moves.push_back(Move(from, to));
            }

			if (from.y == 3 && _Board(to) == Empty) {
				to = from + Tuple<int>(0, 2);

				if (_Board(to) == Empty && _IsLegal(from, to))
                    moves.push_back(Move(from, to));
			}

			continue;
		}


		// add moves of King, Quee, Rook, Bishop, Knight
		Tuple<int> inc;

		for (int i = 0; (inc = fvInc[ piece ][i]) != 0; ++i) {
			if (_IsSlider(piece)) {
				to = from;

				for ( ;; ) {
					to += inc ;

					if (_Board(to) == Empty) {
						if (_IsLegal(from, to))
							moves.push_back(Move(from, to));
					} else if (_IsCapture(_Board(to))) {
						if (_IsLegal(from, to))
							moves.push_back(Move(from, to));
						break;
					} else
						break;
				}
			}
			else {
				to = from + inc;

				if (_Board(to) == Empty) {
					if (_IsLegal(from, to))
						moves.push_back(Move(from, to));
				}
				else if (_IsCapture(_Board(to))) {
					if (_IsLegal(from, to))
						moves.push_back(Move(from, to));
				}
			}
		}

	}

	// AddCastleMoves( moves );
	if (_IsCheck() == false) {
		if (fTurn == White) {
			if (_FindMove(Tuple<uint>(9, 2), fPosK_W.AddX(1), moves))
                if (_FindMove(fPosK_W, fPosK_W.AddX(1), moves))
                    moves.push_back(Move(fPosK_W, fPosK_W.AddX(2), CASTLE));

            if (_FindMove(Tuple<uint>( 2, 2 ), fPosK_W.SubX(1), moves))
                if (_FindMove(fPosK_W, fPosK_W.SubX(1), moves))
                    moves.push_back(Move(fPosK_W, fPosK_W.SubX(2), CASTLE));
		} else {
			if (_FindMove(Tuple<uint>(9, 9), fPosK_B.AddX(1), moves))
                if (_FindMove(fPosK_B, fPosK_B.AddX(1), moves))
                    moves.push_back(Move(fPosK_B, fPosK_B.AddX(2), CASTLE));

			if (_FindMove(Tuple<uint>(2, 9), fPosK_B.SubX(1), moves))
                if (_FindMove(fPosK_B, fPosK_B.SubX(1), moves))
                    moves.push_back(Move(fPosK_B, fPosK_B.SubX(2), CASTLE));
		}
	}

	return moves;
}


std::list<Move>
BackBoard::GetValidMoves(Tuple<uint> const& from) const
{
	std::list<Move> moves;

	std::list<Move>::const_iterator itr = fMoves.begin();

	for (; itr != fMoves.end(); ++itr)
		if (itr->From() == from)
			moves.push_back(*itr);

	return moves;
}


void
BackBoard::Print(void)
{
    if (out.state() == false)
        return;

    !out;
	for (unsigned int i = 2; i < 10; ++i)
    for (unsigned int j = 2; j < 10; ++j) {
        out << _Board( j, 11 - i ) << " ";

        if (j == 9)
            out << std::endl;
    }
}


void
BackBoard::MoveFirst(void)
{
	_InitBoard();
	fCurrentMove = 0;
	fMoves = _GenMoves();
}


void
BackBoard::MoveBack(void)
{
	if (fCurrentMove < 2) {
        MoveFirst();
		return;
    }

	fCurrentMove--;
	_DoMoves(fCurrentMove);
}


void
BackBoard::MoveNext(void)
{
	if (fCurrentMove >= fPlayedMoves.size())
		return;

	fCurrentMove++;
	_DoMoves(fCurrentMove);
}


void
BackBoard::MoveLast(void)
{
	fCurrentMove = fPlayedMoves.size();
	_DoMoves(fCurrentMove);
}


void
BackBoard::NewGame(void)
{
	fPlayedMoves.clear();
	fCurrentMove = 0;

	_InitBoard();
	fMoves = _GenMoves();
	Print();
}


BString
BackBoard::_MoveToString(Move const& move)
{
    BString moveStr;
    Tuple<uint> from = move.From();
    Tuple<uint> to   = move.To();

    int piece = _Board(from);

    switch (piece) {
        case K_W: case K_B:
            if (from.x == 6) {
                if      (to.x == 8) { moveStr = "0-0"  ; return(moveStr); }
                else if (to.x == 4) { moveStr = "0-0-0"; return(moveStr); }
            }

            moveStr = "K";
            break;

        case Q_W: case Q_B:     moveStr = "Q"; break;
        case R_W: case R_B:     moveStr = "R"; break;
        case B_W: case B_B:     moveStr = "B"; break;
        case N_W: case N_B:     moveStr = "N"; break;
        case P_W: case P_B:
            moveStr = "P";
            if (move.HasFlag(ENP))
                moveStr += "x";

            break;

        default:
            break;
    }


	std::list<Move>::const_iterator itr = fMoves.begin();

    Tuple<uint> temp;

    bool sameX = false, sameY = false;

	for (; itr != fMoves.end(); ++itr) {
        temp = itr->From();

		if (itr->To() == to && temp!=from && _Board(from)  == _Board(temp)) {
            if (temp.x == from.x)     sameX = true;
            else                      sameY = true;
        }

	}

    if (sameY)                  moveStr += ('a' + from.x - 2);
    if (sameX)                  moveStr += ('1' + from.y - 2);
    if (_Board(to) != Empty)    moveStr += "x";

    moveStr += ('a' + to.x - 2);

    if ((_Board(to) == Empty && move.HasFlag(ENP) == false)
        || _IsFlag(piece, Pawn) == false)
        moveStr += ('1' + to.y - 2);

    if (move.HasFlag(PROM))
        moveStr += ((move.String().ToUpper())[4]);

    return moveStr;
}


BString
BackBoard::MovesEF(void)
{
    BString moves = "";

    for (uint i = 0; i < fCurrentMove; ++i)
        moves << " " << fPlayedMoves[i].String();

    return moves;
}


int
BackBoard::_ToPiece(char character)
{
    int piece = Empty;

    switch (character) {
        case 'K': case 'k':     piece = King;   break;
        case 'Q': case 'q':     piece = Queen;  break;
        case 'R': case 'r':     piece = Rook;   break;
        case 'B': case 'b':     piece = Bishop; break;
        case 'N': case 'n':     piece = Knight; break;
        case 'P': case 'p':     piece = Pawn;   break;
        default:                piece = Empty;  break;
    }

    if (fTurn == White)
        return (piece | WhiteFlag);
    else
        return (piece | BlackFlag);
}

// TODO: remove that method
BString
BackBoard::GetMaterial(void)
{
    BString material;

    int     piece[5]  = {0, 0, 0, 0, 0};
    char    wpiece[5] = {'Q', 'R', 'B', 'N', 'P'};
    char    bpiece[5] = {'q', 'r', 'b', 'n', 'p'};

    for (uint i = 2; i < 10; ++i)
    for (uint j = 2; j < 10; ++j) {
        switch(_Board(i, j)) {
            case Q_W: ++piece[0]; break; case Q_B: --piece[0]; break;
            case R_W: ++piece[1]; break; case R_B: --piece[1]; break;
            case B_W: ++piece[2]; break; case B_B: --piece[2]; break;
            case N_W: ++piece[3]; break; case N_B: --piece[3]; break;
            case P_W: ++piece[4]; break; case P_B: --piece[4]; break;
            default:    break;
        }
    }

    for (uint i = 0; i < 5; ++i) {
        if (piece[i] > 0)
            for (int j = 0; j < piece[i]; ++j)
                material <<  (char)wpiece[i];
        else
            for (int j = 0; j < -piece[i]; ++j)
                material <<  (char) bpiece[i];
    }

    return material;
}


void
BackBoard::GotoMove(uint idx)
{
    if (idx < fPlayedMoves.size())
        fCurrentMove = idx;
    else
        fCurrentMove = fPlayedMoves.size();

    _DoMoves(fCurrentMove);
}
