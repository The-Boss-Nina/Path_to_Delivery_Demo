#include "StageState.h"
#include "Camera.h"
#include "Delivery.h"
#include "Gps.h"
#include "Speedometer.h"
#include "HpMeter.h"
#include "DeliveryPlayer.h"
#include "Game.h"
#include "GameObject.h"
#include "GameOverState.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "Text.h"
#include "TileMap.h"
#include "TileSet.h"
#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include "Vehicle.h"
#include "VictoryState.h"
#include "Music.h"

int SetDeliveryGoalBasedOnDifficulty() {
    int deliveryGoal = 0;

    if (Game::GetInstance().dificulty == 1) {
        deliveryGoal = 2;
    } else if (Game::GetInstance().dificulty == 2) {
        deliveryGoal = 3;
    } else if (Game::GetInstance().dificulty == 3) {
        deliveryGoal = 5;
    } else {
        deliveryGoal = 5; // Default case
    }

    return deliveryGoal;
}

namespace {
    constexpr float kGameDuration = 300.0f;  // 5 minutos

    std::string FormatTime(float seconds) {
        int total = static_cast<int>(std::ceil(seconds));
        if (total < 0) total = 0;
        int minutes = total / 60;
        int secs = total % 60;
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%d:%02d", minutes, secs);
        return std::string(buf);
    }
}

StageState::StageState(VehicleType selectedVehicle)
    : State(), selectedVehicle(selectedVehicle), tileSet(nullptr), backgroundMusic(), tileMap(nullptr),
      delivery(nullptr), timeText(nullptr), goalText(nullptr), deliveryCountText(nullptr), endBannerText(nullptr),
      endBannerObj(nullptr),
      timeRemaining(kGameDuration), gameEnded(false), lastDisplayedSeconds(-1), lastDisplayedCount(-1),
      deliveryGoal(SetDeliveryGoalBasedOnDifficulty()) {}

StageState::~StageState() {
    tileSet = nullptr;
}

static GameObject* CreateSpriteObject(State& state,
                                      const std::string& file,
                                      float x,
                                      float y,
                                      int frameCountW = 1,
                                      int frameCountH = 1,
                                      int frame = 0,
                                      float scaleX = 1.0f,
                                      float scaleY = 1.0f) {
    GameObject* object = new GameObject();
    SpriteRenderer* sr = new SpriteRenderer(*object, file, frameCountW, frameCountH);
    sr->SetFrame(frame);
    sr->SetScale(scaleX, scaleY);
    object->AddComponent(sr);
    object->box.x = x;
    object->box.y = y;
    state.AddObject(object);
    return object;
}

// Varre a camada de colisão (camada 1) do TileMap e agrupa os quarteirões marcados com o tile 12 (ver cidade2.txt)
static std::vector<Rect> FindBuildingBlocks(TileMap* tileMap) {
    std::vector<Rect> blocks;
    if (!tileMap) return blocks;

    int width = tileMap->GetWidth();
    int height = tileMap->GetHeight();
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (visited[y][x] || tileMap->At(x, y, 1) != 12) continue;

            std::vector<std::pair<int, int>> stack{{x, y}};
            visited[y][x] = true;
            int minX = x, maxX = x, minY = y, maxY = y;

            while (!stack.empty()) {
                auto [cx, cy] = stack.back();
                stack.pop_back();
                minX = std::min(minX, cx);
                maxX = std::max(maxX, cx);
                minY = std::min(minY, cy);
                maxY = std::max(maxY, cy);

                for (int d = 0; d < 4; ++d) {
                    int nx = cx + dx[d];
                    int ny = cy + dy[d];
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                        !visited[ny][nx] && tileMap->At(nx, ny, 1) == 12) {
                        visited[ny][nx] = true;
                        stack.push_back({nx, ny});
                    }
                }
            }

            blocks.emplace_back(
                static_cast<float>(minX * tileMap->GetTileWidth()),
                static_cast<float>(minY * tileMap->GetTileHeight()),
                static_cast<float>((maxX - minX + 1) * tileMap->GetTileWidth()),
                static_cast<float>((maxY - minY + 1) * tileMap->GetTileHeight()));
        }
    }

    return blocks;
}

void StageState::Start() {
    if (started) return;

    LoadAssets();
    StartArray();
    started = true;
}

