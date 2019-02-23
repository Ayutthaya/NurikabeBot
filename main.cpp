#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <sstream>
#include <cassert>

using namespace std;

// TODO:
// tests
// test play
// main loop
// Nurikabe object
// Square -> Cell
// int i, int j -> {i, j}
// typedef Position
// Nurikabe::cell(Position pos)
// README

struct Square
{
  Square(int i, int j);
  enum Color {BLACK, WHITE, UNKNOWN};
  Color color;
  int number;
  int component_size;
  pair<int, int> parent;
  mutable bool visited = false;
};

Square::Square(int i, int j) :
  color(Square::UNKNOWN),
  number(0),
  component_size(1),
  parent({i, j})
{
}

int n_black;

typedef vector<vector<Square> > Grid;

// Returns true if the coordinates are outside the grid.
bool offside(const Grid& grid, int i, int j) {
  int n_rows = grid.size();
  int n_cols = grid[0].size();
  return i<0 || i>=n_rows || j<0 || j>=n_cols;
}

// Returns the list of neighbors' coordinates.
vector<pair<int, int> > neighbors(const Grid& grid, int i, int j)
{
  vector<pair<int, int> > res;
  for (int shift : {-1, 1}) {
    if (!offside(grid, i+shift, j)) {
      res.push_back({i+shift, j});
    }
    if (!offside(grid, i, j+shift)) {
      res.push_back({i, j+shift});
    }
  }
  return res;
}

// Returns false if the square is contained
// in a 2x2 block such that all other squares
// in that 2x2 block are black.
// It is assumed that square (i, j) is black.
bool in2x2block(const Grid& grid, int i, int j)
{
  for (int di : {-1, 1})
    for (int dj : {-1, 1})
      if (!offside(grid, i+di, j) && grid[i+di][j].color == Square::BLACK &&
          !offside(grid, i, j+dj) && grid[i][j+dj].color == Square::BLACK &&
          !offside(grid, i+di, j+dj) && grid[i+di][j+dj].color == Square::BLACK)
        return true;

  return false;
}

// Nurikabe rules forbid 2x2 block with only black squares.
// This function is called after painting a square black.
// It returns the list of squares which need to be painted white
// to ensure that the aforementionned rule is respected.
vector<pair<int, int> > whitelist(const Grid& grid, int i, int j)
{
  vector<pair<int, int> > candidates;

  for (int di : {-1, 1})
    for (int dj : {-1, 1})
      if (!offside(grid, i+di, j) && grid[i+di][j].color == Square::BLACK &&
          !offside(grid, i, j+dj) && grid[i][j+dj].color == Square::BLACK) {
        candidates.push_back({i+di, j+dj});
      }

  for (int di : {-1, 1})
    for (int dj : {-1, 1})
      if (!offside(grid, i, j+dj) && grid[i][j+dj].color == Square::BLACK &&
          !offside(grid, i+di, j+dj) && grid[i+di][j+dj].color == Square::BLACK) {
        candidates.push_back({i+di, j});
      }

  for (int di : {-1, 1})
    for (int dj : {-1, 1})
      if (!offside(grid, i+di, j) && grid[i+di][j].color == Square::BLACK &&
          !offside(grid, i+di, j+dj) && grid[i+di][j+dj].color == Square::BLACK) {
        candidates.push_back({i, j+dj});
      }

  // Only return unknown squares.
  vector<pair<int, int> > res;
  for (auto ij : candidates)
    if (grid[ij.first][ij.second].color == Square::UNKNOWN)
      res.push_back({ij.first, ij.second});

  return res;
}

// Find the root of a component.
pair<int, int> find_root(const Grid& grid, int i, int j)
{
  while (grid[i][j].parent != pair<int, int>{i, j}) {
    auto parent = grid[i][j].parent;
    i = parent.first;
    j = parent.second;
  }

  return {i, j};
}

