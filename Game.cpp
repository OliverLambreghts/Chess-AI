#include "pch.h"
#include "Game.h"
#include <algorithm>
Game::Game(const Window& window)
	: m_Grid{ 8,8, window.width, window.height },
	m_SelectedIndex{ -1 },
	m_SelectedMove{ -1 },
	m_WhiteScore{ 1290 },
	m_BlackScore{ 1290 }
{
	Initialize();

}
Game::~Game()
{
	Cleanup();
}
void Game::InitializeTextures()
{
	for (int i = -6; i < 7; i++)
	{
		if (i != 0)
		{
			m_Textures.emplace(i, Texture("Resources/" + std::to_string(i) + ".png"));
		}
	}
}

void Game::Initialize()
{
	InitializeTextures();
	PrintBoard();
}
void Game::Cleanup()
{

}
void Game::Update(float elapsedSec)
{
	// Check keyboard state
	//const Uint8 *pStates = SDL_GetKeyboardState( nullptr );
	//if ( pStates[SDL_SCANCODE_RIGHT] )
	//{
	//	std::cout << "Right arrow key is down\n";
	//}
	//if ( pStates[SDL_SCANCODE_LEFT] && pStates[SDL_SCANCODE_UP])
	//{
	//	std::cout << "Left and up arrow keys are down\n";
	//}
	if (m_GameOver) return;
	if (m_WhiteToPlay)
		PlayMove(m_SelectedIndex, m_SelectedMove);
	else
	{
		int boardCopy[8][8]{ 0 };
		for (int i{}; i < 8; ++i)
		{
			for (int j{}; j < 8; ++j)
			{
				boardCopy[i][j] = m_Board[i][j];
			}
		}
		int alpha{ -99999 }, beta{ 99999 };
		Minimax(boardCopy, m_Depth, false, m_BlackScore, m_WhiteScore, m_WhiteToPlay, m_SelectedIndex, m_SelectedMove, m_HasWhiteKingMoved, m_HasBlackKingMoved,
			m_HasWhiteRightRookMoved, m_HasWhiteLeftRookMoved, m_HasBlackRightRookMoved, m_HasBlackLeftRookMoved, m_WhiteThreats, m_BlackThreats, m_GameOver, m_WhiteInCheck,
			m_BlackInCheck, alpha, beta);
	}
}

void Game::CheckMate()
{
	if (m_GameOver) return;
	bool whiteInMate{ true }, blackInMate{ true };
	// Check if white's in mate
	for (int i{}; i < 63; ++i)
	{
		if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] > 0)
		{
			if (!GetAllLegalMoves(i).empty())
				whiteInMate = false;
		}
	}
	if (whiteInMate)
	{
		m_GameOver = true;
		std::cout << "White is in checkmate\n";
		return;
	}
	// Check if black's in mate
	for (int i{}; i < 63; ++i)
	{
		if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] < 0)
		{
			if (!GetAllLegalMoves(i).empty())
				blackInMate = false;
		}
	}
	if (blackInMate)
	{
		m_GameOver = true;
		std::cout << "Black is in checkmate\n";
		return;
	}

	// Print if in check
	int whiteKing{};
	for (int i{}; i < 63; ++i)
	{
		// Find White's king index
		if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == 5)
		{
			whiteKing = i;
			break;
		}
	}
	// Als de witte king niet meer in een threat tile staat true returnen
	if (m_BlackThreats[m_Grid.GetRowFromIdx(whiteKing)][m_Grid.GetColFromIdx(whiteKing)] != 0)
	{
		m_WhiteInCheck = true;
		std::cout << "White's king is in check\n";
		return;
	}
	m_WhiteInCheck = false;

		int blackKing{};
	for (int i{}; i < 63; ++i)
	{
		// Find Black's king index
		if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == -5)
		{
			blackKing = i;
			break;
		}
	}
	if (m_WhiteThreats[m_Grid.GetRowFromIdx(blackKing)][m_Grid.GetColFromIdx(blackKing)] != 0)
	{
		m_BlackInCheck = true;
		std::cout << "Black's king is in check\n";
		return;
	}
	m_BlackInCheck = false;
}

void Game::CheckMate(int board[8][8])
{
	if (m_GameOver) return;
	bool whiteInMate{ true }, blackInMate{ true };
	// Check if white's in mate
	for (int i{}; i < 63; ++i)
	{
		if (board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] > 0)
		{
			if (!GetAllLegalMoves(i, board, m_Depth).empty())
				whiteInMate = false;
		}
	}
	if (whiteInMate)
	{
		m_GameOver = true;
		//std::cout << "White is in checkmate\n";
		return;
	}
	// Check if black's in mate
	for (int i{}; i < 63; ++i)
	{
		if (board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] < 0)
		{
			if (!GetAllLegalMoves(i, board, m_Depth).empty())
				blackInMate = false;
		}
	}
	if (blackInMate)
	{
		m_GameOver = true;
		//std::cout << "Black is in checkmate\n";
		return;
	}

	// Print if in check
	int whiteKing{};
	for (int i{}; i < 63; ++i)
	{
		// Find White's king index
		if (board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == 5)
		{
			whiteKing = i;
			break;
		}
	}
	// Als de witte king niet meer in een threat tile staat true returnen
	if (m_BlackThreats[m_Grid.GetRowFromIdx(whiteKing)][m_Grid.GetColFromIdx(whiteKing)] != 0)
	{
		m_WhiteInCheck = true;
		//std::cout << "White's king is in check\n";
		return;
	}
	m_WhiteInCheck = false;

	int blackKing{};
	for (int i{}; i < 63; ++i)
	{
		// Find Black's king index
		if (board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == -5)
		{
			blackKing = i;
			break;
		}
	}
	if (m_WhiteThreats[m_Grid.GetRowFromIdx(blackKing)][m_Grid.GetColFromIdx(blackKing)] != 0)
	{
		m_BlackInCheck = true;
		//std::cout << "Black's king is in check\n";
		return;
	}
	m_BlackInCheck = false;
}

void Game::PlayMove(int startIdx, int destIdx)
{
	if (m_SelectedIndex == -1 || m_SelectedMove == -1) return;
	if (m_WhiteToPlay && m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] < 0)
	{
		m_SelectedIndex = -1;
		m_SelectedMove = -1;
		return;
	}
	if (!m_WhiteToPlay && m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] > 0)
	{
		m_SelectedIndex = -1;
		m_SelectedMove = -1;
		return;
	}
	auto legalMoves = GetAllLegalMoves(m_SelectedIndex);
	if (legalMoves.empty())
	{
		m_SelectedIndex = -1;
		m_SelectedMove = -1;
		return;
	}
	if (std::find(legalMoves.begin(), legalMoves.end(), m_SelectedMove) == legalMoves.end())
	{
		if (m_SelectedMove > 0)
		{
			m_SelectedIndex = -1;
			m_SelectedMove = -1;
		}
		return;
	}
	MovePiece(m_SelectedIndex, m_SelectedMove);
	
	CheckCastlingMovement();

	UpdateThreats();
	//PrintThreatMatrices();
	
	m_SelectedIndex = -1;
	m_SelectedMove = -1;
	m_WhiteToPlay = !m_WhiteToPlay;

	CheckMate();
}

void Game::PlayMove(int startIdx, int destIdx, int board[8][8])
{
	if (m_SelectedIndex == -1) return;
	/*if (m_WhiteToPlay && board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] < 0)
	{
		m_SelectedIndex = -1;
		m_SelectedMove = -1;
		return;
	}*/
	/*if (!m_WhiteToPlay && board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] > 0)
	{
		m_SelectedIndex = -1;
		m_SelectedMove = -1;
		return;
	}*/
	auto legalMoves = GetAllLegalMoves(m_SelectedIndex, board, m_Depth);
	if (legalMoves.empty())
	{
		m_SelectedIndex = -1;
		m_SelectedMove = -1;
		return;
	}
	if (std::find(legalMoves.begin(), legalMoves.end(), m_SelectedMove) == legalMoves.end())
	{
		if (m_SelectedMove > 0)
		{
			m_SelectedIndex = -1;
			m_SelectedMove = -1;
		}
		return;
	}
	MovePiece(m_SelectedIndex, m_SelectedMove, board);

	CheckCastlingMovement(board);

	UpdateThreats(board);
	//PrintThreatMatrices();

	m_SelectedIndex = -1;
	m_SelectedMove = -1;
	m_WhiteToPlay = !m_WhiteToPlay;

	CheckMate(board);
}

void Game::CheckCastlingMovement()
{
	// Castling checks
	// White king has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == 5)
		if (!m_HasWhiteKingMoved)
			m_HasWhiteKingMoved = true;
	// Black king has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == -5)
		if (!m_HasBlackKingMoved)
			m_HasBlackKingMoved = true;
	// White right rook has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == 1 && m_SelectedIndex == 7)
		if (!m_HasWhiteRightRookMoved)
			m_HasWhiteRightRookMoved = true;
	// White left rook has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == 1 && m_SelectedIndex == 0)
		if (!m_HasWhiteLeftRookMoved)
			m_HasWhiteLeftRookMoved = true;
	// Black right rook has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == -1 && m_SelectedIndex == 63)
		if (!m_HasBlackRightRookMoved)
			m_HasBlackRightRookMoved = true;
	// Black left rook has moved
	if (m_Board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == -1 && m_SelectedIndex == 56)
		if (!m_HasBlackLeftRookMoved)
			m_HasBlackLeftRookMoved = true;
}

