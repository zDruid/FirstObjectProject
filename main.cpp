#include "GameBoard.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
static const int kInitialPlayerCalories = 120;
static const int kPlayerCaloriesPerMove = 5;
static const int kCalorieGainPerMeal = 60;
static const int kCaloriesGainedPerRest = 5;

/*****************************************************************************/
// TurnInfo
//
// This struct contains parameters required for a Creature to take its turn.
// In this project, it contains just the board dimensions; but the structure
// allows us the opportunity to change this later without changing method
// signatures.
/*****************************************************************************/

struct TurnInfo {
  int rowCount;
  int colCount;
};

/*****************************************************************************/
// Creature
/*****************************************************************************/

class Creature {
public:
  Creature() = default;

  // Initialize the creature's instance data using values in creatureParams
  // as you've done in the lab.

  // format is: name glyph row col dRow dCol calories
  //"Mouse M 5 2 1 1 90"
  Creature(string creatureParams) {
    stringstream ss(creatureParams);
    ss >> _name >> _glyph >> _row >> _col >> _dRow >> _dCol >> _calories;
    _initialCalories = _calories;
  }

  void rest() {
    _calories += kCaloriesGainedPerRest;

    if (_calories > 0.75 * kInitialPlayerCalories) {
      _resting = false;
    }
  }

  // return true if the Creature's position equals the passed position.
  bool positionEquals(int row, int col) {
    return ((row == _row) && (col == _col));
  }

  // The takeTurn() method is coded for you. It is called by the main loop
  // to prompt the Creature to behave. For now, it simply calls the move()
  // method.
  // Use it as-is in Part A of the project. You will enhance it in Part B.
  void takeTurn(const TurnInfo &turnInfo) {
    if (_resting) {
      rest();
    } else {
      move(turnInfo);
    }
  }

  // move() determines where the Creature will move next, checking that the
  // location is valid (within bounds on the board).
  // Use it as-is in Part A of the project. You will enhance it in Part B.
  void move(const TurnInfo &turnInfo) {

    int candidateRow = _row + _dRow;
    if (candidateRow < 0 || candidateRow >= turnInfo.rowCount) {
      _dRow *= -1;
      candidateRow = _row + _dRow;
    }

    int candidateCol = _col + _dCol;
    if (candidateCol < 0 || candidateCol >= turnInfo.colCount) {
      _dCol *= -1;
      candidateCol = _col + _dCol;
    }

    if (_row != candidateRow || _col != candidateCol) {
      _row = candidateRow;
      _col = candidateCol;
      _calories -= kPlayerCaloriesPerMove;
      if (_calories <= 0) {
        _resting = true;
      }
    }
  }

  void setTile(GameBoard *board) const {
    // set a tile representing the creature's current state on the game board
    board->setTileAt(_row, _col, _glyph, color());
  }

  void clearTile(GameBoard *board) const {
    // clear a tile representing the creature's current state on the game board
    board->clearTileAt(_row, _col);
  }

  // use this method as-is. it provides a formatted string representing the
  // creature's state
  string toString() const {
    ostringstream oss;
    oss << left << setw(16) << _name << "(" << _glyph << ") " << _calories
        << (_resting ? " R" : " ");
    return oss.str();
  }

  // Use this method as-is, or feel free to change Creatures' colors to any one
  // of the enum constants defined in GameBoard.h
  Color color() const { return Color::red; }

  // Part A, step 3: Define getters. These should follow the naming and style
  // standards discussed in class and in labs.

  // e.g.: string name() const { return _name; }

  // Getters
  string name() const { return _name; }
  int row() const { return _row; }
  int col() const { return _col; }
  int dRow() const { return _dRow; }
  int dCol() const { return _dCol; }
  char glyph() const { return _glyph; }
  int calories() const { return _calories; }
  int initialCalories() const { return _initialCalories; }
  bool resting() const { return _resting; }

private:
  // Part A, step 2: Define the instance vars:

  // a string called _name which is the creature's name
  // an int called _row that represents the creature's row on the GameBoard
  // an int called _col that represents the creature's column on the GameBoard
  // two ints called _dRow and _dCol ("d" stands for delta) which can be -1, 0,
  // or +1,
  //   these describe a direction vector. they are added to the Creature's row
  //   and col as it moves, and they are changed as it "bounces" off the walls
  // a char called _glyph which represents the creature's symbol on the
  // GameBoard you will be asked to create a few more variables in Part B

  // e.g.: string _name = "";