void StageState::LoadAssets() {
    Camera::pos = Vec2(0.0f, 0.0f);
    Camera::Unfollow();

    // Fundo fixo na tela.
    GameObject* bg = new GameObject();
    SpriteRenderer* bgSr = new SpriteRenderer(*bg, "recursos/img/Background.png");
    bgSr->SetCameraFollower(true);
    bg->AddComponent(bgSr);
    bg->box.x = 0;
    bg->box.y = 0;
    bg->box.w = 1280;
    bg->box.h = 720;
    AddObject(bg);

    // Mapa da primeira fase usando o tileset de ruas do Path to Delivery.
    GameObject* mapObject = new GameObject();
    tileSet = new TileSet(512, 512, "recursos/img/v2RuasSpritesheet.png", 512, 512);
    tileMap = new TileMap(*mapObject, "recursos/map/cidade2.txt", tileSet);
    mapObject->AddComponent(tileMap);
    mapObject->box.x = 0;
    mapObject->box.y = 0;
    AddObject(mapObject);

    const float mapWidthPx = static_cast<float>(tileMap->GetWidth() * tileMap->GetTileWidth());
    const float mapHeightPx = static_cast<float>(tileMap->GetHeight() * tileMap->GetTileHeight());
    Camera::SetBounds(mapWidthPx, mapHeightPx);

    // O cenário e o spawn do jogador abaixo foram posicionados quando o
    // tileset renderizava a 128px/tile. Escalamos essas coordenadas pelo
    // tamanho real do tile configurado acima, para continuarem alinhadas
    // com as ruas independente do tamanho do tile usado.
    const float kLegacyTileSize = 128.0f;
    const float worldScale = static_cast<float>(tileMap->GetTileWidth()) / kLegacyTileSize;


    const char* buildingSprites[] = {
        "recursos/img/predio2andaresV1.png",
        "recursos/img/predio4andaresV1.png"
    };
    const float buildingSourceWidth = 512.0f;
    const float buildingSourceHeight = 256.0f;
    std::vector<Rect> buildingBlocks = FindBuildingBlocks(tileMap);
    for (size_t i = 0; i < buildingBlocks.size(); ++i) {
        const Rect& block = buildingBlocks[i];
        CreateSpriteObject(*this, buildingSprites[i % 2], block.x, block.y,
                            1, 1, 0,
                            block.w / buildingSourceWidth,
                            block.h / buildingSourceHeight);
    }

    // Elementos de cenário.
    CreateSpriteObject(*this, "recursos/img/farol.png", 760 * worldScale, 470 * worldScale);
    CreateSpriteObject(*this, "recursos/img/farol.png", 1780 * worldScale, 1230 * worldScale);
    CreateSpriteObject(*this, "recursos/img/bloqueio.png", 2450 * worldScale, 720 * worldScale, 9, 1, 0);
    CreateSpriteObject(*this, "recursos/img/bloqueio.png", 650 * worldScale, 1750 * worldScale, 9, 1, 4);

    // Veículos simples, ainda estáticos nesta entrega de 30%.
    CreateSpriteObject(*this, "recursos/img/V3vezpa.png", 512 * worldScale, 128 * worldScale, 4, 3, 4);
    CreateSpriteObject(*this, "recursos/img/V3uno.png", 512 * worldScale, 128 * worldScale, 4, 3, 4);
    CreateSpriteObject(*this, "recursos/img/V3_harley_sheet.png", 1360 * worldScale, 490 * worldScale, 10, 1, 9);
    CreateSpriteObject(*this, "recursos/img/V3_mustang0008-sheet.png", 1280 * worldScale, 128 * worldScale, 10, 1, 9);

    // Jogador principal: o sprite depende da escolha feita na tela anterior.
    std::string playerSpritePath = "recursos/img/harley_final_entrega.png";
    int playerFrameCountW = 4;
    int playerFrameCountH = 4;

    if (selectedVehicle == VehicleType::VESPA) {
        playerSpritePath = "recursos/img/V3vezpa.png";
        playerFrameCountW = 4;
        playerFrameCountH = 3;
    } else if (selectedVehicle == VehicleType::UNO) {
        playerSpritePath = "recursos/img/uno_luz.png";
        playerFrameCountW = 4;
        playerFrameCountH = 4;
    } else if (selectedVehicle == VehicleType::HARLEY) {
        playerSpritePath = "recursos/img/harley_luz.png";
        playerFrameCountW = 4;
        playerFrameCountH = 4;
    } else if (selectedVehicle == VehicleType::MUSTANG) {
        playerSpritePath = "recursos/img/V3_mustang0008-sheet.png";
        playerFrameCountW = 10;
        playerFrameCountH = 1;
    }

    GameObject* player = new GameObject();
    player->AddComponent(new Vehicle(*player, playerSpritePath, true, playerFrameCountW, playerFrameCountH));
    player->AddComponent(new DeliveryPlayer(*player, selectedVehicle));
    player->box.x = 780.0f * worldScale;
    player->box.y = 1180.0f * worldScale;
    AddObject(player);

    Camera::Follow(player);

    // Sistema de entregas
    GameObject* deliveryManager = new GameObject();
    delivery = new Delivery(*deliveryManager, *this, tileMap, player);
    deliveryManager->AddComponent(delivery);
    AddObject(deliveryManager);

    // Widget GPS — renderizado fixo no canto inferior esquerdo
    GameObject* gpsObj = new GameObject();
    SpriteRenderer* gpsSr = new SpriteRenderer(*gpsObj, "recursos/img/computador_bordo.png", 3, 3);
    gpsSr->SetCameraFollower(true);
    gpsSr->SetFrame(0);
    gpsObj->AddComponent(gpsSr);
    gpsObj->box.x = 10.0f;
    gpsObj->box.y = 720.0f - gpsObj->box.h - 10.0f;
    gpsObj->AddComponent(new Gps(*gpsObj, delivery));
    AddObject(gpsObj);

    // Widget do velocímetro — renderizado fixo no canto inferior direito.
    GameObject* speedoObj = new GameObject();
    SpriteRenderer* speedoSr = new SpriteRenderer(*speedoObj, "recursos/img/velocimetro.png", 5, 5);
    speedoSr->SetCameraFollower(true);
    speedoSr->SetFrame(0);
    speedoObj->AddComponent(speedoSr);
    speedoObj->box.x = 1280.0f - speedoObj->box.w - 10.0f;
    speedoObj->box.y = 720.0f - speedoObj->box.h - 10.0f;
    speedoObj->AddComponent(new Speedometer(*speedoObj, Vehicle::player));
    AddObject(speedoObj);

    // Widget de vida — renderizado fixo à esquerda do velocímetro.
    GameObject* hpObj = new GameObject();
    SpriteRenderer* hpSr = new SpriteRenderer(*hpObj, "recursos/img/marcador_vida_sheet.png", 5, 5);
    hpSr->SetCameraFollower(true);
    hpSr->SetFrame(0);
    hpObj->AddComponent(hpSr);
    hpObj->box.x = speedoObj->box.x - hpObj->box.w - 10.0f;
    hpObj->box.y = 720.0f - hpObj->box.h - 10.0f;
    hpObj->AddComponent(new HpMeter(*hpObj, Vehicle::player));
    AddObject(hpObj);

    // HUD de objetivo — canto superior esquerdo, uma linha embaixo da outra.
    SDL_Color hudWhite = { 255, 255, 255, 255 };
    const float kHudX = 10.0f;
    const float kHudLineHeight = 32.0f;

    GameObject* timeObj = new GameObject();
    timeText = new Text(*timeObj, "recursos/font/neodgm.ttf", 28, Text::BLENDED,
                        "Tempo Restante: " + FormatTime(timeRemaining), hudWhite);
    timeObj->AddComponent(timeText);
    timeObj->box.x = kHudX;
    timeObj->box.y = 10.0f;
    AddObject(timeObj);

    GameObject* goalObj = new GameObject();
    goalText = new Text(*goalObj, "recursos/font/neodgm.ttf", 28, Text::BLENDED,
                        "Objetivo: " + std::to_string(deliveryGoal), hudWhite);
    goalObj->AddComponent(goalText);
    goalObj->box.x = kHudX;
    goalObj->box.y = 10.0f + kHudLineHeight;
    AddObject(goalObj);

    GameObject* deliveryCountObj = new GameObject();
    deliveryCountText = new Text(*deliveryCountObj, "recursos/font/neodgm.ttf", 28, Text::BLENDED,
                                 "Numero Entregas: 0", hudWhite);
    deliveryCountObj->AddComponent(deliveryCountText);
    deliveryCountObj->box.x = kHudX;
    deliveryCountObj->box.y = 10.0f + 2 * kHudLineHeight;
    AddObject(deliveryCountObj);

    // Faixa de vitória/derrota — vazia (invisível) até o jogo terminar.
    endBannerObj = new GameObject();
    endBannerText = new Text(*endBannerObj, "recursos/font/neodgm.ttf", 48, Text::BLENDED, "", hudWhite);
    endBannerObj->AddComponent(endBannerText);
    endBannerObj->box.x = 0.0f;
    endBannerObj->box.y = 300.0f;
    AddObject(endBannerObj);

    backgroundMusic.Open("recursos/sound/FASE.mp3");
    backgroundMusic.Play(-1);
}

