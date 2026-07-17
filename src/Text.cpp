#include "Text.h"
#include "Game.h"
#include "Camera.h"
#include <stdexcept>

Text::Text(GameObject& associated,
           std::string fontFile,
           int fontSize,
           TextStyle style,
           std::string text,
           SDL_Color color)
    : Component(associated),
      font(nullptr),
      texture(nullptr),
      text(text),
      style(style),
      fontFile(fontFile),
      fontSize(fontSize),
      color(color) {
    RemakeTexture();
}

Text::~Text() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void Text::Update(float dt) {
}

void Text::Render() {
    if (texture == nullptr) return;

    SDL_Rect clipRect = { 0, 0, (int)associated.box.w, (int)associated.box.h };
    SDL_Rect dstRect  = {
        (int)(associated.box.x),
        (int)(associated.box.y),
        clipRect.w,
        clipRect.h
    };

    SDL_RenderCopyEx(
        Game::GetInstance().GetRenderer(),
        texture,
        &clipRect,
        &dstRect,
        associated.angleDeg,
        nullptr,
        SDL_FLIP_NONE
    );
}

void Text::SetText(std::string text) {
    this->text = text;
    RemakeTexture();
}

void Text::SetColor(SDL_Color color) {
    this->color = color;
    RemakeTexture();
}

void Text::SetStyle(TextStyle style) {
    this->style = style;
    RemakeTexture();
}

void Text::SetFontFile(std::string fontFile) {
    this->fontFile = fontFile;
    RemakeTexture();
}

void Text::SetFontSize(int fontSize) {
    this->fontSize = fontSize;
    RemakeTexture();
}

void Text::RemakeTexture() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    font = Resources::GetFont(fontFile, fontSize);
    if (font == nullptr) return;

    SDL_Surface* surface = nullptr;

    switch (style) {
        case SOLID:
            surface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
            break;
        case SHADED: {
            SDL_Color black = { 0, 0, 0, 255 };
            surface = TTF_RenderUTF8_Shaded(font, text.c_str(), color, black);
            break;
        }
        case BLENDED:
            surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
            break;
    }

    if (surface == nullptr) return;

    texture = SDL_CreateTextureFromSurface(Game::GetInstance().GetRenderer(), surface);

    // Atualiza o tamanho da box com base na textura gerada
    associated.box.w = surface->w;
    associated.box.h = surface->h;

    SDL_FreeSurface(surface);
}