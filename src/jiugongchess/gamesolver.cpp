#include "gamesolver.h"
#include <queue>
#include <sstream>
#include <cassert>
#include <algorithm>
#include "ashes/math.h"

//==============================================================================
// GameSituation
// Note: stable algorithm is important to keep result ordered! 
//==============================================================================

GameSituation::GameSituation(const Chessboard& c, Chessboard::PlayerSide p)
    : chessboard_(c), player_(p)
{
    if (chessboard_.IsPlayerWin(Chessboard::EnemyOfPlayer(player_)))
    {
        type_ = Type::Failed;
        end_steps_min_ = 0;
        end_steps_max_ = 0;
    }
}

GameSituation::~GameSituation()
{
}

const Chessboard& GameSituation::GetChessboard() const
{
    return chessboard_;
}

Chessboard::PlayerSide GameSituation::GetPlayer() const
{
    return player_;
}

GameSituation::Type GameSituation::GetType() const
{
    return type_;
}

auto GameSituation::GetSortedStrategys() const -> std::vector<Strategy>
{
    std::vector<Strategy> strategys;

    // fail links: the slower the better.
    for (const Link& link : fail_links_)
    {
        int num_fail_steps = link.next->end_steps_min_;
        int score = ashes::math::Clamp(num_fail_steps, 0, 9);
        strategys.push_back({link.step, score});
    }

    // draw links: the enemy's mistake choice more the better.
    for (const Link& link : draw_links_)
    {
        int num_win_cases = static_cast<int>(link.next->fail_links_.size());
        int score = 10 + ashes::math::Clamp(num_win_cases, 0, 9);
        strategys.push_back({link.step, score});
    }

    // win links: the faster the better.
    for (const Link& link : win_links_)
    {
        int num_win_steps = link.next->end_steps_max_;
        int score = 29 - ashes::math::Clamp(num_win_steps, 0, 9);
        strategys.push_back({link.step, score});
    }
    
    // sort by score.
    std::stable_sort(strategys.begin(), strategys.end(),
        [](const auto& a, const auto& b) { return a.score < b.score; });

    return strategys;
}

void GameSituation::AddLink(const Chessboard::PieceStep& step, GameSituation* next)
{
    draw_links_.push_back({step, next});
}

bool GameSituation::Update()
{
    const Type type_copy = type_;

    const auto pos = std::stable_partition(draw_links_.begin(), draw_links_.end(),
        [](const Link& link) { return link.next->type_ == Type::Draw; });

    if (pos != draw_links_.end())
    {
        for (auto iter = pos; iter != draw_links_.end(); ++iter)
        {
            const GameSituation& next = *iter->next;
            end_steps_min_ = (std::min)(end_steps_min_, next.end_steps_min_ + 1);
            end_steps_max_ = (std::max)(end_steps_max_, next.end_steps_max_ + 1);
            (next.type_ == Type::Win ? fail_links_ : win_links_).push_back(*iter);
        }
        draw_links_.erase(pos, draw_links_.end());
    }

    if (type_ == Type::Draw)
    {
        if (!win_links_.empty())
        {
            type_ = Type::Win;
        }
        else if (draw_links_.empty())
        {
            type_ = Type::Fail;
        }
    }

    return type_ != type_copy;
}

//==============================================================================
// GameSolver
//==============================================================================

std::uint16_t GameSolver::CompressStrategy(const GameSituation::Strategy& strategy)
{
    int piece_index = Chessboard::CoordToIndex(strategy.step.first);
    int space_index = Chessboard::CoordToIndex(strategy.step.second);
    int bits = strategy.score << 8 | piece_index << 4 | space_index;
    return static_cast<std::uint16_t>(bits);
}

std::uint32_t GameSolver::CompressChessboard(const Chessboard& chessboard)
{
    const Chessboard::GridStorage& grids = chessboard.GetGridStorage();
    std::uint32_t bits = 0;
    std::for_each(grids.rbegin(), grids.rend(), [&bits](Chessboard::GridType type) {
        bits = bits << 2 | static_cast<std::uint32_t>(type); });
    return bits;
}

