#pragma once
#include <vector>
#include <map>
#include "Texture.h"
#include "Grid.h"
class Game
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update( float elapsedSec );
	void Draw( ) const;
	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e );
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e );
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e );
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e );
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e );

private:
	// DATA MEMBERS
	const Window m_Window;
	// FUNCTIONS
	void Initialize( );
	void Cleanup( );
	void ClearBackground( ) const;
	void DrawPieces() const;
	void DrawLegalMoves() const;
	void DrawCheckedKing() const;
	void InitializeTextures();
	int convert2DTo1D(int row, int column) const {return  (row * 8) + column; };
	
	int m_SelectedIndex, m_SelectedMove;
	Grid m_Grid;
	int m_Board[8][8] = {
		 1, 2, 3, 4, 5, 3, 2, 1,
		 6, 6, 6, 6, 6, 6, 6, 6,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0,
		 -6, -6, -6, -6, -6, -6, -6, -6,
		 -1, -2, -3, -4, -5, -3, -2, -1};
	int m_WhiteThreats[8][8] = {
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 1,  1,  1,  1,  1,  1,  1,  1,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		-1, -1, -1, -1, -1, -1, -1, -1,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0
	};
	int m_BlackThreats[8][8] = {
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 1,  1,  1,  1,  1,  1,  1,  1,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		-1, -1, -1, -1, -1, -1, -1, -1,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0
	};
	float size = m_Window.width/8;
	Vector2f offset{ 0.f,0.f };
	std::map<int, Texture> m_Textures;
	bool m_WhiteToPlay = true;
	bool m_GameOver = false;
	bool m_WhiteInCheck{ false }, m_BlackInCheck{ false };
	bool m_HasWhiteKingMoved{ false }, m_HasWhiteLeftRookMoved{ false }, m_HasWhiteRightRookMoved{ false };
	bool m_HasBlackKingMoved{ false }, m_HasBlackLeftRookMoved{ false }, m_HasBlackRightRookMoved{ false };
	bool m_EnPassant{ false };
	int m_EnPassantIdx{};
	void PrintBoard();
	void PrintThreatMatrices();

	void UpdateThreats();
	void UpdateThreats(int board[8][8]);
	bool EvaluateThreats(int startIdx, int newIdx, bool isWhite) const;
	bool EvaluateThreats(int startIdx, int newIdx, bool isWhite, int board[8][8], int depth) const;
	void CheckCheck(int index, int king, std::vector<int>& moves) const;
	void CheckCheck(int index, int king, std::vector<int>& moves, int board[8][8], int depth) const;
	void CheckMate();
	void CheckMate(int board[8][8]);

	// -----
	// AI
	// -----
	int m_WhiteScore, m_BlackScore, m_Depth{3};
	// ------------------------ WEIGHTS -------------------------
	enum class Weights
	{
		Pawn = 10,
		Knight = 30,
		Bishop = 30,
		Rook = 50,
		Queen = 90,
		King = 900
	};

	void AdjustScores(int destIdx);
	void AdjustScores(int destIdx, int board[8][8]);
	// -------------- HEURISTIC EVALUATION FUNCTION --------------
	int Evaluate(int board[8][8], int maximizingColor);
	int NewEvaluate(int blackScore, int whiteScore);
	// -------------------- MINIMAX ALGORITHM --------------------
	int Minimax(int board[8][8], int depth, bool maximizingPlayer, int blackScore, int whiteScore, bool whiteToPlay, int selectedIdx, int selectedMove, bool hasWhiteKingMoved,
		bool hasBlackKingMoved,
	bool hasWhiteRightRookMoved,
	bool hasWhiteLeftRookMoved,
	bool hasBlackRightRookMoved,
	bool hasBlackLeftRookMoved,
	int whiteThreats[8][8],
	int blackThreats[8][8],
	bool gameOver,
	bool whiteInCheck,
	bool blackInCheck);

	// --------
	// PLAYING
	// --------
	void PlayMove(int startIdx, int destIdx);
	void PlayMove(int startIdx, int destIdx, int board[8][8]);
	void CheckCastlingMovement();
	void CheckCastlingMovement(int board[8][8]);
	void MovePiece(int startIdx, int destIdx);
	void MovePiece(int startIdx, int destIdx, int board[8][8]);
	bool Castle(int startIdx, int destIdx);
	bool Castle(int startIdx, int destIdx, int board[8][8]);
	bool Promote(int startIdx, int destIdx);
	bool Promote(int startIdx, int destIdx, int board[8][8]);
	void CheckEnPassant(int startIdx, int destIdx);
	void CheckEnPassant(int startIdx, int destIdx, int board[8][8]);

	// ---------------------------------------------------
	//				  LEGAL MOVE GETTERS				  
	// ---------------------------------------------------
	std::vector<int> GetAllLegalMoves(int index) const;
	std::vector<int> GetAllLegalMoves(int index, int board[8][8], int depth) const;
	// ------------------- PAWNS -------------------------
	std::vector<int> GetWhitePawnMoves(int index) const;
	std::vector<int> GetBlackPawnMoves(int index) const;
	std::vector<int> GetWhitePawnMoves(int index, int board[8][8], int depth) const;
	std::vector<int> GetBlackPawnMoves(int index, int board[8][8], int depth) const;
	// ------------------- ROOKS -------------------------
	std::vector<int> GetWhiteRookMoves(int index) const;
	std::vector<int> GetBlackRookMoves(int index) const;
	std::vector<int> GetWhiteRookMoves(int index, int board[8][8], int depth) const;
	std::vector<int> GetBlackRookMoves(int index, int board[8][8], int depth) const;
	// ------------------ KNIGHTS ------------------------
	std::vector<int> GetWhiteKnightMoves(int index) const;
	std::vector<int> GetBlackKnightMoves(int index) const;
	std::vector<int> GetWhiteKnightMoves(int index, int board[8][8], int depth) const;
	std::vector<int> GetBlackKnightMoves(int index, int board[8][8], int depth) const;
	// ------------------ BISHOPS ------------------------
	std::vector<int> GetWhiteBishopMoves(int index) const;
	std::vector<int> GetBlackBishopMoves(int index) const;
	std::vector<int> GetWhiteBishopMoves(int index, int board[8][8], int depth) const;
	std::vector<int> GetBlackBishopMoves(int index, int board[8][8], int depth) const;
	// ------------------ QUEENS -------------------------
	std::vector<int> GetWhiteQueenMoves(int index) const;
	std::vector<int> GetBlackQueenMoves(int index) const;
	std::vector<int> GetWhiteQueenMoves(int index, int board[8][8], int depth) const;
	std::vector<int> GetBlackQueenMoves(int index, int board[8][8], int depth) const;
	// ------------------ KINGS --------------------------
	std::vector<int> GetWhiteKingMoves(int index) const;
	std::vector<int> GetBlackKingMoves(int index) const;
	std::vector<int> GetWhiteKingMoves(int index, int board[8][8], int depth) const;
	std::vector<int> GetBlackKingMoves(int index, int board[8][8], int depth) const;
};