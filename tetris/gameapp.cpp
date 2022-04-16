#include "gameapp.h"
#include <chrono>
#include <functional>
#include "ashes/random.h"
#include "ashes/console.h"
#include "polyominoshapepool.h"

using namespace std::chrono_literals;

const std::vector<ashes::String> GameApp::kGameWindowTexts = {
	TEXT("╔══════════╦══════╗"),
	TEXT("║　　　　　　　　　　║　 下一个 　║"),
	TEXT("║　　　　　　　　　　║　　　　　　║"),
	TEXT("║　　　　　　　　　　║　　　　　　║"),
	TEXT("║　　　　　　　　　　║　　　　　　║"),
	TEXT("║　　　　　　　　　　║　　　　　　║"),
	TEXT("║　　　　　　　　　　║　　　　　　║"),
	TEXT("║　　　　　　　　　　║　　　　　　║"),
	TEXT("║　　　　　　　　　　╠══════╣"),
	TEXT("║　　　　　　　　　　║　　分数　　║"),
	TEXT("║　　　　　　　　　　║　　　　　　║"),
	TEXT("║　　　　　　　　　　╠══════╣"),
	TEXT("║　　　　　　　　　　║　　速度　　║"),
	TEXT("║　　　　　　　　　　║　　　　　　║"),
	TEXT("║　　　　　　　　　　╠══════╣"),
	TEXT("║　　　　　　　　　　║ 开始 ENTER ║"),
	TEXT("║　　　　　　　　　　║ 旋转 ↑    ║"),
	TEXT("║　　　　　　　　　　║ 左移 ←    ║"),
	TEXT("║　　　　　　　　　　║ 右移 →    ║"),
	TEXT("║　　　　　　　　　　║ 下降 ↓    ║"),
	TEXT("║　　　　　　　　　　║ 降落 SPACE ║"),
	TEXT("╚══════════╩══════╝"),};

const std::vector<ashes::String> GameApp::kGameOverTexts = {
	TEXT("　　　　　　"),
	TEXT("　游戏结束　"),
	TEXT("　　　　　　")};

const ashes::Rect GameApp::kPlayingFieldRect = {2, 1, 21, 20};

const ashes::Rect GameApp::kNoticeBoardRect = {26, 3, 33, 6};

const ashes::Rect GameApp::kScoreBarRect = {24, 10, 35, 10};

const ashes::Rect GameApp::kDifficultyBarRect = {24, 13, 35, 13};

GameApp::GameApp()
{
}

GameApp::~GameApp()
{
}

void GameApp::OnInit()
{
	ashes::random::ResetSeed();

	// initialize polyomino.
	PolyominoShapePool::Initialize();

	// setup console.
	ashes::Console& console = ashes::GConsole();
	console.Resize(MeasureGameWindowSize());
	console.SetUnresizable();
	console.SetCodePage(CP_UTF8);
	console.SetTitle(TEXT("俄罗斯方块 2.0 [ @南城边 ]"));
	console.SetUneditable();
	console.EnableDoubleBuffer(true);
	console.DrawStrings(kGameWindowTexts, console.Region(), 0x0F);
	console.Center();

	// setup events. 
	game_.OnPolyominoTransformed.Bind(this, &GameApp::OnPolyominoTransformed);
	game_.OnPolyominoPlaced.Bind(this, &GameApp::OnPolyominoPlaced);
	GetTimerManager().NewTimer2(std::bind(&GameCore::TickGame, &game_), 5ms, true);
}

void GameApp::DealKeyEvent(WORD key, bool pressed)
{
	if (pressed)
	{
		if (!game_.IsGameOVer())
		{
			switch (key)
			{
				case VK_UP:    game_.RotatePolyomino();         break;
				case VK_LEFT:  game_.ShiftPolyominoL();         break;
				case VK_RIGHT: game_.ShiftPolyominoR();         break;
				case VK_DOWN:  game_.DescendPolyomino(1);       break;
				case VK_SPACE: game_.DescendPolyomino(INT_MAX); break;
			}
		}
		else
		{
			if (key == VK_RETURN)
			{
				game_.StartGame();
				DrawGame();
			}
		}
	}
}

