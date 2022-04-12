#include "gameapp.h"
#include <chrono>
#include <cassert>
#include <algorithm>
#include "ashes/winapi.h"
#include "ashes/console.h"

#define LOCTEXT(ID) (in_chinese_ ? ID##CN : ID##EN)

using namespace std::chrono_literals;

const std::vector<ashes::String> GameApp::kGameWindowTextsCN = {
	TEXT("╔════╦═════╦═════╦═════╦══════════╗"),
	TEXT("║  玩家  ║愚蠢的电脑║平庸的电脑║无敌的电脑║                    ║"),
	TEXT("╠════╩═════╩═════╩═════╣   九    宫    棋   ║"),
	TEXT("║                                            ║                    ║"),
	TEXT("║  ┏━━┓        ┏━━┓        ┏━━┓  ║                    ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ║   第  ９９９  回   ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ║                    ║"),
	TEXT("║  ┗━━┛╲      ┗━━┛      ╱┗━━┛  ║   ■  方  行  棋   ║"),
	TEXT("║    ┃┃    ╲      ┃┃      ╱    ┃┃    ║                    ║"),
	TEXT("║    ┃┃      ╲    ┃┃    ╱      ┃┃    ╠══════════╣"),
	TEXT("║    ┃┃        ╲  ┃┃  ╱        ┃┃    ║                    ║"),
	TEXT("║  ┏━━┓        ┏━━┓        ┏━━┓  ║   红  方  悔  棋   ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ║                    ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ╠══════════╣"),
	TEXT("║  ┗━━┛        ┗━━┛        ┗━━┛  ║                    ║"),
	TEXT("║    ┃┃        ╱  ┃┃  ╲        ┃┃    ║   蓝  方  悔  棋   ║"),
	TEXT("║    ┃┃      ╱    ┃┃    ╲      ┃┃    ║                    ║"),
	TEXT("║    ┃┃    ╱      ┃┃      ╲    ┃┃    ╠══════════╣"),
	TEXT("║  ┏━━┓╱      ┏━━┓      ╲┏━━┓  ║                    ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ║   重  新  开  始   ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ║                    ║"),
	TEXT("║  ┗━━┛        ┗━━┛        ┗━━┛  ╠══════════╣"),
	TEXT("║                                            ║                    ║"),
	TEXT("╠════╦═════╦═════╦═════╣   切  换  语  言   ║"),
	TEXT("║  玩家  ║愚蠢的电脑║平庸的电脑║无敌的电脑║                    ║"),
	TEXT("╚════╩═════╩═════╩═════╩══════════╝")};

const std::vector<ashes::String> GameApp::kGameWindowTextsEN = {
	TEXT("╔════╦═════╦═════╦═════╦══════════╗"),
	TEXT("║ Player ║ StupidAI ║MediocreAI║InvincibAI║                    ║"),
	TEXT("╠════╩═════╩═════╩═════╣   JIUGONGCHESS     ║"),
	TEXT("║                                            ║                    ║"),
	TEXT("║  ┏━━┓        ┏━━┓        ┏━━┓  ║                    ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ║ ROUND ９９９ GOING ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ║                    ║"),
	TEXT("║  ┗━━┛╲      ┗━━┛      ╱┗━━┛  ║  ■ PLAYER ACTION  ║"),
	TEXT("║    ┃┃    ╲      ┃┃      ╱    ┃┃    ║                    ║"),
	TEXT("║    ┃┃      ╲    ┃┃    ╱      ┃┃    ╠══════════╣"),
	TEXT("║    ┃┃        ╲  ┃┃  ╱        ┃┃    ║                    ║"),
	TEXT("║  ┏━━┓        ┏━━┓        ┏━━┓  ║   TAKE RED  BACK   ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ║                    ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ╠══════════╣"),
	TEXT("║  ┗━━┛        ┗━━┛        ┗━━┛  ║                    ║"),
	TEXT("║    ┃┃        ╱  ┃┃  ╲        ┃┃    ║   TAKE BLUE BACK   ║"),
	TEXT("║    ┃┃      ╱    ┃┃    ╲      ┃┃    ║                    ║"),
	TEXT("║    ┃┃    ╱      ┃┃      ╲    ┃┃    ╠══════════╣"),
	TEXT("║  ┏━━┓╱      ┏━━┓      ╲┏━━┓  ║                    ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ║    RESTART GAME    ║"),
	TEXT("║  ┃    ┃━━━━┃    ┃━━━━┃    ┃  ║                    ║"),
	TEXT("║  ┗━━┛        ┗━━┛        ┗━━┛  ╠══════════╣"),
	TEXT("║                                            ║                    ║"),
	TEXT("╠════╦═════╦═════╦═════╣  SWITCH  LANGUAGE  ║"),
	TEXT("║ Player ║ StupidAI ║MediocreAI║InvincibAI║                    ║"),
	TEXT("╚════╩═════╩═════╩═════╩══════════╝")};

const std::vector<ashes::String> GameApp::kGameGuideTextsCN = {
	TEXT("                    "),
	TEXT("移动棋子至共线则获胜"),
	TEXT("                    "),
	TEXT("点击任意位置开始游戏"),
	TEXT("                    "),};

const std::vector<ashes::String> GameApp::kGameGuideTextsEN = {
	TEXT("                        "),
	TEXT("win by collinear pieces "),
	TEXT("                        "),
	TEXT("click anywhere to start "),
	TEXT("                        "),};

const std::vector<ashes::String> GameApp::kGameStateTextsCN = {
	TEXT("■  方  行  棋"),
	TEXT("强  制  平  局"),
	TEXT("■  方  获  胜"),};

const std::vector<ashes::String> GameApp::kGameStateTextsEN = {
	TEXT("■ PLAYER ACTION"),
	TEXT("  FORCED  DRAW  "),
	TEXT(" ■ PLAYER WIN  "),};