void Game::CheckCastlingMovement(int board[8][8])
{
	// Castling checks
	// White king has moved
	if (board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == 5)
		if (!m_HasWhiteKingMoved)
			m_HasWhiteKingMoved = true;
	// Black king has moved
	if (board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == -5)
		if (!m_HasBlackKingMoved)
			m_HasBlackKingMoved = true;
	// White right rook has moved
	if (board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == 1 && m_SelectedIndex == 7)
		if (!m_HasWhiteRightRookMoved)
			m_HasWhiteRightRookMoved = true;
	// White left rook has moved
	if (board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == 1 && m_SelectedIndex == 0)
		if (!m_HasWhiteLeftRookMoved)
			m_HasWhiteLeftRookMoved = true;
	// Black right rook has moved
	if (board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == -1 && m_SelectedIndex == 63)
		if (!m_HasBlackRightRookMoved)
			m_HasBlackRightRookMoved = true;
	// Black left rook has moved
	if (board[m_Grid.GetRowFromIdx(m_SelectedMove)][m_Grid.GetColFromIdx(m_SelectedMove)] == -1 && m_SelectedIndex == 56)
		if (!m_HasBlackLeftRookMoved)
			m_HasBlackLeftRookMoved = true;
}

void Game::UpdateThreats()
{
	// Update threat matrices:
	// First reset the threat matrices to zero matrices
	for (int i{}; i < 8; ++i)
	{
		for (int j{}; j < 8; ++j)
		{
			m_WhiteThreats[i][j] = 0;
			m_BlackThreats[i][j] = 0;
		}
	}
	// Then refill it with new values
	for (int i{}; i < 63; ++i)
	{
		int piece = m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)];
		// Black threat
		if (piece < 0)
		{
			// If piece is a pawn treat it differently (they can only take diagonally)
			if (piece == -6)
			{
				if(i - 9 >= 0)
					if (m_Grid.GetColFromIdx(i - 9) < m_Grid.GetColFromIdx(i))
						if (i - 9 >= 0 && m_Board[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] >= 0)
							m_BlackThreats[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] = -1;
				if(i - 7 >= 0)
					if (m_Grid.GetColFromIdx(i - 7) > m_Grid.GetColFromIdx(i))
						if (i - 7 >= 0 && m_Board[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] >= 0)
							m_BlackThreats[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] = -1;
				continue;
			}
			for (auto legalMove : GetAllLegalMoves(i))
			{
				m_BlackThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = -1;
			}
		}
		// White threat
		else if (piece > 0)
		{
			if (piece == 6)
			{
				if(i + 9 <= 63)
					if (m_Grid.GetColFromIdx(i + 9) > m_Grid.GetColFromIdx(i))
						if (i + 9 <= 63 && m_Board[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] <= 0)
							m_WhiteThreats[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] = 1;
				if(i + 7 <= 63)
					if (m_Grid.GetColFromIdx(i + 7) < m_Grid.GetColFromIdx(i))
						if (i + 7 <= 63 && m_Board[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] <= 0)
							m_WhiteThreats[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] = 1;
				continue;
			}
			for (auto legalMove : GetAllLegalMoves(i))
			{
				m_WhiteThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = 1;
			}
		}
	}
}

void Game::UpdateThreats(int board[8][8])
{
	// Update threat matrices:
	// First reset the threat matrices to zero matrices
	for (int i{}; i < 8; ++i)
	{
		for (int j{}; j < 8; ++j)
		{
			m_WhiteThreats[i][j] = 0;
			m_BlackThreats[i][j] = 0;
		}
	}
	// Then refill it with new values
	for (int i{}; i < 63; ++i)
	{
		int piece = board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)];
		// Black threat
		if (piece < 0)
		{
			// If piece is a pawn treat it differently (they can only take diagonally)
			if (piece == -6)
			{
				if (i - 9 >= 0)
					if (m_Grid.GetColFromIdx(i - 9) < m_Grid.GetColFromIdx(i))
						if (i - 9 >= 0 && board[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] >= 0)
							m_BlackThreats[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] = -1;
				if (i - 7 >= 0)
					if (m_Grid.GetColFromIdx(i - 7) > m_Grid.GetColFromIdx(i))
						if (i - 7 >= 0 && board[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] >= 0)
							m_BlackThreats[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] = -1;
				continue;
			}
			for (auto legalMove : GetAllLegalMoves(i, board, m_Depth))
			{
				m_BlackThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = -1;
			}
		}
		// White threat
		else if (piece > 0)
		{
			if (piece == 6)
			{
				if (i + 9 <= 63)
					if (m_Grid.GetColFromIdx(i + 9) > m_Grid.GetColFromIdx(i))
						if (i + 9 <= 63 && board[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] <= 0)
							m_WhiteThreats[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] = 1;
				if (i + 7 <= 63)
					if (m_Grid.GetColFromIdx(i + 7) < m_Grid.GetColFromIdx(i))
						if (i + 7 <= 63 && board[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] <= 0)
							m_WhiteThreats[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] = 1;
				continue;
			}
			for (auto legalMove : GetAllLegalMoves(i, board, m_Depth))
			{
				m_WhiteThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = 1;
			}
		}
	}
}

bool Game::EvaluateThreats(int startIdx, int newIdx, bool isWhite) const
{
	int blackThreats[8][8]{ 0 };
	int whiteThreats[8][8]{ 0 };
	int board[8][8]{ 0 };
	for (int i{}; i < 8; ++i)
	{
		for (int j{}; j < 8; ++j)
		{
			board[i][j] = m_Board[i][j];
		}
	}
	// Now adjust the piece based on its legal move to evaluate threats
	board[m_Grid.GetRowFromIdx(newIdx)][m_Grid.GetColFromIdx(newIdx)] = board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
	board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;

	for (int i{}; i < 63; ++i)
	{
		int piece = board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)];
		// Enkel zwarte threats evalueren als wit een move wilt spelen
		if (isWhite)
		{
			if (piece < 0)
			{
				if (piece == -6)
				{
					if (i - 9 >= 0)
						if (m_Grid.GetColFromIdx(i - 9) < m_Grid.GetColFromIdx(i))
							if (i - 9 >= 0 && board[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] >= 0)
								blackThreats[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] = -1;
					if (i - 7 >= 0)
						if (m_Grid.GetColFromIdx(i - 7) > m_Grid.GetColFromIdx(i))
							if (i - 7 >= 0 && board[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] >= 0)
								blackThreats[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] = -1;
					continue;
				}
				for (auto legalMove : GetAllLegalMoves(i, board, m_Depth))
				{
					blackThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = -1;
				}
			}
		}
		// Enkel witte threats evalueren als zwart een move wilt maken
		else
		{
			if (piece > 0)
			{
				if (piece == 6)
				{
					if (i + 9 <= 63)
						if (m_Grid.GetColFromIdx(i + 9) > m_Grid.GetColFromIdx(i))
							if (i + 9 <= 63 && board[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] <= 0)
								whiteThreats[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] = 1;
					if (i + 7 <= 63)
						if (m_Grid.GetColFromIdx(i + 7) < m_Grid.GetColFromIdx(i))
							if (i + 7 <= 63 && board[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] <= 0)
								whiteThreats[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] = 1;
					continue;
				}
				for (auto legalMove : GetAllLegalMoves(i, board, m_Depth))
				{
					whiteThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = 1;
				}
			}
		}
	}
	//	int piece = board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)];
	//	// Black threat
	//	if (piece < 0)
	//	{
	//		// If piece is a pawn treat it differently (they can only take diagonally)
	//		if (piece == -6)
	//		{
	//			if(i - 9 >= 0)
	//				if (m_Grid.GetColFromIdx(i - 9) < m_Grid.GetColFromIdx(i))
	//					if (i - 9 >= 0 && board[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] >= 0)
	//						blackThreats[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] = -1;
	//			if(i - 7 >= 0)
	//				if (m_Grid.GetColFromIdx(i - 7) > m_Grid.GetColFromIdx(i))
	//					if (i - 7 >= 0 && board[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] >= 0)
	//						blackThreats[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] = -1;
	//			continue;
	//		}
	//		for (auto legalMove : GetAllLegalMoves(i, board, m_Depth))
	//		{
	//			blackThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = -1;
	//		}
	//	}
	//	// White threat
	//	else if (piece > 0)
	//	{
	//		if (piece == 6)
	//		{
	//			if(i + 9 <= 63)
	//				if (m_Grid.GetColFromIdx(i + 9) > m_Grid.GetColFromIdx(i))
	//					if (i + 9 <= 63 && board[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] <= 0)
	//						whiteThreats[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] = 1;
	//			if(i + 7 <= 63)
	//				if (m_Grid.GetColFromIdx(i + 7) < m_Grid.GetColFromIdx(i))
	//					if (i + 7 <= 63 && board[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] <= 0)
	//						whiteThreats[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] = 1;
	//			continue;
	//		}
	//		for (auto legalMove : GetAllLegalMoves(i, board, m_Depth))
	//		{
	//			whiteThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = 1;
	//		}
	//	}
	//}

	if (isWhite)
	{
		int whiteKing{};
		for (int i{}; i < 63; ++i)
		{
			// Find White's king index
			if (board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == 5)
			{
				whiteKing = i;
				break;
			}
		}
		// Als de witte king niet meer in een threat tile staat true returnen
		if (blackThreats[m_Grid.GetRowFromIdx(whiteKing)][m_Grid.GetColFromIdx(whiteKing)] != 0)
		{
			return false;
		}
	}

	if (!isWhite)
	{
		int blackKing{};
		for (int i{}; i < 63; ++i)
		{
			// Find Black's king index
			if (board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == -5)
			{
				blackKing = i;
				break;
			}
		}
		if (whiteThreats[m_Grid.GetRowFromIdx(blackKing)][m_Grid.GetColFromIdx(blackKing)] != 0)
		{
			return false;
		}
	}

	return true;
}

