/*
 * Copyright 2012-2012 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#include "BackPiece.h"

BString
PieceToStr(int piece)
{
    BString str = "";

    switch (piece) {
        case K_W:   str = "K";  break;
        case Q_W:   str = "Q";  break;
        case R_W:   str = "R";  break;
        case B_W:   str = "B";  break;
        case N_W:   str = "N";  break;
        case P_W:   str = "P";  break;
        case K_B:   str = "k";  break;
        case Q_B:   str = "q";  break;
        case R_B:   str = "r";  break;
        case B_B:   str = "b";  break;
        case N_B:   str = "n";  break;
        case P_B:   str = "p";  break;
        default:                break;
    }

    return str;
}


