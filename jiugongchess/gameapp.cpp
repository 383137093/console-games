#include "gameapp.h"
#include <chrono>
#include <cassert>
#include <algorithm>
#include "ashes/console.h"

using namespace std::chrono_literals;

const std::vector<ashes::String> GameApp::kWindow = {
	TEXT("╔══╦════╦════╦════╦════╦═══════════╗"),
	TEXT("║玩家║初级电脑║中级电脑║高级电脑║终极电脑║　　　　　　　　　　　║"),
	TEXT("╠══╩════╩════╩════╩════╣　　九　　宫　　棋　　║"),
	TEXT("║　　　　　　　　　　　　　　　　　　　　　　║　　　　　　　　　　　║"),
	TEXT("║　┏━━┓　　　　┏━━┓　　　　┏━━┓　║　　　　　　　　　　　║"),
	TEXT("║　┃　　┃━━━━┃　　┃━━━━┃　　┃　║　　第　９９９　回　　║"),
	TEXT("║　┃　　┃━━━━┃　　┃━━━━┃　　┃　║　　　　　　　　　　　║"),
	TEXT("║　┗━━┛╲　　　┗━━┛　　　╱┗━━┛　║　　？　方　执　子　　║"),
	TEXT("║　　┃┃　　╲　　　┃┃　　　╱　　┃┃　　║　　　　　　　　　　　║"),
	TEXT("║　　┃┃　　　╲　　┃┃　　╱　　　┃┃　　╠═══════════╣"),
	TEXT("║　　┃┃　　　　╲　┃┃　╱　　　　┃┃　　║　　　　　　　　　　　║"),
	TEXT("║　┏━━┓　　　　┏━━┓　　　　┏━━┓　║　　黄　方　悔　棋　　║"),
	TEXT("║　┃　　┃━━━━┃　　┃━━━━┃　　┃　║　　　　　　　　　　　║"),
	TEXT("║　┃　　┃━━━━┃　　┃━━━━┃　　┃　╠═══════════╣"),
	TEXT("║　┗━━┛　　　　┗━━┛　　　　┗━━┛　║　　　　　　　　　　　║"),
	TEXT("║　　┃┃　　　　╱　┃┃　╲　　　　┃┃　　║　　绿　方　悔　棋　　║"),
	TEXT("║　　┃┃　　　╱　　┃┃　　╲　　　┃┃　　║　　　　　　　　　　　║"),
	TEXT("║　　┃┃　　╱　　　┃┃　　　╲　　┃┃　　╠═══════════╣"),
	TEXT("║　┏━━┓╱　　　┏━━┓　　　╲┏━━┓　║　　　　　　　　　　　║"),
	TEXT("║　┃　　┃━━━━┃　　┃━━━━┃　　┃　║　　重　新　开　始　　║"),
	TEXT("║　┃　　┃━━━━┃　　┃━━━━┃　　┃　║　　　　　　　　　　　║"),
	TEXT("║　┗━━┛　　　　┗━━┛　　　　┗━━┛　╠═══════════╣"),
	TEXT("║　　　　　　　　　　　　　　　　　　　　　　║　　　　　　　　　　　║"),
	TEXT("╠══╦════╦════╦════╦════╣　　退　　　　　出　　║"),
	TEXT("║玩家║初级电脑║中级电脑║高级电脑║终极电脑║　　　　　　　　　　　║"),
	TEXT("╚══╩════╩════╩════╩════╩═══════════╝")};

const ashes::Coord GameApp::kWindowSize = {
	ashes::Console::MeasureWidth(kWindow[0].data()),
	static_cast<SHORT>(kWindow.size())};

const std::vector<const std::vector<ashes::Rect>*> GameApp::kPanelRects = {
	&kChessboardGridRects, &kSenteRadioButtonRects,
	&kGoteRadioButtonRects, &kMenuItemRects};

const std::vector<ashes::Rect> GameApp::kChessboardGridRects = {
	{6, 5,  9, 6 }, {22, 5,  25, 6 }, {38, 5,  41, 6 },
	{6, 12, 9, 13}, {22, 12, 25, 13}, {38, 12, 41, 13},
	{6, 19, 9, 20}, {22, 19, 25, 20}, {38, 19, 41, 20}};