bool Game::EvaluateThreats(int startIdx, int newIdx, bool isWhite, int board[8][8], int depth) const
{
	int blackThreats[8][8]{ 0 };
	int whiteThreats[8][8]{ 0 };
	int localBoard[8][8]{ 0 };
	for (int i{}; i < 8; ++i)
	{
		for (int j{}; j < 8; ++j)
		{
			localBoard[i][j] = board[i][j];
		}
	}
	// Now adjust the piece based on its legal move to evaluate threats
	localBoard[m_Grid.GetRowFromIdx(newIdx)][m_Grid.GetColFromIdx(newIdx)] = localBoard[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
	localBoard[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;

	for (int i{}; i < 63; ++i)
	{
		int piece = localBoard[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)];
		if (isWhite)
		{
			if (piece < 0)
			{
				// If piece is a pawn treat it differently (they can only take diagonally)
				if (piece == -6)
				{
					if (i - 9 >= 0)
						if (m_Grid.GetColFromIdx(i - 9) < m_Grid.GetColFromIdx(i))
							if (i - 9 >= 0 && localBoard[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] >= 0)
								blackThreats[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] = -1;
					if (i - 7 >= 0)
						if (m_Grid.GetColFromIdx(i - 7) > m_Grid.GetColFromIdx(i))
							if (i - 7 >= 0 && localBoard[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] >= 0)
								blackThreats[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] = -1;
					continue;
				}
				for (auto legalMove : GetAllLegalMoves(i, localBoard, depth))
				{
					blackThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = -1;
				}
			}
		}
		else
		{
			if (piece > 0)
			{
				if (piece == 6)
				{
					if (i + 9 <= 63)
						if (m_Grid.GetColFromIdx(i + 9) > m_Grid.GetColFromIdx(i))
							if (i + 9 <= 63 && localBoard[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] <= 0)
								whiteThreats[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] = 1;
					if (i + 7 <= 63)
						if (m_Grid.GetColFromIdx(i + 7) < m_Grid.GetColFromIdx(i))
							if (i + 7 <= 63 && localBoard[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] <= 0)
								whiteThreats[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] = 1;
					continue;
				}
				for (auto legalMove : GetAllLegalMoves(i, localBoard, depth))
				{
					whiteThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = 1;
				}
			}
		}
	}
	//	int piece = localBoard[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)];
	//	// Black threat
	//	if (piece < 0)
	//	{
	//		// If piece is a pawn treat it differently (they can only take diagonally)
	//		if (piece == -6)
	//		{
	//			if (i - 9 >= 0)
	//				if (m_Grid.GetColFromIdx(i - 9) < m_Grid.GetColFromIdx(i))
	//					if (i - 9 >= 0 && localBoard[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] >= 0)
	//						blackThreats[m_Grid.GetRowFromIdx(i - 9)][m_Grid.GetColFromIdx(i - 9)] = -1;
	//			if (i - 7 >= 0)
	//				if (m_Grid.GetColFromIdx(i - 7) > m_Grid.GetColFromIdx(i))
	//					if (i - 7 >= 0 && localBoard[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] >= 0)
	//						blackThreats[m_Grid.GetRowFromIdx(i - 7)][m_Grid.GetColFromIdx(i - 7)] = -1;
	//			continue;
	//		}
	//		for (auto legalMove : GetAllLegalMoves(i, localBoard, depth))
	//		{
	//			blackThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = -1;
	//		}
	//	}
	//	// White threat
	//	else if (piece > 0)
	//	{
	//		if (piece == 6)
	//		{
	//			if (i + 9 <= 63)
	//				if (m_Grid.GetColFromIdx(i + 9) > m_Grid.GetColFromIdx(i))
	//					if (i + 9 <= 63 && localBoard[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] <= 0)
	//						whiteThreats[m_Grid.GetRowFromIdx(i + 9)][m_Grid.GetColFromIdx(i + 9)] = 1;
	//			if (i + 7 <= 63)
	//				if (m_Grid.GetColFromIdx(i + 7) < m_Grid.GetColFromIdx(i))
	//					if (i + 7 <= 63 && localBoard[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] <= 0)
	//						whiteThreats[m_Grid.GetRowFromIdx(i + 7)][m_Grid.GetColFromIdx(i + 7)] = 1;
	//			continue;
	//		}
	//		for (auto legalMove : GetAllLegalMoves(i, localBoard, depth))
	//		{
	//			whiteThreats[m_Grid.GetRowFromIdx(legalMove)][m_Grid.GetColFromIdx(legalMove)] = 1;
	//		}
	//	}
	//}

	if (isWhite)
	{
		int whiteKing{};
		for (int i{}; i < 63; ++i)
		{
			// Find White's king index
			if (localBoard[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == 5)
			{
				whiteKing = i;
				break;
			}
		}
		// Als de witte king niet meer in een threat tile staat true returnen
		if (blackThreats[m_Grid.GetRowFromIdx(whiteKing)][m_Grid.GetColFromIdx(whiteKing)] != 0)
		{
			return false;
		}
	}

	if (!isWhite)
	{
		int blackKing{};
		for (int i{}; i < 63; ++i)
		{
			// Find Black's king index
			if (localBoard[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == -5)
			{
				blackKing = i;
				break;
			}
		}
		if (whiteThreats[m_Grid.GetRowFromIdx(blackKing)][m_Grid.GetColFromIdx(blackKing)] != 0)
		{
			return false;
		}
	}

	return true;
}

void Game::MovePiece(int startIdx, int destIdx)
{
	AdjustScores(destIdx);

	if (Castle(startIdx, destIdx))
		return;

	if (Promote(startIdx, destIdx))
		return;

	CheckEnPassant(startIdx, destIdx);

	m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
	m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
}

void Game::MovePiece(int startIdx, int destIdx, int board[8][8])
{
	AdjustScores(destIdx, board);

	if (Castle(startIdx, destIdx, board))
		return;

	if (Promote(startIdx, destIdx, board))
		return;

	CheckEnPassant(startIdx, destIdx, board);

	board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
	board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
}

void Game::AdjustScores(int destIdx)
{
	// Hier captures checken en de scores van de players aanpassen:
	// Als de captured piece een zwarte piece is
	int capturedPiece = m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)];
	if (capturedPiece < 0)
	{
		switch (capturedPiece)
		{
		case -6:
			m_BlackScore -= int(Weights::Pawn);
			break;
		case -4:
			m_BlackScore -= int(Weights::Queen);
			break;
		case -3:
			m_BlackScore -= int(Weights::Bishop);
			break;
		case -2:
			m_BlackScore -= int(Weights::Knight);
			break;
		case -1:
			m_BlackScore -= int(Weights::Rook);
			break;
		};
		//std::cout << "Black's new score: " << m_BlackScore << '\n';
	}
	// Als de captured piece een witte piece is
	else if (capturedPiece > 0)
	{
		switch (capturedPiece)
		{
		case 6:
			m_WhiteScore -= int(Weights::Pawn);
			break;
		case 4:
			m_WhiteScore -= int(Weights::Queen);
			break;
		case 3:
			m_WhiteScore -= int(Weights::Bishop);
			break;
		case 2:
			m_WhiteScore -= int(Weights::Knight);
			break;
		case 1:
			m_WhiteScore -= int(Weights::Rook);
			break;
		};
		//std::cout << "White's new score: " << m_WhiteScore << '\n';
	}
}

void Game::AdjustScores(int destIdx, int board[8][8])
{
	// Hier captures checken en de scores van de players aanpassen:
	// Als de captured piece een zwarte piece is
	int capturedPiece = board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)];
	if (capturedPiece < 0)
	{
		switch (capturedPiece)
		{
		case -6:
			m_BlackScore -= int(Weights::Pawn);
			break;
		case -4:
			m_BlackScore -= int(Weights::Queen);
			break;
		case -3:
			m_BlackScore -= int(Weights::Bishop);
			break;
		case -2:
			m_BlackScore -= int(Weights::Knight);
			break;
		case -1:
			m_BlackScore -= int(Weights::Rook);
			break;
		};
		//std::cout << "Black's new score: " << m_BlackScore << '\n';
	}
	// Als de captured piece een witte piece is
	else if (capturedPiece > 0)
	{
		switch (capturedPiece)
		{
		case 6:
			m_WhiteScore -= int(Weights::Pawn);
			break;
		case 4:
			m_WhiteScore -= int(Weights::Queen);
			break;
		case 3:
			m_WhiteScore -= int(Weights::Bishop);
			break;
		case 2:
			m_WhiteScore -= int(Weights::Knight);
			break;
		case 1:
			m_WhiteScore -= int(Weights::Rook);
			break;
		};
		//std::cout << "White's new score: " << m_WhiteScore << '\n';
	}
}

