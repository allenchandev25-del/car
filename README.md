# Car Racing with Pedestrians & Gravity Flip 🚗🌀

A thrilling 2D racing game built with Raylib where you navigate through traffic, avoid pedestrians, and use gravity flips to drive on the ceiling! Test your reflexes and master gravity manipulation to achieve the highest score.

![Game Screenshot](https://via.placeholder.com/800x400/333333/FFFFFF?text=Car+Racing+with+Gravity+Flip)

## ✨ Features

- **Three-Lane Racing**: Navigate through dynamic traffic across three lanes
- **Gravity Mechanics**: Flip gravity to drive upside-down on the ceiling
- **Pedestrian AI**: Watch out for crossing pedestrians that can end your game
- **Speed Control**: Adjust your speed strategically with arrow keys
- **Visual Effects**: Particle effects, car rotation, and color changes
- **Score System**: Compete for the highest survival time
- **Game States**: Complete game over and restart functionality

## 🎮 Controls

| Key | Action | Description |
|-----|--------|-------------|
| **← →** | Lane Change | Move between three lanes |
| **↑** | Speed Boost | Increase speed (12 units) |
| **↓** | Slow Down | Decrease speed (5 units) |
| **G** | Gravity Flip | Flip gravity for 5 seconds |
| **R** | Restart | Restart after game over |

## 🚀 Installation

### Prerequisites
- [Raylib 4.0+](https://www.raylib.com/)
- C Compiler (GCC, Clang, or MSVC)

### Linux/macOS
```bash
# Ubuntu/Debian
sudo apt-get install libraylib-dev

# macOS
brew install raylib

# Compile
gcc -o gravity_racing main.c -lraylib -lm -lpthread -ldl

# Run
./gravity_racing

Windows
1. Download Raylib from raylib.com
2. Add Raylib to include/library paths
3. Compile:
gcc -o gravity_racing main.c -lraylib -lopengl32 -lgdi32 -lwinmm

PROJECT STRUCTURE
car-racing-gravity/
├── main.c              # Main game source code
├── README.md          # This file
├── LICENSE            # MIT License
└── assets/            # Future assets folder

🎯 Gameplay Tips
1.Use Gravity Strategically: Flip gravity to avoid clusters of obstacles
2.Watch Pedestrians: Green pedestrians may suddenly turn red and cross
3.Manage Speed: Slow down in crowded areas, speed up when clear
4.Time Your Flips: Gravity flips last only 5 seconds - use them wisely
5.Stay Centered: Middle lane gives you more reaction time

🔧 Customization
You can easily modify game parameters in the code:
// Change these values in main.c:
#define SCREEN_WIDTH 800      // Window width
#define SCREEN_HEIGHT 600     // Window height
#define ROAD_WIDTH 400        // Road width
#define CAR_SIZE 40           // Vehicle size
#define LANE_COUNT 3          // Number of lanes
#define GRAVITY_FLIP_DURATION 5.0f  // Flip duration

