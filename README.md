#  Horrified Game - Advanced Programming Final Project 

This project is a full implementation of the HORRIFIED board game as the final project for the Advanced Programming course.
The game was first developed in Phase 1 (Terminal Edition – TUI) and then extended in Phase 2 (Graphical Edition – raylib) with a complete graphical user interface.

> Course Instructor: Dr. Morteza Yousef Sanati  
> Term: 4032  
> Contributors: Zeinab Khalili & Fatemeh Shabani
> GitHub Repository: [https://github.com/AiKhalili/Horrified-Game]

---

##  Build & Run Instructions

### Prerequisites
- C++17 or later
- CMake 3.10+
- A C++ compiler (e.g., g++, clang++, MSVC)
- raylib

### Steps

```bash
# Clone the repository
git clone https://github.com/AiKhalili/Horrified-Game.git
cd Horified-Game
mkdir build && cd build
cmake ..
make
./Horrified
```

---

##  Project Structure

```bash
.
├── assets/              # Images, sounds, fonts
├── src/                    
│   ├── app/             #GameApp
│   ├── audio/           # AudioManager 
│   ├── core/            # Core game logic (Hero, Monster, Villager, Game)
│   ├── graphics/ui/     # Custom UI system (UIButton, UILabel, UIManager , ...) 
│   └── graphics/scenes  # Graphical scenes (Intro, MainMenu, Board, etc.)
│   └── saves/           # Save/Load system 
├── include/                  
│   ├── app/
│   ├── audio/ 
│   ├── core/
│   ├── graphics/ui/
│   └── graphics/scenes
│   └── saves/              
├── CMakeLists.txt  
├── main.cpp                            
└── README.md                
```

---

##  Project Overview

In this game, two players take on the roles of heroes:
- **Archaeologist** (with a special action)
- **Mayor** (with more actions but no special action)
- **Scientist** (with special ability)
- **Courier** (with special action)

They work together to defeat:
- **Dracula** (by destroying four coffins)
- **Invisible Man** (by collecting proof items)

They must also protect and guide **villagers** to their safe locations.  
The game ends in victory if both monsters are defeated, or in loss if the **Terror Level** reaches 5 , and time's up **monster cards run out**.

---

## Features

## Phase 1 – Terminal Edition (TUI)

- Fully object-oriented, modular C++ design
- Core gameplay phases (Hero Phase, Monster Phase)
- Item management system (3 types, 60 items)
- Monster & Perk cards
- Frenzy system and monster
- Turn-based dice-driven monster attacks
- Structured exception handling
- Text-based user interface

## Phase 2 – Graphical Edition (raylib)

- Complete graphical environment built with raylib
- Scene-based architecture (Intro, Main Menu, Board, Save/Load, Info scenes)
- Custom UI system (UIButton, UILabel, UIManager)
- Save & Load functionality with multiple slots
- Sound effects and background music with AudioManager
- Hover glow effects on locations, visual hero/monster placement

---

##  Team Collaboration (Git Workflow)

We followed a Git-based collaboration model:

- Each contributor worked on their own branch (`feature-monster`, `feature-hero` ,  `graphics/khalili` , `graphics/shabani`)
- Commits were made frequently with meaningful messages
- Final integration was done via merging branches
- Entire history is visible in the GitHub repository

---