bool Game::Castle(int startIdx, int destIdx)
{
	// Check for castle:
	// White king
	if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == 5)
	{
		// Right castle
		if (destIdx - startIdx == 2)
		{
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			m_Board[m_Grid.GetRowFromIdx(destIdx - 1)][m_Grid.GetColFromIdx(destIdx - 1)] = 1;
			m_Board[m_Grid.GetRowFromIdx(7)][m_Grid.GetColFromIdx(7)] = 0;
			return true;
		}
		// Left castle
		else if (startIdx - destIdx == 2)
		{
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			m_Board[m_Grid.GetRowFromIdx(destIdx + 1)][m_Grid.GetColFromIdx(destIdx + 1)] = 1;
			m_Board[m_Grid.GetRowFromIdx(0)][m_Grid.GetColFromIdx(0)] = 0;
			return true;
		}
	}
	// Black king
	if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == -5)
	{
		// Right castle
		if (destIdx - startIdx == 2)
		{
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			m_Board[m_Grid.GetRowFromIdx(destIdx - 1)][m_Grid.GetColFromIdx(destIdx - 1)] = -1;
			m_Board[m_Grid.GetRowFromIdx(63)][m_Grid.GetColFromIdx(63)] = 0;
			return true;
		}
		// Left castle
		else if (startIdx - destIdx == 2)
		{
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			m_Board[m_Grid.GetRowFromIdx(destIdx + 1)][m_Grid.GetColFromIdx(destIdx + 1)] = -1;
			m_Board[m_Grid.GetRowFromIdx(56)][m_Grid.GetColFromIdx(56)] = 0;
			return true;
		}
	}
	return false;
}

bool Game::Castle(int startIdx, int destIdx, int board[8][8])
{
	// Check for castle:
	// White king
	if (board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == 5)
	{
		// Right castle
		if (destIdx - startIdx == 2)
		{
			board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			board[m_Grid.GetRowFromIdx(destIdx - 1)][m_Grid.GetColFromIdx(destIdx - 1)] = 1;
			board[m_Grid.GetRowFromIdx(7)][m_Grid.GetColFromIdx(7)] = 0;
			return true;
		}
		// Left castle
		else if (startIdx - destIdx == 2)
		{
			board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			board[m_Grid.GetRowFromIdx(destIdx + 1)][m_Grid.GetColFromIdx(destIdx + 1)] = 1;
			board[m_Grid.GetRowFromIdx(0)][m_Grid.GetColFromIdx(0)] = 0;
			return true;
		}
	}
	// Black king
	if (board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == -5)
	{
		// Right castle
		if (destIdx - startIdx == 2)
		{
			board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			board[m_Grid.GetRowFromIdx(destIdx - 1)][m_Grid.GetColFromIdx(destIdx - 1)] = -1;
			board[m_Grid.GetRowFromIdx(63)][m_Grid.GetColFromIdx(63)] = 0;
			return true;
		}
		// Left castle
		else if (startIdx - destIdx == 2)
		{
			board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)];
			board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			board[m_Grid.GetRowFromIdx(destIdx + 1)][m_Grid.GetColFromIdx(destIdx + 1)] = -1;
			board[m_Grid.GetRowFromIdx(56)][m_Grid.GetColFromIdx(56)] = 0;
			return true;
		}
	}
	return false;
}

bool Game::Promote(int startIdx, int destIdx)
{
	// Check if piece is a pawn and check if it can promote if yes
	// White pawn
	if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == 6)
	{
		// If pawn has reached the top row
		if (m_Grid.GetRowFromIdx(destIdx) == 7)
		{
			int input{};
			std::cout << "Promotion:\n1 for Rook\n2 for Knight\n3 for Bishop\n4 for Queen\n";
			while (input <= 0 || input > 4)
			{
				std::cin >> input;
			}
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = input;
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			return true;
		}
	}
	// Black paw,
	if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == -6)
	{
		// If pawn has reached the top row
		if (m_Grid.GetRowFromIdx(destIdx) == 0)
		{
			int input{};
			std::cout << "Promotion:\n1 for Rook\n2 for Knight\n3 for Bishop\n4 for Queen\n";
			while (input <= 0 || input > 4)
			{
				std::cin >> input;
			}
			m_Board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = -input;
			m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			return true;
		}
	}
	return false;
}

bool Game::Promote(int startIdx, int destIdx, int board[8][8])
{
	// Check if piece is a pawn and check if it can promote if yes
	// White pawn
	if (board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == 6)
	{
		// If pawn has reached the top row
		if (m_Grid.GetRowFromIdx(destIdx) == 7)
		{
			int input{4};
			/*std::cout << "Promotion:\n1 for Rook\n2 for Knight\n3 for Bishop\n4 for Queen\n";
			while (input <= 0 || input > 4)
			{
				std::cin >> input;
			}*/
			board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = input;
			board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			return true;
		}
	}
	// Black paw,
	if (board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == -6)
	{
		// If pawn has reached the top row
		if (m_Grid.GetRowFromIdx(destIdx) == 0)
		{
			int input{4};
			/*std::cout << "Promotion:\n1 for Rook\n2 for Knight\n3 for Bishop\n4 for Queen\n";
			while (input <= 0 || input > 4)
			{
				std::cin >> input;
			}*/
			board[m_Grid.GetRowFromIdx(destIdx)][m_Grid.GetColFromIdx(destIdx)] = -input;
			board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] = 0;
			return true;
		}
	}
	return false;
}

void Game::CheckEnPassant(int startIdx, int destIdx)
{
	// En passant
	// White pawn
	if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == 6)
	{
		// Take in passing
		if (m_EnPassant)
		{
			if (destIdx - 8 == m_EnPassantIdx)
			{
				m_Board[m_Grid.GetRowFromIdx(m_EnPassantIdx)][m_Grid.GetColFromIdx(m_EnPassantIdx)] = 0;
			}
		}

		if (destIdx - startIdx == 16)
		{
			// Activate en passant
			m_EnPassant = true;
			m_EnPassantIdx = destIdx;
		}
		else
			m_EnPassant = false;
	}
	// Black paw,
	else if (m_Board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == -6)
	{
		if (m_EnPassant)
		{
			if (destIdx + 8 == m_EnPassantIdx)
			{
				m_Board[m_Grid.GetRowFromIdx(m_EnPassantIdx)][m_Grid.GetColFromIdx(m_EnPassantIdx)] = 0;
			}
		}

		if (startIdx - destIdx == 16)
		{
			m_EnPassant = true;
			m_EnPassantIdx = destIdx;
		}
		else
			m_EnPassant = false;
	}
	else
	{
		m_EnPassant = false;
	}
}

void Game::CheckEnPassant(int startIdx, int destIdx, int board[8][8])
{
	// En passant
	// White pawn
	if (board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == 6)
	{
		// Take in passing
		if (m_EnPassant)
		{
			if (destIdx - 8 == m_EnPassantIdx)
			{
				board[m_Grid.GetRowFromIdx(m_EnPassantIdx)][m_Grid.GetColFromIdx(m_EnPassantIdx)] = 0;
			}
		}

		if (destIdx - startIdx == 16)
		{
			// Activate en passant
			m_EnPassant = true;
			m_EnPassantIdx = destIdx;
		}
		else
			m_EnPassant = false;
	}
	// Black paw,
	else if (board[m_Grid.GetRowFromIdx(startIdx)][m_Grid.GetColFromIdx(startIdx)] == -6)
	{
		if (m_EnPassant)
		{
			if (destIdx + 8 == m_EnPassantIdx)
			{
				board[m_Grid.GetRowFromIdx(m_EnPassantIdx)][m_Grid.GetColFromIdx(m_EnPassantIdx)] = 0;
			}
		}

		if (startIdx - destIdx == 16)
		{
			m_EnPassant = true;
			m_EnPassantIdx = destIdx;
		}
		else
			m_EnPassant = false;
	}
	else
	{
		m_EnPassant = false;
	}
}

void Game::PrintBoard()
{
	for (int i = 7; i >= 0; i--)
	{
		for (int j = 0; j < 8; j++) //reverse because the 0,0 index of the array is bottom left of the visual board
		{
			if (m_Board[i][j] >= 0)
				std::cout << " ";
			std::cout << m_Board[i][j] << " ";
		}
		std::cout << "\n";
	}
}

void Game::PrintThreatMatrices()
{
	// Print threat matrices
	std::cout << "White threats:\n";
	for (int i = 7; i >= 0; i--)
	{
		for (int j = 0; j < 8; j++) //reverse because the 0,0 index of the array is bottom left of the visual board
		{
			if (m_WhiteThreats[i][j] >= 0)
				std::cout << " ";
			std::cout << m_WhiteThreats[i][j] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "Black threats:\n";
	for (int i = 7; i >= 0; i--)
	{
		for (int j = 0; j < 8; j++) //reverse because the 0,0 index of the array is bottom left of the visual board
		{
			if (m_BlackThreats[i][j] >= 0)
				std::cout << " ";
			std::cout << m_BlackThreats[i][j] << " ";
		}
		std::cout << "\n";
	}
}

void Game::Draw() const
{
	ClearBackground();
	m_Grid.DrawGrid(true);
	// Draw selected tile red
	utils::SetColor(Color4f{ 1.f,0.f,0.f,1.f });
	if (m_SelectedIndex >= 0)
		utils::FillRect(m_Grid.GetCell(m_SelectedIndex));
	DrawPieces();
	DrawLegalMoves();
	DrawCheckedKing();
}

void Game::DrawCheckedKing() const
{
	// Draw king's tile red if in check
	int kingIdx{};
	if (m_BlackInCheck)
	{
		for (int i{}; i < 63; ++i)
		{
			if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == -5)
			{
				kingIdx = i;
				break;
			}
		}
	}
	else if (m_WhiteInCheck)
	{
		for (int i{}; i < 63; ++i)
		{
			if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == 5)
			{
				kingIdx = i;
				break;
			}
		}
	}
	if (m_WhiteInCheck || m_BlackInCheck)
	{
		utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 100.f / 255.f });
		utils::FillRect(m_Grid.GetCell(kingIdx));
	}
}

