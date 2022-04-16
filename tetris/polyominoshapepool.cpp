#include "polyominoshapepool.h"
#include <cctype>
#include <cassert>
#include <sstream>
#include <iterator>
#include <algorithm>
#include "ashes/random.h"
#include "ashes/winapi.h"
#include "res/resource.h"

void PolyominoShapePool::Initialize()
{
	Instance();
}

const PolyominoShapePool& PolyominoShapePool::Instance()
{
	static const PolyominoShapePool instance;
	return instance;
}

const Polyomino::Shape& PolyominoShapePool::RandShape() const
{
	using ashes::random::IntRange;
	int seq = IntRange(0, static_cast<int>(shape_book_.size() - 1));
	int idx = IntRange(0, static_cast<int>(shape_book_[seq].size() - 1));
	return shape_book_[seq][idx];
}

const Polyomino::Shape& PolyominoShapePool::RotateShapeCW(
	const Polyomino::Shape& shape) const
{
	const ShapeSequence& shape_seq = QueryShapeInBook(shape);
	std::size_t idx = &shape - shape_seq.data();
	std::size_t next_idx = (idx + 1) % shape_seq.size();
	return shape_seq[next_idx];
}

const Polyomino::Shape& PolyominoShapePool::RotateShapeCCW(
	const Polyomino::Shape& shape) const
{
	const ShapeSequence& shape_seq = QueryShapeInBook(shape);
	std::size_t idx = &shape - shape_seq.data();
	std::size_t prev_idx = (idx - 1 + shape_seq.size()) % shape_seq.size();
	return shape_seq[prev_idx];
}

PolyominoShapePool::PolyominoShapePool()
{
	shape_book_ = LoadShapeBookFromResource();
}

PolyominoShapePool::~PolyominoShapePool()
{
}

PolyominoShapePool::ShapeBook PolyominoShapePool::LoadShapeBookFromResource()
{
	const char* name = MAKEINTRESOURCEA(IDR_TXT_SHAPEBOOK);
	const std::string resource = ashes::win::LoadModuleResource(name, "TXT");
	assert(!resource.empty());

	std::stringstream sstream(resource);
	std::vector<std::string> lines;

	for (std::string line; std::getline(sstream, line); )
	{
		// remove trailing whitespace.
		auto iter = std::find_if_not(line.rbegin(), line.rend(), std::isspace);
		line.erase(iter.base(), line.end());
		lines.push_back(line);
	}

	return ParseShapeBook(lines);
}

PolyominoShapePool::ShapeBook PolyominoShapePool::ParseShapeBook(
	const std::vector<std::string>& lines)
{
	ShapeBook shape_book;

	for (std::size_t y = 0; y < lines.size(); y += Polyomino::Shape::kMaxEdge)
	{
		y = std::find_if(lines.begin() + y, lines.end(),
			[](const std::string& s) {return !s.empty(); }) - lines.begin();

		if (y + Polyomino::Shape::kMaxEdge <= lines.size())
		{
			shape_book.push_back(ParseShapeSequence(lines, y));
		}
	}

	return shape_book;
}

PolyominoShapePool::ShapeSequence PolyominoShapePool::ParseShapeSequence(
	const std::vector<std::string>& lines,
	std::size_t y)
{
	const std::string& line0 = lines[y];

	ShapeSequence shape_seq;
	shape_seq.reserve(4);

	for (std::size_t x = 0; x < line0.size(); x += Polyomino::Shape::kMaxEdge)
	{
		x = std::find_if(line0.begin() + x, line0.end(),
			[](char c) { return !std::isspace(c); }) - line0.begin();

		if (x + Polyomino::Shape::kMaxEdge <= line0.size())
		{
			shape_seq.push_back(ParseShape(lines, y, x));
		}
	}

	return shape_seq;
}

Polyomino::Shape PolyominoShapePool::ParseShape(
	const std::vector<std::string>& lines,
	std::size_t y,
	std::size_t x)
{
	Polyomino::Shape shape;
	shape.bounding = {Polyomino::Shape::kMaxEdge, Polyomino::Shape::kMaxEdge, 0, 0};

	for (SHORT row = 0; row < Polyomino::Shape::kMaxEdge; ++row)
	{
		for (SHORT col = 0; col < Polyomino::Shape::kMaxEdge; ++col)
		{
			if (lines[row + y][col + x] == 'O')
			{
				shape.bitmap.set(Polyomino::Shape::CoordToIndex({col, row}), true);

				shape.bounding.Left   = (std::min)(shape.bounding.Left,   col);
				shape.bounding.Right  = (std::max)(shape.bounding.Right,  col);
				shape.bounding.Top    = (std::min)(shape.bounding.Top,    row);
				shape.bounding.Bottom = (std::max)(shape.bounding.Bottom, row);
			}
		}
	}

	return shape;
}

const PolyominoShapePool::ShapeSequence& PolyominoShapePool::QueryShapeInBook(
	const Polyomino::Shape& shape) const
{
	for (const ShapeSequence& shape_seq : shape_book_)
	{
		std::size_t idx = &shape - shape_seq.data();
		if (0 <= idx && idx < shape_seq.size())
			return shape_seq;
	}

	assert(false);
	return shape_book_.front();
}