// Merge components of the two positions.
// Check first the components of the two squares can be merged.
// * Check that they are of the same color.
// * If they are both white, check that at most one of them contains a number.
// * If they are both white, check that the total size of the two components
// is less than the number contained in one of the components, if any.
bool merge_component(Grid& grid, pair<int, int> pos1, pair<int, int> pos2)
{
  Square& s1 = grid[pos1.first][pos1.second];
  Square& s2 = grid[pos2.first][pos2.second];

  // Can't merge with unknown square.
  if (s1.color == Square::UNKNOWN || s2.color == Square::UNKNOWN)
    return false;

  // If the colors are different, they cannot be merged.
  if (s1.color != s2.color)
    return false;

  auto root1 = find_root(grid, pos1.first, pos1.second);
  auto root2 = find_root(grid, pos2.first, pos2.second);

  Square& rs1 = grid[root1.first][root1.second];
  Square& rs2 = grid[root2.first][root2.second];

  // It the two components have the same root,
  // then nothing to do.
  if (rs1.parent == rs2.parent)
    return true;

  // At most one component contains a number.
  // Otherwise they can't be merged.
  if (rs1.number > 0 && rs2.number > 0)
    return false;

  int number = rs1.number > 0? rs1.number : rs2.number;
  int new_size = rs1.component_size + rs2.component_size;

  // The component size can't exceed its number, if any.
  if (s1.color == Square::WHITE && number > 0 && new_size > number)
    return false;

  // The new parent is the largest component.
  auto new_parent = rs1.component_size > rs2.component_size? rs1.parent : rs2.parent;

  // Set new parent, new size and number.
  rs1.parent = new_parent;
  rs2.parent = new_parent;
  rs1.component_size = new_size;
  rs2.component_size = new_size;
  rs1.number = number;
  rs2.number = number;

  return true;
}

// Visit a component and collect escape squares.
void visit_and_collect(const Grid& grid, pair<int, int> pos, Square::Color color, set<pair<int, int> >& res) {
  for (auto elem : neighbors(grid, pos.first, pos.second)) {
    int i = elem.first, j = elem.second;
    if (grid[i][j].color == Square::UNKNOWN) {
      // Discard square if the component color is black
      // and if it would form a 2x2 black block.
      if (color == Square::WHITE || !in2x2block(grid, i, j))
        res.insert({i, j});
    }
  }

  // Set visited to avoid revisiting twice.
  grid[pos.first][pos.second].visited = true;
  
  // Visit neighbors.
  for (auto elem : neighbors(grid, pos.first, pos.second)) {
    if (!grid[elem.first][elem.second].visited
        && grid[elem.first][elem.second].color == color)
      visit_and_collect(grid, elem, color, res);
  }

  // Restore visited.
  grid[pos.first][pos.second].visited = false;
}

// Find all unknown squares which are candidates to expand a component.
// If the component is black, then squares in a black 2x2 block are not included.
set<pair<int, int> > escape_squares(const Grid& grid, pair<int, int> pos)
{
  set<pair<int, int> > res;
  visit_and_collect(grid, pos, grid[pos.first][pos.second].color, res);
  return res;
}

// Return the number of squares left to complete the component.
int component_delta(const Grid& grid, pair<int, int> pos)
{
  const Square& square = grid[pos.first][pos.second];

  auto root = find_root(grid, pos.first, pos.second);
  const Square& rs = grid[root.first][root.second];

  if (square.color == Square::BLACK)
    return n_black - rs.component_size;
  else if (square.color == Square::WHITE) {
    if (rs.number > 0)
      return rs.number - rs.component_size;
  }

  // -1 means that this number is unknown;
  return -1;
}

// Paint a square and go on with subsequent resulting moves.
int play(Grid& grid, pair<int, int> pos, Square::Color color)
{
  int n_resulting_moves = 0;

  queue<pair<int, int> > posq;
  queue<Square::Color> colq;
  posq.push(pos);
  colq.push(color);

  while(!posq.empty()) {

    pos = posq.front();
    posq.pop();
    color = colq.front();
    colq.pop();

    Square& square = grid[pos.first][pos.second];
    Square::Color opposite_color = color == Square::WHITE? Square::BLACK : Square::WHITE;

    // If square is already of the same color, nothing to do.
    if (square.color == color) {
      return 0;
    }

    // If square is not unknown, then the move is impossible.
    if (square.color != Square::UNKNOWN) {
      return -1;
    }

    // 2x2 black block rule.
    if (color == Square::BLACK && in2x2block(grid, pos.first, pos.second)) {
      return -1;
    }

    // Paint square.
    square.color = color;

    // Update counter of compulsory moves.
    n_resulting_moves++;

    for (auto elem : neighbors(grid, pos.first, pos.second)) {
      // If neighor has the same color, merge component.
      if (grid[elem.first][elem.second].color == color) {
        // If the two components can't be merged, return -1.
        if (!merge_component(grid, pos, elem))
          return -1;
      }

      else if (grid[elem.first][elem.second].color != Square::UNKNOWN) {
        // We use the following reasoning:
        // The new painted square is blocking the neighbor's component.
        // We need to compute escape squares for this component.
        // We also check if that component needs to grow.

        set<pair<int, int> > squares = escape_squares(grid, elem);

        // Compute by how many squares the component needs to grow.
        int delta = component_delta(grid, elem);

        // If we know this number and it is not null.
        if (delta > 0) {
          // If the component needs to grow but there is no escape squares.
          if (squares.size() == 0) {
            return -1;
          }

          // If there is exactly one escape square, paint this square.
          if (squares.size() == 1) {
            auto escape = squares.begin();

            // escape should be painted opposite color
            posq.push(*escape);
            colq.push(opposite_color);
          }
        }
      }

      // If we paint a square black and if this would cause
      // an unknown square to be in a 2x2 block if it were black
      // then it should be painted white.
      else if (color == Square::BLACK &&
          grid[elem.first][elem.second].color == Square::UNKNOWN) {

        if (in2x2block(grid, elem.first, elem.second)) {

          // this square should be painted white
          posq.push(elem);
          colq.push(opposite_color);

        }
      }
    }

    // Now that the components has grown by one square,
    // compute by how many squares the component needs to grow.
    int delta = component_delta(grid, pos);

    // Also compute escape squares.
    set<pair<int, int> > squares = escape_squares(grid, pos);

    if (delta > 0 && squares.size() == 0)
      return -1;

    if (delta > 0 && squares.size() == 1) {
      auto escape = squares.begin();

      // escape should be painted color
      posq.push(*escape);
      colq.push(color);

    }

    // If the component is complete, escape squares
    // should be painted with the opposite color.
    if (delta == 0) {
      for (auto elem : squares) {

        // elem should be painted opposite color
        posq.push(elem);
        colq.push(opposite_color);
      }
    }
  }

  return n_resulting_moves;
}