std::uint32_t GameSolver::CompressSituation(
    const Chessboard& chessboard, 
    Chessboard::PlayerSide player)
{
    std::uint32_t bits1 = CompressChessboard(chessboard);
    std::uint32_t bits2 = static_cast<std::uint8_t>(player) << 28;
    return bits1 | bits2;
}

GameSituation::Strategy GameSolver::DeCompressStrategy(std::uint32_t bits)
{
    int score = static_cast<int>(bits >> 8);
    ashes::Coord piece_coord = Chessboard::IndexToCoord((bits >> 4) & 0xF);
    ashes::Coord space_coord = Chessboard::IndexToCoord(bits & 0xF);
    return {{piece_coord, space_coord}, score};
}

Chessboard GameSolver::DeCompressChessboard(std::uint32_t bits)
{
    Chessboard::GridStorage grids;
    std::size_t idx = 0;
    std::generate(grids.begin(), grids.end(), [bits, &idx]() {
        return static_cast<Chessboard::GridType>((bits >> (idx++ << 1)) & 3); });
    return Chessboard(grids);
}

std::pair<Chessboard, Chessboard::PlayerSide> GameSolver::DeCompressSituation(
    std::uint32_t bits)
{
    auto item1 = DeCompressChessboard(bits & 0xFFFFFFF);
    auto item2 = static_cast<Chessboard::PlayerSide>(bits >> 28);
    return {item1, item2};
}

GameSolver::GameSolver()
{
}

GameSolver::~GameSolver()
{
}

void GameSolver::Print(std::ostream& os) const
{
    for (const auto& item : situations_)
    {
        if (item.second.GetType() == GameSituation::Type::Failed)
            continue;

        const auto strategys = item.second.GetSortedStrategys();
        os << "\n" << item.first << " " << strategys.size();

        for (const GameSituation::Strategy& strategy : strategys)
        {
            os << " " << CompressStrategy(strategy);
        }
    }
}

void GameSolver::BruteForceSolve()
{
    BruteForceSearch();
    BruteForceUpdate();
}

std::pair<GameSituation*, bool> GameSolver::FindOrAddSituation(
    const Chessboard& chessboard,
    Chessboard::PlayerSide player)
{
    const std::uint32_t key = CompressSituation(chessboard, player);
    auto iter = situations_.find(key);
    bool existed = (iter != situations_.end());

    if (!existed)
    {
        GameSituation situation(chessboard, player);
        iter = situations_.emplace(key, std::move(situation)).first;
    }

    return {&iter->second, !existed};
}

void GameSolver::BruteForceSearch()
{
    situations_.clear();
    FindOrAddSituation(Chessboard(), Chessboard::PlayerSide::Sente);
    std::queue<GameSituation*> open_queue;
    open_queue.push(&situations_.begin()->second);

    while (!open_queue.empty())
    {
        GameSituation* cur = open_queue.front();
        open_queue.pop();
        if (cur->GetType() == GameSituation::Type::Failed)
            continue;

        const auto next_player = Chessboard::EnemyOfPlayer(cur->GetPlayer());
        std::vector<Chessboard::PieceStep> steps;
        cur->GetChessboard().GetOptionalMove(cur->GetPlayer(), steps);

        for (const Chessboard::PieceStep& step : steps)
        {
            Chessboard next_chessboard = cur->GetChessboard();
            next_chessboard.MovePiece(step);
            auto ret = FindOrAddSituation(next_chessboard, next_player);
            if (ret.second) { open_queue.push(ret.first); }
            cur->AddLink(step, ret.first);
        }
    }
}

void GameSolver::BruteForceUpdate()
{
    for (bool has_update = true; has_update; )
    {
        has_update = false;

        for (auto& item : situations_)
        {
            has_update |= item.second.Update();
        }
    }
}