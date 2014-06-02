#include "NurikabeBot.h"

using namespace std;

int main()
{

  int init_state [20][20] =
{{1, 0, 2, 0, 1, 0, 11, 0, 2, 0, 4, 0, 0, 0, 1, 0, 3, 0, 2, 0, }, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, {1, 0, 0, 2, 0, 0, 0, 0, 0, 5, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, }, {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 0, 2, }, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, }, {0, 0, 7, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, }, {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, }, {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 3, 0, 2, 0, 0, 0, }, {0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, }, {0, 0, 1, 0, 3, 0, 0, 0, 5, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, }, {1, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }, {0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 2, 0, }, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 0, 0, 0, 0, }, {1, 0, 0, 4, 0, 0, 3, 0, 0, 2, 0, 3, 0, 0, 2, 0, 0, 1, 0, 1, }, {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, }, {0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 3, }, {0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, }, {2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, }, {0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, }};

  NurikabeBot<20> nb;
  nb.solve(init_state);

#if 0
  int init_state [10][10] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
                             {0, 0, 0, 0, 0, 0, 2, 0, 0, 2}, 
                             {0, 2, 0, 0, 0, 0, 0, 0, 0, 0}, 
                             {1, 0, 0, 4, 0, 0, 0, 2, 0, 0},
                             {0, 0, 0, 0, 0, 2, 0, 0, 0, 2},
                             {0, 0, 0, 3, 0, 0, 0, 4, 0, 0},
                             {2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                             {0, 0, 0, 0, 2, 0, 0, 0, 2, 0},
                             {3, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                             {0, 0, 0, 0, 0, 4, 0, 1, 0, 2}};
  NurikabeBot<10> nb(init_state);
  nb.solve();

  int init_state [5][5] = {{0, 0, 2, 0, 0}, {0, 0, 0, 0, 0}, {1, 0, 1, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 3, 0, 4}};
  NurikabeBot<5> nb;
  nb.solve(init_state);

  int init_state [4][4] = {{0, 0, 2, 0}, {0, 0, 0, 0}, {1, 0, 1, 0}, {0, 0, 0, 0}};
  NurikabeBot<4> nb;
  nb.solve();

  int init_state [3][3] = {{0, 0, 2}, {0, 0, 0}, {1, 0, 1}};
  NurikabeBot<3> nb;
  nb.solve(init_state);
#endif

  return 0;
}