const std::vector<ashes::Rect> GameApp::kSenteRadioButtonRects = {
	{2,  24, 5,  24}, {8,  24, 15, 24}, {18, 24, 25, 24},
	{28, 24, 35, 24}, {38, 24, 45, 24}};
    
const std::vector<ashes::Rect> GameApp::kGoteRadioButtonRects = {
	{2,  1, 5,  1}, {8,  1, 15, 1}, {18, 1, 25, 1},
	{28, 1, 35, 1}, {38, 1, 45, 1}};
    
const std::vector<ashes::Rect> GameApp::kMenuItemRects = {
	{48, 10, 69, 12}, {48, 14, 69, 16}, 
	{48, 18, 69, 20}, {48, 22, 69, 24}};

const ashes::Rect GameApp::kGameRoundsCounterRect = {56, 5, 61, 5};

const ashes::Rect GameApp::kGameStateBarRect = {52, 7, 65, 7};

const ashes::Coord GameApp::kInvalidPieceCoord = {-1, -1};

GameApp::GameApp()
{
}

GameApp::~GameApp()
{
}

void GameApp::OnInit()
{
	ashes::random::ResetSeed();
	GameAI::Initialize();

	// setup console.
	ashes::Console& console = ashes::GConsole();
	console.Resize(kWindowSize);
	console.Center();
	console.SetUnresizable();
	console.SetTitle(TEXT("九宫棋 4.0 [ @南城边 ]"));
	console.SetUneditable();

	// initialize ui.
	console.DrawStrings(kWindow, console.Region(), kBasicColor);
	PaintWidget({PanelID::SenteRadioGroup, sente_type_}, kSenteColor, kBasicColor);
	PaintWidget({PanelID::GoteRadioGroup, gote_type_}, kGoteColor, kBasicColor);
	OnNewGameTurn();

	// setup events. 
	game_.OnNewTurn.Bind(this, &GameApp::OnNewGameTurn);
	activate_ai_timer_ = GetTimerManager().NewTimer2(
		[this] { OnActivateGameAI(); }, 15ms, true);
}

GameApp::WidgetID GameApp::FindFocusWidget(const COORD& mouse_coord) const
{
	for (std::size_t panel = 0; panel < kPanelRects.size(); ++panel)
	{
		const std::vector<ashes::Rect>& rects = *kPanelRects[panel];

		auto iter = std::find_if(rects.begin(), rects.end(),[&mouse_coord](
			const ashes::Rect& rect) { return rect.Contain(mouse_coord); });

		if (iter != rects.end())
		{
			return MakeWidgetID({panel + 1, iter - rects.begin()});
		}
	}

	return kNullWidgetID;
}

void GameApp::DealMouseClickEvent()
{
	WidgetID focus_widget = GetFocusWidget();

	if (BreakWidgetID(focus_widget).panel != PanelID::Chessboard)
	{
		OnClickChessboard(kInvalidPieceCoord);
	}

	SimpleConsoleApp::DealMouseClickEvent();
}

void GameApp::OnMouseEnter(WidgetID widget)
{
	if (IsWidgetHoverStateEnabled(widget))
	{
		PaintWidget(BreakWidgetID(widget), ashes::graph::MakeBGColor(kAccentColor));
	}
}

void GameApp::OnMouseLeave(WidgetID widget)
{
	if (IsWidgetHoverStateEnabled(widget))
	{
		PaintWidget(BreakWidgetID(widget), ashes::graph::MakeBGColor(kBasicColor));
	}
}

void GameApp::OnMouseClick(WidgetID widget)
{
	const PanelID panel = BreakWidgetID(widget).panel;
	const int     index = BreakWidgetID(widget).index;

	if (panel == PanelID::Chessboard)
	{
		OnClickChessboard(Chessboard::IndexToCoord(index));
	}
	else if (panel == PanelID::SenteRadioGroup)
	{
		OnClickSenteRadioGroup(static_cast<PlayerType>(index));
	}
	else if (panel == PanelID::GoteRadioGroup)
	{
		OnClickGoteRadioGroup(static_cast<PlayerType>(index));
	}
	else if (panel == PanelID::Menu)
	{
		OnClickMenu(static_cast<MenuItemID>(index));
	}
}