void Game::DrawPieces() const
{
	for (int i{ 0 }; i < 8; i++)
	{
		for (int j{ 0 }; j < 8; j++)
		{
			if (m_Board[i][j] != 0)
				m_Textures.at(m_Board[i][j]).Draw(Rectf{ m_Grid.GetPosFromIdx(convert2DTo1D(i,j)).x,m_Grid.GetPosFromIdx(convert2DTo1D(i,j)).y,size * 2,size * 2 }, Rectf{ 0,0,68,68 });
		}
	}
}
void Game::DrawLegalMoves() const
{
	for (int idx : GetAllLegalMoves(m_SelectedIndex))
	{
		utils::SetColor(Color4f{ 0.f, 0.f, 0.f, 100.f / 255.f });
		utils::FillEllipse(m_Grid.GetPosFromIdx(idx).x + (m_Grid.GetCellWidth() / 2), m_Grid.GetPosFromIdx(idx).y + (
			m_Grid.GetCellWidth() / 2), 20.f, 20.f);
	}
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{

}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		Point2f mousePos = Point2f{ float(e.x), float(e.y) };
		if (m_SelectedIndex == -1)
		{
			m_SelectedIndex = m_Grid.GetIndexFromPos(mousePos);
			//std::cout << "Index " << m_SelectedIndex << " selected\n";
		//	auto moves = GetAllLegalMoves(m_Grid.GetIndexFromPos(mousePos));
		//	std::cout << "Legal moves: ";
		//	std::copy(moves.begin(), moves.end(), std::ostream_iterator<int>{std::cout, " "});
		//	std::cout << '\n';
		}
		else
		{
			m_SelectedMove = m_Grid.GetIndexFromPos(mousePos);
		}
		break;
	}
}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Game::CheckCheck(int index, int king, std::vector<int>& moves) const
{
	// Check for check(mate)
	int kingIdx{};
	for (int i{}; i < 63; ++i)
	{
		// Find White's king index
		if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == king)
		{
			kingIdx = i;
			break;
		}
	}
	for (int i{}; i < 63; ++i)
	{
		// Als op deze tile een zwarte piece staat:
		int attacker = m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)];
		if (king > 0 && attacker < 0)
		{
			//auto attackerMoves = GetAllLegalMoves(i);
			// Als White's king in check staat
			//if (std::find(attackerMoves.begin(), attackerMoves.end(), kingIdx) != attackerMoves.end())
			//if(m_BlackThreats[m_Grid.GetRowFromIdx(kingIdx)][m_Grid.GetColFromIdx(kingIdx)] != 0)
			//{
				// Hier alle huidige legal moves van de pawn evalueren:
				std::vector<int>::iterator it{ moves.begin() };
				while (it != moves.end())
				{
					if (!EvaluateThreats(index, *it, true))
					{
						it = moves.erase(it);
						continue;
					}
					++it;
				}
			//}
		}
		else if (king < 0 && attacker > 0)
		{
			//auto attackerMoves = GetAllLegalMoves(i);
			// Als White's king in check staat
			//if (std::find(attackerMoves.begin(), attackerMoves.end(), kingIdx) != attackerMoves.end())
			//if(m_WhiteThreats[m_Grid.GetRowFromIdx(kingIdx)][m_Grid.GetColFromIdx(kingIdx)] != 0)
			//{
				// Hier alle huidige legal moves van de pawn evalueren:
				std::vector<int>::iterator it{ moves.begin() };
				while (it != moves.end())
				{
					if (!EvaluateThreats(index, *it, false))
					{
						it = moves.erase(it);
						continue;
					}
					++it;
				}
			//}
		}
	}
}

void Game::CheckCheck(int index, int king, std::vector<int>& moves, int board[8][8], int depth) const
{
	if (moves.empty()) return;
	// Check for check(mate)
	int kingIdx{};
	for (int i{}; i < 63; ++i)
	{
		// Find White's king index
		if (board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] == king)
		{
			kingIdx = i;
			break;
		}
	}
	for (int i{}; i < 63; ++i)
	{
		// Als op deze tile een zwarte piece staat:
		int attacker = board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)];
		if (king > 0 && attacker < 0)
		{
			//auto attackerMoves = GetAllLegalMoves(i);
			// Als White's king in check staat
			//if (std::find(attackerMoves.begin(), attackerMoves.end(), kingIdx) != attackerMoves.end())
			//if(m_BlackThreats[m_Grid.GetRowFromIdx(kingIdx)][m_Grid.GetColFromIdx(kingIdx)] != 0)
			//{
				// Hier alle huidige legal moves van de pawn evalueren:
			std::vector<int>::iterator it{ moves.begin() };
			while (it != moves.end())
			{
				if (!EvaluateThreats(index, *it, true, board, depth - 1))
				{
					it = moves.erase(it);
					continue;
				}
				++it;
			}
			//}
		}
		else if (king < 0 && attacker > 0)
		{
			//auto attackerMoves = GetAllLegalMoves(i);
			// Als White's king in check staat
			//if (std::find(attackerMoves.begin(), attackerMoves.end(), kingIdx) != attackerMoves.end())
			//if(m_WhiteThreats[m_Grid.GetRowFromIdx(kingIdx)][m_Grid.GetColFromIdx(kingIdx)] != 0)
			//{
				// Hier alle huidige legal moves van de pawn evalueren:
			std::vector<int>::iterator it{ moves.begin() };
			while (it != moves.end())
			{
				if (!EvaluateThreats(index, *it, false, board, depth - 1))
				{
					it = moves.erase(it);
					continue;
				}
				++it;
			}
			//}
		}
	}
}

// -------------------------------------------------------
//							AI
// -------------------------------------------------------

// -------------- HEURISTIC EVALUATION FUNCTION --------------
int Game::Evaluate(int board[8][8], int maximizingColor)
{
	// De maximizing color is de kleur die nu aan de zet is en die de best mogelijke
	// zet moet zoeken voor zichzelf
	// If the winning color is white
	if (maximizingColor > 0)
	{
		return m_WhiteScore - m_BlackScore;
	}
	// If the winning color is black
	return m_BlackScore - m_WhiteScore;
}

int Game::NewEvaluate(int blackScore, int whiteScore)
{
	return whiteScore - blackScore;
}

// -------------------- MINIMAX ALGORITHM --------------------