  // Part A, step 3: Define the setters. These should follow the naming and
  // style standards discussed in class and in labs.

  // e.g.: void setName(string name) { _name = name; }

  string _name = "";
  int _row = 0;
  int _col = 0;
  int _dRow = 0;
  int _dCol = 0;
  char _glyph;
  int _calories = 0;
  int _initialCalories = 0;
  bool _resting = false;

  void setName(string name) { _name = name; }
  void setRow(int row) { _row = row; }
  void setCol(int col) { _col = col; }
  void setDRow(int dRow) { _dRow = dRow; }
  void setDCol(int dCol) { _dCol = dCol; }
  void setGlyph(char glyph) { _glyph = glyph; }
  void setCalories(int calories) { _calories = calories; }
  void setInitialCalories(int initialCalories) {
    _initialCalories = initialCalories;
  }
  void setResting(bool resting) { _resting = resting; }
};

/*****************************************************************************/
// CreatureList
/*****************************************************************************/

class CreatureList {
public:
  ~CreatureList() {
    // Iterate the _creatures vector, deleting the creature pointers it contains
    for (int i = 0; i < _creatures.size(); i++) {
      delete _creatures[i];
    }
  }

  void addCreaturesFromFile(string fileName) {
    // Like the lab on this topic, open the file, check that it has opened
    // Read each line, rejecting comment and blank lines.  Use the string
    // found on each line to create a new Creature, adding the resulting pointer
    // to the _creatures vector. Close the file.

ifstream file(fileName);
  if (!file.is_open()) {
    cout << "Failed to open file: " << fileName << endl;
    exit(0);
  }

  string line;
  while (getline(file, line)) {
    // Skip empty lines and comments (lines starting with two slashes)
    if (line.size() == 0 || line.find("//") == 0)
      continue;

    // cout << line << endl;
    Creature *c = new Creature(line);

    _creatures.push_back(c);
  }

  file.close();
}

  int creatureCount() {
    // return the size of the _creatures vector.
    return _creatures.size();
  }

  void setTiles(GameBoard *board) {
    // Iterate the _creatures vector, invoking the setTile() method on each
    // one.
    for (int i = 0; i < _creatures.size(); i++) {
      _creatures[i]->setTile(board);
    }
  }

  void clearTiles(GameBoard *board) {
    // Iterate the _creatures vector, invoking the clearTile() method on
    // each one.
    for (int i = 0; i < _creatures.size(); i++) {
      _creatures[i]->clearTile(board);
    }
  }

  void takeTurns(const TurnInfo &turnInfo) {
    // Iterate the _creatures vector, invoking the takeTurn() method on each
    // one.
    for (int i = 0; i < _creatures.size(); i++) {
      _creatures[i]->takeTurn(turnInfo);
    }
  }

  Creature *creatureAtPosition(int row, int col) {
    // Search the _creatures vector for a Creature with matching row and
    // col. If found, return it; otherwise return nullptr.
    for (int i = 0; i < _creatures.size(); i++) {
      if (_creatures[i]->positionEquals(row, col))
        return _creatures[i];
    }
    return nullptr;
  }

  // Use this method as-is. When a creature is eaten, it must be removed
  // from the _creatures vector. This delete's the pointer before dropping
  // the reference to it so the corresponding Creature will be deallocated
  // from the heap.
  void eraseCreature(Creature *creature) {
    for (int index = 0; index < creatureCount(); index++) {
      if (_creatures[index] == creature) {
        delete _creatures[index];
        _creatures.erase(_creatures.begin() + index);
        break;
      }
    }
  }

  string toString() {
    // Iterate the _creatures vector, calling each Creature's toString().
    // Concatenate each string on an osstringstream, placing endl's between
    // them. return the resulting string
    ostringstream oss;
    for (int i = 0; i < _creatures.size(); i++) {
      oss << _creatures[i]->toString() << endl;
    }
    return oss.str();
  }

private:
  // Part A, step 5: Define the instance var
  vector<Creature *> _creatures;
};

/*****************************************************************************/
// Utils
/*****************************************************************************/

// Use this function as-is. It formats a complete status message using the
// player's status as well as the CreatureList status string (toString())
// The main loop places this result on the GameBoard, by invoking the
// board's setMessage() method.
string MessageLine(CreatureList *creatureList, int playerCalories,
                   string status = "") {
  ostringstream oss;
  oss << "Player has " << playerCalories << " cals " << status;
  if (status.length())
    oss << ". " << status << endl;
  else
    oss << endl;
  return oss.str();
}