void StageState::Update(float dt) {
    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY)) {
        quitRequested = true;
        return;
    }

    if (gameEnded) return;

    timeRemaining -= dt;
    if (timeRemaining < 0.0f) timeRemaining = 0.0f;

    int deliveryCount = delivery ? delivery->deliveryCount : 0;

    if (deliveryCount >= deliveryGoal) {
        gameEnded = true;
        if (endBannerText && endBannerObj) {
            SDL_Color green = { 60, 220, 90, 255 };
            endBannerText->SetColor(green);
            endBannerText->SetText("Voce venceu! Todas as entregas concluidas.");
            endBannerObj->box.x = 640.0f - endBannerObj->box.w / 2.0f;
        }
        Game::GetInstance().Push(new VictoryState());
    } else if (timeRemaining <= 0.0f) {
        gameEnded = true;
        if (endBannerText && endBannerObj) {
            SDL_Color red = { 220, 60, 60, 255 };
            endBannerText->SetColor(red);
            endBannerText->SetText("Tempo esgotado! Voce perdeu.");
            endBannerObj->box.x = 640.0f - endBannerObj->box.w / 2.0f;
        }
        Game::GetInstance().Push(new GameOverState());
    }

    int seconds = static_cast<int>(std::ceil(timeRemaining));
    if (seconds != lastDisplayedSeconds) {
        lastDisplayedSeconds = seconds;
        if (timeText) timeText->SetText("Tempo Restante: " + FormatTime(timeRemaining));
    }

    if (deliveryCount != lastDisplayedCount) {
        lastDisplayedCount = deliveryCount;
        if (deliveryCountText) deliveryCountText->SetText("Numero Entregas: " + std::to_string(deliveryCount));
    }

    Camera::Update(dt);
    UpdateArray(dt);
}

