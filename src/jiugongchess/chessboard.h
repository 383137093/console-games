#ifndef ASHES_JIUGONGCHESS_CHESSBOARD_H
#define ASHES_JIUGONGCHESS_CHESSBOARD_H

#include <array>
#include <vector>
#include <utility>
#include "ashes/coord.h"

class Chessboard
{
public:

    //==========================================================================
    // Constants & Types
    // sente player: the player who moves first.
    // gote player: the player who moves second.
    //==========================================================================

    static const int kNumRows = 3;
    static const int kNumCols = 3;
    static const int kNumGrids = kNumRows * kNumCols;
    static const int kNumPiecesEachPlayer = 3;
    static const int kNumSpaces = kNumGrids - 2 * kNumPiecesEachPlayer;
    
    enum class PlayerSide { Sente, Gote };
    enum class GridType { Space, SentePiece, GotePiece };

    typedef ashes::Coord                    Coord;
    typedef std::pair<Coord, Coord>         PieceStep;
    typedef std::array<GridType, kNumGrids> GridStorage;

    //==========================================================================
    // Play game
    //==========================================================================
    
    Chessboard();
    explicit Chessboard(const GridStorage& grids);
    ~Chessboard();

    bool IsPlayerWin(PlayerSide player) const;
    void MovePiece(const PieceStep& step);
    void Reset();

    //==========================================================================
    // Grids & GridStorage
    //==========================================================================

    static bool ValidateCoord(const Coord& coord);
    static int CoordToIndex(const Coord& coord);
    static Coord IndexToCoord(int index);

    static bool ValidateGridStorage(const GridStorage& grids);
    static const GridStorage& GetInitialGridStorage();
    const GridStorage& GetGridStorage() const;

    static int CountGrid(GridType type);
    GridType GetGridType(const Coord& coord) const;

    //==========================================================================
    // Pieces & Players
    //==========================================================================

    static GridType PieceTypeOwnedByPlayer(PlayerSide player);
    static PlayerSide OwnerPlayerOfPieceType(GridType type);
    static PlayerSide EnemyOfPlayer(PlayerSide player);
    
    bool CanMovePiece(PlayerSide player, const PieceStep& step) const;
    bool CanMovePiece(const PieceStep& step) const;
    void GetOptionalMove(const Coord& piece, std::vector<PieceStep>& steps) const;
    void GetOptionalMove(PlayerSide player, std::vector<PieceStep>& steps) const;

private:

    //==========================================================================
    // Privates
    //==========================================================================

    static bool IsGridConnected(int index1, int index2);
    static std::vector<Coord> FindGridCoords(const GridStorage& grids, GridType type);
    
    GridStorage grids_;
};

#endif