// Initialize grid from an initial state.
void initialize(Grid& grid, const vector<vector<int> >& initial_state)
{
  int n_rows = initial_state.size();
  int n_cols = initial_state[0].size();

  grid.clear();

  for (int i=0; i<n_rows; i++) {
    grid.push_back(vector<Square>());
    for (int j=0; j<n_cols; j++) {
      int number = initial_state[i][j];

      Square square(i, j);

      // If number is non negative, then it is either unknown (0)
      // or a number (therfore white).
      if (number >=0) {
        square.number = number;
        square.color = number == 0? Square::UNKNOWN : Square::WHITE;
      }

      // Otherwise, it is black (-2) or white (-1).
      else {
        square.number = 0;
        if (number == -1)
          square.color = Square::WHITE;
        if (number == -2)
          square.color = Square::BLACK;
      }

      grid[i].push_back(square);
    }
  }
}

// Parse input stream.
//
// Input has the following format:
// N M
// a(1,1) a(1,2) ... a(1,M)
// a(2,1) a(2,2) ... a(2,M)
// ...
// a(N,1) a(N,2) ... a(N,M)
//
// Each element of the matrix is either
// * -2 -> a black cell
// * -1 -> an empty white cell
// *  0 -> an unknown cell
// *  x -> a white cell with a positive number x.
vector<vector<int > > parse_input_stream(istream& stream)
{
  int n_rows, n_cols;
  stream >> n_rows >> n_cols;

  vector<vector<int> > initial_state;

  for (int i=0; i<n_rows; i++) {
    initial_state.push_back(vector<int>());
    for (int j=0; j<n_cols; j++) {
      int number;
      stream >> number;
      initial_state[i].push_back(number);
    }
  }

  return initial_state;
}

// Create grid from string
Grid create_grid(string input)
{
  stringstream input_stream(input);
  Grid grid;
  initialize(grid, parse_input_stream(input_stream));

  return grid;
}

void test_offside()
{
  string input = 
R"====(
3 3
0 -1 -2
2 0 0
0 0 0
)====";

  Grid grid = create_grid(input);

  assert(!offside(grid, 1, 1));
  assert(offside(grid, 1, 10));
}

// test neighbors
void test_neighbors()
{
  string input = 
R"====(
3 3
0 -1 -2
2 0 0
0 0 0
)====";

  string output =
R"====(
1 0
0 1
)====";

  Grid grid = create_grid(input);

  stringstream stream;
  stream << endl;
  for (auto elem : neighbors(grid, 0, 0)) {
    stream << elem.first << " " << elem.second << endl;
  }

  string my_output = stream.str();
  assert(my_output == output);
}

// test in2x2block
void test_in2x2block()
{
  string input = 
R"====(
3 3
0 -2 -1
2 -2 -2 
0 0 0
)====";

  Grid grid = create_grid(input);

  assert(!in2x2block(grid, 0, 0));
  assert(in2x2block(grid, 0, 2));
  assert(!in2x2block(grid, 2, 0));
}

// test whitelist
void test_whitelist()
{
  string input = 
R"====(
3 3
-2 -1 -2
-2 0 0
-2 0 0
)====";

  string output =
R"====(
2 1
)====";

  Grid grid = create_grid(input);

  stringstream stream;
  stream << endl;
  for (auto elem : whitelist(grid, 1, 1)) {
    stream << elem.first << " " << elem.second << endl;
  }

  string my_output = stream.str();
  assert(my_output == output);
}