const std::vector<const std::vector<ashes::Rect>*> GameApp::kPanelRects = {
	&kChessboardGridRects, &kSenteRadioButtonRects,
	&kGoteRadioButtonRects, &kMenuItemRects};

const std::vector<ashes::Rect> GameApp::kChessboardGridRects = {
	{6, 5,  9, 6 }, {22, 5,  25, 6 }, {38, 5,  41, 6 },
	{6, 12, 9, 13}, {22, 12, 25, 13}, {38, 12, 41, 13},
	{6, 19, 9, 20}, {22, 19, 25, 20}, {38, 19, 41, 20}};

const std::vector<ashes::Rect> GameApp::kSenteRadioButtonRects = {
	{2, 24, 9, 24}, {12, 24, 21, 24}, {24, 24, 33, 24}, {36, 24, 45, 24}};
    
const std::vector<ashes::Rect> GameApp::kGoteRadioButtonRects = {
	{2, 1, 9, 1}, {12, 1, 21, 1}, {24, 1, 33, 1}, {36, 1, 45, 1}};
    
const std::vector<ashes::Rect> GameApp::kMenuItemRects = {
	{48, 10, 67, 12}, {48, 14, 67, 16}, 
	{48, 18, 67, 20}, {48, 22, 67, 24}};

const ashes::Rect GameApp::kGameGuideRect = {6, 5, 41, 20};

const ashes::Rect GameApp::kGameRoundsCounterRect = {48, 5, 67, 5};

const ashes::Rect GameApp::kGameStateBarRect = {48, 7, 67, 7};

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

	// initialize AI.
	GameAI::Initialize();
	SetupGameAI(sente_ai_, sente_type_);
	SetupGameAI(gote_ai_, gote_type_);

	// setup console.
	ashes::Console& console = ashes::GConsole();
	console.Resize(MeasureGameWindowSize());
	console.Center();
	console.SetUnresizable();
	console.SetCodePage(CP_UTF8);
	console.SetTitle(TEXT("九宫棋 4.0 [ @南城边 ]"));
	console.SetUneditable();

	// initialize ui.
	in_game_guide_ = true;
	in_chinese_ = ashes::win::IsAtLocaleZN();
	DrawGameWindow();
	DrawGameGuide();

	// setup events. 
	game_.OnNewTurn.Bind(this, &GameApp::DrawGameTurn);
	activate_ai_timer_ = GetTimerManager().NewTimer2(
		std::bind(&GameApp::OnActivateGameAI, this), 15ms, true);
}

void GameApp::DealMouseClickEvent()
{
	WidgetID focus_widget = GetFocusWidget();

	if (in_game_guide_)
	{
		DrawGameWindow();
		in_game_guide_ = false;
	}

	if (BreakWidgetID(focus_widget).panel != PanelID::Chessboard)
	{
		OnClickChessboard(kInvalidPieceCoord);
	}

	SimpleConsoleApp::DealMouseClickEvent();
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

ashes::Coord GameApp::MeasureGameWindowSize()
{
	return {ashes::Console::MeasureWidth(kGameWindowTextsCN[0].data()),
	        static_cast<SHORT>(kGameWindowTextsCN.size())};
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

void GameApp::DrawGameWindow() const
{
	ashes::Console& console = ashes::GConsole();
	console.DrawStrings(LOCTEXT(kGameWindowTexts), console.Region(), kBasicColor);
	PaintWidget({PanelID::SenteRadioGroup, sente_type_}, kSenteColor, kBasicColor);
	PaintWidget({PanelID::GoteRadioGroup, gote_type_}, kGoteColor, kBasicColor);
	DrawGameTurn();
}

void GameApp::DrawGameTurn() const
{
	DrawChessboardGrids();
	DrawGameStateBar();
	DrawGameRoundsCounter();
}

void GameApp::DrawGameGuide() const
{
	ashes::graph::Border border = ashes::graph::BorderThickDoubleLine();
	border.color = 0x70;
	ashes::GConsole().DrawFramedStrings(LOCTEXT(kGameGuideTexts),
		kGameGuideRect, 0x70, &border);
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
	const auto& kTexts = LOCTEXT(kGameStateTexts);
	const ashes::String* text = &kTexts[0];
	WORD color = ashes::graph::MakeFGColor(GetPlayerColor(game_.GetCurrentPlayer()));

	if (game_.GetGameState() == GameState::Draw)
	{
		text  = &kTexts[1];
		color = ashes::graph::MakeFGColor(kBasicColor);
	}
	else if (game_.GetGameState() == GameState::SenteWin)
	{
		text  = &kTexts[2];
		color = ashes::graph::MakeFGColor(kSenteColor);
	}
	else if (game_.GetGameState() == GameState::GoteWin)
	{
		text  = &kTexts[2];
		color = ashes::graph::MakeFGColor(kGoteColor);
	}

	ashes::GConsole().DrawString(*text, kGameStateBarRect, color);
}

void GameApp::DrawGameRoundsCounter() const
{
	ashes::String text = ashes::graph::ToFullWidthDigit(game_.GetNumRounds(), 3);
	ashes::GConsole().DrawString(text, kGameRoundsCounterRect, kBasicColor);
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
	else if (item == MenuItemID::SwitchLanguage)
	{
		in_chinese_ ^= true;
		DrawGameWindow();
		OnMouseEnter(GetFocusWidget());
	}
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
	static const int kIQMins[] = {0, 0,  2,  30};
	static const int kIQMaxs[] = {0, 20, 22, 30};
	int index = static_cast<int>(type);
	game_ai.SetIQ(kIQMins[index], kIQMaxs[index]);
}
