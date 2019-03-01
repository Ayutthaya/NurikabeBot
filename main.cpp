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
// Nurikabe object
// Cell -> Cell
// int i, int j -> {i, j}
// typedef Position
// Nurikabe::cell(Position pos)
// cell -> get_cell
// square -> cell

// main.cpp -> nurikabe.cpp nurikabe.h
// public / private
// main loop
// README

class Nurikabe
{

  public:

  typedef pair<int, int> Position;

  struct Cell
  {
    enum Color {BLACK, WHITE, UNKNOWN};

    Cell(Position pos) : parent(pos) {}

    Position parent;
    Color color = Cell::UNKNOWN;
    int number = 0;
    int component_size = 1;
    mutable bool visited = false;
  };

  typedef vector<vector<Cell> > Grid;

  // TODO: compute n_black
  Grid grid;
  int n_black;
  int n_rows;
  int n_cols;

  Cell& get_cell(Position pos)
  {
    return grid[pos.first][pos.second];
  }

  const Cell& get_cell(Position pos) const
  {
    return grid[pos.first][pos.second];
  }

  int get_n_rows() const
  {
    return n_rows;
  }

  int get_n_cols() const
  {
    return n_cols;
  }

  Nurikabe(string input)
  {
    // Create grid from string
    stringstream input_stream(input);
    initialize(parse_input_stream(input_stream));
  }


  // Returns true if the coordinates are outside the grid.
  bool offside(Position pos) const {
    int i = pos.first, j = pos.second;
    return i<0 || i>=n_rows || j<0 || j>=n_cols;
  }

  // Returns the list of neighbors' coordinates.
  vector<Position> neighbors(Position pos) const
  {
    vector<Position> res;
    int i = pos.first, j = pos.second;
    for (int shift : {-1, 1}) {
      if (!offside({i+shift, j})) {
        res.push_back({i+shift, j});
      }
      if (!offside({i, j+shift})) {
        res.push_back({i, j+shift});
      }
    }
    return res;
  }

  // Returns false if the cell is contained
  // in a 2x2 block such that all other cells
  // in that 2x2 block are black.
  // It is assumed that cell (i, j) is black.
  bool in2x2block(Position pos) const
  {
    int i = pos.first, j = pos.second;
    for (int di : {-1, 1})
      for (int dj : {-1, 1})
        if (!offside({i+di, j}) && grid[i+di][j].color == Cell::BLACK &&
            !offside({i, j+dj}) && grid[i][j+dj].color == Cell::BLACK &&
            !offside({i+di, j+dj}) && grid[i+di][j+dj].color == Cell::BLACK)
          return true;

    return false;
  }

  // Nurikabe rules forbid 2x2 block with only black cells.
  // This function is called after painting a cell black.
  // It returns the list of cells which need to be painted white
  // to ensure that the aforementionned rule is respected.
  vector<Position> whitelist(Position pos) const
  {
    int i = pos.first, j = pos.second;
    vector<Position> candidates;

    for (int di : {-1, 1})
      for (int dj : {-1, 1})
        if (!offside({i+di, j}) && grid[i+di][j].color == Cell::BLACK &&
            !offside({i, j+dj}) && grid[i][j+dj].color == Cell::BLACK) {
          candidates.push_back({i+di, j+dj});
        }

    for (int di : {-1, 1})
      for (int dj : {-1, 1})
        if (!offside({i, j+dj}) && grid[i][j+dj].color == Cell::BLACK &&
            !offside({i+di, j+dj}) && grid[i+di][j+dj].color == Cell::BLACK) {
          candidates.push_back({i+di, j});
        }

    for (int di : {-1, 1})
      for (int dj : {-1, 1})
        if (!offside({i+di, j}) && grid[i+di][j].color == Cell::BLACK &&
            !offside({i+di, j+dj}) && grid[i+di][j+dj].color == Cell::BLACK) {
          candidates.push_back({i, j+dj});
        }

    // Only return unknown cells.
    vector<Position> res;
    for (auto ij : candidates)
      if (get_cell(ij).color == Cell::UNKNOWN)
        res.push_back(ij);

    return res;
  }

  // Find the root of a component.
  Position find_root(Position pos) const
  {
    while (get_cell(pos).parent != pos) {
      auto parent = get_cell(pos).parent;
      pos = parent;
    }

    return pos;
  }

