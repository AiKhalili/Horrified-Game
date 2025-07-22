#include "graphics/scenes/HeroSelectionScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include <iostream>
#include "core/Game.hpp"

HeroSelectionScene::HeroSelectionScene()
{
    background = LoadTexture("assets/images/backhero.png");
    font = LoadFont("assets/fonts/simple.ttf");

    float size = 350.f;
    float spacing = 370.f;
    float totalWidth = HERO_COUNT * size + (HERO_COUNT - 1) * (spacing - size);
    float startX = (1600.0f - totalWidth) / 2.0f;
    float y = 250.f;

    for (int i = 0; i < HERO_COUNT; ++i)
        heroButtonRects[i] = {startX + i * spacing, y, size, size};

    heroTextures[0] = LoadTexture("assets/images/Heros/Archaeologist.png");
    heroTextures[1] = LoadTexture("assets/images/Heros/Courier.png");
    heroTextures[2] = LoadTexture("assets/images/Heros/Mayor.png");
    heroTextures[3] = LoadTexture("assets/images/Heros/Scientist.png");

    continueButton = std::make_unique<UIButton>(Rectangle{725, 700, 150, 50}, "SUBMIT", 30, WHITE, DARKGRAY, GRAY, WHITE);
    continueButton->setFont(font);
    continueButton->setFocus(true);
    continueButton->setOnClick([this]()
                               {
    if (phase == SelectionPhase::Player1_Submit)
    {
        phase = SelectionPhase::Player2_Select;
        currentPlayer = (currentPlayer == 1 ? 2 : 1);
        Game& game = Game::getInstance();
        currentMessage = (currentPlayer == 1 ? game.getName1() : game.getName2()) + ", choose your hero";
    }
    else if (phase == SelectionPhase::Player2_Submit)
    {
        Game &game = Game::getInstance();
        try
        {
            game.assignHeroes(
                game.getName1(),
                heroNameFromIndex(playerSelections[0]),
                heroNameFromIndex(playerSelections[1])
            );
        }
        catch (const GameException &ex)
        {
            std::cerr << "Error assigning heroes: " << ex.what() << std::endl;
            return;
        }

        currentMessage = "Both players have selected their heroes.";
        phase = SelectionPhase::Done;

        // مقداردهی تایمر برای نمایش پیام قبل از تغییر صحنه
        submitTimer = 2.0f;  // 2 ثانیه پیام نمایش داده شود
    } });

    backButton = std::make_unique<UIButton>(Rectangle{40, 800, 200, 50}, "Main Menu", 30, WHITE, DARKGRAY, GRAY, WHITE);
    backButton->setFont(font);
    backButton->setFocus(true);
    backButton->setOnClick([this]()
                           { SceneManager::getInstance().goTo(SceneKeys::MAIN_MENU_SCENE); });
}

HeroSelectionScene::~HeroSelectionScene()
{
    UnloadTexture(background);
    UnloadFont(font);
    for (int i = 0; i < HERO_COUNT; i++)
        UnloadTexture(heroTextures[i]);
}

void HeroSelectionScene::onEnter()
{
    Game &game = Game::getInstance();
    playerSelections[0] = -1;
    playerSelections[1] = -1;

    if (game.getTime1() <= game.getTime2())
    {
        currentPlayer = 1;
        phase = SelectionPhase::Player1_Select;
        currentMessage = game.getName1() + ", choose your hero";
    }
    else
    {
        currentPlayer = 2;
        phase = SelectionPhase::Player1_Select;
        currentMessage = game.getName2() + ", choose your hero";
    }
}

void HeroSelectionScene::onExit()
{
    // در صورت نیاز پاک‌سازی خاص انجام بده
}