ashes::Coord GameApp::MeasureGameWindowSize()
{
	return {ashes::Console::MeasureWidth(kGameWindowTexts[0].data()),
		    static_cast<SHORT>(kGameWindowTexts.size())};
}

void GameApp::DrawGame()
{
	DrawPlayingField();
	DrawNoticeBoard();
	DrawScoreBar();
	DrawDifficultyBar();
}

void GameApp::DrawPlayingField()
{
	DrawPlayingField(0, PlayingField::kHeight - 1);
}

void GameApp::DrawPlayingField(SHORT top, SHORT bottom)
{
	const PlayingField& field = game_.GetPlayingField();
	const Polyomino& polyomino = game_.GetPolyomino();

	std::vector<ashes::String> texts(bottom - top + 1);
	
	for (SHORT y = top; y <= bottom; ++y)
	{
		for (SHORT x = 0; x < PlayingField::kWidth; ++x)
		{
			bool is_square = polyomino.TestSquare({x, y}) || field.TestSquare({x, y});
			texts[y - top].append(is_square ? TEXT("■") : TEXT("　"));
		}
	}

	ashes::Rect rect;
	rect.Left = kPlayingFieldRect.Left;
	rect.Right = kPlayingFieldRect.Right;
	rect.Top = kPlayingFieldRect.Top + top;
	rect.Bottom = kPlayingFieldRect.Top + bottom;

	ashes::GConsole().DrawBlank(rect, 0x0F);
	ashes::GConsole().DrawStrings(texts, rect, 0x0F);
}

void GameApp::DrawNoticeBoard()
{
	const Polyomino& polyomino = game_.GetNextPolyomino();
	const ashes::Rect bounding = polyomino.Bounding();

	std::vector<ashes::String> texts(bounding.Height());

	for (SHORT y = bounding.Top; y <= bounding.Bottom; ++y)
	{
		for (SHORT x = bounding.Left; x <= bounding.Right; ++x)
		{
			bool is_square = polyomino.TestSquare({x, y});
			texts[y - bounding.Top].append(is_square ? TEXT("■") : TEXT("　"));
		}
	}

	ashes::GConsole().DrawBlank(kNoticeBoardRect, 0x0F);
	ashes::GConsole().DrawStrings(texts, kNoticeBoardRect, 0x0F);
}

void GameApp::DrawScoreBar()
{
	ashes::String text = ashes::graph::ToFullWidthDigit(game_.GetScore(), 1);
	ashes::GConsole().DrawBlank(kScoreBarRect, 0x0F);
	ashes::GConsole().DrawString(text, kScoreBarRect, 0x0F);
}

void GameApp::DrawDifficultyBar()
{
	ashes::String text;

	for (int num_stars = game_.GetDifficultyLevel() / 2; num_stars > 0; --num_stars)
	{
		text.append(TEXT("★"));
	}

	if (game_.GetDifficultyLevel() % 2 != 0)
	{
		text.append(TEXT("☆"));
	}

	ashes::GConsole().DrawBlank(kDifficultyBarRect, 0x0F);
	ashes::GConsole().DrawString(text, kDifficultyBarRect, 0x0F);
}

void GameApp::DrawGameOverTip()
{
	ashes::graph::Border border = ashes::graph::BorderThickDoubleLine();
	border.color = 0x0F;
	ashes::GConsole().DrawFramedStrings(kGameOverTexts, kPlayingFieldRect,
		0x0F, &border);
}

void GameApp::OnPolyominoTransformed(const Polyomino& original_polyomino)
{
	ashes::Rect bounding1 = original_polyomino.Bounding();
	ashes::Rect bounding2 = game_.GetPolyomino().Bounding();

	ashes::Rect redraw_region = ashes::Rect::Intersect(
		ashes::Rect::Union(bounding1, bounding2),
		ashes::Rect({0, 0}, PlayingField::kSize));

	if (!redraw_region.IsEmpty())
	{
		DrawPlayingField(redraw_region.Top, redraw_region.Bottom);
	}
}

void GameApp::OnPolyominoPlaced(int num_completed_lines)
{
	if (game_.IsGameOVer())
	{
		DrawPlayingField();
		DrawGameOverTip();
	}
	else
	{
		if (num_completed_lines > 0)
		{
			DrawGame();
		}
		else
		{
			DrawPlayingField();
			DrawNoticeBoard();
		}
	}
}