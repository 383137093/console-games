#ifndef TETRIS_GAMEAPP_H
#define TETRIS_GAMEAPP_H

#include <vector>
#include "ashes/rect.h"
#include "ashes/coord.h"
#include "ashes/graph.h"
#include "ashes/consoleapp.h"
#include "gamecore.h"

class GameApp : public ashes::SimpleConsoleApp
{
public:

    GameApp();
    ~GameApp();

protected:

    void OnInit() override;
    void DealKeyEvent(WORD key, bool pressed) override;

private:

    //==========================================================================
    // Window
    //==========================================================================

    static const std::vector<ashes::String> kGameWindowTexts;
    static const std::vector<ashes::String> kGameOverTexts;

    static const ashes::Rect kPlayingFieldRect;
    static const ashes::Rect kNoticeBoardRect;
    static const ashes::Rect kScoreBarRect;
    static const ashes::Rect kDifficultyBarRect;

    static ashes::Coord MeasureGameWindowSize();

    //==========================================================================
    // Draw
    //==========================================================================

    void DrawGame();
    void DrawPlayingField();
    void DrawPlayingField(SHORT top, SHORT bottom);
    void DrawNoticeBoard();
    void DrawScoreBar();
    void DrawDifficultyBar();
    void DrawGameOverTip();

    //==========================================================================
    // Event handling
    //==========================================================================

    void OnPolyominoTransformed(const Polyomino& original_polyomino);
    void OnPolyominoPlaced(int num_completed_lines);

private:
    
    GameCore game_;
};

#endif