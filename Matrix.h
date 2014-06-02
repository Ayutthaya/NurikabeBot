#ifndef MATRIX_H_
#define MATRIX_H_

#include <iostream>
#include <list>
#include "Square.h"

template <int S>
class Matrix
{
public:
  Matrix(const int init_state [S][S]);

  int play(int i, int j, Square::Color color);

  void dumpComponent()
  {
    for (int i=0; i<S; i++) {
      for (int j=0; j<S; j++) {
        std::cout << getComponentSize(i, j) << "\t";
      }
      std::cout << std::endl;
    }
  }

  void dumpColor()
  {
    for (int i=0; i<S; i++) {
      for (int j=0; j<S; j++) {
        std::cout << matrix[i][j].color << "\t";
      }
      std::cout << std::endl;
    }
  }

  void dumpKnownNeighbors()
  {
    for (int i=0; i<S; i++) {
      for (int j=0; j<S; j++) {
        std::cout << matrix[i][j].known_neighbors << "\t";
      }
      std::cout << std::endl;
    }
  }

  // needs to be defined inline because of Scott Meyers' item 46 !
  // otherwise we could have defined the function with template 
  // parameters.
  friend std::ostream& operator<<(std::ostream& os, const Matrix& m)
  {
    for (int i=0; i<S; i++) {
      for (int j=0; j<S; j++) {
        int nb = m.matrix[i][j].component_nb;
        os << (nb > 0 ? nb : 0) << "\t";
      }
      os << std::endl;
    }
    return os;
  }

  struct MatrixIterator
  {
    int i; // row
    int j; // column
    Matrix<S> *m;

    MatrixIterator(int i, int j, Matrix<S> *m) : i(i), j(j), m(m) {}

    MatrixIterator& operator++()
    {
      if (j < S-1) {
        j++;
        return *this;
      }
      else if (i < S-1) {
        i++;
        j=0;
        return *this;
      }
      else {
        i = j = -1;
        return *this;
      }
    }

    bool operator==(const MatrixIterator& it)
    {
      return this->i == it.i && this->j == it.j;
    }

    bool operator!=(const MatrixIterator& it)
    {
      return ! (*this == it);
    }

    Square& operator*()
    {
      return m->matrix[i][j];
    }

    Square* operator->()
    {
      return &m->matrix[i][j];
    }
  };

  MatrixIterator begin()
  {
    return MatrixIterator(0, 0, this);
  }

  MatrixIterator end()
  {
    return MatrixIterator(-1, -1, this);
  }

private:
  struct NeighborIterator
  {
    int count; // 0:E, 1:N, 2:W, 3:S, 4:end
    int i;
    int j;
    Matrix<S> *m;

    NeighborIterator(int i, int j, Matrix<S> *m) : i(i), j(j), m(m)
    {
      count = -1;
      ++(*this); // point to next valid neighbor;
    }

    NeighborIterator& operator++()
    {
      count++;
      switch (count)
      {
      case 0: // E
        if (j+1 >= S) {
          return ++(*this);
        }
        break;

      case 1: // N
        if (i-1 < 0) {
          return ++(*this);
        }
        break;

      case 2: // W
        if (j-1 < 0) {
          return ++(*this);
        }
        break;

      case 3: // S
        if (i+1 >= S) {
          return ++(*this);
        }
        break;

      case 4: // end
        break;
      }

      return *this;
    }

    bool operator==(const NeighborIterator& it)
    {
      return this->count == it.count;
    }

    bool operator!=(const NeighborIterator& it)
    {
      return !(*this == it);
    }

    int getNeighI()
    {
      switch(count)
      {
      case 1:
        return i-1;
      case 3:
        return i+1;
      default:
        return i;
      }
    }

    int getNeighJ()
    {
      switch(count)
      {
      case 0:
        return j+1;
      case 2:
        return j-1;
      default:
        return j;
      }
    }

