#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ROAD_WIDTH 400
#define CAR_SIZE 40
#define LANE_COUNT 3
#define PEDESTRIAN_SIZE 20
#define GRAVITY_FLIP_DURATION 5.0f  // seconds gravity stays flipped

typedef struct {
    int x, y;
    int lane;
    int speed;
    Color color;
    bool gravityFlipped;  // New: track if gravity is flipped
    float flipTimer;      // New: timer for gravity flip
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
    bool gravityFlippedGlobal;  // New: global gravity state for other objects
} GameState;

void DrawRoad();
void DrawCar(Car car);
void DrawObstacle(Obstacle obstacle);
void DrawPedestrian(Pedestrian pedestrian);
void MoveObstacles(Obstacle obstacles[], int count, int speed, bool gravityFlipped);
void MovePedestrians(Pedestrian pedestrians[], int count, bool gravityFlipped);
void CreateObstacle(Obstacle obstacles[], int count, bool gravityFlipped);
void CreatePedestrian(Pedestrian pedestrians[], int count, bool gravityFlipped);
int CheckCollision(Car car, Obstacle obstacle);
int CheckPedestrianCollision(Car car, Pedestrian pedestrian);
int GetLaneX(int lane);
void ApplyGravityFlip(Car *player, GameState *game);  // New function
void DrawGravityIndicator(Car player);  // New function

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Car Racing with Pedestrians & Gravity Flip!");
    SetTargetFPS(60);
    
    srand(time(NULL));
    
    Car player = {0, 500, 1, 8, RED, false, 0.0f};
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
    
    GameState game = {0, false, false};
    
    while (!WindowShouldClose()) {
        // Handle gravity flip
        if (IsKeyPressed(KEY_G) && !game.gameOver) {
            ApplyGravityFlip(&player, &game);
        }
        
        // Update gravity flip timer
        if (player.gravityFlipped) {
            player.flipTimer -= GetFrameTime();
            if (player.flipTimer <= 0) {
                // Flip back to normal gravity
                ApplyGravityFlip(&player, &game);
            }
        }
        
        // Car movement controls
        if (IsKeyPressed(KEY_LEFT) && player.lane > 0) {
            player.lane--;
            player.x = GetLaneX(player.lane);
        }
        if (IsKeyPressed(KEY_RIGHT) && player.lane < LANE_COUNT - 1) {
            player.lane++;
            player.x = GetLaneX(player.lane);
        }
        
        // Speed controls
        if (IsKeyDown(KEY_UP)) player.speed = 12;
        else if (IsKeyDown(KEY_DOWN)) player.speed = 5;
        else player.speed = 8;
        
        if (!game.gameOver) {
            MoveObstacles(obstacles, 5, player.speed, game.gravityFlippedGlobal);
            
            MovePedestrians(pedestrians, 3, game.gravityFlippedGlobal);
            
            if (rand() % 100 < 5) {
                CreateObstacle(obstacles, 5, game.gravityFlippedGlobal);
            }
            
            if (rand() % 100 < 3) {
                CreatePedestrian(pedestrians, 3, game.gravityFlippedGlobal);
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
        
        // Restart game
        if (game.gameOver && IsKeyPressed(KEY_R)) {
            game.score = 0;
            game.gameOver = false;
            game.gravityFlippedGlobal = false;
            player.lane = 1;
            player.x = GetLaneX(player.lane);
            player.gravityFlipped = false;
            player.flipTimer = 0;
            player.y = 500;
            for (int i = 0; i < 5; i++) obstacles[i].active = false;
            for (int i = 0; i < 3; i++) pedestrians[i].active = false;
        }
        
        BeginDrawing();
        ClearBackground(BLACK);
        
        DrawRoad();
        
        // Draw obstacles and pedestrians
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
        
        // UI Elements
        DrawText(TextFormat("Score: %d", game.score), 10, 10, 20, WHITE);
        DrawText("Arrow Keys: Move (Up/Down for speed)", 10, 570, 15, WHITE);
        DrawText("G: Flip Gravity (5 sec)", 10, 40, 15, SKYBLUE);
        DrawText("Avoid pedestrians! They end your game!", 10, 70, 15, RED);
        
        // Draw gravity indicator
        DrawGravityIndicator(player);
        
        if (game.gameOver) {
            DrawText("GAME OVER - Press R to restart", 200, 300, 30, RED);
        }
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

void ApplyGravityFlip(Car *player, GameState *game) {
    player->gravityFlipped = !player->gravityFlipped;
    game->gravityFlippedGlobal = player->gravityFlipped;
    
    if (player->gravityFlipped) {
        // Flip to ceiling
        player->y = SCREEN_HEIGHT - 100;  // Position on ceiling
        player->flipTimer = GRAVITY_FLIP_DURATION;
        player->color = PURPLE;  // Change color when flipped
    } else {
        // Return to road
        player->y = 500;
        player->flipTimer = 0;
        player->color = RED;  // Return to normal color
    }
}

void DrawGravityIndicator(Car player) {
    if (player.gravityFlipped) {
        int timerBarWidth = 100;
        int timerX = SCREEN_WIDTH - timerBarWidth - 20;
        int timerY = 20;
        
        // Draw background
        DrawRectangle(timerX, timerY, timerBarWidth, 20, DARKGRAY);
        
        // Draw progress bar
        float progress = player.flipTimer / GRAVITY_FLIP_DURATION;
        DrawRectangle(timerX, timerY, (int)(timerBarWidth * progress), 20, 
                     progress > 0.5f ? GREEN : (progress > 0.25f ? YELLOW : RED));
        
        // Draw outline
        DrawRectangleLines(timerX, timerY, timerBarWidth, 20, WHITE);
        
        // Draw label
        DrawText("GRAVITY FLIP", timerX - 120, timerY + 5, 20, PURPLE);
        
        // Draw arrow indicator
        DrawTriangle((Vector2){SCREEN_WIDTH - 40, 70}, 
                    (Vector2){SCREEN_WIDTH - 20, 70}, 
                    (Vector2){SCREEN_WIDTH - 30, 90}, PURPLE);
    }
}

void DrawRoad() {
    int roadX = (SCREEN_WIDTH - ROAD_WIDTH) / 2;
    
    DrawRectangle(roadX, 0, ROAD_WIDTH, SCREEN_HEIGHT, GRAY);
    
    // Draw lane markings
    for (int i = 1; i < LANE_COUNT; i++) {
        int laneX = roadX + i * (ROAD_WIDTH / LANE_COUNT);
        for (int y = 0; y < SCREEN_HEIGHT; y += 40) {
            DrawRectangle(laneX - 2, y, 4, 20, WHITE);
        }
    }
    
    // Draw road borders
    DrawRectangle(roadX, 0, 5, SCREEN_HEIGHT, WHITE);
    DrawRectangle(roadX + ROAD_WIDTH - 5, 0, 5, SCREEN_HEIGHT, WHITE);
    
    // Draw sidewalks
    DrawRectangle(roadX - 30, 0, 30, SCREEN_HEIGHT, DARKBROWN);
    DrawRectangle(roadX + ROAD_WIDTH, 0, 30, SCREEN_HEIGHT, DARKBROWN);
    
    // Draw ceiling road markings if gravity is flipped
    // (This would require additional logic to track gravity state)
}

void DrawCar(Car car) {
    // Save current transformation matrix
    rlPushMatrix();
    
    // Translate to car position
    rlTranslatef(car.x + CAR_SIZE/2, car.y + CAR_SIZE/2, 0);
    
    // Rotate 180 degrees if gravity is flipped
    if (car.gravityFlipped) {
        rlRotatef(180, 0, 0, 1);
    }
    
    // Translate back for drawing
    rlTranslatef(-(car.x + CAR_SIZE/2), -(car.y + CAR_SIZE/2), 0);
    
    // Draw car body
    DrawRectangle(car.x, car.y, CAR_SIZE, CAR_SIZE, car.color);
    
    // Draw windows
    DrawRectangle(car.x + 5, car.y + 5, CAR_SIZE - 10, 10, SKYBLUE);
    DrawRectangle(car.x + 5, car.y + CAR_SIZE - 15, CAR_SIZE - 10, 10, SKYBLUE);
    
    // Draw wheels
    DrawRectangle(car.x - 3, car.y + 10, 6, CAR_SIZE - 20, BLACK);
    DrawRectangle(car.x + CAR_SIZE - 3, car.y + 10, 6, CAR_SIZE - 20, BLACK);
    
    // Restore transformation matrix
    rlPopMatrix();
    
    // Draw gravity effect particles when flipped
    if (car.gravityFlipped) {
        for (int i = 0; i < 5; i++) {
            int particleX = car.x + rand() % CAR_SIZE;
            int particleY = car.y + CAR_SIZE + rand() % 10;
            DrawCircle(particleX, particleY, 2, (Color){200, 100, 255, 150});
        }
    }
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

void MoveObstacles(Obstacle obstacles[], int count, int speed, bool gravityFlipped) {
    for (int i = 0; i < count; i++) {
        if (obstacles[i].active) {
            if (gravityFlipped) {
                // Move upward when gravity is flipped
                obstacles[i].y -= speed;
                if (obstacles[i].y < -CAR_SIZE) {
                    obstacles[i].active = false;
                }
            } else {
                // Normal downward movement
                obstacles[i].y += speed;
                if (obstacles[i].y > SCREEN_HEIGHT) {
                    obstacles[i].active = false;
                }
            }
        }
    }
}

void MovePedestrians(Pedestrian pedestrians[], int count, bool gravityFlipped) {
    for (int i = 0; i < count; i++) {
        if (pedestrians[i].active) {
            if (pedestrians[i].crossing) {
                pedestrians[i].x += pedestrians[i].direction * 2;
                
                if (pedestrians[i].x < 0 || pedestrians[i].x > SCREEN_WIDTH) {
                    pedestrians[i].active = false;
                }
            }
            
            // Adjust pedestrian vertical position based on gravity
            if (gravityFlipped) {
                // Pedestrians "fall" upward when gravity is flipped
                pedestrians[i].y -= 1;
                if (pedestrians[i].y < -PEDESTRIAN_SIZE) {
                    pedestrians[i].active = false;
                }
            } else {
                // Normal gravity - pedestrians stay at their y position
                // (They don't fall down in the original game)
            }
        }
    }
}

void CreateObstacle(Obstacle obstacles[], int count, bool gravityFlipped) {
    for (int i = 0; i < count; i++) {
        if (!obstacles[i].active) {
            obstacles[i].active = true;
            obstacles[i].lane = rand() % LANE_COUNT;
            obstacles[i].x = GetLaneX(obstacles[i].lane);
            
            if (gravityFlipped) {
                // Spawn at bottom when gravity is flipped
                obstacles[i].y = SCREEN_HEIGHT;
            } else {
                // Spawn at top normally
                obstacles[i].y = -CAR_SIZE;
            }
            
            obstacles[i].color = BLUE;
            break;
        }
    }
}

void CreatePedestrian(Pedestrian pedestrians[], int count, bool gravityFlipped) {
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
            
            if (gravityFlipped) {
                // Spawn pedestrians lower when gravity is flipped
                pedestrians[i].y = SCREEN_HEIGHT - 50 - (rand() % 100);
            } else {
                pedestrians[i].y = SCREEN_HEIGHT - 100 - (rand() % 200);
            }
            
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