// test components
void test_components()
{
  string input = 
R"====(
3 3
-2 -1 -1 
-2 0 4
-2 0 0
)====";

  string output =
R"====(
1 0
3
0
0
0 2
3
4
1
1 0
3
0
0
)====";

  Grid grid = create_grid(input);

  stringstream stream;
  stream << endl;

  int n_rows = grid.size();
  int n_cols = grid[0].size();

  for (int i=0; i<n_rows; i++) {
    for (int j=0; j<n_cols; j++) {
      for (auto elem : neighbors(grid, i, j)) {
        merge_component(grid, {i, j}, elem);
      }
    }
  }

  vector<pair<int, int> > positions;
  positions.push_back({0, 0});
  positions.push_back({0, 2});
  positions.push_back({2, 0});

  for (auto elem : positions) {
    auto root = find_root(grid, elem.first, elem.second);
    Square& square = grid[root.first][root.second];
    stream << root.first << " " << root.second << endl;
    stream << square.component_size << endl;
    stream << square.number << endl;
    stream << square.color << endl;
  }

  string my_output = stream.str();
  assert(my_output == output);
}

// test escape squares
void test_escape_squares()
{
  string input = 
R"====(
3 3
-2 -1 -1 
-2 0 4
-2 -2 0
)====";

  string output =
R"====(
2 2
1 1
2 2
)====";

  stringstream stream;
  stream << endl;

  Grid grid = create_grid(input);

  for (auto elem : escape_squares(grid, {1, 0}))
    stream << elem.first << " " << elem.second << endl;

  for (auto elem : escape_squares(grid, {0, 1}))
    stream << elem.first << " " << elem.second << endl;

  string my_output = stream.str();
  assert(my_output == output);
}

// test play
void test_play()
{
  string input = 
R"====(
3 3
-2 -1 -1 
-2 0 4
-2 -2 0
)====";

  string output =
R"====(
-1
2
)====";

  stringstream stream;
  stream << endl;

  vector<pair<int, int> > positions;
  positions.push_back({2, 2});
  positions.push_back({1, 1});

  for (auto move : positions) {
    Grid grid = create_grid(input);
    int n_rows = grid.size();
    int n_cols = grid[0].size();

    for (int i=0; i<n_rows; i++) {
      for (int j=0; j<n_cols; j++) {
        for (auto elem : neighbors(grid, i, j)) {
          merge_component(grid, {i, j}, elem);
        }
      }
    }

    stream << play(grid, move, Square::WHITE) << endl;
  }

  string my_output = stream.str();
  assert(my_output == output);
}

int main()
{
  test_offside();
  test_neighbors();
  test_whitelist();
  test_in2x2block();
  test_components();
  test_escape_squares();
  test_play();

  vector<vector<int> > initial_state = parse_input_stream(cin);

  Grid grid;
  initialize(grid, initial_state);

  return 0;
}

#if 0
// Returns true if there exists at least one solution.
bool solve(vector<vector<int, int> > grid)
{
  for (;;) {
    int unknown_square = 0;
    int best_score = 0;
    int best_i = 0;
    int best_j = 0;
    Square::Color best_color;
    typename Matrix<S>::MatrixIterator it = m.begin();
    bool restart = false;

    for (it = m.begin(); it != m.end(); ++it) {
      if (it->color == Square::UNKNOWN) {
        unknown_square++;

        Matrix<S> copy_m = m;
        int score = copy_m.play(it->i, it->j, Square::WHITE);
        // std::cout << score << std::endl;
        if (score == -1) {
          if (m.play(it->i, it->j, Square::BLACK) == -1) {
            // std::cout << "impossible" << std::endl;
            return false;
          }
          else {
            restart = true;
          }
        }

        else if (score > best_score) {
          best_score = score;
          best_i = it->i;
          best_j = it->j;
          best_color = Square::BLACK;
        }

        copy_m = m;
        score = copy_m.play(it->i, it->j, Square::BLACK);
        // std::cout << score << std::endl;
        if (score == -1) {
          if (m.play(it->i, it->j, Square::WHITE) == -1) {
            // std::cout << "impossible" << std::endl;
            return false;
          }
          else {
            restart = true;
          }
        }

        else if (score > best_score) {
          best_score = score;
          best_i = it->i;
          best_j = it->j;
          best_color = Square::WHITE;
        }
      }
    }

    if (unknown_square == 0) {
      m.dumpColor();
      return true;
    }

    if (!restart) {
      // std::cout << "try " << best_i << " " << best_j << " " << best_color << std::endl;
      Matrix<S> copy_m = m;
      copy_m.play(best_i, best_j, best_color == Square::WHITE? Square::BLACK : Square::WHITE);
      if (solve(copy_m)) {
        return true;
      }
      else {
        m.play(best_i, best_j, best_color);
      }
    }
  }
}
#endif