    Square& operator*()
    {
      return m->matrix[getNeighI()][getNeighJ()];
    }

    Square* operator->()
    {
      return &m->matrix[getNeighI()][getNeighJ()];
    }
  };

  template <int I, int J>
  class In2x2Square
  {
  public:
    In2x2Square() {}
    bool operator()(const int& i, const int& j, const Matrix& m)
    {
      int k = i + I;
      int l = j + J;
      if (k<S && l<S && k>=0 && l>=0 &&
          i<S && j<S && i>=0 && j>=0 &&
          m.matrix[i][l].color == Square::BLACK &&
          m.matrix[k][j].color == Square::BLACK &&
          m.matrix[k][l].color == Square::BLACK) {
        return true;
      }
      return false;
    }
  };

  template <int I, int J> 
  friend class In2x2Square;

  template <int I, int J>
  class Neigh2x2Square
  {
  public:
    Neigh2x2Square() {}
    std::pair<int, int> operator()(const int& i, const int& j, const Matrix& m)
    {
      return In2x2Square<-I, -J>()(i+I, j+J, m) ? std::pair<int, int>(i+I, j+J) : std::pair<int, int>(-1, -1);
    }
  };
  
  class Or2x2
  {
  public:
    typedef bool result_type;
    result_type operator()(const bool& a, const bool& b, const bool& c, const bool& d)
    {
      return a || b || c || d;
    }
  };
  
  class List2x2
  {
  public:
    typedef std::list<std::pair<int, int> > result_type;
    result_type operator()(const std::pair<int, int>& a,
                           const std::pair<int, int>& b,
                           const std::pair<int, int>& c,
                           const std::pair<int, int>& d)
    {
      std::list<std::pair<int, int> > result;
      if (a.first != -1) result.push_back(a);
      if (b.first != -1) result.push_back(b);
      if (c.first != -1) result.push_back(c);
      if (d.first != -1) result.push_back(d);
      return result;
    }
  };
  
  template <template <int, int> class a2x2SquareChecker, class Result>
  class Check2x2Square
  {
  public:
    Check2x2Square() {}
    typename Result::result_type operator()(const int& i, const int& j, const Matrix& m)
    {
      return Result()(a2x2SquareChecker<1, 1>()(i, j, m),
                      a2x2SquareChecker<1, -1>()(i, j, m),
                      a2x2SquareChecker<-1, 1>()(i, j, m),
                      a2x2SquareChecker<-1, -1>()(i, j, m));
    }
  };

  struct Event
  {
    int i;
    int j;
    Square::Color color;
    Event(int i, int j, Square::Color color) : i(i), j(j), color(color) {}
  };

  class EventManager
  {
    std::list<Event> event_list;
    bool black [S][S];
    bool white [S][S];
    bool failure;
    int count;

  public:
    EventManager()
    : count(0), failure(false)
    {
      for (int i=0; i<S; i++) {
        for (int j=0; j<S; j++) {
          black[i][j] = white[i][j] = false;
        }
      }
    }

    void push(const Event& ev)
    {
      // std::cout << "push " << ev.i << ev.j << ev.color << std::endl;
      if (ev.color == Square::BLACK) {
        if (black[ev.i][ev.j]) {
          return;
        }
        else if (white[ev.i][ev.j]) {
          failure = true;
          return;
        }
        else {
          event_list.push_back(ev);
          black[ev.i][ev.j] = true;
          count++;
        }
      }

      else {
        if (white[ev.i][ev.j]) {
          return;
        }
        else if (black[ev.i][ev.j]) {
          failure = true;
          return;
        }
        else {
          event_list.push_back(ev);
          white[ev.i][ev.j] = true;
          count++;
        }
      }
    }

    Event pop()
    {
      Event ev = event_list.front();
      event_list.pop_front();
      return ev;
    }

    bool empty()
    {
      return event_list.empty();
    }

    bool has_failed()
    {
      return failure;
    }

