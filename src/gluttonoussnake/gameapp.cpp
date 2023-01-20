#include "gameapp.h"
#include "ashes/random.h"
#include "ashes/console.h"

using namespace std::chrono_literals;

GameApp::GameApp()
{
    renderers_.push_back(GameRenderer::MakeLineStyleRenderer());
    renderers_.push_back(GameRenderer::MakeSquareStyleRenderer());
}

GameApp::~GameApp()
{
}

void GameApp::OnInit()
{
    ashes::random::ResetSeed();

    // setup console.
    ashes::Console& console = ashes::GConsole();
    console.SetUnresizable();
    console.SetCodePage(CP_UTF8);
    console.SetTitle(TEXT("贪吃蛇 3.0 [ @南城边 ]"));
    console.SetUneditable();
    GetRenderer().SetupConsole(game_);
    GetRenderer().DrawGuide(game_);

    // setup events. 
    game_.OnSnakeMoved.Bind(this, &GameApp::OnSnakeMoved);
    GetTimerManager().NewTimer2(std::bind(&GameCore::TickGame, &game_), 5ms, true);
}

void GameApp::DealKeyEvent(WORD key, bool pressed)
{
    switch (key)
    {
        case VK_LEFT:  OnDirectionKeyEvent(ashes::Direction4::Leftward,  pressed); break;
        case VK_UP:    OnDirectionKeyEvent(ashes::Direction4::Forward,   pressed); break;
        case VK_RIGHT: OnDirectionKeyEvent(ashes::Direction4::Rightward, pressed); break;
        case VK_DOWN:  OnDirectionKeyEvent(ashes::Direction4::Backward,  pressed); break;
    }
    
    if (pressed)
    {
        switch (key)
        {
            case VK_SPACE:   OnSpaceKey();               break;
            case VK_RETURN:  OnEnterKey();               break;
            case VK_TAB:     OnSwitchPaintingStyle();    break;
            case VK_CONTROL: OnToggleAIController();     break;
            case 'W':        OnExpandGameMap();          break;
            case 'S':        OnShrinkGameMap();          break;
            case 'A':        game_.AccelerateSnake();    break;
            case 'D':        game_.DecelerateSnake();    break;
            case 'H':        OnToggleResultVisibility(); break;
        }
    }
}

void GameApp::OnSnakeMoved(const GameCore::SnakeMoveEvent& event)
{
    if (event.result == GameCore::SnakeMoveResult::Move)
    {
        GetRenderer().DrawSnakeMove(event);
    }
    else if (event.result == GameCore::SnakeMoveResult::Grow)
    {
        GetRenderer().DrawSnakeMove(event);
        GetRenderer().DrawFood(game_.GetFood());
    }
    else if (event.result == GameCore::SnakeMoveResult::Die ||
             event.result == GameCore::SnakeMoveResult::Win)
    {
        GetRenderer().DrawResult(game_);
        result_visible_ = true;
    }
    
    UpdateSnakeForwardDirection();
}

void GameApp::OnSpaceKey()
{
    if (game_.GetGameState() == GameCore::GameState::Ready)
    {
        game_.StartGame();
        UpdateSnakeForwardDirection();
        GetRenderer().DrawGaming(game_);
        result_visible_ = false;
    }
    else if (game_.GetGameState() == GameCore::GameState::UnderWay)
    {
        game_.PauseGame(true);
    }
    else if (game_.GetGameState() == GameCore::GameState::Pause)
    {
        game_.PauseGame(false);
    }
    else if (game_.GetGameState() == GameCore::GameState::Over ||
             game_.GetGameState() == GameCore::GameState::Win)
    {
        game_.ReadyGame();
        game_.StartGame();
        UpdateSnakeForwardDirection();
        GetRenderer().DrawGaming(game_);
        result_visible_ = false;
    }
}

void GameApp::OnEnterKey()
{
    if (game_.GetGameState() == GameCore::GameState::Over ||
        game_.GetGameState() == GameCore::GameState::Win)
    {
        game_.ReadyGame();
        GetRenderer().DrawGuide(game_);
    }
}

GameRenderer& GameApp::GetRenderer() const
{
    return *renderers_[renderer_index_];
}

void GameApp::OnSwitchPaintingStyle()
{
    renderer_index_ = (renderer_index_ + 1) % renderers_.size();

    if (game_.GetGameState() == GameCore::GameState::Ready)
    {
        GetRenderer().SetupConsole(game_);
        GetRenderer().DrawGuide(game_);
    }
    else if (game_.GetGameState() == GameCore::GameState::UnderWay ||
             game_.GetGameState() == GameCore::GameState::Pause)
    {
        GetRenderer().SetupConsole(game_);
        GetRenderer().DrawGaming(game_);
    }
    else if (game_.GetGameState() == GameCore::GameState::Over ||
             game_.GetGameState() == GameCore::GameState::Win)
    {
        GetRenderer().SetupConsole(game_);
        DrawFinishedGame();
    }
}

void GameApp::OnToggleResultVisibility()
{
    if (game_.GetGameState() == GameCore::GameState::Over ||
        game_.GetGameState() == GameCore::GameState::Win)
    {
        result_visible_ ^= true;
        DrawFinishedGame();
    }
}

void GameApp::DrawFinishedGame()
{
    GetRenderer().DrawGaming(game_);
    
    if (result_visible_)
    {
        GetRenderer().DrawResult(game_);
    }
}

void GameApp::OnExpandGameMap()
{
    if (game_.GetGameState() == GameCore::GameState::Ready)
    {
        if (game_.ExpandGameMap())
        {
            GetRenderer().SetupConsole(game_);
            GetRenderer().DrawGuide(game_);
        }
    }
}

void GameApp::OnShrinkGameMap()
{
    if (game_.GetGameState() == GameCore::GameState::Ready)
    {
        if (game_.ShrinkGameMap())
        {
            GetRenderer().SetupConsole(game_);
            GetRenderer().DrawGuide(game_);
        }
    }
}

void GameApp::OnDirectionKeyEvent(ashes::Direction4 dir, bool pressed)
{
    int index = ashes::direction4::ToInt(dir);
    bool pressed_already = dir_key_states_.test(index);
    dir_key_states_.set(index, pressed);

    if (game_.GetGameState() == GameCore::GameState::UnderWay ||
        game_.GetGameState() == GameCore::GameState::Pause)
    {
        if (pressed && !pressed_already && !ai_enabled_)
        {
            game_.SetSnakeForwardDirection(dir);
        }
    }
}

void GameApp::OnToggleAIController()
{
    ai_enabled_ ^= true;

    if (game_.GetGameState() == GameCore::GameState::UnderWay ||
        game_.GetGameState() == GameCore::GameState::Pause)
    {
        UpdateSnakeForwardDirection();
    }
}

void GameApp::UpdateSnakeForwardDirection()
{
    if (ai_enabled_)
    {
        ashes::Direction4 dir = ai_.MakeStrategy(game_);
        game_.SetSnakeForwardDirection(dir);
    }
    else
    {
        for (ashes::Direction4 dir : ashes::direction4::kFullSet)
        {
            if (dir_key_states_.test(ashes::direction4::ToInt(dir)) &&
                dir != game_.GetSnake().Head().dir)
            {
                if (game_.SetSnakeForwardDirection(dir))
                    break;
            }
        }
    }
}
