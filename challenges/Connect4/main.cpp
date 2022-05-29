#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <utility>

// runs in the terminal 
// no input type checking

class Board {
public:
  enum class CellType { player1, player2, empty };
  using board_type = std::array<std::array<CellType, 7>, 6>;

private:
  board_type m_board{};
  std::pair<uint8_t, uint8_t> m_latestHit{};

  std::string getCellColor(CellType type) const noexcept {
    switch (type) {
    case CellType::player1:
      return "\033[1;33m O\033[0m"; // red
    case CellType::player2:
      return "\033[1;36m O\033[0m"; // blue
    case CellType::empty:
      return "\033[1;30m O\033[0m"; // black
    }
  }

public:
  Board() {
    for (uint8_t cols{0}; cols < 7; ++cols) {
      for (uint8_t rows{0}; rows < 6; ++rows) {
        m_board[rows][cols] = CellType::empty;
      }
    }
  }

  void reset() noexcept {
    std::for_each(m_board.begin(), m_board.end(), [](auto &col) {
      std::for_each(col.begin(), col.end(),
                    [](auto &cell) { cell = CellType::empty; });
    });
  }

  void print() const noexcept {
    std::for_each(m_board.begin(), m_board.end(), [&](auto &col) {
      std::cout << '\t';
      std::for_each(col.begin(), col.end(),
                    [&](auto &cell) { std::cout << getCellColor(cell); });
      std::cout << '\n';
    });
  };

  void hit(uint8_t col, CellType player) noexcept {
    // the first cell on the column that is not already hit
    uint8_t freeCell{static_cast<uint8_t>(getColDepth(col, m_board) - 1)};
    m_board[freeCell][col] = player;
    m_latestHit = {freeCell, col};
  };

  // returns how many cells are not empty in that column
  [[nodiscard]] static constexpr uint8_t
  getColDepth(uint8_t col, const board_type &board) noexcept {
    uint8_t depth{};
    // start loop at top of col
    for (uint8_t row{0}; row < 6; ++row) {
      if (board[row][col] != CellType::empty)
        return row;
    }
    // if all are empty return 6
    return 6;
  };

  [[nodiscard]] board_type getBoard() const { return m_board; };

  class Check {
    board_type board_h{};
    Board::CellType player_h;
    uint8_t m_row{};
    uint8_t m_col{};

  public:
    Check(Board::CellType player, const board_type &board, uint8_t row,
          uint8_t col)
        : player_h{player}, board_h{board}, m_row(row), m_col{col} {};

    [[nodiscard]] bool vertical() noexcept {
      // if row is too low to be able to win return false
      if (m_row >= 3)
        return false;
      for (uint8_t i = 0; i < 4; ++i) {
        if (board_h[m_row + 1][m_col] != player_h)
          return false;
      };
      return true;
    };

    [[nodiscard]] bool horizontal() noexcept {

      // if center column check for win on both sides
      if (m_col == 3) {
        return left() || right();

        // check only right side
      } else if (m_col < 3) {
        return right();

        // check noly left side
      } else if (m_col > 3) {
        return left();
      }
      // default
      return false;
    };

    [[nodiscard]] bool left() noexcept {

      for (uint8_t i = 0; i < 4; ++i) {
        if (board_h[m_row][m_col - i] != player_h)
          return false;
      }
      return true;
    };

    [[nodiscard]] bool right() noexcept {

      for (uint8_t i = 0; i < 4; ++i) {
        if (board_h[m_row][m_col + i] != player_h)
          return false;
      };
      return true;
    };

    [[nodiscard]] bool diagonal() noexcept {
      // for every direction check if the next 4 are equal
      // loop continues until a bad value is found
      // if no bad value is found during the loop return true

      // diagonal right down
      for (uint8_t i = 0; i < 4; ++i) {
        if (board_h[m_row + i][m_col + i] == player_h) {
          if (i == 3)
            return true;
          else
            continue;
        } else
          break;
      };

      // diagonal left down
      for (uint8_t i = 0; i < 4; ++i) {
        if (board_h[m_row + i][m_col - i] == player_h) {
          if (i == 3)
            return true;
          else
            continue;
        } else
          break;
      };

      // diagonal left up
      for (uint8_t i = 0; i < 4; ++i) {
        if (board_h[m_row - i][m_col - i] == player_h) {
          if (i == 3)
            return true;
          else
            continue;
        } else
          break;
      }

      // diagonal right up
      for (uint8_t i = 0; i < 4; ++i) {
        if (board_h[m_row - i][m_col + i] == player_h) {
          if (i == 3)
            return true;
          else
            continue;
        } else
          break;
      }

      // if no wins return false
      return false;
    }
  };

  [[nodiscard]] bool checkForWin(CellType player) noexcept {
    uint8_t row{m_latestHit.first};
    uint8_t col{m_latestHit.second};
    Check check{player, m_board, row, col};
    return (check.diagonal() || check.horizontal() || check.vertical());
  };
};

class Player {
  std::string m_name{};
  Board::CellType m_player{};

public:
  Player(std::string name = "", Board::CellType player = Board::CellType::empty)
      : m_name{name}, m_player{player} {};

  [[nodiscard]] Board::CellType getPlayer() const { return m_player; };

  std::string getName() const { return m_name; };
};

class Game {

private:
  Board board{};
  Player m_p1{};
  Player m_p2{};
  std::array<Player, 2> players{m_p1, m_p2};

  [[nodiscard]] uint8_t getInput() noexcept {
    std::cout << "Enter column:\n";
    uint8_t col;
    std::cin >> col;
    return col;
  }

public:
  Game(Player p1, Player p2) : m_p1{p1}, m_p2{p2} {};

  [[nodiscard]] Player play() {
    uint8_t currentPlayer{};
    uint8_t rounds{};
    uint8_t maxRounds{42};

    while (true) {
      if (rounds == maxRounds)
        return {"", Board::CellType::empty};
      board.print();

      uint8_t col;
      while (true) {
        col = getInput();
        // if column is full
        if (Board::getColDepth(col, board.getBoard()) != 1)
          break;

        std::cout << "column is full!\n";
      }
      board.hit(col, players[currentPlayer].getPlayer());

      if (board.checkForWin(players[currentPlayer].getPlayer())) {
        board.print();
        
      }
      currentPlayer = (currentPlayer == 1 ? 0 : 1);
    }
  }
};

[[nodiscard]] bool playAgain() noexcept {
  std::cout << "Restart game? [y/n]\n";
  char x;
  std::cin >> x;
  return (toupper(x) == 'Y');
}

[[nodiscard]] std::string getName(uint8_t nth) noexcept {
  std::cout << "Player " << nth << " name:\n";
  std::string name{};
  std::cin >> name;
  return name;
}

int main() {

  do {
    

      Player p1{getName(1), Board::CellType::player1};
      Player p2{getName(2), Board::CellType::player2};
      Game game{p1, p2};

      Player winner{game.play()};
      if (winner.getName() == "") {
        std::cout << "tie!\n";
      } else
      std::cout << "\nWinner: " << winner.getName() << '\n';
    
  } while (playAgain());
};