GameApp::WidgetID GameApp::MakeWidgetID(BreakedWidgetID bkid)
{
	return MAKELONG(bkid.panel, bkid.index);
}

GameApp::BreakedWidgetID GameApp::BreakWidgetID(WidgetID id)
{
	return {LOWORD(id), HIWORD(id)};
}

WORD GameApp::GetGridColor(Chessboard::GridType grid)
{
	switch (grid)
	{
		case Chessboard::GridType::Space: return kBasicColor;
		case Chessboard::GridType::SentePiece: return kSenteColor;
		case Chessboard::GridType::GotePiece: return kGoteColor;
		default: assert(false);
	}
	return 0x00;
}

WORD GameApp::GetPlayerColor(Chessboard::PlayerSide player)
{
	return GetGridColor(Chessboard::PieceTypeOwnedByPlayer(player));
}

WORD GameApp::GetGridColor(const ashes::Coord& coord) const
{
	return GetGridColor(game_.GetChessboard().GetGridType(coord));
}

bool GameApp::IsWidgetHoverStateEnabled(WidgetID widget) const
{
	return BreakWidgetID(widget).panel != PanelID::Chessboard
		&& widget != MakeWidgetID({PanelID::SenteRadioGroup, sente_type_})
		&& widget != MakeWidgetID({PanelID::GoteRadioGroup, gote_type_});
}

void GameApp::PaintWidget(BreakedWidgetID widget, WORD color) const
{
	int panel_index = static_cast<int>(widget.panel) - 1;
	const ashes::Rect& rect = (*kPanelRects[panel_index])[widget.index];
	ashes::GConsole().DrawColor(color, rect);
}

void GameApp::PaintWidget(BreakedWidgetID widget, WORD fgcolor, WORD bgcolor) const
{
	PaintWidget(widget, ashes::graph::CrossColor(fgcolor, bgcolor));
}

void GameApp::PaintPieceOptionalMove(const ashes::Coord& coord, bool accent) const
{
	const WORD color = (accent ? GetGridColor(coord) : kBasicColor);
	const WORD fgcolor = ashes::graph::MakeFGColor(color);

	std::vector<Chessboard::PieceStep> steps;
	game_.GetChessboard().GetOptionalMove(coord, steps);

	for (const Chessboard::PieceStep& step : steps)
	{
		int index = Chessboard::CoordToIndex(step.second);
		const ashes::Rect& rect = kChessboardGridRects[index];
		ashes::Rect outer_rect = ashes::Rect(rect).Extend({2, 1});
		ashes::GConsole().DrawColor(fgcolor, outer_rect);
	}
}

void GameApp::DrawChessboardGrid(const ashes::Coord& coord, bool darking) const
{
	WORD color = (darking ? kBasicColor : GetGridColor(coord));
	int index = Chessboard::CoordToIndex(coord);
	PaintWidget({PanelID::Chessboard, index}, kBasicColor, color);
}

void GameApp::DrawChessboardGrids() const
{
	for (int index = 0; index < Chessboard::kNumGrids; ++index)
	{
		DrawChessboardGrid(Chessboard::IndexToCoord(index), false);
	}
}

void GameApp::DrawGameStateBar() const
{
	ashes::String text;
	WORD color = 0x00;

	if (game_.GetGameState() == GameState::UnderWay)
	{
		text  = TEXT("■　方　执　子");
		color = GetPlayerColor(game_.GetCurrentPlayer());
	}
	else if (game_.GetGameState() == GameState::Draw)
	{
		text  = TEXT("强　制　平　局");
		color = kBasicColor;
	}
	else if (game_.GetGameState() == GameState::SenteWin)
	{
		text  = TEXT("■　方　胜　利");
		color = kSenteColor;
	}
	else if (game_.GetGameState() == GameState::GoteWin)
	{
		text  = TEXT("■　方　胜　利");
		color = kGoteColor;
	}

	color = ashes::graph::MakeFGColor(color);
	ashes::GConsole().DrawString(text, kGameStateBarRect, color);
}