void HeroSelectionScene::update(float deltaTime)
{
    Vector2 mousePos = GetMousePosition();

    if (phase != SelectionPhase::Done) // فقط وقتی هنوز انتخاب کامل نشده است
    {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            for (int i = 0; i < HERO_COUNT; ++i)
            {
                if (CheckCollisionPointRec(mousePos, heroButtonRects[i]) && isButtonEnabled(i))
                {
                    onHeroSelected(i);
                    break;
                }
            }
        }

        if (continueButton)
            continueButton->update();
        if (backButton)
            backButton->update();
    }
    else
    {
        // وقتی phase == Done است، تایمر رو کاهش بده
        if (submitTimer > 0.0f)
        {
            submitTimer -= deltaTime;
            if (submitTimer <= 0.0f)
            {
                // وقتی تایمر تمام شد برو به صفحه بورد گیم
                SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE); // نام صحنه بوردگیم رو جایگزین کن
            }
        }
    }
}

void HeroSelectionScene::render()
{
    BeginDrawing();
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    ClearBackground(BLACK);

    DrawTexturePro(background,
                   Rectangle{0, 0, (float)background.width, (float)background.height},
                   Rectangle{0, 0, 1600, 900},
                   Vector2{0, 0}, 0.0f, WHITE);

    Vector2 mousePos = GetMousePosition();

    // اگر هر دو انتخاب شدند (phase == Done) تصاویر رو نمایش نده
    if (phase != SelectionPhase::Done)
    {
        for (int i = 0; i < HERO_COUNT; ++i)
        {
            // پنهان کردن قهرمان انتخاب شده نفر اول در کل مراحل انتخاب و تأیید نفر دوم
            if ((phase == SelectionPhase::Player2_Select || phase == SelectionPhase::Player2_Submit) && playerSelections[0] != -1 && i == playerSelections[0])
                continue;

            Rectangle sourceRec = {0, 0, (float)heroTextures[i].width, (float)heroTextures[i].height};
            Rectangle destRect = heroButtonRects[i];
            Vector2 origin = {0, 0};

            DrawTexturePro(heroTextures[i], sourceRec, destRect, origin, 0.0f, WHITE);

            // رسم کادر سفید دور قهرمان انتخاب شده ولی هنوز submit نشده
            // یعنی اگر phase تو حالت انتخاب/submit مربوط به بازیکن فعلی هست و این قهرمان انتخاب شده
            bool isSelectedButNotSubmitted = false;

            if (phase == SelectionPhase::Player1_Submit && playerSelections[0] == i)
                isSelectedButNotSubmitted = true;
            else if (phase == SelectionPhase::Player2_Submit && playerSelections[1] == i)
                isSelectedButNotSubmitted = true;

            if (isSelectedButNotSubmitted)
            {
                DrawRectangleLinesEx(destRect, 5, WHITE);
            }
            else if (CheckCollisionPointRec(mousePos, destRect))
            {
                // وقتی موس روشه کادر نازک رسم کن
                DrawRectangleLinesEx(destRect, 3, WHITE);
            }
        }
    }

    UILabel label({(1600.0f / 2) - 250, 50}, currentMessage, 50, 0.0f, WHITE);
    label.setFont(font);
    label.render();

    if (continueButton)
        continueButton->render();
    if (backButton)
        backButton->render();

    EndDrawing();
}

bool HeroSelectionScene::isButtonEnabled(int heroIndex) const
{
    if (phase != SelectionPhase::Player1_Select && phase != SelectionPhase::Player2_Select)
        return false;

    if (currentPlayer == 2 && heroIndex == playerSelections[0])
        return false;

    return true;
}

void HeroSelectionScene::onHeroSelected(int heroIndex)
{
    playerSelections[currentPlayer - 1] = heroIndex;

    if (phase == SelectionPhase::Player1_Select)
    {
        phase = SelectionPhase::Player1_Submit;
        currentMessage = "Click SUBMIT to continue to next player";
    }
    else if (phase == SelectionPhase::Player2_Select)
    {
        phase = SelectionPhase::Player2_Submit;
        currentMessage = "Click SUBMIT to start the game";
    }
}

std::string HeroSelectionScene::heroNameFromIndex(int index) const
{
    switch (index)
    {
    case 0:
        return "Archaeologist";
    case 1:
        return "Courier";
    case 2:
        return "Mayor";
    case 3:
        return "Scientist";
    default:
        return "";
    }
}