  // Merge components of the two positions.
  // Check first the components of the two cells can be merged.
  // * Check that they are of the same color.
  // * If they are both white, check that at most one of them contains a number.
  // * If they are both white, check that the total size of the two components
  // is less than the number contained in one of the components, if any.
  bool merge_component(Position pos1, Position pos2)
  {
    Cell& s1 = get_cell(pos1);
    Cell& s2 = get_cell(pos2);

    // Can't merge with unknown cell.
    if (s1.color == Cell::UNKNOWN || s2.color == Cell::UNKNOWN)
      return false;

    // If the colors are different, they cannot be merged.
    if (s1.color != s2.color)
      return false;

    auto root1 = find_root(pos1);
    auto root2 = find_root(pos2);

    Cell& rs1 = get_cell(root1);
    Cell& rs2 = get_cell(root2);

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
    if (s1.color == Cell::WHITE && number > 0 && new_size > number)
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

  // Visit a component and collect escape cells.
  void visit_and_collect(Position pos, Cell::Color color, set<Position>& res) const
  {
    for (auto elem : neighbors(pos)) {
      if (get_cell(elem).color == Cell::UNKNOWN) {
        // Discard cell if the component color is black
        // and if it would form a 2x2 black block.
        if (color == Cell::WHITE || !in2x2block(elem))
          res.insert(elem);
      }
    }

    // Set visited to avoid revisiting twice.
    get_cell(pos).visited = true;

    // Visit neighbors.
    for (auto elem : neighbors(pos)) {
      if (!get_cell(elem).visited
          && get_cell(elem).color == color)
        visit_and_collect(elem, color, res);
    }

    // Restore visited.
    get_cell(pos).visited = false;
  }

  // Find all unknown cells which are candidates to expand a component.
  // If the component is black, then cells in a black 2x2 block are not included.
  set<Position> escape_cells(Position pos) const
  {
    set<Position> res;
    visit_and_collect(pos, get_cell(pos).color, res);
    return res;
  }

  // Return the number of cells left to complete the component.
  int component_delta(Position pos) const
  {
    const Cell& cell = get_cell(pos);

    auto root = find_root(pos);
    const Cell& rs = get_cell(root);

    if (cell.color == Cell::BLACK)
      return n_black - rs.component_size;
    else if (cell.color == Cell::WHITE) {
      if (rs.number > 0)
        return rs.number - rs.component_size;
    }

    // -1 means that this number is unknown;
    return -1;
  }

  // Paint a cell and go on with subsequent resulting moves.
  int play(Position pos, Cell::Color color)
  {
    int n_resulting_moves = 0;

    queue<Position> posq;
    queue<Cell::Color> colq;
    posq.push(pos);
    colq.push(color);

    while(!posq.empty()) {

      pos = posq.front();
      posq.pop();
      color = colq.front();
      colq.pop();

      Cell& cell = get_cell(pos);
      Cell::Color opposite_color = color == Cell::WHITE? Cell::BLACK : Cell::WHITE;

      // If cell is already of the same color, nothing to do.
      if (cell.color == color) {
        continue;
      }

      // If cell is not unknown, then the move is impossible.
      if (cell.color != Cell::UNKNOWN) {
        return -1;
      }

      // 2x2 black block rule.
      if (color == Cell::BLACK && in2x2block(pos)) {
        return -1;
      }

      // Paint cell.
      cell.color = color;

      // Update counter of compulsory moves.
      n_resulting_moves++;

      for (auto elem : neighbors(pos)) {
        // If neighor has the same color, merge component.
        if (grid[elem.first][elem.second].color == color) {
          // If the two components can't be merged, return -1.
          if (!merge_component(pos, elem))
            return -1;
        }

        else if (get_cell(elem).color != Cell::UNKNOWN) {
          // We use the following reasoning:
          // The new painted cell is blocking the neighbor's component.
          // We need to compute escape cells for this component.
          // We also check if that component needs to grow.

          set<Position> cells = escape_cells(elem);

          // Compute by how many cells the component needs to grow.
          int delta = component_delta(elem);

          // If we know this number and it is not null.
          if (delta > 0) {
            // If the component needs to grow but there is no escape cells.
            if (cells.size() == 0) {
              return -1;
            }

            // If there is exactly one escape cell, paint this cell.
            if (cells.size() == 1) {
              auto escape = cells.begin();

              // escape should be painted opposite color
              posq.push(*escape);
              colq.push(opposite_color);
            }
          }
        }

        // If we paint a cell black and if this would cause
        // an unknown cell to be in a 2x2 block if it were black
        // then it should be painted white.
        else if (color == Cell::BLACK &&
            get_cell(elem).color == Cell::UNKNOWN) {

          if (in2x2block(elem)) {

            // this cell should be painted white
            posq.push(elem);
            colq.push(opposite_color);

          }
        }
      }

      // Now that the components has grown by one cell,
      // compute by how many cells the component needs to grow.
      int delta = component_delta(pos);

      // Also compute escape cells.
      set<Position> cells = escape_cells(pos);

      if (delta > 0 && cells.size() == 0)
        return -1;

      if (delta > 0 && cells.size() == 1) {
        auto escape = cells.begin();

        // escape should be painted color
        posq.push(*escape);
        colq.push(color);

      }

      // If the component is complete, escape cells
      // should be painted with the opposite color.
      if (delta == 0) {
        for (auto elem : cells) {

          // elem should be painted opposite color
          posq.push(elem);
          colq.push(opposite_color);
        }
      }
    }

    return n_resulting_moves;
  }

  // Initialize grid from an initial state.
  void initialize(const vector<vector<int> >& initial_state)
  {
    n_rows = initial_state.size();
    n_cols = initial_state[0].size();

    grid.clear();

    // sum of numbers in the grid
    // equal to the final number of white cells
    int sum = 0;

    for (int i=0; i<n_rows; i++) {
      grid.push_back(vector<Cell>());
      for (int j=0; j<n_cols; j++) {
        int number = initial_state[i][j];

        Cell cell({i, j});

        // If number is non negative, then it is either unknown (0)
        // or a number (therfore white).
        if (number >=0) {
          cell.number = number;
          cell.color = number == 0? Cell::UNKNOWN : Cell::WHITE;
          sum += number;
        }

        // Otherwise, it is black (-2) or white (-1).
        else {
          cell.number = 0;
          if (number == -1)
            cell.color = Cell::WHITE;
          if (number == -2)
            cell.color = Cell::BLACK;
        }

        grid[i].push_back(cell);
      }
    }

    // number of black cells
    n_black = n_rows * n_cols - sum;
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
  static vector<vector<int > > parse_input_stream(istream& stream)
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
};

void test_offside()
{
  string input = 
R"====(
3 3
0 -1 -2
2 0 0
0 0 0
)====";

  Nurikabe nurikabe(input);

  assert(!nurikabe.offside({1, 1}));
  assert(nurikabe.offside({1, 10}));
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

  Nurikabe nurikabe(input);

  stringstream stream;
  stream << endl;
  for (auto elem : nurikabe.neighbors({0, 0})) {
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

  Nurikabe nurikabe(input);

  assert(!nurikabe.in2x2block({0, 0}));
  assert(nurikabe.in2x2block({0, 2}));
  assert(!nurikabe.in2x2block({2, 0}));
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

  Nurikabe nurikabe(input);

  stringstream stream;
  stream << endl;
  for (auto elem : nurikabe.whitelist({1, 1})) {
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

  Nurikabe nurikabe(input);

  stringstream stream;
  stream << endl;

  int n_rows = nurikabe.get_n_rows();
  int n_cols = nurikabe.get_n_cols();

  for (int i=0; i<n_rows; i++) {
    for (int j=0; j<n_cols; j++) {
      for (auto elem : nurikabe.neighbors({i, j})) {
        nurikabe.merge_component({i, j}, elem);
      }
    }
  }

  vector<Nurikabe::Position> positions;
  positions.push_back({0, 0});
  positions.push_back({0, 2});
  positions.push_back({2, 0});

  for (auto elem : positions) {
    auto root = nurikabe.find_root(elem);
    Nurikabe::Cell& cell = nurikabe.get_cell(root);
    stream << root.first << " " << root.second << endl;
    stream << cell.component_size << endl;
    stream << cell.number << endl;
    stream << cell.color << endl;
  }

  string my_output = stream.str();
  assert(my_output == output);
}

// test escape cells
void test_escape_cells()
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

  Nurikabe nurikabe(input);

  for (auto elem : nurikabe.escape_cells({1, 0}))
    stream << elem.first << " " << elem.second << endl;

  for (auto elem : nurikabe.escape_cells({0, 1}))
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

  vector<Nurikabe::Position> positions;
  positions.push_back({2, 2});
  positions.push_back({1, 1});

  for (auto move : positions) {
    Nurikabe nurikabe(input);
    int n_rows = nurikabe.get_n_rows();
    int n_cols = nurikabe.get_n_cols();

    for (int i=0; i<n_rows; i++) {
      for (int j=0; j<n_cols; j++) {
        for (auto elem : nurikabe.neighbors({i, j})) {
          nurikabe.merge_component({i, j}, elem);
        }
      }
    }

    stream << nurikabe.play(move, Nurikabe::Cell::WHITE) << endl;
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
  test_escape_cells();
  test_play();

  //vector<vector<int> > initial_state = parse_input_stream(cin);

  //Grid grid;
  //initialize(grid, initial_state);

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
    Cell::Color best_color;
    typename Matrix<S>::MatrixIterator it = m.begin();
    bool restart = false;

    for (it = m.begin(); it != m.end(); ++it) {
      if (it->color == Cell::UNKNOWN) {
        unknown_square++;

        Matrix<S> copy_m = m;
        int score = copy_m.play(it->i, it->j, Cell::WHITE);
        // std::cout << score << std::endl;
        if (score == -1) {
          if (m.play(it->i, it->j, Cell::BLACK) == -1) {
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
          best_color = Cell::BLACK;
        }

        copy_m = m;
        score = copy_m.play(it->i, it->j, Cell::BLACK);
        // std::cout << score << std::endl;
        if (score == -1) {
          if (m.play(it->i, it->j, Cell::WHITE) == -1) {
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
          best_color = Cell::WHITE;
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
      copy_m.play(best_i, best_j, best_color == Cell::WHITE? Cell::BLACK : Cell::WHITE);
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
