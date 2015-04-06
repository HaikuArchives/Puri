/*
 * Copyright 2011-2011 Haiku Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ciprian Nedisan (cipri)
 *
 */
#ifndef BACKPIECE_H
#define BACKPIECE_H

#include <String.h>

const int White = 0;
const int Black = 1;

const int WhiteFlag		= 1 << White;
const int BlackFlag		= 1 << Black;

const int Pawn		= 1 << 2;
const int Knight	= 1 << 3;
const int Bishop	= 1 << 4;
const int Rook		= 1 << 5;
const int King		= 1 << 6;
const int ENP		= 1 << 7;
const int PROM		= 1 << 8;
const int CASTLE	= 1 << 9;

const int Queen	= Bishop | Rook;

const int Empty		= 0;
const int Edge		= Knight;

const int P_W = Pawn   | WhiteFlag;
const int N_W = Knight | WhiteFlag;
const int B_W = Bishop | WhiteFlag;
const int R_W = Rook   | WhiteFlag;
const int Q_W = Queen  | WhiteFlag;
const int K_W = King   | WhiteFlag;

const int P_B = Pawn   | BlackFlag;
const int N_B = Knight | BlackFlag;
const int B_B = Bishop | BlackFlag;
const int R_B = Rook   | BlackFlag;
const int Q_B = Queen  | BlackFlag;
const int K_B = King   | BlackFlag;


BString PieceToStr(int piece);

#endif
