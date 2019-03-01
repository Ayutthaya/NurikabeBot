#ifndef NURIKABE_H
#define NURIKABE_H

#include <iostream>
#include <vector>
#include <set>

class Nurikabe
{
public:
  typedef std::pair<int, int> Position;

  // Simple structure representing a cell of the grid.
  struct Cell
  {
    enum Color {BLACK, WHITE, UNKNOWN};

    Cell(Position pos)
      : parent(pos)
    {
    }

    Position parent;
    Color color = Cell::UNKNOWN;
    int number = 0;
    int component_size = 1;
    mutable bool visited = false;
  };

  typedef std::vector<std::vector<Cell> > Grid;

  // Constructor with a string describing a grid.
  Nurikabe(std::string input);

  // Get cell at a given position.
  Cell& get_cell(Position pos);
  const Cell& get_cell(Position pos) const;

  // Get number of rows / columns.
  int get_n_rows() const;
  int get_n_cols() const;

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
  static std::vector<std::vector<int > > parse_input_stream(std::istream& stream);

  // Initialize grid from an initial state.
  void initialize(const std::vector<std::vector<int> >& initial_state);

  // Returns true if the coordinates are outside the grid.
  bool offside(Position pos) const;

  // Returns the list of neighbors' coordinates.
  std::vector<Position> neighbors(Position pos) const;

  // Returns false if the cell is contained
  // in a 2x2 block such that all other cells
  // in that 2x2 block are black.
  // It is assumed that cell (i, j) is black.
  bool in2x2block(Position pos) const;

  // Nurikabe rules forbid 2x2 block with only black cells.
  // This function is called after painting a cell black.
  // It returns the list of cells which need to be painted white
  // to ensure that the aforementionned rule is respected.
  std::vector<Position> whitelist(Position pos) const;

  // Find the root of a component.
  Position find_root(Position pos) const;

  // Merge components of the two positions.
  // Check first the components of the two cells can be merged.
  // * Check that they are of the same color.
  // * If they are both white, check that at most one of them contains a number.
  // * If they are both white, check that the total size of the two components
  // is less than the number contained in one of the components, if any.
  bool merge_component(Position pos1, Position pos2);

  // Visit a component and collect escape cells.
  void visit_and_collect(Position pos, Cell::Color color, std::set<Position>& res) const;

  // Find all unknown cells which are candidates to expand a component.
  // If the component is black, then cells in a black 2x2 block are not included.
  std::set<Position> escape_cells(Position pos) const;

  // Return the number of cells left to complete the component.
  int component_delta(Position pos) const;

  // Paint a cell and go on with subsequent resulting moves.
  int play(Position pos, Cell::Color color);

private:
  Grid grid;
  int n_black;
  int n_rows;
  int n_cols;
};

#endif
