#include "gameai.h"
#include <queue>

//==============================================================================
// Navigation
//==============================================================================

namespace navigation {

bool BreadthFirstSearch(
	const GameMap& map,
	const ashes::Coord& src,
	const ashes::Coord* dest,
	std::vector<int>& dists)
{
	assert(map.NumGrids() > 0);
	assert(map.ValidateCoord(src));

	dists.assign(map.NumGrids(), INT_MAX);
	dists[map.CoordToIndex(src)] = 0;
	std::queue<ashes::Coord> open_queue;
	open_queue.push(src);
	
	while (!open_queue.empty())
	{
		const ashes::Coord cur_coord = open_queue.front();
		const int cur_dist = dists[map.CoordToIndex(cur_coord)];
		open_queue.pop();

		for (ashes::Direction4 dir : ashes::direction4::kFullSet)
		{
			ashes::Coord next_coord = cur_coord.Adjacency(dir);
			if (!map.ValidateCoord(next_coord))
				continue;
			
			int& next_dist = dists[map.CoordToIndex(next_coord)];
			if (next_dist != INT_MAX)
				continue;

			if (dest != nullptr && next_coord == *dest)
			{
				next_dist = cur_dist + 1;
				return true;
			}

			if (map.GetGridType(next_coord) == GameMap::GridType::Space)
			{
				next_dist = cur_dist + 1;
				open_queue.push(next_coord);
			}
		}
	}
	
	return dest == nullptr;
}

bool BuildShortestPath(
	const GameMap& map,
	const ashes::Coord& src,
	const ashes::Coord& dest,
	const std::vector<int>& dists_to_dest,
	ashes::Direction4 preferred_dir,
	std::vector<ashes::Direction4>& dir_path,
	std::vector<ashes::Coord>& coord_path)
{
	assert(map.NumGrids() > 0);
	assert(map.NumGrids() == static_cast<int>(dists_to_dest.size()));
	assert(map.ValidateCoord(src));
	assert(map.ValidateCoord(dest));
	assert(dists_to_dest[map.CoordToIndex(dest)] == 0);
	assert(dists_to_dest[map.CoordToIndex(src)] < map.NumGrids());

	dir_path.clear();
	coord_path.clear();
	
	for (ashes::Coord cur_coord = src; cur_coord != dest;)
	{
		const int cur_dist = dists_to_dest[map.CoordToIndex(cur_coord)];
		const std::size_t cur_path_length = dir_path.size();

		const ashes::Direction4 dirs[] = {preferred_dir,
			ashes::direction4::LeftSideOf(preferred_dir),
			ashes::direction4::RightSideOf(preferred_dir),
			ashes::direction4::NegativeOf(preferred_dir)};

		for (ashes::Direction4 dir : dirs)
		{
			ashes::Coord next_coord = cur_coord.Adjacency(dir);

			if (map.ValidateCoord(next_coord) &&
				dists_to_dest[map.CoordToIndex(next_coord)] == cur_dist - 1)
			{
				dir_path.push_back(dir);
				coord_path.push_back(cur_coord);
				preferred_dir = dir;
				cur_coord = next_coord;
				break;
			}
		}

		if (dir_path.size() <= cur_path_length)
			return false;
	}
	
	coord_path.push_back(dest);
	return true;
}

bool BuildLongestPath(
	GameMap& map,
	const ashes::Coord& src,
	const ashes::Coord& dest,
	const std::vector<int>& dists_to_dest,
	ashes::Direction4 preferred_dir,
	std::vector<ashes::Direction4>& dir_path,
	std::vector<ashes::Coord>& coord_path)
{
	if (!BuildShortestPath(map, src, dest, dists_to_dest, preferred_dir,
			dir_path, coord_path))
		return false;
	
	for (std::size_t i = 1; i + 1 < coord_path.size(); ++i)
	{
		assert(map.GetGridType(coord_path[i]) == GameMap::GridType::Space);
		map.SetGridType(coord_path[i], GameMap::GridType::Snake);
	}

	for (std::size_t i = 0; i + 1 < coord_path.size();)
	{
		const std::size_t cur_path_length = dir_path.size();

		const ashes::Direction4 dirs[] = {
			ashes::direction4::LeftSideOf(dir_path[i]),
			ashes::direction4::RightSideOf(dir_path[i])};

		for (ashes::Direction4 dir : dirs)
		{
			ashes::Direction4 new_dir1 = dir_path[i];
			ashes::Direction4 new_dir2 = ashes::direction4::NegativeOf(dir);
			ashes::Coord new_coord1 = coord_path[i].Adjacency(dir);
			ashes::Coord new_coord2 = new_coord1.Adjacency(new_dir1);

			if (map.ValidateCoord(new_coord1) &&
				map.ValidateCoord(new_coord2) &&
				map.GetGridType(new_coord1) == GameMap::GridType::Space &&
				map.GetGridType(new_coord2) == GameMap::GridType::Space)
			{
				dir_path[i] = dir;
				dir_path.insert(dir_path.begin() + i + 1, new_dir1);
				dir_path.insert(dir_path.begin() + i + 2, new_dir2);
				coord_path.insert(coord_path.begin() + i + 1, new_coord1);
				coord_path.insert(coord_path.begin() + i + 2, new_coord2);
				map.SetGridType(new_coord1, GameMap::GridType::Snake);
				map.SetGridType(new_coord2, GameMap::GridType::Snake);
				break;
			}
		}

		i += (dir_path.size() <= cur_path_length);
	}

	return true;
}

void MoveSnakeAlongPath(
	GameMap& map,
	Snake& snake,
	const std::vector<ashes::Direction4>& dir_path,
	bool grow_at_dest)
{
	for (int i = 0; i < snake.Length(); ++i)
	{
		map.SetGridType(snake.Node(i).pos, GameMap::GridType::Space);
	}

	for (std::size_t i = 0; i < dir_path.size(); ++i)
	{
		bool grow = (i + 1 == dir_path.size() && grow_at_dest);
		snake.SetForwardDirection(dir_path[i]);
		grow ? snake.GrowForward() : snake.MoveForward();
	}

	for (int i = 0; i < snake.Length(); ++i)
	{
		map.SetGridType(snake.Node(i).pos, GameMap::GridType::Snake);
	}
}

}

