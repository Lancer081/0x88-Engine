#include <stdio.h>

enum { e, P, N, B, R, Q, K, p, n, b, r, q, k, o };

enum squares {
    a8 = 0,   b8, c8, d8, e8, f8, g8, h8,
    a7 = 16,  b7, c7, d7, e7, f7, g7, h7,
    a6 = 32,  b6, c6, d6, e6, f6, g6, h6,
    a5 = 48,  b5, c5, d5, e5, f5, g5, h5,
    a4 = 64,  b4, c4, d4, e4, f4, g4, h4,
    a3 = 80,  b3, c3, d3, e3, f3, g3, h3,
    a2 = 96,  b2, c2, d2, e2, f2, g2, h2,
    a1 = 112, b1, c1, d1, e1, f1, g1, h1, no_sqr
};

enum castling { KC = 1, QC = 2, kc = 4, qc = 8 };

enum sides { white, black };

char start_position[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
char tricky_position[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";

char ascii_pieces[] = ".PNBRQKpnbrqko";
char *unicode_pieces[] = {".", "♙", "♘", "♗", "♖", "♕", "♔", "♟︎", "♞", "♝", "♜", "♛", "♚"};

int char_pieces[] = {
    ['P'] = P,
    ['N'] = N,
    ['B'] = B,
    ['R'] = R,
    ['Q'] = Q,
    ['K'] = K,
    ['p'] = p,
    ['n'] = n,
    ['b'] = b,
    ['r'] = r,
    ['q'] = q,
    ['k'] = k,
};

int board[128] = {
	r, n, b, q, k, b, n, r,  o, o, o, o, o, o, o, o,
	p, p, p, p, p, p, p, p,  o, o, o, o, o, o, o, o,
	e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
	e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
	e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
	e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
	P, P, P, P, P, P, P, P,  o, o, o, o, o, o, o, o,
	R, N, B, Q, K, B, N, R,  o, o, o, o, o, o, o, o
};

char *square_to_coords[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "i8", "j8", "k8", "l8", "m8", "n8", "o8", "p8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "i7", "j7", "k7", "l7", "m7", "n7", "o7", "p7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "i6", "j6", "k6", "l6", "m6", "n6", "o6", "p6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "i5", "j5", "k5", "l5", "m5", "n5", "o5", "p5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "i4", "j4", "k4", "l4", "m4", "n4", "o4", "p4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "i3", "j3", "k3", "l3", "m3", "n3", "o3", "p3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "i2", "j2", "k2", "l2", "m2", "n2", "o2", "p2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "i1", "j1", "k1", "l1", "m1", "n1", "o1", "p1"
};

int knight_offsets[8] = { 33, 31, 18, 14, -33, -31, -18, -14 };
int bishop_offsets[4] = { 15, 17, -15, -17 };
int rook_offsets[4] = { 16, -16, 1, -1 };
int king_offsets[8] = { 16, -16, 1, -1, 15, 17, -15, -17 };

int side = white;
int enpassant = no_sqr;
int castle = 15;

void reset_board()
{
	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 16; file++)	
		{
			int square = rank * 16 + file;
			
			if (!(square & 0x88))
				board[square] = e;
		}
	}
	
	side = -1;
	castle = 0;
	enpassant = no_sqr;
}

void parse_fen(char *fen)
{
	reset_board();

	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 16; file++)	
		{
			int square = rank * 16 + file;
			
			// if square is on board
			if (!(square & 0x88))
			{
				if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z'))
				{
					board[square] = char_pieces[*fen];
					
					*fen++;
				}
				
				if (*fen >= '0' && *fen <= '9')
				{
					int offset = *fen - '0';
					
					if (board[square] == e)
						file--;
					
					file += offset;
					
					*fen++;
				}
				
				if (*fen == '/')
					*fen++;
			}
		}
	}
	
	*fen++;
	
	side = (*fen == 'w') ? white : black;
	
	fen += 2;
	
	while(*fen != ' ')
	{
		switch(*fen)
		{
			case 'K': castle |= KC; break;
			case 'Q': castle |= QC; break;
 			case 'k': castle |= kc; break;
			case 'q': castle |= qc; break;
			case '-': break;
		}	
		
		*fen++;
	}
	
	*fen++;
	
	if (*fen != '-')
	{
		int file = fen[0] - 'a';
		int rank = 8 - (fen[1] - '0');
		
		enpassant = rank * 16 + file;
	}
	else
		enpassant = no_sqr;
}

