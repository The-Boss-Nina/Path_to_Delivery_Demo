#include "Game.h"
#include "TitleState.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    Game& game = Game::GetInstance("Path to Delivery - 30%", 1280, 720);
    game.Push(new TitleState());
    game.Run();

    return 0;
}