void StageState::Render() {
    RenderArray();
}

void StageState::Pause() {
    // Encerra tudo que toca em loop (música e som do motor/freio do
    // jogador) antes de ir para VictoryState/GameOverState — sem isso o
    // Game::Run() apenas pausa a StageState (ela continua viva, empilhada
    // por baixo), e os canais do Mix continuam tocando indefinidamente.
    backgroundMusic.Stop();

    if (Vehicle* vehicle = Vehicle::player) {
        vehicle->engineSound.Stop();
        vehicle->brakeSound.Stop();
        vehicle->crashSound.Stop();
    }
}

void StageState::Resume() {}

bool StageState::IsCollidingWithTileMap(const Rect& rect) {
    if (tileMap == nullptr) {
        return false;
    }

    int tileWidth = tileMap->GetTileWidth();
    int tileHeight = tileMap->GetTileHeight();
    int left = (int)std::floor((rect.x - tileMap->GetX()) / (float)tileWidth);
    int right = (int)std::floor((rect.x + rect.w - 1 - tileMap->GetX()) / (float)tileWidth);
    int top = (int)std::floor((rect.y - tileMap->GetY()) / (float)tileHeight);
    int bottom = (int)std::floor((rect.y + rect.h - 1 - tileMap->GetY()) / (float)tileHeight);

    for (int y = top; y <= bottom; y++) {
        for (int x = left; x <= right; x++) {
            if (tileMap->IsTileBlocked(x, y)) {
                return true;
            }
        }
    }

    return false;
}