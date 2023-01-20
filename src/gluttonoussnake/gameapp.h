#ifndef GLUTTONOUSSNAKE_GAMEAPP_H
#define GLUTTONOUSSNAKE_GAMEAPP_H

#include <bitset>
#include <memory>
#include <vector>
#include "ashes/consoleapp.h"
#include "ashes/direction4.h"
#include "gameai.h"
#include "gamecore.h"
#include "gamerenderer.h"

class GameApp : public ashes::SimpleConsoleApp
{

public:

    GameApp();
    ~GameApp();

protected:

    void OnInit() override;
    void DealKeyEvent(WORD key, bool pressed) override;

private:

    void OnSnakeMoved(const GameCore::SnakeMoveEvent& result);
    void OnSpaceKey();
    void OnEnterKey();

    GameRenderer& GetRenderer() const;
    void OnSwitchPaintingStyle();
    void OnToggleResultVisibility();
    void DrawFinishedGame();

    void OnExpandGameMap();
    void OnShrinkGameMap();
    void OnDirectionKeyEvent(ashes::Direction4 dir, bool pressed);
    void OnToggleAIController();
    void UpdateSnakeForwardDirection();

private:

    GameCore                                   game_;
    std::vector<std::unique_ptr<GameRenderer>> renderers_;
    std::size_t                                renderer_index_ = 0;
    bool                                       result_visible_ = false;
    std::bitset<4>                             dir_key_states_;
    GameAI                                     ai_;
    bool                                       ai_enabled_ = false;;
};

#endif