int Game::Minimax(int board[8][8], int depth, bool maximizingPlayer, int blackScore, int whiteScore, bool whiteToPlay, int selectedIdx, int selectedMove, bool hasWhiteKingMoved,
	bool hasBlackKingMoved,
	bool hasWhiteRightRookMoved,
	bool hasWhiteLeftRookMoved,
	bool hasBlackRightRookMoved,
	bool hasBlackLeftRookMoved,
	int whiteThreats[8][8],
	int blackThreats[8][8],
	bool gameOver,
	bool whiteInCheck,
	bool blackInCheck,
	int& alpha, int& beta)
{
	if (depth == 0 || m_GameOver)
	{
		return NewEvaluate(m_BlackScore, m_WhiteScore);
	}

	std::pair<int, int> bestMove{};
	// Zwart is de minimizingPlayer: zoekt naar de laagste evaluatie dus we gaan eerst checken voor zwart
	if (!maximizingPlayer)
	{
		std::vector<std::pair<int, int>> moves;
		std::vector<int> currentPieceMoves;
		int minEval = 99999;
		int currentEval{};
		// Eerst alle mogelijke moves voor zwarte verzamelen
		for (int i{}; i < 63; ++i)
		{
			// Als de piece op tile met idx i negatief is (en dus zwart is)
			if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] < 0)
			{
				currentPieceMoves = GetAllLegalMoves(i, board, depth);
				// Als die piece geen legal moves heeft naar de volgende piece gaan
				if (currentPieceMoves.empty()) continue;
				for (int move : currentPieceMoves)
				{
					moves.push_back(std::make_pair(i, move));
				}
				
			}
		}
		// Hier moeten we over alle mogelijke moves voor zwart loopen en de minimax voor elke move oproepen
		for (auto move : moves)
		{
			// Voor elke move een kopie maken van het bord en alle variabelen
			int blackScoreCopy{blackScore};
			int whiteScoreCopy{whiteScore};
			bool whiteToPlayCopy{whiteToPlay};
			int selectedIdxCopy{selectedIdx};
			int selectedMoveCopy{selectedMove};
			bool hasWhiteKingMovedCopy{hasWhiteKingMoved};
			bool hasBlackKingMovedCopy{hasBlackKingMoved};
			bool hasWhiteRightRookMovedCopy{hasWhiteRightRookMoved};
			bool hasWhiteLeftRookMovedCopy{hasWhiteLeftRookMoved};
			bool hasBlackRightRookMovedCopy{hasBlackRightRookMoved};
			bool hasBlackLeftRookMovedCopy{hasBlackLeftRookMoved};
			int whiteThreatsCopy[8][8]{ 0 };
			int blackThreatsCopy[8][8]{ 0 };
			for (int i{}; i < 8; ++i)
			{
				for (int j{}; j < 8; ++j)
				{
					whiteThreats[i][j] = whiteThreats[i][j];
					blackThreats[i][j] = blackThreats[i][j];
				}
			}
			bool gameOverCopy{gameOver};
			bool whiteInCheckCopy{whiteInCheck};
			bool blackInCheckCopy{blackInCheck};

			int boardCopy[8][8]{ 0 };
			for (int i{}; i < 8; ++i)
			{
				for (int j{}; j < 8; ++j)
				{
					boardCopy[i][j] = board[i][j];
				}
			}
			m_SelectedIndex = move.first;
			m_SelectedMove = move.second;
			PlayMove(m_SelectedIndex, m_SelectedMove, boardCopy);
			// De eerstvolgende mogelijke move recursief door de minimax laten gaan
			currentEval = Minimax(boardCopy, depth - 1, true, blackScore, whiteScore, whiteToPlay, selectedIdx, selectedMove, hasWhiteKingMoved, hasBlackKingMoved, hasWhiteRightRookMoved,
				hasWhiteLeftRookMoved, hasBlackRightRookMoved, hasBlackLeftRookMoved, whiteThreats, blackThreats, gameOver, whiteInCheck, blackInCheck, alpha, beta);
			// De meest negatieve evaluatie verkiezen
			minEval = std::min(currentEval, minEval);
			
			// Als een nieuwe minEval gekozen wordt, wordt ook een nieuwe bestMove geupdate
			if (minEval == currentEval) bestMove = move;

			// Originelen terugzetten, move dus undo'en eigenlijk
			m_BlackScore = blackScoreCopy;
			m_WhiteScore = whiteScoreCopy;
			m_WhiteToPlay = whiteToPlayCopy;
			m_SelectedIndex = selectedIdxCopy;
			m_SelectedMove = selectedMoveCopy;
			m_HasWhiteKingMoved = hasWhiteKingMovedCopy;
			m_HasBlackKingMoved = hasBlackKingMovedCopy;
			m_HasWhiteRightRookMoved = hasWhiteRightRookMovedCopy;
			m_HasWhiteLeftRookMoved = hasWhiteLeftRookMovedCopy;
			m_HasBlackRightRookMoved = hasBlackRightRookMovedCopy;
			m_HasBlackLeftRookMoved = hasBlackLeftRookMovedCopy;
			for (int i{}; i < 8; ++i)
			{
				for (int j{}; j < 8; ++j)
				{
					m_WhiteThreats[i][j] = whiteThreatsCopy[i][j];
					m_BlackThreats[i][j] = blackThreatsCopy[i][j];
				}
			}
			m_GameOver = gameOverCopy;
			m_WhiteInCheck = whiteInCheckCopy;
			m_BlackInCheck = blackInCheckCopy;

			beta = std::min(beta, currentEval);
			if (beta <= alpha) break;
		}
		// Als er terug naar de update gereturned gaat worden, kunnen de de move zetten 
		if (depth == m_Depth)
		{
			m_SelectedIndex = bestMove.first;
			m_SelectedMove = bestMove.second;
			PlayMove(bestMove.first, bestMove.second);
		}
		return minEval;
	}
	// Als het aan wit is (maximizing player)
	else
	{
		std::vector<std::pair<int, int>> moves;
		std::vector<int> currentPieceMoves;
		int maxEval = -99999;
		int currentEval{};
		// Eerst alle mogelijke moves voor wit verzamelen
		for (int i{}; i < 63; ++i)
		{
			// Als de piece op tile met idx i positief is (en dus wit is)
			if (m_Board[m_Grid.GetRowFromIdx(i)][m_Grid.GetColFromIdx(i)] > 0)
			{
				currentPieceMoves = GetAllLegalMoves(i, board, depth);
				// Als die piece geen legal moves heeft naar de volgende piece gaan
				if (currentPieceMoves.empty()) continue;
				for (int move : currentPieceMoves)
				{
					moves.push_back(std::make_pair(i, move));
				}

			}
		}
		// Hier moeten we over alle mogelijke moves voor wit loopen en de minimax voor elke move oproepen
		for (auto move : moves)
		{
			int blackScoreCopy{ blackScore };
			int whiteScoreCopy{ whiteScore };
			bool whiteToPlayCopy{ whiteToPlay };
			int selectedIdxCopy{ selectedIdx };
			int selectedMoveCopy{ selectedMove };
			bool hasWhiteKingMovedCopy{ hasWhiteKingMoved };
			bool hasBlackKingMovedCopy{ hasBlackKingMoved };
			bool hasWhiteRightRookMovedCopy{ hasWhiteRightRookMoved };
			bool hasWhiteLeftRookMovedCopy{ hasWhiteLeftRookMoved };
			bool hasBlackRightRookMovedCopy{ hasBlackRightRookMoved };
			bool hasBlackLeftRookMovedCopy{ hasBlackLeftRookMoved };
			int whiteThreatsCopy[8][8]{ 0 };
			int blackThreatsCopy[8][8]{ 0 };
			for (int i{}; i < 8; ++i)
			{
				for (int j{}; j < 8; ++j)
				{
					whiteThreats[i][j] = whiteThreats[i][j];
					blackThreats[i][j] = blackThreats[i][j];
				}
			}
			bool gameOverCopy{ gameOver };
			bool whiteInCheckCopy{ whiteInCheck };
			bool blackInCheckCopy{ blackInCheck };

			// Voor elke move een kopie maken van het bord
			int boardCopy[8][8]{ 0 };
			for (int i{}; i < 8; ++i)
			{
				for (int j{}; j < 8; ++j)
				{
					boardCopy[i][j] = board[i][j];
				}
			}
			m_SelectedIndex = move.first;
			m_SelectedMove = move.second;
			PlayMove(m_SelectedIndex, m_SelectedMove, boardCopy);
			// De eerstvolgende mogelijke move recursief door de minimax laten gaan
			currentEval = Minimax(boardCopy, depth - 1, false, blackScore, whiteScore, whiteToPlay, selectedIdx, selectedMove, hasWhiteKingMoved, hasBlackKingMoved, hasWhiteRightRookMoved,
				hasWhiteLeftRookMoved, hasBlackRightRookMoved, hasBlackLeftRookMoved, whiteThreats, blackThreats, gameOver, whiteInCheck, blackInCheck, alpha, beta);
			// De meest positieve evaluatie verkiezen
			maxEval = std::max(currentEval, maxEval);
			
			// Hier nog de move undo'en op het bord
			m_BlackScore = blackScoreCopy;
			m_WhiteScore = whiteScoreCopy;
			m_WhiteToPlay = whiteToPlayCopy;
			m_SelectedIndex = selectedIdxCopy;
			m_SelectedMove = selectedMoveCopy;
			m_HasWhiteKingMoved = hasWhiteKingMovedCopy;
			m_HasBlackKingMoved = hasBlackKingMovedCopy;
			m_HasWhiteRightRookMoved = hasWhiteRightRookMovedCopy;
			m_HasWhiteLeftRookMoved = hasWhiteLeftRookMovedCopy;
			m_HasBlackRightRookMoved = hasBlackRightRookMovedCopy;
			m_HasBlackLeftRookMoved = hasBlackLeftRookMovedCopy;
			for (int i{}; i < 8; ++i)
			{
				for (int j{}; j < 8; ++j)
				{
					m_WhiteThreats[i][j] = whiteThreatsCopy[i][j];
					m_BlackThreats[i][j] = blackThreatsCopy[i][j];
				}
			}
			m_GameOver = gameOverCopy;
			m_WhiteInCheck = whiteInCheckCopy;
			m_BlackInCheck = blackInCheckCopy;

			alpha = std::max(alpha, currentEval);
			if (beta <= alpha) break;
		}
		return maxEval;
	}
}

// -------------------------------------------------------
//					LEGAL MOVE GETTERS
// -------------------------------------------------------

