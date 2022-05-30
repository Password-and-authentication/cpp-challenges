#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

// THIS PROJECT IS ABANDONED IT DOES NOT WORK


constexpr int farLeftPosition = 100;
constexpr int farRightPosition = 500;
constexpr int farTopPosition = 100;
constexpr int farBottomPosition = 420;

static int getRandomNumber(int min, int max) {
  static constexpr double fraction{1.0 / (static_cast<double>(RAND_MAX) + 1.0)};
  return static_cast<int>(std::rand() * fraction * (max - min + 1) + min);
}

void processInput(SDL_Event &e, bool &running) {
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT:
      running = 0;
    }
  }
};

class Position {
public:
  uint16_t x1;
  uint16_t y1;

  friend bool operator==(const Position &pos1, const Position &pos2);

  void print() {} // disabled for now
};

bool operator==(const Position &pos1, const Position &pos2) {
  return (pos1.x1 == pos2.x1 && pos1.y1 == pos2.y1);
}

class Wall {
public:
  enum class Type { left, right, bottom, top };

private:
  uint16_t m_x1;
  uint16_t m_y1;
  uint16_t m_x2;
  uint16_t m_y2;
  uint16_t m_HEIGHT{20};
  uint16_t m_WIDTH{20};
  SDL_Renderer *renderer;
  bool m_display;

public:
  Wall(Position &pos, Type wallType, bool display, SDL_Renderer *r)
      : m_x1{pos.x1}, m_y1{pos.y1}, m_x2{pos.x1}, m_y2{pos.y1},
        m_display{display}, renderer{r} {
    std::cout << pos.x1 << "x1\n";
    std::cout << pos.y1 << "y1\n";

    if (wallType == Type::left) {
      m_y2 = pos.y1 + m_HEIGHT;
    } else if (wallType == Type::right) {
      m_x1 = pos.x1 + 20;
      m_x2 = pos.x1 + 20;
      m_y2 = pos.y1 + 20;
    } else if (wallType == Type::bottom) {
      m_y1 = pos.y1 + 20;
      m_x2 = pos.x1 + 20;
      m_y2 = pos.y1 + 20;
    } else if (wallType == Type::top) {
      m_x2 = pos.x1 + 20;
    }
  }

  void draw() {
    std::cout << m_x1 << "mx1\n"; // this prints crazy values
    assert(m_x1 != 0);
    assert(m_y1 != 0);
    assert(m_x2 != 0);
    assert(m_y2 != 0);
    std::cout << m_y2 << "y2\n";

    SDL_RenderDrawLine(renderer, m_x1, m_y1, m_x2, m_y2);
  }

  [[nodiscard]] Position getPos() const { return {m_x1, m_y1}; };
};

class Cell {
  Wall left;
  Wall right;
  Wall bot;
  Wall top;
  Position m_position{};
  std::vector<Position> m_friends;

public:
  bool visited;

  Cell(Position &pos, SDL_Renderer *r)
      : left{{pos}, Wall::Type::left, true, r},
        right{{pos}, Wall::Type::right, true, r}, bot{{pos},
                                                      Wall::Type::bottom,
                                                      true,
                                                      r},
        top{{pos}, Wall::Type::top, true, r}, m_position{pos} {
    pos.print();
    if (pos.x1 > farLeftPosition) {
      m_friends.push_back({pos.x1 - 20, pos.y1});
    }
    if (pos.x1 < farRightPosition) {
      m_friends.push_back({pos.x1 + 20, pos.y1});
    }
    if (pos.y1 < farTopPosition) {
      m_friends.push_back({pos.x1, pos.y1 + 20});
    }
    if (pos.y1 > farBottomPosition) {
      m_friends.push_back({pos.x1, pos.y1 - 20});
    };
  };

  void draw() { left.draw(); }

  std::vector<Position> getFriends() const { return m_friends; };

  Position getPosition() { return m_position; };

  void printFriends() {

    for (auto &f : m_friends) {
    };
  }

  Cell &getRandomFriend(std::vector<Cell> &cells);
};

Cell *getCell(std::vector<Cell> &cells, Position &position) {
  for (auto &cell : cells) {
    if (cell.getPosition() == position)
      return &cell;
  }
}

Cell &Cell::getRandomFriend(std::vector<Cell> &cells) {

  printFriends();
  int index{getRandomNumber(0, m_friends.size() - 1)};
  assert(index != m_friends.size());
  Position position{m_friends[index]};
  assert(!(getPosition() == position));
  Cell *cell{getCell(cells, position)};
  if (cell->visited == true)
    return getRandomFriend(cells);
  return *cell;
};

[[nodiscard]] std::vector<Cell> initializeGrid(int cellCount,
                                               SDL_Renderer *renderer) {

  int rows{static_cast<int>(std::sqrt(cellCount))};
  std::vector<Cell> cells{};

  int xDistance{};
  int yDistance{};

  for (int i = 1; i <= cellCount; ++i) {
    Position pos{static_cast<uint16_t>(100 + xDistance),
                 static_cast<uint16_t>(100 + yDistance)};
    Cell cell{pos, renderer};
    cells.push_back({cell});
    if (i % rows == 0) {
      xDistance = 20;
      yDistance += 20;
    } else
      xDistance += 20;
  }
  return cells;
};

void generateMaze(std::vector<Cell> &cells) {

  auto rng = std::default_random_engine{};
  std::shuffle(cells.begin(), cells.end(), rng);

  for (Cell& cell : cells) {
    cell.visited = true;

    int friendIndex{getRandomNumber(0, cell.getFriends().size() - 1)};
    assert(friendIndex != cell.getFriends().size());
    Position friendPosition{cell.getFriends()[friendIndex]};

    // if i delete this everything works
    Cell& friendCell{cell.getRandomFriend(cells)};

    cell.draw();
  }
}

int main(int argc, char *argv[]) {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  std::rand();

  SDL_Window *win;
  SDL_Renderer *renderer;
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_CreateWindowAndRenderer(1280, 720, 0, &win, &renderer);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 250, 255, 0, 255);
  // SDL_RenderDrawLine(renderer, 100, 100, 200, 100);

  std::vector<Cell> cells{initializeGrid(625, renderer)};
  generateMaze(cells);

  SDL_RenderPresent(renderer);

  bool running{1};
  while (running) {
    SDL_Event e;
    processInput(e, running);
  };

  return 0;
}