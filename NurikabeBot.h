#ifndef NURIKABEBOT_H_
#define NURIKABEBOT_H_

#include "Matrix.h"

template <int S>
class NurikabeBot
{
  bool solve(Matrix<S>& m)
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

public:
  NurikabeBot() {}
  bool solve(int init_state [S][S])
  {
    Matrix<S> m(init_state);
    return solve(m);
  }
};

#endif