std::vector<int> Game::GetAllLegalMoves(int index) const
{
	std::vector<int> moves;
	if (index < 0 || index > 63) return moves;
	int pieceOnIdx = m_Board[m_Grid.GetRowFromIdx(index)][m_Grid.GetColFromIdx(index)];
	// Checken of er op die indexpositie effectief een piece staat:
	if (pieceOnIdx == 0) return moves;

	switch (pieceOnIdx)
	{
		// White Pawn
	case 6:
		moves = GetWhitePawnMoves(index);
		break;
		// Black Pawn
	case -6:
		moves = GetBlackPawnMoves(index);
		break;
		// White Rook
	case 1:
		moves = GetWhiteRookMoves(index);
		break;
		// Black Rook
	case -1:
		moves = GetBlackRookMoves(index);
		break;
		// White Knight
	case 2:
		moves = GetWhiteKnightMoves(index);
		break;
		// Black Knight
	case -2:
		moves = GetBlackKnightMoves(index);
		break;
		// White Bishop
	case 3:
		moves = GetWhiteBishopMoves(index);
		break;
		// Black Bishop
	case -3:
		moves = GetBlackBishopMoves(index);
		break;
		// White Queen
	case 4:
		moves = GetWhiteQueenMoves(index);
		break;
		// Black Queen
	case -4:
		moves = GetBlackQueenMoves(index);
		break;
		// White King
	case 5:
		moves = GetWhiteKingMoves(index);
		break;
		// Black King
	case -5:
		moves = GetBlackKingMoves(index);
		break;
	}

	return moves;
}
std::vector<int> Game::GetAllLegalMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;
	if (depth == 0) return moves;
	if (index < 0 || index > 63) return moves;
	// Checken of er op die indexpositie effectief een piece staat:
	int pieceOnIdx = board[m_Grid.GetRowFromIdx(index)][m_Grid.GetColFromIdx(index)];
	if (pieceOnIdx == 0) return moves;

	switch (pieceOnIdx)
	{
		// White Pawn
	case 6:
		moves = GetWhitePawnMoves(index, board, depth);
		break;
		// Black Pawn
	case -6:
		moves = GetBlackPawnMoves(index, board, depth);
		break;
		// White Rook
	case 1:
		moves = GetWhiteRookMoves(index, board, depth);
		break;
		// Black Rook
	case -1:
		moves = GetBlackRookMoves(index, board, depth);
		break;
		// White Knight
	case 2:
		moves = GetWhiteKnightMoves(index, board, depth);
		break;
		// Black Knight
	case -2:
		moves = GetBlackKnightMoves(index, board, depth);
		break;
		// White Bishop
	case 3:
		moves = GetWhiteBishopMoves(index, board, depth);
		break;
		// Black Bishop
	case -3:
		moves = GetBlackBishopMoves(index, board, depth);
		break;
		// White Queen
	case 4:
		moves = GetWhiteQueenMoves(index, board, depth);
		break;
		// Black Queen
	case -4:
		moves = GetBlackQueenMoves(index, board, depth);
		break;
		// White King
	case 5:
		moves = GetWhiteKingMoves(index, board, depth);
		break;
		// Black King
	case -5:
		moves = GetBlackKingMoves(index, board, depth);
		break;
	}

	return moves;
}

// ------------------- PAWNS -------------------------

std::vector<int> Game::GetWhitePawnMoves(int index) const
{
	std::vector<int> moves;
	// Als de pawn op een rij staat boven de 2de rij (hier 1 want rijen beginnen op rij 0) betekent dat dat die pawn al heeft bewogen
	if (m_Grid.GetRowFromIdx(index) > 1 && m_Grid.GetRowFromIdx(index) < 7)
	{
		// Als er vlak boven de pawn niets staat kan die daar bewegen:
		if (m_Board[m_Grid.GetRowFromIdx(index + 8)][m_Grid.GetColFromIdx(index + 8)] == 0)
			moves.push_back(index + 8);
		// Als er op de diagonalen boven de pawn een zwarte piece staat kan die daar bewegen:
		if(m_Grid.GetColFromIdx(index + 7) < m_Grid.GetColFromIdx(index))
			if (m_Board[m_Grid.GetRowFromIdx(index + 7)][m_Grid.GetColFromIdx(index + 7)] < 0)
				moves.push_back(index + 7);
		if (m_Grid.GetColFromIdx(index + 9) > m_Grid.GetColFromIdx(index))
			if (m_Board[m_Grid.GetRowFromIdx(index + 9)][m_Grid.GetColFromIdx(index + 9)] < 0)
				moves.push_back(index + 9);
	}
	// Hieronder staan de legal moves van de pawn als die nog niet bewogen is en dus 2 tiles omhoog kan gaan
	else
	{
		if (index + 8 <= 63)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index + 8)][m_Grid.GetColFromIdx(index + 8)] == 0)
			{
				moves.push_back(index + 8);
				if (m_Board[m_Grid.GetRowFromIdx(index + 16)][m_Grid.GetColFromIdx(index + 16)] == 0)
					moves.push_back(index + 16);
			}
		}
		if(index + 7 <= 63)
			if (m_Grid.GetColFromIdx(index + 7) < m_Grid.GetColFromIdx(index))
				if (m_Board[m_Grid.GetRowFromIdx(index + 7)][m_Grid.GetColFromIdx(index + 7)] < 0)
					moves.push_back(index + 7);
		if(index + 9 <= 63)
			if (m_Grid.GetColFromIdx(index + 9) > m_Grid.GetColFromIdx(index))
				if (m_Board[m_Grid.GetRowFromIdx(index + 9)][m_Grid.GetColFromIdx(index + 9)] < 0)
					moves.push_back(index + 9);
	}

	if (m_EnPassant)
	{
		if (m_EnPassantIdx == index + 1)
			moves.push_back(index + 9);
		if (m_EnPassantIdx == index - 1)
			moves.push_back(index + 7);
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackPawnMoves(int index) const
{
	std::vector<int> moves;
	if (m_Grid.GetRowFromIdx(index) < 6 && m_Grid.GetRowFromIdx(index) > 0)
	{
		if (m_Board[m_Grid.GetRowFromIdx(index - 8)][m_Grid.GetColFromIdx(index - 8)] == 0)
			moves.push_back(index - 8);
		if (m_Grid.GetColFromIdx(index - 7) > m_Grid.GetColFromIdx(index))
			if (m_Board[m_Grid.GetRowFromIdx(index - 7)][m_Grid.GetColFromIdx(index - 7)] > 0)
				moves.push_back(index - 7);
		if (m_Grid.GetColFromIdx(index - 9) < m_Grid.GetColFromIdx(index))
			if (m_Board[m_Grid.GetRowFromIdx(index - 9)][m_Grid.GetColFromIdx(index - 9)] > 0)
				moves.push_back(index - 9);
	}
	else
	{
		if (index - 8 >= 0)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index - 8)][m_Grid.GetColFromIdx(index - 8)] == 0)
			{
				moves.push_back(index - 8);
				if (m_Board[m_Grid.GetRowFromIdx(index - 16)][m_Grid.GetColFromIdx(index - 16)] == 0)
					moves.push_back(index - 16);
			}
		}
		if(index - 7 >= 0)
			if (m_Grid.GetColFromIdx(index - 7) > m_Grid.GetColFromIdx(index))
				if (m_Board[m_Grid.GetRowFromIdx(index - 7)][m_Grid.GetColFromIdx(index - 7)] > 0)
					moves.push_back(index - 7);
		if(index - 9 >= 0)
			if (m_Grid.GetColFromIdx(index - 9) < m_Grid.GetColFromIdx(index))
				if (m_Board[m_Grid.GetRowFromIdx(index - 9)][m_Grid.GetColFromIdx(index - 9)] > 0)
					moves.push_back(index - 9);
	}

	if (m_EnPassant)
	{
		if (m_EnPassantIdx == index + 1)
			moves.push_back(index - 7);
		if (m_EnPassantIdx == index - 1)
			moves.push_back(index - 9);
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhitePawnMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;
	// Als de pawn op een rij staat boven de 2de rij (hier 1 want rijen beginnen op rij 0) betekent dat dat die pawn al heeft bewogen
	if (m_Grid.GetRowFromIdx(index) > 1 && m_Grid.GetRowFromIdx(index) < 7)
	{
		// Als er vlak boven de pawn niets staat kan die daar bewegen:
		if (board[m_Grid.GetRowFromIdx(index + 8)][m_Grid.GetColFromIdx(index + 8)] == 0)
			moves.push_back(index + 8);
		// Als er op de diagonalen boven de pawn een zwarte piece staat kan die daar bewegen:
		if (m_Grid.GetColFromIdx(index + 7) < m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index + 7)][m_Grid.GetColFromIdx(index + 7)] < 0)
				moves.push_back(index + 7);
		if (m_Grid.GetColFromIdx(index + 9) > m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index + 9)][m_Grid.GetColFromIdx(index + 9)] < 0)
				moves.push_back(index + 9);
	}
	// Hieronder staan de legal moves van de pawn als die nog niet bewogen is en dus 2 tiles omhoog kan gaan
	else
	{
		if (board[m_Grid.GetRowFromIdx(index + 8)][m_Grid.GetColFromIdx(index + 8)] == 0)
		{
			moves.push_back(index + 8);
			if (board[m_Grid.GetRowFromIdx(index + 16)][m_Grid.GetColFromIdx(index + 16)] == 0)
				moves.push_back(index + 16);
		}
		if (m_Grid.GetColFromIdx(index + 7) < m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index + 7)][m_Grid.GetColFromIdx(index + 7)] < 0)
				moves.push_back(index + 7);
		if (m_Grid.GetColFromIdx(index + 9) > m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index + 9)][m_Grid.GetColFromIdx(index + 9)] < 0)
				moves.push_back(index + 9);
	}

	if (m_EnPassant)
	{
		if (m_EnPassantIdx == index + 1)
			moves.push_back(index + 9);
		if (m_EnPassantIdx == index - 1)
			moves.push_back(index + 7);
	}

	//CheckCheck(index, 5, moves, board, depth);

	return moves;
}

