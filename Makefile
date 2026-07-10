CXX      = g++
CXXFLAGS = -Wall -I./include $(shell pkg-config --cflags sdl2 SDL2_image SDL2_mixer SDL2_ttf)
LIBS     = $(shell pkg-config --libs sdl2 SDL2_image SDL2_mixer SDL2_ttf)

TARGET   = Game
SRC_DIR  = src

SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/Game.cpp \
       $(SRC_DIR)/State.cpp \
       $(SRC_DIR)/StageState.cpp \
       $(SRC_DIR)/VehicleSelectState.cpp \
       $(SRC_DIR)/Music.cpp \
       $(SRC_DIR)/Sound.cpp \
       $(SRC_DIR)/Sprite.cpp \
       $(SRC_DIR)/Vec2.cpp \
       $(SRC_DIR)/Rect.cpp \
       $(SRC_DIR)/Component.cpp \
       $(SRC_DIR)/GameObject.cpp \
       $(SRC_DIR)/SpriteRenderer.cpp \
       $(SRC_DIR)/Animation.cpp \
       $(SRC_DIR)/Animator.cpp \
       $(SRC_DIR)/TileSet.cpp \
       $(SRC_DIR)/TileMap.cpp \
       $(SRC_DIR)/Resources.cpp \
       $(SRC_DIR)/Camera.cpp \
       $(SRC_DIR)/Timer.cpp \
       $(SRC_DIR)/InputManager.cpp \
       $(SRC_DIR)/DeliveryPlayer.cpp \
       $(SRC_DIR)/Vehicle.cpp \
       $(SRC_DIR)/Collider.cpp \
       $(SRC_DIR)/Collision.cpp \
       $(SRC_DIR)/Delivery.cpp \
       $(SRC_DIR)/Gps.cpp \
       $(SRC_DIR)/Speedometer.cpp \
       $(SRC_DIR)/HpMeter.cpp \
       $(SRC_DIR)/TitleState.cpp \
       $(SRC_DIR)/Text.cpp \
       $(SRC_DIR)/InstructionsState.cpp \
       $(SRC_DIR)/ObjectivesState.cpp \
       ${SRC_DIR}/GameOverState.cpp \
       ${SRC_DIR}/VictoryState.cpp      

# CITY_TARGET = GenerateCity

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(SRCS) $(CXXFLAGS) $(LIBS) -o $(TARGET)

run: all
	./$(TARGET)

# # Ferramenta standalone (sem SDL2) que gera recursos/map/cidade_criada.txt.
# # Uso: make generate-city [ARGS="saida.txt largura altura semente"]
# $(CITY_TARGET): $(SRC_DIR)/GenerateCity.cpp include/GenerateCity.h
# 	$(CXX) -Wall -I./include $(SRC_DIR)/GenerateCity.cpp -o $(CITY_TARGET)

# generate-city: $(CITY_TARGET)
# 	./$(CITY_TARGET) $(ARGS)

clean:
	rm -f $(TARGET) Game.exe $(CITY_TARGET)

# .PHONY: all clean run generate-city