    int get_count()
    {
      return count;
    }
  };

  NeighborIterator begin(int i, int j)
  {
    return NeighborIterator(i, j, this);
  }

  NeighborIterator end(int i, int j)
  {
    NeighborIterator it(i, j, this);
    it.count = 4;
    return it;
  }

  Square& findRoot(Square& sq)
  {
    Square *p = &sq;

    while (!(p->isRoot())) {
      p = &matrix[p->p_i][p->p_j];
    }

    return *p;
  }

  int getComponentSize(int i, int j)
  {
    Square &root = findRoot(matrix[i][j]);
    return root.component_size;
  }

  int getComponentNb(int i, int j)
  {
    Square &root = findRoot(matrix[i][j]);
    return root.component_nb;
  }

  bool mergeComponent(Square& a, Square& b);

  bool in2x2Square(int i, int j);

  void closing(Square& sq, int processed_book[S][S], int closing_book[S][S],
               Square::Color color);

  std::list<std::pair<int, int> > white_closing(Square &sq);

  std::list<std::pair<int, int> > white_closing(int i, int j)
  {
    return white_closing(matrix[i][j]);
  }

  std::list<std::pair<int, int> > black_closing(Square &sq);

  std::list<std::pair<int, int> > black_closing(int i, int j)
  {
    return black_closing(matrix[i][j]);
  }

  bool playWhite(int i, int j, EventManager &em);

  bool playBlack(int i, int j, EventManager &em);

  Square matrix [S][S];
  int target_size;
};

template <int S>
Matrix<S>::Matrix(const int init_state [S][S])
{
  target_size = S*S;

  // init numbers

  for (MatrixIterator it = this->begin(); it != this->end(); ++it) {
    *it = Square(S, it.i, it.j, Square::UNKNOWN);
    if (init_state[it.i][it.j] > 0) {
      it->setNb(init_state[it.i][it.j]);
      target_size -= init_state[it.i][it.j];
    }
  }

  // init neighbors

  for (MatrixIterator it = this->begin(); it != this->end(); ++it) {
    if (init_state[it.i][it.j] > 0) {
      for (NeighborIterator itn = this->begin(it.i, it.j); 
           itn != this->end(it.i, it.j); ++itn) {
        itn->known_neighbors++;
      }
    }
  }
}

template <int S>
bool Matrix<S>::mergeComponent(Square& a, Square& b)
{
  // std::cout << "trying to merge (" << a.i << ", " << a.j << ") with (" 
  //       << b.i << ", " << b.j << ") " << a.color << " " << b.color 
  //       << std::endl;

  Square& ra = findRoot(a);  // root of a
  Square& rb = findRoot(b);  // root of b

  if (ra == rb) {

    // already in the same component

    // std::cout << "already in the same component" << std::endl;
    return true;
  }

  if (ra.color != rb.color) {

    // not the same color

    //std::cout << "not the same color" << std::endl;
    return false;
  }

  // this and sq belongs to different components

  if (ra.component_nb > 0 && rb.component_nb > 0) {

    // both components already have a number

    // std::cout << "both components already have a number" << std::endl;
    return false;
  }

  // size of the new component

  int new_size = ra.component_size + rb.component_size;

  if ((ra.component_nb > 0 && ra.component_nb < new_size)
   || (rb.component_nb > 0 && rb.component_nb < new_size)) {

    // size of component is bigger than number

    // std::cout << "size of component is bigger than number" << std::endl;
    return false;
  }

  // merge components

  if (ra.component_size > rb.component_size) {
    ra.p_i = rb.i;
    ra.p_j = rb.j;
    rb.component_size = new_size;
    if (ra.component_nb > 0) {
      rb.component_nb = ra.component_nb;
    }
  }

  else {
    rb.p_i = ra.i;
    rb.p_j = ra.j;
    ra.component_size = new_size;
    if (rb.component_nb > 0) {
      ra.component_nb = rb.component_nb;
    }
  }

  // std::cout << "success" << std::endl;

  return true;
}

