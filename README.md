#  Horrified Game - Advanced Programming Project (Phase 1)

This project is the **first phase** of the final project for the *Advanced Programming* course, based on the logic of the board game **HORRIFIED**.  
Implemented in **C++** using object-oriented principles and modular design, this terminal-based game simulates a cooperative experience where two heroes must defeat monsters and save villagers.

> Course Instructor: Dr. Morteza Yousef Sanati  
> Term: 4032  
> Contributors: Zeinab Khalili & Fatemeh Shabani
> GitHub Repository: [https://github.com/AiKhalili/Horrified-Game]

---

##  Build & Run Instructions

### Prerequisites
- C++11 or later
- CMake 3.10+
- A C++ compiler (e.g., g++, clang++, MSVC)

### Steps

```bash
# Clone the repository
git clone https://github.com/AiKhalili/Horrified-Game.git
cd Horified-Game
mkdir build && cd build
cmake ..
make
./HorrifiedGame
```

---

##  Project Structure

```bash
.
├── src/                    
│   ├── Game.cpp / 
│   ├── Hero.cpp /   
│   ├── Villager.cpp /  
│   ├── ItemManager.cpp /  
│   └── ...
├── include/
│   ├── Game.hpp / 
│   ├── Hero.hpp /   
│   ├── Villager.hpp /  
│   ├── ItemManager.hpp /  
│   └── ...                
├── CMakeLists.txt        
├── README.md                       
└── build/                
```

---

##  Project Overview

In this game, two players take on the roles of heroes:
- **Archaeologist** (with a special ability)
- **Mayor** (with more actions but no special ability)

They work together to defeat:
- **Dracula** (by destroying four coffins)
- **Invisible Man** (by collecting proof items)

They must also protect and guide **villagers** to their safe locations.  
The game ends in victory if both monsters are defeated, or in loss if the **Terror Level** reaches 5 , and time's up **monster cards run out**.

---

## Features

- Fully object-oriented C++ implementation
- Modular & maintainable codebase
- Heroes and Monsters with unique abilities and logic
- Game phases: Hero Phase, Monster Phase
- Action system: `Move`, `Pick Up`, `Guide`, `Advance`, `Defeat`, `Special Action`
- Complete item management (3 types, 60 items)
- Perk cards and monster cards system
- Frenzy system for monster aggression
- Turn-based dice-driven monster strikes
- Detailed terminal-based interface (TUI)
- Structured exception handling
- Built with CMake
- Version controlled via Git

---

##  Team Collaboration (Git Workflow)

We followed a Git-based collaboration model:

- Each contributor worked on their own branch (`feature-monster`, `feature-hero`)
- Commits were made frequently with meaningful messages
- Final integration was done via merging branches
- Entire history is visible in the GitHub repository

---

##  Game Highlights

-  Monster phase uses custom dice logic with symbols (`*`, `!`)
-  Dracula’s defeat requires destroying 4 coffins using red items
-  Invisible Man requires collecting 5 items from specific locations and placing them at the precinct
-  Villagers must be guided to their safe places to earn Perk cards
-  Perk cards can be used at any time in the Hero phase (not counted as actions)

---