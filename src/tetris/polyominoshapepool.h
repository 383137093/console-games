#ifndef TETRIS_POLYOMINOSHAPEPOOL_H
#define TETRIS_POLYOMINOSHAPEPOOL_H

#include <string>
#include <vector>
#include "polyomino.h"

class PolyominoShapePool
{
public:
	
	static void Initialize();
	static const PolyominoShapePool& Instance();

	const Polyomino::Shape& RandShape() const;
	const Polyomino::Shape& RotateShapeCW(const Polyomino::Shape& shape) const;
	const Polyomino::Shape& RotateShapeCCW(const Polyomino::Shape& shape) const;

private:
		
	typedef std::vector<Polyomino::Shape> ShapeSequence;
	typedef std::vector<ShapeSequence>    ShapeBook;

	PolyominoShapePool();
	PolyominoShapePool(const PolyominoShapePool&) = delete;
	~PolyominoShapePool();
	PolyominoShapePool& operator = (const PolyominoShapePool&) = delete;

private:

	static ShapeBook LoadShapeBookFromResource();
	
	static ShapeBook ParseShapeBook(const std::vector<std::string>& lines);
	
	static ShapeSequence ParseShapeSequence(
		const std::vector<std::string>& lines, std::size_t y);
	
	static Polyomino::Shape ParseShape(
		const std::vector<std::string>& lines, std::size_t y, std::size_t x);
	
	const ShapeSequence& QueryShapeInBook(const Polyomino::Shape& shape) const;

	ShapeBook shape_book_;
};

#endif