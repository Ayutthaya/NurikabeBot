#include "nurikabe.h"

#include <sstream>
#include <cassert>

using namespace std;

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

void test_solve_1()
{
  string input =
R"====(
2 2
0 0
0 0
)====";

  Nurikabe nurikabe(input);
  assert(!nurikabe.solve());
}

// test solve
void test_solve_2()
{
  string input =
R"====(
3 3
0 0 0 
0 0 1
0 0 0
)====";

  Nurikabe nurikabe(input);
  assert(!nurikabe.solve());
}

void test_solve_3()
{
  string input =
R"====(
3 3
0 0 2
0 0 0
1 0 1
)====";

  Nurikabe nurikabe(input);
  //nurikabe.set_print_solution(true);
  assert(nurikabe.solve());
}

void test_solve_4()
{
  string input =
R"====(
4 4
0 0 2 0
0 0 0 0
1 0 1 0
0 0 0 0
)====";

  Nurikabe nurikabe(input);
  //nurikabe.set_print_solution(true);
  assert(nurikabe.solve());
}

void test_solve_5()
{
  string input =
R"====(
5 5
0 0 2 0 0
0 0 0 0 0
1 0 1 0 0
0 0 0 0 0
0 0 3 0 4
)====";

  Nurikabe nurikabe(input);
  //nurikabe.set_print_solution(true);
  assert(nurikabe.solve());
}

void test_solve_6()
{
  string input =
R"====(
10 10
0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 2 0 0 2
0 2 0 0 0 0 0 0 0 0
1 0 0 4 0 0 0 2 0 0
0 0 0 0 0 2 0 0 0 2
0 0 0 3 0 0 0 4 0 0
2 0 0 0 0 0 0 0 0 0
0 0 0 0 2 0 0 0 2 0
3 0 0 0 0 0 0 0 0 0
0 0 0 0 0 4 0 1 0 2
)====";

  Nurikabe nurikabe(input);
  //nurikabe.set_print_solution(true);
  assert(nurikabe.solve());
}

void test_solve_7()
{
  string input =
R"====(
20 20
1 0 2 0 1 0 11 0 2 0 4 0 0 0 1 0 3 0 2 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
1 0 0 2 0 0 0 0 0 5 0 0 0 3 0 0 0 0 0 0
0 2 0 0 0 0 0 0 0 0 0 0 3 0 0 0 0 1 0 2
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0
0 0 7 0 0 4 0 0 0 0 0 0 0 0 1 0 2 0 0 0
2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0
0 0 0 0 0 0 0 2 0 0 0 3 0 0 3 0 2 0 0 0
0 0 0 1 0 0 0 0 0 2 0 0 0 0 0 0 0 0 0 0
2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 0 0
0 0 1 0 3 0 0 0 5 0 0 2 0 0 0 2 0 0 0 1
1 0 0 1 0 0 0 2 0 0 0 0 0 0 0 0 0 0 0 0
0 1 0 0 2 0 0 0 0 0 0 0 3 0 0 0 0 0 2 0
0 0 1 0 0 0 0 0 0 0 1 0 0 0 0 3 0 0 0 0
1 0 0 4 0 0 3 0 0 2 0 3 0 0 2 0 0 1 0 1
0 2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 0 0
0 0 0 3 0 0 0 0 0 0 2 0 0 1 0 0 0 0 0 3
0 0 0 0 0 0 3 0 0 0 0 0 1 0 2 0 0 0 0 0
2 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 4 0 0 0
0 0 0 1 0 0 0 2 0 0 3 0 0 0 0 0 0 0 0 0
)====";

  Nurikabe nurikabe(input);
  //nurikabe.set_print_solution(true);
  assert(nurikabe.solve());
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
  test_solve_1();
  test_solve_2();
  test_solve_3();
  test_solve_4();
  test_solve_5();
  test_solve_6();
  test_solve_7();

  return 0;
}