int is_square_attacked(int square, int side)
{
	if (!side)
	{
		if(!((square + 17) & 0x88) && (board[square + 17] == P))
			return 1;
			
		if (!((square + 15) & 0x88) && (board[square + 15] == P))
			return 1;
	}
	else
	{
		if(!((square - 17) & 0x88) && (board[square - 17] == p))
			return 1;
			
		if(!((square - 15) & 0x88) && (board[square - 15] == p))
			return 1;
	}
	
	for (int index = 0; index < 8; index++)
	{
		int target_square = square + knight_offsets[square];
		int target_piece = board[target_square];
		
		if (!(target_square & 0x88))
		{
			if (!side ? target_piece == N : target_piece == n)
				return 1;
		}
	}
	
	for (int index = 0; index < 8; index++)
	{
		int target_square = square + king_offsets[square];
		int target_piece = board[target_square];
		
		if (!(target_square & 0x88))
		{
			if (!side ? target_piece == K : target_piece == k)
				return 1;
		}
	}
	
	for (int index = 0; index < 4; index++)
	{
		int target_square = square + bishop_offsets[index];
		
		while (!(target_square & 0x88))
		{
			int target_piece = board[target_square];
			
			if (!side ? (target_piece == B || target_piece == Q) : (target_piece == b || target_piece == q))
				return 1;
				
			if (target_piece)
				break;
				
			target_square += bishop_offsets[index];
		}
	}
	
	for (int index = 0; index < 4; index++)
	{
		int target_square = square + rook_offsets[index];
		
		while (!(target_square & 0x88))
		{
			int target_piece = board[target_square];
			
			if (!side ? (target_piece == R || target_piece == Q) : (target_piece == r || target_piece == q))
				return 1;
				
			if (target_piece)
				break;
				
			target_square += rook_offsets[index];
		}
	}
	
	return 0;
}

void print_attacked_squares(int side)
{
	printf("\n");
	printf(" Attacking side: %s\n", !side ? "white" : "black");

	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 16; file++)	
		{
			int square = rank * 16 + file;
			
			if (file == 0)
				printf("%d ", 8 - rank);
				
			// if square is on board
			if (!(square & 0x88))
				printf("%c ", is_square_attacked(square, side) ? 'x' : '.');
		}
		
		printf("\n");
	}
	
	printf("\n  a b c d e f g h\n\n");
}

void print_board()
{
	printf("\n");

	for (int rank = 0; rank < 8; rank++)
	{
		for (int file = 0; file < 16; file++)	
		{
			int square = rank * 16 + file;
			
			if (file == 0)
				printf("%d ", 8 - rank);
				
			// if square is on board
			if (!(square & 0x88))
				printf("%s ", unicode_pieces[board[square]]);
		}
		
		printf("\n");
	}
	
	printf("  a b c d e f g h\n\n");
	
	printf("    Side:     %s\n", (side == white) ? "white": "black");
    printf("    Castling:  %c%c%c%c\n", (castle & KC) ? 'K' : '-', 
                                    (castle & QC) ? 'Q' : '-',
                                    (castle & kc) ? 'k' : '-',
                                    (castle & qc) ? 'q' : '-');
    printf("    Enpassant:   %s\n\n", (enpassant != no_sqr) ? square_to_coords[enpassant] : "no");
}