template <int S>
bool Matrix<S>::in2x2Square(int i, int j)
{
  int array [2][4] = {{-1, -1, 1, 1}, {-1, 1, -1, 1}};
  for (int a=0; a<4; a++) {
    int k = i + array[0][a];
    int l = j + array[1][a];
    if (k<S && l<S && k>=0 && l>=0 &&
        matrix[i][l].color == Square::BLACK &&
        matrix[k][j].color == Square::BLACK &&
        matrix[k][l].color == Square::BLACK) {
      return true;
    }
  }
  return false;
}

template <int S>
void Matrix<S>::closing(Square& sq, int processed_book[S][S], 
                     int closing_book[S][S], Square::Color color) {

  for (NeighborIterator it = begin(sq.i, sq.j); it != end(sq.i, sq.j); ++it) {
    if (it->color == Square::UNKNOWN) {
      closing_book[it->i][it->j] = 1;
    }
  }

  processed_book[sq.i][sq.j] = 1;

  for (NeighborIterator it = begin(sq.i, sq.j); it != end(sq.i, sq.j); ++it) {
    if (it->color == color && processed_book[it->i][it->j] == 0) {
      closing(*it, processed_book, closing_book, color);
    }
  }
}

template <int S>
std::list<std::pair<int, int> > Matrix<S>::white_closing(Square &sq)
{
  int processed_book [S][S];
  int closing_book [S][S];
  std::list<std::pair<int, int> > l;

  if (sq.color != Square::WHITE) {
    return l;
  }

  // init books

  for (int i=0; i<S; i++) {
    for (int j=0; j<S; j++) {
      processed_book[i][j] = 0;
      closing_book[i][j] = 0;
    }
  }

  // compute closing

  closing(sq, processed_book, closing_book, Square::WHITE);

  // gather closing squares

  for (int i=0; i<S; i++) {
    for (int j=0; j<S; j++) {
      if (closing_book[i][j]>0) {
        l.push_back(std::pair<int, int>(i, j));
      }
    }
  }

  return l;
}

template <int S>
std::list<std::pair<int, int> > Matrix<S>::black_closing(Square &sq)
{
  int processed_book [S][S];
  int closing_book [S][S];
  std::list<std::pair<int, int> > l;

  if (sq.color != Square::BLACK) {
    // std::cout << "black closing from non black square" << std::endl;
    return l;
  }

  // init books

  for (int i=0; i<S; i++) {
    for (int j=0; j<S; j++) {
      processed_book[i][j] = 0;
      closing_book[i][j] = 0;
    }
  }

  // compute closing

  closing(sq, processed_book, closing_book, Square::BLACK);

  // gather closing squares

  for (int i=0; i<S; i++) {
    for (int j=0; j<S; j++) {
      if (closing_book[i][j]>0 && !in2x2Square(i, j)) {
        l.push_back(std::pair<int, int>(i, j));
      }
    }
  }

  return l;
}

template <int S>
int Matrix<S>::play(int i, int j, Square::Color color) {
   // std::cout << "play " << i << " " << j << " " << color << std::endl;
   // dumpColor();

  EventManager em;
  em.push(Event(i, j, color));

  while(!em.empty()) {
    Event ev = em.pop();
    bool success;

    if (ev.color == Square::BLACK) {
      success = playBlack(ev.i, ev.j, em);
    }

    else {
      success = playWhite(ev.i, ev.j, em);
    }

    if (!success || em.has_failed()) {
      // std::cout << "event has failed " << ev.i << ev.j << ev.color << std::endl;
      return -1;
    }
  }

  return em.get_count();
}

