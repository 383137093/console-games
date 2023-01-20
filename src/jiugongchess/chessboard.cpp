#include "chessboard.h"
#include <cassert>
#include <algorithm>

Chessboard::Chessboard()
    : Chessboard(GetInitialGridStorage())
{
}

Chessboard::Chessboard(const GridStorage& grids)
    : grids_(grids)
{
    assert(ValidateGridStorage(grids));
}

Chessboard::~Chessboard()
{
}

bool Chessboard::IsPlayerWin(PlayerSide player) const
{
    static const std::vector<Coord> kInitialSentePieceCoords =
        FindGridCoords(GetInitialGridStorage(), GridType::SentePiece);

    static const std::vector<Coord> kInitialGotePieceCoords =
        FindGridCoords(GetInitialGridStorage(), GridType::GotePiece);

    const std::vector<Coord>& initial_piece_coords = (player == PlayerSide::Sente
        ? kInitialSentePieceCoords : kInitialGotePieceCoords);

    const std::vector<Coord> piece_coords =
        FindGridCoords(grids_, PieceTypeOwnedByPlayer(player));

    return piece_coords[1] - piece_coords[0] == piece_coords[2] - piece_coords[1]
        && piece_coords != initial_piece_coords;
}

void Chessboard::MovePiece(const PieceStep& step)
{
    assert(CanMovePiece(step));
    int piece_index = CoordToIndex(step.first);
    int space_index = CoordToIndex(step.second);
    grids_[space_index] = grids_[piece_index];
    grids_[piece_index] = GridType::Space;
}

void Chessboard::Reset()
{
    grids_ = GetInitialGridStorage();
}

bool Chessboard::ValidateCoord(const Coord& coord)
{
    return 0 <= coord.X && coord.X < kNumCols
        && 0 <= coord.Y && coord.Y < kNumRows;
}

int Chessboard::CoordToIndex(const Coord& coord)
{
    return coord.X + coord.Y * kNumCols;
}

Chessboard::Coord Chessboard::IndexToCoord(int index)
{
    return {static_cast<SHORT>(index % kNumCols), 
            static_cast<SHORT>(index / kNumCols)};
}

bool Chessboard::ValidateGridStorage(const GridStorage& grids)
{
    auto n1 = std::count(grids.begin(), grids.end(), GridType::SentePiece);
    auto n2 = std::count(grids.begin(), grids.end(), GridType::GotePiece);
    return n1 == kNumPiecesEachPlayer && n2 == kNumPiecesEachPlayer;
}

const Chessboard::GridStorage& Chessboard::GetInitialGridStorage()
{
    static const GridStorage kGrids = {
        GridType::GotePiece,  GridType::GotePiece,  GridType::GotePiece,
        GridType::Space,      GridType::Space,      GridType::Space,
        GridType::SentePiece, GridType::SentePiece, GridType::SentePiece,};
    return kGrids;
}

const Chessboard::GridStorage& Chessboard::GetGridStorage() const
{
    return grids_;
}

int Chessboard::CountGrid(GridType type)
{
    return type == GridType::Space ? kNumSpaces : kNumPiecesEachPlayer;
}

Chessboard::GridType Chessboard::GetGridType(const Coord& coord) const
{
    assert(ValidateCoord(coord));
    return grids_[CoordToIndex(coord)];
}

Chessboard::GridType Chessboard::PieceTypeOwnedByPlayer(PlayerSide player)
{
    return player == PlayerSide::Sente ? GridType::SentePiece : GridType::GotePiece;
}

Chessboard::PlayerSide Chessboard::OwnerPlayerOfPieceType(GridType type)
{
    assert(type != GridType::Space);
    return type == GridType::SentePiece ? PlayerSide::Sente: PlayerSide::Gote;
}

Chessboard::PlayerSide Chessboard::EnemyOfPlayer(PlayerSide player)
{
    return player == PlayerSide::Sente ? PlayerSide::Gote : PlayerSide::Sente;
}

bool Chessboard::CanMovePiece(PlayerSide player, const PieceStep& step) const
{
    return PieceTypeOwnedByPlayer(player) == GetGridType(step.first)
        && CanMovePiece(step);
}

bool Chessboard::CanMovePiece(const PieceStep& step) const
{
    assert(ValidateCoord(step.first));
    assert(ValidateCoord(step.second));

    int piece_index = CoordToIndex(step.first);
    int space_index = CoordToIndex(step.second);

    return grids_[piece_index] != GridType::Space
        && grids_[space_index] == GridType::Space
        && IsGridConnected(piece_index, space_index);
}

void Chessboard::GetOptionalMove(
    const Coord& piece, 
    std::vector<PieceStep>& steps) const
{
    for (int index = 0; index < kNumGrids; ++index)
    {
        PieceStep step = {piece, IndexToCoord(index)};

        if (CanMovePiece(step))
        {
            steps.push_back(step);
        }
    }
}

void Chessboard::GetOptionalMove(
    PlayerSide player,
    std::vector<PieceStep>& steps) const
{
    const std::vector<Coord> coords = FindGridCoords(
        grids_, PieceTypeOwnedByPlayer(player));

    for (const Coord& coord : coords)
    {
        GetOptionalMove(coord, steps);
    }
}

bool Chessboard::IsGridConnected(int index1, int index2)
{
    static const bool kAdjacencyMatrix[kNumGrids][kNumGrids] = {
        {false, true,  false, true,  true,  false, false, false, false},
        {true,  false, true,  false, true,  false, false, false, false},
        {false, true,  false, false, true,  true,  false, false, false},
        {true,  false, false, false, true,  false, true,  false, false},
        {true,  true,  true,  true,  false, true,  true,  true,  true },
        {false, false, true,  false, true,  false, false, false, true },
        {false, false, false, true,  true,  false, false, true,  false},
        {false, false, false, false, true,  false, true,  false, true },
        {false, false, false, false, true,  true,  false, true,  false}};

    assert(0 <= index1 && index1 < kNumGrids);
    assert(0 <= index2 && index2 < kNumGrids);

    return kAdjacencyMatrix[index1][index2];
}

std::vector<Chessboard::Coord> Chessboard::FindGridCoords(
    const GridStorage& grids,
    GridType type)
{
    std::vector<Coord> coords;

    for (int index = 0; index < kNumGrids; ++index)
    {
        if (grids[index] == type)
        {
            coords.push_back(IndexToCoord(index));
        }
    }

    assert(static_cast<int>(coords.size()) == CountGrid(type));
    return coords;
}