void generate_moves()
{
	for (int square = 0; square < 128; square++)
	{
		if (!(square & 0x88))
		{
			if (!side)
			{
				// quiet pawn moves
				if (board[square] == P)
				{
					int to_square = square - 16;
				
					if (!(to_square & 0x88) && !board[to_square])
					{
						// pawn promotions
						if (square >= a7 && square <= h7)
						{
							printf("pawn promotion\n");
						}
						else
						{
							// single pawn pushs
							printf("%s%s\n", square_to_coords[square], square_to_coords[to_square]);
							
							// double pawn pushs
							if ((square >= a2 && square <= h2) && !board[square - 32])
							{
								printf("%s%s\n", square_to_coords[square], square_to_coords[square - 32]);
							}
						}
					}
				
					for (int index = 0; index < 4; index++)
					{
						int pawn_offset = bishop_offsets[index];
						
						if (pawn_offset > 0)
						{
							int to_square = square + pawn_offset;
							
							if (!(to_square & 0x88))
							{
								// capture promotion
								if ((square >= a7 && square <= h7) && (board[to_square] >= 7 && board[to_square] <= 12))
								{
									printf("capture promotion\n");
								}
								else
								{
									// capture
									if(board[to_square] >= 7 && board[to_square] <= 12)
										printf("%s%s\n", square_to_coords[square], square_to_coords[to_square]);
										
									// enpassant capture
									if (to_square == enpassant)
										printf("%s%s ep\n", square_to_coords[square], square_to_coords[to_square]);
								}
							}
						}
					}
				}
				else if (board[square] == K)
				{
					if (castle & KC)
					{
						if (!board[f1] && !board[g1])
						{
							if (!is_square_attacked(e1, black) && !is_square_attacked(f1, black))
								printf("white kingside castle\n");
						}
					}
					
					if (castle & QC)
					{
						if (!board[d1] && !board[b1] && !board[c1])
						{
							if (!is_square_attacked(e1, black) && !is_square_attacked(d1, black))
								printf("white queenside castle\n");
						}
					}
				}
			}
			else
			{
				// quiet pawn moves
				if (board[square] == p)
				{
					int to_square = square + 16;
				
					if (!(to_square & 0x88) && !board[to_square])
					{
						// pawn promotions
						if (square >= a2 && square <= h2)
						{
							printf("pawn promotion\n");
						}
						else
						{
							// single pawn pushs
							printf("%s%s\n", square_to_coords[square], square_to_coords[to_square]);
							
							// double pawn pushs
							if ((square >= a7 && square <= h7) && !board[square + 32])
							{
								printf("%s%s\n", square_to_coords[square], square_to_coords[square + 32]);
							}
						}
					}
				
					for (int index = 0; index < 4; index++)
					{
						int pawn_offset = bishop_offsets[index];
						
						if (pawn_offset > 0)
						{
							int to_square = square + pawn_offset;
							
							if (!(to_square & 0x88))
							{
								// capture promotion
								if ((square >= a7 && square <= h7) && (board[to_square] >= 1 && board[to_square] <= 6))
								{
									printf("capture promotion\n");
								}
								else
								{
									// capture
									if(board[to_square] >= 1 && board[to_square] <= 6)
										printf("%s%s\n", square_to_coords[square], square_to_coords[to_square]);
										
									// enpassant capture
									if (to_square == enpassant)
										printf("%s%s ep\n", square_to_coords[square], square_to_coords[to_square]);
								}
							}
						}
					}
				}
				else if (board[square] == k)
				{
					if (castle & kc)
					{
						if (!board[f8] && !board[g8])
						{
							if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white))
								printf("black kingside castle\n");
						}
					}
					
					if (castle & qc)
					{
						if (!board[d8] && !board[b8] && !board[c8])
						{
							if (!is_square_attacked(e8, white) && !is_square_attacked(d8, white))
								printf("black queenside castle\n");
						}
					}
				}
			}
			
			if (!side ? board[square] == N : board[square] == n)
			{
				for (int dir = 0; dir < 8; dir++)
				{
					int to_square = square + knight_offsets[dir];
					
					int piece = board[to_square];
					
					if (!(to_square & 0x88))
					{
						if (!side ? (!piece || (piece >= 7 && piece <= 12)) : (!piece || (piece >= 1 && piece <= 6)))
						{
							if (piece)
								printf("%s%s capture \n", square_to_coords[square], square_to_coords[to_square]);
							else
								printf("%s%s \n", square_to_coords[square], square_to_coords[to_square]);
						}
					}
				}
			}
			
			if (!side ? board[square] == K : board[square] == k)
			{
				for (int dir = 0; dir < 8; dir++)
				{
					int to_square = square + king_offsets[dir];
					
					int piece = board[to_square];
					
					if (!(to_square & 0x88))
					{
						if (!side ? (!piece || (piece >= 7 && piece <= 12)) : (!piece || (piece >= 1 && piece <= 6)))
						{
							if (piece)
								printf("%s%s capture \n", square_to_coords[square], square_to_coords[to_square]);
							else
								printf("%s%s \n", square_to_coords[square], square_to_coords[to_square]);
						}
					}
				}
			}
		}
	}
}

int main()
{
	parse_fen(tricky_position);
	print_board();
	generate_moves();
	
	return 0;
}
