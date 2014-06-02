#ifndef SQUARE_H_
#define SQUARE_H_

class Square
{
public:
  enum Color {BLACK, WHITE, UNKNOWN};

  int N;              // size of the matrix
  int i;              // row
  int j;              // column
  Color color;        // color of square
  int component_size; // size of the component
  int component_nb;   // applicable for white squares only
  int p_i;            // row parent square in the component
  int p_j;            // column parent square in the component
  int known_neighbors;// number of known neighbours

  Square()
  : N(-1), i(-1), j(-1), color(UNKNOWN), component_size(-1),
    component_nb(-1), p_i(i), p_j(j), known_neighbors(0)
  {
  }

  Square(int N, int i, int j, Color color)
  : N(N), i(i), j(j), color(color), component_size(-1),
    component_nb(-1), p_i(i), p_j(j),  known_neighbors(0)
  {
  }

  bool operator==(const Square& sq) {
    return this->i == sq.i && this->j == sq.j;
  }

  void setNb(int nb)
  {
    component_nb = nb;
    component_size = 1;
    color = WHITE;
  }

  void setWhite()
  {
    color = WHITE;
    component_size = 1;
  }

  void setBlack()
  {
    color = BLACK;
    component_size = 1;
  }

  bool isRoot() const
  {
    return p_i == i && p_j == j;
  }
};

#endif