void LogGameStats(int turnCount, char cmd, int playerCalories,
                  int creatureCount) {
  static ofstream logFile("CreatureLogFile.txt");
  string move = "\\" + to_string(cmd);
  switch (cmd) {
  case arrowUpKey:
    move = "up";
    break;
  case arrowDownKey:
    move = "down";
    break;
  case arrowRightKey:
    move = "right";
    break;
  case arrowLeftKey:
    move = "left";
    break;
  case ' ':
    move = "rest";
    break;
  }
  logFile << "turn:" << turnCount << " cmd:'" << move
          << "' calories:" << playerCalories << " creatures:" << creatureCount
          << endl;
}

/*****************************************************************************/
// main / controller
/*****************************************************************************/

// Use this code as-is in Part A. In Part B, you will make a small edit to
// it.
int main() {
  bool gameOver = false;
  int turnCount = 0;
  string status = "";

  const int kBoardRowCount = 20;
  const int kBoardColCount = 30;
  GameBoard board(kBoardRowCount, kBoardColCount);
  board.setDisplayCoords(false);

  int playerRow = 5;
  int playerCol = 5;
  int playerCalories = kInitialPlayerCalories;

  CreatureList creatureList;
  creatureList.addCreaturesFromFile("creature-data.txt");
  board.setLogLineCount(creatureList.creatureCount()+1);

  while (!gameOver) {
    // set board tile for the player, set the board message to the current
    // status
    creatureList.setTiles(&board);
    board.setTileAt(playerRow, playerCol, '@', defaultColor);
    board.setMessage(MessageLine(&creatureList, playerCalories, status));
    board.clearLog();
    board << creatureList.creatureCount() << " creatures" << endl;
    board << creatureList.toString();
    status = "";

    // draw the tiles and status on the console
    board.updateConsole();

    board.clearTileAt(playerRow, playerCol);
    creatureList.clearTiles(&board);

    // get user input
    char cmd = board.nextCommandKey();

    bool playerMoved = false;

    switch (cmd) {
    case arrowUpKey:
      if (playerRow > 0) {
        playerMoved = true;
        --playerRow;
      }
      break;
    case arrowDownKey:
      if (playerRow + 1 < board.rowCount()) {
        playerMoved = true;
        ++playerRow;
      }
      break;
    case arrowRightKey:
      if (playerCol + 1 < board.colCount()) {
        playerMoved = true;
        ++playerCol;
      }
      break;
    case arrowLeftKey:
      if (playerCol > 0) {
        playerMoved = true;
        --playerCol;
      }
      break;
    case 'q':
    case 'Q':
      cout << "Bye" << endl;
      gameOver = true;
      break;
    default:
      break;
    }
    if (playerMoved) {
      playerCalories -= kPlayerCaloriesPerMove;
    }

    // here, we're passing a const TurnInfo ref, creating one inline
    // using the "designated initialiation" style which names the member
    // vars
    creatureList.takeTurns(
        {.rowCount = board.rowCount(), .colCount = board.colCount()});

    // the player and creatures may have moved. find out if a creature
    // occupies the same position as the player. if so, the player will eat
    // that creature
    Creature *creatureToEat =
        creatureList.creatureAtPosition(playerRow, playerCol);
    if (creatureToEat) {
      status = "Ate the " + creatureToEat->name();
      playerCalories += creatureToEat->calories();
      creatureList.eraseCreature(creatureToEat);
      if (creatureList.creatureCount() == 0) {
        status = "Having eaten everything, player wins!";
        gameOver = true;
      }
    }

    if (playerCalories <= 0) {
      status = "Having starved, player dies.";
      gameOver = true;
    }

    LogGameStats(turnCount, cmd, playerCalories, creatureList.creatureCount());
    ++turnCount;
  }
  creatureList.setTiles(&board);
  board.setTileAt(playerRow, playerCol, '@', defaultColor);
  board.setMessage(MessageLine(&creatureList, playerCalories, status));
  board.updateConsole();
}

/*
Jonathan - I know from direct experince with you that you've got aptitude
well in excess of what's needed to get this project done well. I think the
thing that hurt you was process: starting at the 11th hour gave you no time
to work deliberately or to seek help if you needed it.

There a four major projects, so there's time to get the next ones back on
track.

I just noticed that your zyBook hasn't been opened at all during the
quarter. Is there a technical problem that we need to address?

20/100

Revised, despite late submission, late fixes, to 70/100

*/