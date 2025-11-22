#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ROAD_WIDTH 400
#define CAR_SIZE 40
#define LANE_COUNT 3
#define PEDESTRIAN_SIZE 20

typedef struct {
    int x, y;
    int lane;
    int speed;
    Color color;
} Car;

typedef struct {
    int x, y;
    int lane;
    bool active;
    Color color;
} Obstacle;

typedef struct {
    int x, y;
    int lane;
    bool active;
    bool crossing;  
    int direction;
    Color color;
} Pedestrian;

typedef struct {
    int score;
    bool gameOver;
} GameState;

void DrawRoad();
void DrawCar(Car car);
void DrawObstacle(Obstacle obstacle);
void DrawPedestrian(Pedestrian pedestrian);
void MoveObstacles(Obstacle obstacles[], int count, int speed);
void MovePedestrians(Pedestrian pedestrians[], int count);
void CreateObstacle(Obstacle obstacles[], int count);
void CreatePedestrian(Pedestrian pedestrians[], int count);
int CheckCollision(Car car, Obstacle obstacle);
int CheckPedestrianCollision(Car car, Pedestrian pedestrian);
int GetLaneX(int lane);

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Car Racing with Pedestrians");
    SetTargetFPS(60);
    
    srand(time(NULL));
    
    Car player = {0, 500, 1, 8, RED};
    player.x = GetLaneX(player.lane);
    
    Obstacle obstacles[5];
    for (int i = 0; i < 5; i++) {
        obstacles[i].active = false;
        obstacles[i].lane = rand() % LANE_COUNT;
        obstacles[i].color = BLUE;
    }
    
    Pedestrian pedestrians[3];
    for (int i = 0; i < 3; i++) {
        pedestrians[i].active = false;
        pedestrians[i].lane = rand() % LANE_COUNT;
        pedestrians[i].color = GREEN;
        pedestrians[i].crossing = false;
    }
    
    GameState game = {0, false};
    
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_LEFT) && player.lane > 0) {
            player.lane--;
            player.x = GetLaneX(player.lane);
        }
        if (IsKeyPressed(KEY_RIGHT) && player.lane < LANE_COUNT - 1) {
            player.lane++;
            player.x = GetLaneX(player.lane);
        }
        
        if (IsKeyDown(KEY_UP)) player.speed = 12;
        else if (IsKeyDown(KEY_DOWN)) player.speed = 5;
        else player.speed = 8;
        
        if (!game.gameOver) {
            MoveObstacles(obstacles, 5, player.speed);
            
            MovePedestrians(pedestrians, 3);
            
            if (rand() % 100 < 5) {
                CreateObstacle(obstacles, 5);
            }
            
            if (rand() % 100 < 3) {
                CreatePedestrian(pedestrians, 3);
            }
            
            for (int i = 0; i < 5; i++) {
                if (obstacles[i].active && CheckCollision(player, obstacles[i])) {
                    game.gameOver = true;
                }
            }
            
            for (int i = 0; i < 3; i++) {
                if (pedestrians[i].active && CheckPedestrianCollision(player, pedestrians[i])) {
                    game.gameOver = true;
                }
            }
            
            game.score++;
        }
        
        if (game.gameOver && IsKeyPressed(KEY_R)) {
            game.score = 0;
            game.gameOver = false;
            player.lane = 1;
            player.x = GetLaneX(player.lane);
            for (int i = 0; i < 5; i++) obstacles[i].active = false;
            for (int i = 0; i < 3; i++) pedestrians[i].active = false;
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawRoad();
        
        for (int i = 0; i < 5; i++) {
            if (obstacles[i].active) {
                DrawObstacle(obstacles[i]);
            }
        }
        
        for (int i = 0; i < 3; i++) {
            if (pedestrians[i].active) {
                DrawPedestrian(pedestrians[i]);
            }
        }
        
        DrawCar(player);
        
        DrawText(TextFormat("Score: %d", game.score), 10, 10, 20, WHITE);
        DrawText("Arrow Keys: Move (Up/Down for speed)", 10, 570, 15, WHITE);
        DrawText("Avoid pedestrians! They end your game!", 10, 40, 15, RED);
        
        if (game.gameOver) {
            DrawText("GAME OVER - Press R to restart", 200, 300, 30, RED);
        }
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

void DrawRoad() {
    int roadX = (SCREEN_WIDTH - ROAD_WIDTH) / 2;
    
    DrawRectangle(roadX, 0, ROAD_WIDTH, SCREEN_HEIGHT, GRAY);
    
    for (int i = 1; i < LANE_COUNT; i++) {
        int laneX = roadX + i * (ROAD_WIDTH / LANE_COUNT);
        for (int y = 0; y < SCREEN_HEIGHT; y += 40) {
            DrawRectangle(laneX - 2, y, 4, 20, WHITE);
        }
    }
    
    DrawRectangle(roadX, 0, 5, SCREEN_HEIGHT, WHITE);
    DrawRectangle(roadX + ROAD_WIDTH - 5, 0, 5, SCREEN_HEIGHT, WHITE);
    
    DrawRectangle(roadX - 30, 0, 30, SCREEN_HEIGHT, DARKBROWN);
    DrawRectangle(roadX + ROAD_WIDTH, 0, 30, SCREEN_HEIGHT, DARKBROWN);
}

void DrawCar(Car car) {
    DrawRectangle(car.x, car.y, CAR_SIZE, CAR_SIZE, car.color);
    
    DrawRectangle(car.x + 5, car.y + 5, CAR_SIZE - 10, 10, SKYBLUE);
    DrawRectangle(car.x + 5, car.y + CAR_SIZE - 15, CAR_SIZE - 10, 10, SKYBLUE);
    
    DrawRectangle(car.x - 3, car.y + 10, 6, CAR_SIZE - 20, BLACK);
    DrawRectangle(car.x + CAR_SIZE - 3, car.y + 10, 6, CAR_SIZE - 20, BLACK);
}

void DrawObstacle(Obstacle obstacle) {
    DrawRectangle(obstacle.x, obstacle.y, CAR_SIZE, CAR_SIZE, obstacle.color);
    
    DrawRectangle(obstacle.x + 5, obstacle.y + 5, CAR_SIZE - 10, 10, YELLOW);
    DrawRectangle(obstacle.x + 5, obstacle.y + CAR_SIZE - 15, CAR_SIZE - 10, 10, YELLOW);
}

void DrawPedestrian(Pedestrian pedestrian) {
    DrawRectangle(pedestrian.x, pedestrian.y, PEDESTRIAN_SIZE, PEDESTRIAN_SIZE, pedestrian.color);
    
    DrawCircle(pedestrian.x + PEDESTRIAN_SIZE/2, pedestrian.y - 5, 7, LIGHTGRAY);
    
    if (pedestrian.crossing) {
        DrawRectangleLines(pedestrian.x - 2, pedestrian.y - 2, 
                          PEDESTRIAN_SIZE + 4, PEDESTRIAN_SIZE + 4, RED);
    }
}

void MoveObstacles(Obstacle obstacles[], int count, int speed) {
    for (int i = 0; i < count; i++) {
        if (obstacles[i].active) {
            obstacles[i].y += speed;
            
            if (obstacles[i].y > SCREEN_HEIGHT) {
                obstacles[i].active = false;
                obstacles[i].color = BLUE;
            }
        }
    }
}

void MovePedestrians(Pedestrian pedestrians[], int count) {
    for (int i = 0; i < count; i++) {
        if (pedestrians[i].active) {
            if (pedestrians[i].crossing) {
                pedestrians[i].x += pedestrians[i].direction * 2;
                
                if (pedestrians[i].x < 0 || pedestrians[i].x > SCREEN_WIDTH) {
                    pedestrians[i].active = false;
                }
            }
        }
    }
}

void CreateObstacle(Obstacle obstacles[], int count) {
    for (int i = 0; i < count; i++) {
        if (!obstacles[i].active) {
            obstacles[i].active = true;
            obstacles[i].lane = rand() % LANE_COUNT;
            obstacles[i].x = GetLaneX(obstacles[i].lane);
            obstacles[i].y = -CAR_SIZE;
            obstacles[i].color = BLUE;
            break;
        }
    }
}

void CreatePedestrian(Pedestrian pedestrians[], int count) {
    for (int i = 0; i < count; i++) {
        if (!pedestrians[i].active) {
            pedestrians[i].active = true;
            pedestrians[i].lane = rand() % LANE_COUNT;
            pedestrians[i].x = GetLaneX(pedestrians[i].lane);
            
            int roadX = (SCREEN_WIDTH - ROAD_WIDTH) / 2;
            if (rand() % 2 == 0) {
                pedestrians[i].x = roadX - 25;
                pedestrians[i].direction = 1;
            } else {
                pedestrians[i].x = roadX + ROAD_WIDTH + 5;
                pedestrians[i].direction = -1;
            }
            
            pedestrians[i].y = SCREEN_HEIGHT - 100 - (rand() % 200);
            pedestrians[i].color = GREEN;
            pedestrians[i].crossing = false;
            
            if (rand() % 100 < 30) {
                pedestrians[i].crossing = true;
                pedestrians[i].color = RED;
            }
            break;
        }
    }
}

int CheckCollision(Car car, Obstacle obstacle) {
    return (car.x < obstacle.x + CAR_SIZE &&
            car.x + CAR_SIZE > obstacle.x &&
            car.y < obstacle.y + CAR_SIZE &&
            car.y + CAR_SIZE > obstacle.y);
}

int CheckPedestrianCollision(Car car, Pedestrian pedestrian) {
    return (car.x < pedestrian.x + PEDESTRIAN_SIZE &&
            car.x + CAR_SIZE > pedestrian.x &&
            car.y < pedestrian.y + PEDESTRIAN_SIZE &&
            car.y + CAR_SIZE > pedestrian.y);
}

int GetLaneX(int lane) {
    int roadX = (SCREEN_WIDTH - ROAD_WIDTH) / 2;
    int laneWidth = ROAD_WIDTH / LANE_COUNT;
    return roadX + (lane * laneWidth) + (laneWidth - CAR_SIZE) / 2;
}