//==============================================================================
// GameAI
// algorithm reference: https://github.com/chuyangliu/snake
//==============================================================================

GameAI::GameAI()
{
}

GameAI::~GameAI()
{
}

ashes::Direction4 GameAI::MakeStrategy(const GameCore& game)
{
	std::pair<bool, ashes::Direction4> optional_dir;

	CopyGameData(game);
	optional_dir = FindSafelyPathToFood();
	if (!optional_dir.first)
	{
		CopyGameData(game);
		optional_dir = FindPathToTail();
		if (!optional_dir.first)
		{
			optional_dir.second = game.GetSnake().Head().dir;
		}
	}

	return optional_dir.second;
}

void GameAI::CopyGameData(const GameCore& game)
{
	game_map_ = game.GetGameMap();
	snake_ = game.GetSnake();
	food_ = game.GetFood();
}

std::pair<bool, ashes::Direction4> GameAI::FindSafelyPathToFood()
{
	using namespace navigation;
	
	if (BreadthFirstSearch(game_map_, food_, &snake_.Head().pos, dists_))
	{
		if (BuildShortestPath(game_map_, snake_.Head().pos, food_, dists_,
				snake_.Head().dir, dir_path_, coord_path_))
		{
			ashes::Direction4 dir = dir_path_.front();
			MoveSnakeAlongPath(game_map_, snake_, dir_path_, true);

			if (snake_.Length() <= 1 || game_map_.NumSpaces() == 0 ||
				FindPathToTail().first)
			{
				return {true, dir};
			}
		}
	}

	return {false, ashes::Direction4::Forward};
}

std::pair<bool, ashes::Direction4> GameAI::FindPathToTail()
{
	using namespace navigation;

	if (BreadthFirstSearch(game_map_, snake_.Tail().pos,
			&snake_.Head().pos, dists_))
	{
		if (BuildLongestPath(game_map_, snake_.Head().pos, snake_.Tail().pos,
				dists_, snake_.Head().dir, dir_path_, coord_path_))
		{
			if (dir_path_.size() > 1)
			{
				return {true, dir_path_.front()};
			}
		}
	}

	return {false, ashes::Direction4::Forward};
}
