#include "graphics/scenes/MonsterPhaseScene.hpp"
#include "graphics/scenes/SceneManager.hpp"
#include "audio/AudioManager.hpp"
#include "graphics/TextureManager.hpp"
#include "graphics/scenes/SceneKeys.hpp"
#include "core/Game.hpp"

void MonsterPhaseScene::onEnter()
{
    background = TextureManager::getInstance().getOrLoadTexture("monster_phase", "assets/images/background/monster_phase.png");
    font = LoadFontEx("assets/fonts/simple.otf", 100, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    AudioManager::getInstance().playMusic();
}

void MonsterPhaseScene::onExit()
{
    stepTimer = 0.0f;
    UnloadFont(font);
    ui.clear();
}

void MonsterPhaseScene::render()
{
    ClearBackground(BLACK);

    float scaleX = 1600.0f / background.width;
    DrawTextureEx(background, {0.0}, 0.0f, scaleX, WHITE);

    if (cardDrawn)
    {
        float cardW = 300.0f * cardScale;
        float cardH = 400.0f * cardScale;
        Vector2 pos = {1600 - cardW - 50, 50}; // گوشه راست بالا

        Rectangle src = {0, 0, (float)cardTexture.width, (float)cardTexture.height};
        Rectangle dst = {pos.x, pos.y, cardW, cardH};
        Vector2 origin = {0, 0};

        DrawTexturePro(cardTexture, src, dst, origin, 0.0f, WHITE);
    }

    ui.render();
}

void MonsterPhaseScene::update(float deleteTime)
{
    switch (currentStep)
    {
    case MonsterPhaseStep::CheckMonsterPhasePerk:
        step_CheckMonsterPhasePerk(deleteTime);
        break;
    case MonsterPhaseStep::DrawMonsterCard:
        step_DrawMonsterCard(deleteTime);
        break;
    }

    ui.update();
}

void MonsterPhaseScene::step_CheckMonsterPhasePerk(float deltaTime)
{
    if (stepTimer == 0.0f)
    {
        skipPhase = !game.canStartMonsterPhase();

        if (skipPhase)
        {
            Vector2 center = {600, 100};
            Color color = {170, 20, 20, 255};

            auto label = std::make_unique<UILabel>(
                center,
                "Monster Phase skipped due to Perk Card!",
                44, 0.0f, color, color);

            label->enableBackground(BLACK, 20.0f);
            label->setFont(font);
            ui.add(std::move(label));

            game.setGameState(GameState::EndMonsterPhase);
        }
    }

    stepTimer += deltaTime;

    if (skipPhase && stepTimer >= 3.0f)
    {
        SceneManager::getInstance().goTo(SceneKeys::BOARD_SCENE);
    }

    if (!skipPhase)
    {
        currentStep = MonsterPhaseStep::DrawMonsterCard;
        stepTimer = 0.0f;
    }
}

void MonsterPhaseScene::step_DrawMonsterCard(float deltaTime)
{
    if (!cardDrawn)
    {
        game.drawMonsterCard();
        card = game.currentMosnterCard;

        std::string name = card.get_name();
        std::string path = "assets/images/Monster_Cards/" + name + ".png";

        cardTexture = TextureManager::getInstance().getOrLoadTexture(name, path);

        cardScale = 0.0f;
        cardDrawn = true;
    }

    if (cardScale < 1.0f)
    {
        cardScale += deltaTime * 1.0f;
        if (cardScale >= 1.0f)
        {
            cardScale = 1.0f;
            // AudioManager::getInstance().playSFX("card_hit"); // صدای کوبیدن کارت
            currentStep = MonsterPhaseStep::PlaceItems;
        }
    }
}