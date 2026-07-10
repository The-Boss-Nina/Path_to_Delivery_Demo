#include "StageState.h"
#include "Camera.h"
#include "Delivery.h"
#include "Gps.h"
#include "DeliveryPlayer.h"
#include "Game.h"
#include "GameObject.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "TileMap.h"
#include "TileSet.h"
#include <SDL.h>
#include <string>
#include "Vehicle.h"
#include "Music.h"

StageState::StageState(VehicleType selectedVehicle)
    : State(), selectedVehicle(selectedVehicle), tileSet(nullptr), backgroundMusic(), tileMap(nullptr) {}

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
    tileSet = new TileSet(128, 128, "recursos/img/v2RuasSpritesheet.png", 512, 512);
    tileMap = new TileMap(*mapObject, "recursos/map/cidade2.txt", tileSet);
    mapObject->AddComponent(tileMap);
    mapObject->box.x = 0;
    mapObject->box.y = 0;
    AddObject(mapObject);

    const float mapWidthPx = static_cast<float>(tileMap->GetWidth() * tileMap->GetTileWidth());
    const float mapHeightPx = static_cast<float>(tileMap->GetHeight() * tileMap->GetTileHeight());
    Camera::SetBounds(mapWidthPx, mapHeightPx);

    // Prédios e elementos de cenário.
    CreateSpriteObject(*this, "recursos/img/predio4andaresV1.png", 120, 110);
    CreateSpriteObject(*this, "recursos/img/predio2andaresV1.png", 900, 120);
    CreateSpriteObject(*this, "recursos/img/predio4andaresV1.png", 2100, 110);
    CreateSpriteObject(*this, "recursos/img/predio2andaresV1.png", 3000, 120);

    CreateSpriteObject(*this, "recursos/img/predio2andaresV1.png", 150, 880);
    CreateSpriteObject(*this, "recursos/img/predio4andaresV1.png", 950, 830);
    CreateSpriteObject(*this, "recursos/img/predio2andaresV1.png", 2250, 900);
    CreateSpriteObject(*this, "recursos/img/predio4andaresV1.png", 3100, 830);

    CreateSpriteObject(*this, "recursos/img/farol.png", 760, 470);
    CreateSpriteObject(*this, "recursos/img/farol.png", 1780, 1230);
    CreateSpriteObject(*this, "recursos/img/bloqueio.png", 2450, 720, 9, 1, 0);
    CreateSpriteObject(*this, "recursos/img/bloqueio.png", 650, 1750, 9, 1, 4);

    // Veículos simples, ainda estáticos nesta entrega de 30%.
    CreateSpriteObject(*this, "recursos/img/V3vezpa.png", 512, 128, 10, 3, 4);
    CreateSpriteObject(*this, "recursos/img/V3uno.png", 512, 128, 10, 3, 4);
    CreateSpriteObject(*this, "recursos/img/V3_harley_sheet.png", 1360, 490, 10, 1, 10);
    CreateSpriteObject(*this, "recursos/img/V3_mustang0008-sheet.png", 1280, 128, 10, 1, 10);

    // Jogador principal: o sprite depende da escolha feita na tela anterior.
    std::string playerSpritePath = "recursos/img/HarleyV1.png";
    int playerFrameCountW = 8;
    int playerFrameCountH = 1;

    if (selectedVehicle == VehicleType::VESPA) {
        playerSpritePath = "recursos/img/V3vezpa.png";
        playerFrameCountW = 4;
        playerFrameCountH = 3;
    } else if (selectedVehicle == VehicleType::UNO) {
        playerSpritePath = "recursos/img/V3uno.png";
        playerFrameCountW = 4;
        playerFrameCountH = 3;
    } else if (selectedVehicle == VehicleType::HARLEY) {
        playerSpritePath = "recursos/img/V3_harley_sheet.png";
        playerFrameCountW = 10;
        playerFrameCountH = 1;
    } else if (selectedVehicle == VehicleType::MUSTANG) {
        playerSpritePath = "recursos/img/V3_mustang0008-sheet.png";
        playerFrameCountW = 10;
        playerFrameCountH = 1;
    }

    GameObject* player = new GameObject();
    player->AddComponent(new Vehicle(*player, playerSpritePath, true, playerFrameCountW, playerFrameCountH));
    player->AddComponent(new DeliveryPlayer(*player, selectedVehicle));
    player->box.x = 760.0f;
    player->box.y = 1180.0f;
    AddObject(player);

    Camera::Follow(player);

    // Sistema de entregas
    GameObject* deliveryManager = new GameObject();
    Delivery* delivery = new Delivery(*deliveryManager, *this, tileMap, player);
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

    backgroundMusic.Open("recursos/sound/FASE.mp3");
    backgroundMusic.Play(-1);
}

void StageState::Update(float dt) {
    InputManager& input = InputManager::GetInstance();

    if (input.QuitRequested() || input.KeyPress(ESCAPE_KEY)) {
        quitRequested = true;
        return;
    }

    Camera::Update(dt);
    UpdateArray(dt);
}

void StageState::Render() {
    RenderArray();
}

void StageState::Pause() {}

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