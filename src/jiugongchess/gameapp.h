#ifndef JIUGONGCHESS_GAMEAPP_H
#define JIUGONGCHESS_GAMEAPP_H

#include <vector>
#include "ashes/rect.h"
#include "ashes/coord.h"
#include "ashes/graph.h"
#include "ashes/consoleapp.h"
#include "gameai.h"
#include "jiugongchess.h"

class GameApp : public ashes::SimpleConsoleApp
{

public:

    GameApp();
    ~GameApp();

protected:

    //==========================================================================
    // Overrides
    //==========================================================================

    void OnInit() override;
    void DealMouseClickEvent() override;
    WidgetID FindFocusWidget(const COORD& mouse_coord) const override;
    void OnMouseEnter(WidgetID widget) override;
    void OnMouseLeave(WidgetID widget) override;
    void OnMouseClick(WidgetID widget) override;

private:

    //==========================================================================
    // Types
    //==========================================================================
    
    typedef JiugongChess::GameState GameState;

    enum class PanelID { Null, Chessboard, SenteRadioGroup, GoteRadioGroup, Menu };
    enum class PlayerType { Human, StupidAI, MediocreAI, InvincibleAI };
    enum class MenuItemID { SenteBackMove, GoteBackMove, NewGame, SwitchLanguage };

    struct BreakedWidgetID
    {
        template <class T, class U>
        BreakedWidgetID(T p, U i)
            : panel(static_cast<PanelID>(p)),
              index(static_cast<unsigned int>(i)) {}

        PanelID      panel;  // owner panel of this widget.
        unsigned int index;  // index of this widget in panel.
    };

    //==========================================================================
    // Hierarchy: Window > Panel > Widget.
    //==========================================================================

    static const std::vector<ashes::String> kGameWindowTextsCN;
    static const std::vector<ashes::String> kGameWindowTextsEN;
    static const std::vector<ashes::String> kGameGuideTextsCN;
    static const std::vector<ashes::String> kGameGuideTextsEN;
    static const std::vector<ashes::String> kGameStateTextsCN;
    static const std::vector<ashes::String> kGameStateTextsEN;

    static const std::vector<const std::vector<ashes::Rect>*> kPanelRects;
    static const std::vector<ashes::Rect>                     kChessboardGridRects;
    static const std::vector<ashes::Rect>                     kSenteRadioButtonRects;
    static const std::vector<ashes::Rect>                     kGoteRadioButtonRects;
    static const std::vector<ashes::Rect>                     kMenuItemRects;

    static const ashes::Rect  kGameGuideRect;
    static const ashes::Rect  kGameRoundsCounterRect;
    static const ashes::Rect  kGameStateBarRect;
    static const ashes::Coord kInvalidPieceCoord;

    static ashes::Coord MeasureGameWindowSize();
    static WidgetID MakeWidgetID(BreakedWidgetID bkid);
    static BreakedWidgetID BreakWidgetID(WidgetID id);

    //==========================================================================
    // Color scheme
    //==========================================================================

    static const WORD kBasicColor  = 0x0B;
    static const WORD kSenteColor  = 0xCC;
    static const WORD kGoteColor   = 0x99;
    static const WORD kAccentColor = 0x88;

    static WORD GetGridColor(Chessboard::GridType grid);
    static WORD GetPlayerColor(Chessboard::PlayerSide player);
    WORD GetGridColor(const ashes::Coord& coord) const;
    bool IsWidgetHoverStateEnabled(WidgetID widget) const;

    //==========================================================================
    // Draw
    //==========================================================================
    
    void DrawGameWindow() const;
    void DrawGameGuide() const;
    void DrawGameTurn() const;

    void PaintWidget(BreakedWidgetID widget, WORD color) const;
    void PaintWidget(BreakedWidgetID widget, WORD fgcolor, WORD bgcolor) const;
    void PaintPieceOptionalMove(const ashes::Coord& coord, bool accent) const;

    void DrawChessboardGrid(const ashes::Coord& coord, bool darking) const;
    void DrawChessboardGrids() const;
    void DrawGameStateBar() const;
    void DrawGameRoundsCounter() const;

    //==========================================================================
    // Event handling
    //==========================================================================

    GameAI* GetCurrentGameAI();
    void OnActivateGameAI();

    void OnClickChessboard(const ashes::Coord& coord);
    void OnClickSenteRadioGroup(PlayerType item);
    void OnClickGoteRadioGroup(PlayerType item);
    void OnClickMenu(MenuItemID item);

    void StartBlinkPiece();
    void SetupGameAI(GameAI& game_ai, PlayerType type);

    //==========================================================================
    // Variables
    //==========================================================================

    JiugongChess game_;
    GameAI       sente_ai_{Chessboard::PlayerSide::Sente};
    GameAI       gote_ai_{Chessboard::PlayerSide::Gote};

    ashes::Coord selected_piece_ = kInvalidPieceCoord;
    PlayerType   sente_type_ = PlayerType::Human;
    PlayerType   gote_type_ = PlayerType::StupidAI;

    bool in_game_guide_ = false;
    bool in_chinese_ = true;

    ashes::TimerHandle activate_ai_timer_ = nullptr;
    ashes::TimerHandle blink_piece_timer_ = nullptr;
};

#endif