void GameApp::DrawGameRoundsCounter() const
{
	ashes::String text = ashes::graph::ToFullWidthDigit(game_.GetNumRounds(), 3);
	ashes::GConsole().DrawString(text, kGameRoundsCounterRect, kBasicColor);
}

void GameApp::OnNewGameTurn()
{
	DrawChessboardGrids();
	DrawGameStateBar();
	DrawGameRoundsCounter();
}

void GameApp::OnActivateGameAI()
{
	while (GameAI* ai = GetCurrentGameAI())
	{
		Chessboard::PieceStep step = ai->HandleGameTurn(game_.GetChessboard());
		bool success = game_.MovePiece(step);
		assert(success);
	}
}

void GameApp::OnClickChessboard(const ashes::Coord& coord)
{
	if (game_.GetGameState() == GameState::UnderWay && coord != selected_piece_)
	{
		const ashes::Coord selected_piece_copy = selected_piece_;

		// deselect current piece.
		if (Chessboard::ValidateCoord(selected_piece_))
		{
			PaintPieceOptionalMove(selected_piece_, false);
			DrawChessboardGrid(selected_piece_, false);
			GetTimerManager().InvalidateTimer(blink_piece_timer_);
			selected_piece_ = kInvalidPieceCoord;
		}

		// select a new piece or move current piece.
		if (Chessboard::ValidateCoord(coord))
		{
			if (game_.GetChessboard().GetGridType(coord) ==
				Chessboard::PieceTypeOwnedByPlayer(game_.GetCurrentPlayer()))
			{	
				PaintPieceOptionalMove(coord, true);
				DrawChessboardGrid(coord, true);
				StartBlinkPiece();
				selected_piece_ = coord;
			}
			else if (Chessboard::ValidateCoord(selected_piece_copy))
			{
				game_.MovePiece({selected_piece_copy, coord});
			}
		}
	}
}

void GameApp::OnClickSenteRadioGroup(PlayerType item)
{
	if (item != sente_type_)
	{
		PaintWidget({PanelID::SenteRadioGroup, sente_type_}, kBasicColor);
		PaintWidget({PanelID::SenteRadioGroup, item}, kSenteColor, kBasicColor);
		SetupGameAI(sente_ai_, sente_type_ = item);
	}
}

void GameApp::OnClickGoteRadioGroup(PlayerType item)
{
	if (item != gote_type_)
	{
		PaintWidget({PanelID::GoteRadioGroup, gote_type_}, kBasicColor);
		PaintWidget({PanelID::GoteRadioGroup, item}, kGoteColor, kBasicColor);
		SetupGameAI(gote_ai_, gote_type_ = item);
	}
}

void GameApp::OnClickMenu(MenuItemID item)
{
	if (item == MenuItemID::SenteBackMove)
	{
		game_.BackMove(Chessboard::PlayerSide::Sente);
	}
	else if (item == MenuItemID::GoteBackMove)
	{
		game_.BackMove(Chessboard::PlayerSide::Gote);
	}
	else if (item == MenuItemID::NewGame)
	{
		game_.NewGame();
	}
	else if (item == MenuItemID::ExitGame)
	{
		Exit();
	}
}

GameAI* GameApp::GetCurrentGameAI()
{
	if (game_.GetGameState() == GameState::UnderWay)
	{
		return game_.GetCurrentPlayer() == Chessboard::PlayerSide::Sente
			? (sente_type_ == PlayerType::Human ? nullptr : &sente_ai_)
			: (gote_type_ == PlayerType::Human ? nullptr : &gote_ai_);
	}
	return nullptr;
}

void GameApp::StartBlinkPiece()
{
	assert(blink_piece_timer_ == nullptr);
	blink_piece_timer_ = GetTimerManager().NewTimer2(
		[this, darking = true]() mutable {
			DrawChessboardGrid(selected_piece_, darking ^= true); },
		800ms, true);
}

void GameApp::SetupGameAI(GameAI& game_ai, PlayerType type)
{
	static const float kIQMins[] = {0.0f, 0.5f, 0.4f, 0.4f, 1.0f};
	static const float kIQMaxs[] = {0.0f, 0.7f, 1.0f, 1.5f, 1.0f};
	int index = static_cast<int>(type);
	game_ai.SetIQ(kIQMins[index], kIQMaxs[index]);
}