std::vector<int> Game::GetBlackPawnMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;
	if (m_Grid.GetRowFromIdx(index) < 6 && m_Grid.GetRowFromIdx(index) > 0)
	{
		if (board[m_Grid.GetRowFromIdx(index - 8)][m_Grid.GetColFromIdx(index - 8)] == 0)
			moves.push_back(index - 8);
		if (m_Grid.GetColFromIdx(index - 7) > m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index - 7)][m_Grid.GetColFromIdx(index - 7)] > 0)
				moves.push_back(index - 7);
		if (m_Grid.GetColFromIdx(index - 9) < m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index - 9)][m_Grid.GetColFromIdx(index - 9)] > 0)
				moves.push_back(index - 9);
	}
	else
	{
		if (board[m_Grid.GetRowFromIdx(index - 8)][m_Grid.GetColFromIdx(index - 8)] == 0)
		{
			moves.push_back(index - 8);
			if (board[m_Grid.GetRowFromIdx(index - 16)][m_Grid.GetColFromIdx(index - 16)] == 0)
				moves.push_back(index - 16);
		}
		if (m_Grid.GetColFromIdx(index - 7) > m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index - 7)][m_Grid.GetColFromIdx(index - 7)] > 0)
				moves.push_back(index - 7);
		if (m_Grid.GetColFromIdx(index - 9) < m_Grid.GetColFromIdx(index))
			if (board[m_Grid.GetRowFromIdx(index - 9)][m_Grid.GetColFromIdx(index - 9)] > 0)
				moves.push_back(index - 9);
	}

	if (m_EnPassant)
	{
		if (m_EnPassantIdx == index + 1)
			moves.push_back(index - 7);
		if (m_EnPassantIdx == index - 1)
			moves.push_back(index - 9);
	}

	CheckCheck(index, -5, moves, board, depth);

	return moves;
}

// ------------------- ROOKS -------------------------

std::vector<int> Game::GetWhiteRookMoves(int index) const
{
	std::vector<int> moves;

	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + 1 > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackRookMoves(int index) const
{
	std::vector<int> moves;
	
	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhiteRookMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;

	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + 1 > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	//CheckCheck(index, 5, moves, board, depth);

	return moves;
}

std::vector<int> Game::GetBlackRookMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;

	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, -5, moves, board, depth);

	return moves;
}

// ------------------ KNIGHTS ------------------------

std::vector<int> Game::GetWhiteKnightMoves(int index) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 6, 10, 15, 17 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] <= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] <= 0)
				moves.push_back(index - offset);
		}
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackKnightMoves(int index) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 6, 10, 15, 17 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] >= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] >= 0)
				moves.push_back(index - offset);
		}
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhiteKnightMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 6, 10, 15, 17 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] <= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] <= 0)
				moves.push_back(index - offset);
		}
	}

	//CheckCheck(index, 5, moves, board, depth);

	return moves;
}

std::vector<int> Game::GetBlackKnightMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 6, 10, 15, 17 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] >= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 2)
		{
			if (board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] >= 0)
				moves.push_back(index - offset);
		}
	}

	CheckCheck(index, -5, moves, board, depth);

	return moves;
}

// ------------------ BISHOPS ------------------------

std::vector<int> Game::GetWhiteBishopMoves(int index) const
{
	std::vector<int> moves;

	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackBishopMoves(int index) const
{
	std::vector<int> moves;

	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhiteBishopMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;

	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

//	CheckCheck(index, 5, moves, board, depth);

	return moves;
}

std::vector<int> Game::GetBlackBishopMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;

	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, -5, moves, board, depth);

	return moves;
}

// ------------------ QUEENS -------------------------

std::vector<int> Game::GetWhiteQueenMoves(int index) const
{
	std::vector<int> moves;

	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackQueenMoves(int index) const
{
	std::vector<int> moves;

	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) && 
			m_Board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhiteQueenMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;

	// Go up
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] < 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] < 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	//CheckCheck(index, 5, moves, board, depth);

	return moves;
}

std::vector<int> Game::GetBlackQueenMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;

	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index + i > 63) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go down
	for (int i{ 8 }; i <= 56; i += 8)
	{
		if (index - i < 0) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	// Go right
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index + i > 63) break;
		if (m_Grid.GetRowFromIdx(index + i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	// Go left
	for (int i{ 1 }; i < 7; ++i)
	{
		if (index - i < 0) break;
		if (m_Grid.GetRowFromIdx(index - i) != m_Grid.GetRowFromIdx(index)) break;
		if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 9 }; i < 63; i += 9)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index + i > 63) break;
		if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] == 0)
			moves.push_back(index + i);
		else if (m_Grid.GetColFromIdx(index + i) < m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index + i)][m_Grid.GetColFromIdx(index + i)] > 0)
		{
			moves.push_back(index + i);
			break;
		}
		else
			break;
	}
	for (int i{ 7 }; i < 49; i += 7)
	{
		if (index - i < 0) break;
		if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] == 0)
			moves.push_back(index - i);
		else if (m_Grid.GetColFromIdx(index - i) > m_Grid.GetColFromIdx(index) &&
			board[m_Grid.GetRowFromIdx(index - i)][m_Grid.GetColFromIdx(index - i)] > 0)
		{
			moves.push_back(index - i);
			break;
		}
		else
			break;
	}

	CheckCheck(index, -5, moves, board, depth);

	return moves;
}

// ------------------ KINGS --------------------------

std::vector<int> Game::GetWhiteKingMoves(int index) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 1, 7, 8, 9 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] <= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] <= 0)
				moves.push_back(index - offset);
		}
	}

	// Checken of je kan castlen als je niet in check staat
	// Dit nog in een if steken die checked of een hasmoved bool false is
	if (!m_HasWhiteKingMoved)
	{
		if (m_BlackThreats[m_Grid.GetRowFromIdx(index)][m_Grid.GetColFromIdx(index)] == 0)
		{
			if (!m_HasWhiteRightRookMoved)
			{
				if (m_Board[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					m_Board[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0)
				{
					moves.push_back(index + 2);
				}
			}
			if (!m_HasWhiteLeftRookMoved)
			{
				if (m_Board[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					m_Board[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0)
				{
					moves.push_back(index - 2);
				}
			}
		}
	}

	CheckCheck(index, 5, moves);

	return moves;
}

std::vector<int> Game::GetBlackKingMoves(int index) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 1, 7, 8, 9 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] >= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (m_Board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] >= 0)
				moves.push_back(index - offset);
		}
	}

	if (!m_HasBlackKingMoved)
	{
		if (m_WhiteThreats[m_Grid.GetRowFromIdx(index)][m_Grid.GetColFromIdx(index)] == 0)
		{
			if (!m_HasBlackRightRookMoved)
			{
				if (m_Board[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					m_Board[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0)
				{
					moves.push_back(index + 2);
				}
			}
			if (!m_HasBlackLeftRookMoved)
			{
				if (m_Board[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					m_Board[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0)
				{
					moves.push_back(index - 2);
				}
			}
		}
	}

	CheckCheck(index, -5, moves);

	return moves;
}

std::vector<int> Game::GetWhiteKingMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 1, 7, 8, 9 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] <= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] <= 0)
				moves.push_back(index - offset);
		}
	}

	if (!m_HasWhiteKingMoved)
	{
		if (m_BlackThreats[m_Grid.GetRowFromIdx(index)][m_Grid.GetColFromIdx(index)] == 0)
		{
			if (!m_HasWhiteRightRookMoved)
			{
				if (board[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					board[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0)
				{
					moves.push_back(index + 2);
				}
			}
			if (!m_HasWhiteLeftRookMoved)
			{
				if (board[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					board[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					m_BlackThreats[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0)
				{
					moves.push_back(index - 2);
				}
			}
		}
	}

	//CheckCheck(index, 5, moves, board, depth);

	return moves;
}

std::vector<int> Game::GetBlackKingMoves(int index, int board[8][8], int depth) const
{
	std::vector<int> moves;

	std::vector<int> offsets{ 1, 7, 8, 9 };
	for (int offset : offsets)
	{
		if (index + offset <= 63 && abs(m_Grid.GetColFromIdx(index + offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (board[m_Grid.GetRowFromIdx(index + offset)][m_Grid.GetColFromIdx(index + offset)] >= 0)
				moves.push_back(index + offset);
		}
		if (index - offset >= 0 && abs(m_Grid.GetColFromIdx(index - offset) - m_Grid.GetColFromIdx(index)) <= 1)
		{
			if (board[m_Grid.GetRowFromIdx(index - offset)][m_Grid.GetColFromIdx(index - offset)] >= 0)
				moves.push_back(index - offset);
		}
	}

	if (!m_HasBlackKingMoved)
	{
		if (m_WhiteThreats[m_Grid.GetRowFromIdx(index)][m_Grid.GetColFromIdx(index)] == 0)
		{
			if (!m_HasBlackRightRookMoved)
			{
				if (board[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					board[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index + 1)][m_Grid.GetColFromIdx(index + 1)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index + 2)][m_Grid.GetColFromIdx(index + 2)] == 0)
				{
					moves.push_back(index + 2);
				}
			}
			if (!m_HasBlackLeftRookMoved)
			{
				if (board[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					board[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index - 1)][m_Grid.GetColFromIdx(index - 1)] == 0 &&
					m_WhiteThreats[m_Grid.GetRowFromIdx(index - 2)][m_Grid.GetColFromIdx(index - 2)] == 0)
				{
					moves.push_back(index - 2);
				}
			}
		}
	}

	CheckCheck(index, -5, moves, board, depth);

	return moves;
}