template <int S>
bool Matrix<S>::playWhite(int i, int j, EventManager& em)
{
  Square& sq = matrix[i][j];

  if (sq.color == Square::WHITE) {
    return true;
  }

  if (sq.color != Square::UNKNOWN) {
    // std::cout << "already known" << std::endl;
    return false;
  }

  sq.color = Square::WHITE;
  sq.component_size = 1;

  for (NeighborIterator it = begin(i, j); it != end(i, j); ++it) {
    it->known_neighbors++;

    if (it->color == Square::BLACK) {
      std::list<std::pair<int, int> > cl = black_closing(*it);
      if (cl.size() == 0 && getComponentSize(it->i, it->j) != target_size) {
        return false;
      }
      else if (cl.size() == 1) {
        em.push(Event(cl.front().first, cl.front().second, Square::BLACK));
      }
    }

    else if (it->color == Square::WHITE) {
      if (!mergeComponent(sq, *it)) {
        // std::cout << "merge has failed" << std::endl;
        return false;
      }
    }
  }

  int delta = getComponentNb(i, j) - getComponentSize(i, j);
  std::list<std::pair<int, int> > cl = white_closing(i, j);

  if (delta != 0 && cl.size() == 0) {
    return false;
  }

  if (delta > 0 && cl.size() == 1) {
    em.push(Event(cl.front().first, cl.front().second, Square::WHITE));
  }

  if (delta == 0) {
    for (std::list<std::pair<int, int> >::iterator it = cl.begin(); it != cl.end(); it++) {
      em.push(Event(it->first, it->second, Square::BLACK));
    }
  }

  return true;
}

template <int S>
bool Matrix<S>::playBlack(int i, int j, EventManager& em)
{
  Square& sq = matrix[i][j];

  if (sq.color == Square::BLACK) {
    return true;
  }

  if (sq.color != Square::UNKNOWN || in2x2Square(i, j)) {
    return false;
  }

  sq.color = Square::BLACK;
  sq.component_size = 1;

  for (NeighborIterator it = begin(i, j); it != end(i, j); ++it) {
    it->known_neighbors++;

    if (it->color == Square::WHITE) {
      int delta = getComponentNb(it->i, it->j) - getComponentSize(it->i, it->j);
      std::list<std::pair<int, int> > cl = white_closing(*it);
      if (delta == 0 && cl.size() > 0) {
        for (std::list<std::pair<int, int> >::iterator it = cl.begin(); it != cl.end(); it++) {
          em.push(Event(it->first, it->second, Square::BLACK));
        }
      }

      if (delta != 0 && cl.size() == 0) {
        // std::cout << "delta != 0 && cl.size() == 0" << std::endl;
        return false;
      }

      if (delta > 0 && cl.size() == 1) {
        em.push(Event(cl.front().first, cl.front().second, Square::WHITE));
      }
    }

    else if (it->color == Square::BLACK) {
      if (!mergeComponent(sq, *it)) {
        // std::cout << "!mergeComponent" << std::endl;
        return false;
      }
    }

    else if (in2x2Square(it->i, it->j)) {
      em.push(Event(it->i, it->j, Square::WHITE));
    }
  }

  std::list<std::pair<int, int> > neigh2x2 = 
          Check2x2Square<Neigh2x2Square, List2x2>()(i, j, *this);
  for ( std::list<std::pair<int, int> >::iterator it = neigh2x2.begin();
        it != neigh2x2.end(); it++) {
    if (matrix[it->first][it->second].color == Square::UNKNOWN) {
      em.push(Event(it->first, it->second, Square::WHITE));
    }
  }

  std::list<std::pair<int, int> > cl = black_closing(i, j);
  int component_size = getComponentSize(i, j);

  if (component_size > target_size) {
    // std::cout << "component_size > target_size" << std::endl;
    return false;
  }
  if (cl.size() == 0 && component_size != target_size) {
    // std::cout << "cl.size() == 0 && component_size != target_size" << std::endl;
    return false;
  }

  else if (cl.size() == 1 && component_size < target_size) {
    em.push(Event(cl.front().first, cl.front().second, Square::BLACK));
  }

  return true;
}

#endif
