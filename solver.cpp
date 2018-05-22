#include <cstdio>
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;

typedef unsigned long long GAME_STATE;          // flattened game state representation

const bool isPrintSolutionWithState = true;
const int8_t l = 8, l2 = 64;                    // limit of width and height of the puzzle, l2 = l^2
const int8_t dx[4] = {0, 1, 0, -1};
const int8_t dy[4] = {-1, 0, 1, 0};
const char MOVE[4] = {'U', 'R', 'D', 'L'};

short r, c, t;                                  // r = row, c = column, t = tmp int
int8_t i, j, k;                                 // indexes
int8_t rc;                                      // rc = number of squares = r * c
int8_t dd2[l][l];                               // minimum distance between any point to any final position
int8_t visited2d[l][l];                         // visited checking for minimum distance between any point to any final position
int8_t puzzle2d[l][l];                          // validate intermediate position
int8_t dd[l2];                                  // flattened minimum distance between any point to any final position

GAME_STATE state;
GAME_STATE endState;
GAME_STATE blockState;
GAME_STATE tempState;
GAME_STATE collideState[4];
GAME_STATE cantMoveState;
GAME_STATE newState;

struct Point {
  int8_t x, y;
  Point(int8_t a, int8_t b): x(a), y(b){};
};
vector<Point> startingPoints;
vector<Point> endingPoints;
vector<Point> blockPoints;

struct N {
  Point p;
  int8_t s;
  N(Point n, int8_t a): p(n), s(a){};
};

int8_t d;
inline int8_t h(const GAME_STATE &x) {
  d = 0;
  for (tempState = 1, j = 0; j < rc; tempState <<= 1, ++j) {
    if (tempState & x) {
      d = max(d, dd[j]);
    }
  }
  return d;
}

struct Node {
  Node *p;                                      // Pointer to previous node
  int8_t s;                                     // number of steps from start to current state
  int8_t mv;                                    // number representation for last movement, -1 for the first node
  int8_t fx;                                    // f(x)
  GAME_STATE state;                             // game state representation
  
  Node(Node *q, const int8_t &gx, const int8_t &m, const GAME_STATE &x) {
    p = q;
    s = gx;
    mv = m;
    fx = gx + h(x);
    state = x;
  }
};
struct Comparator {
  bool operator() (const Node *lhs, const Node *rhs) {
    if (lhs->fx != rhs->fx) {
      return lhs->fx > rhs->fx;
    }
    return lhs->s < rhs->s;
  }
};

inline GAME_STATE stateFromLocations(const vector<Point> &locations) {
  state = 0;
  for (i = 0; i < locations.size(); ++i) {
    state |= ((GAME_STATE)1 << (locations[i].y * c + locations[i].x));
  }
  return state;
}

void printState(GAME_STATE state, GAME_STATE blockState) {
  GAME_STATE ti, tj, tk;
  for (ti = 0, tk = 1; ti < rc; ti++, tk <<= 1) {
    if ((blockState & tk) > 0) {
      printf(" #");
    } else if ((state & tk) > 0) {
      printf(" @");
    } else {
      printf(" _");
    }
    if (ti % (GAME_STATE)c == (GAME_STATE)c - 1) {
      printf("\n");
    }
  }
}

void printSolution(Node *node) {
  if (node->mv == -1) {
    return;
  }
  printSolution(node->p);

  if (isPrintSolutionWithState) {
    printf("\n%c\n", MOVE[node->mv]);
    printState(node->state, blockState);
  } else {
    printf("%c ", MOVE[node->mv]);
  }
}

inline GAME_STATE stateFrom(GAME_STATE &currentState, int8_t &mv) {
  tempState = currentState & collideState[mv];            // '1' 's that can't move because of wall
  newState = currentState ^ tempState;                    // '1' 's that can 'move'
  if (mv == 0) {
    newState >>= c;
  } else if (mv == 1) {
    newState <<= 1;
  } else if (mv == 2) {
    newState <<= c;
  } else {
    newState >>= 1;
  }

  cantMoveState = (newState & (blockState | tempState));  // '1' 's that can't move because of '1's next to wall or in block
  newState ^= cantMoveState;                              // new state is now ignore '1' that can't move
  while (cantMoveState) {
    if (mv == 0) {
      cantMoveState <<= c;
    } else if (mv == 1) {
      cantMoveState >>= 1;
    } else if (mv == 2) {
      cantMoveState >>= c;
    } else {
      cantMoveState <<= 1;
    }
    tempState = (newState & cantMoveState);
    newState |= cantMoveState;
    cantMoveState = tempState;                            // '1' 's that can't move because of '1'
  }

  return newState | (currentState & collideState[i]);
}

inline void initCollideState() {
  memset(collideState, 0, sizeof collideState);
  for (i = 0; i < r; ++i) {      //  0: 'U', 1: 'R', 2: 'D', 3: 'L'
    for (j = 0; j < c; ++j) {
      if (i == 0) {
        collideState[0] |= ((unsigned long long)1 << (i * c + j));
      }
      if (j == 0) {
        collideState[3] |= ((unsigned long long)1 << (i * c + j));
      }
      if (i == r - 1) {
        collideState[2] |= ((unsigned long long)1 << (i * c + j));
      }
      if (j == c - 1) {
        collideState[1] |= ((unsigned long long)1 << (i * c + j));
      }
    }
  }
}

inline void findMinimumDistanceFromAnyPointToAnyFinalPoint() {
  memset(dd2, 0x7f, sizeof dd2);
  for (i = 0; i < endingPoints.size(); ++i) {
    memset(visited2d, 0, sizeof visited2d);
    queue<N> q;
    q.push(N(endingPoints[i], 0));
    visited2d[endingPoints[i].y][endingPoints[i].x] = 1;
    while (!q.empty()) {
      N tn = q.front();
      q.pop();
      dd2[tn.p.y][tn.p.x] = min(dd2[tn.p.y][tn.p.x], tn.s);
      for (j = 0; j < 4; j++) {
        Point np = tn.p;
        np.x += dx[j];
        np.y += dy[j];

        if (
          np.x >= 0 && np.x < c && np.y >= 0 && np.y < r &&
          puzzle2d[np.y][np.x] != -1 && !visited2d[np.y][np.x]
        ) {
          visited2d[np.y][np.x] = 1;
          q.push(N(np, tn.s + 1));
        }
      }
    }
  }

  // flattened
  for (i = 0; i < r; ++i) {
    for (j = 0; j < c; ++j) {
      dd[r * i + j] = dd2[i][j];
    }
  }
}

priority_queue<Node *, vector<Node *>, Comparator> pq;
Node *node;
unordered_map<GAME_STATE, int8_t> visited;
bool aStar() {
  pq.push(new Node(NULL, 0, -1, stateFromLocations(startingPoints)));
  visited[pq.top()->state] = 0;

  while (!pq.empty()) {
    node = pq.top();
    pq.pop();

    if (node->state == endState) {
      printf("number of steps: %d\n", node->s);
      printSolution(node);
      return true;
    }
    if (visited[node->state] < node->s) {
      delete node;
      continue;
    }

    k = node->s + 1;
    for (i = 0; i < 4; i++) {
      newState = stateFrom(node->state, i);

      // check if visited, if not visited, push to priority queue
      if (visited.find(newState) == visited.end() || visited[newState] > k) {
        visited[newState] = k;
        pq.push(new Node(node, k, i, newState));
      }
    }
  }

  return false;
}

int main() {
  scanf("%hd %hd", &r, &c);
  rc = r * c;

  for (i = 0; i < r; ++i) {
    for (j = 0; j < c; ++j) {
      scanf("%hd", &t);

      if (t == 1 || t == 3) {
        startingPoints.push_back(Point(j, i));
        puzzle2d[i][j] = 0;
      }
      if (t == 2 || t == 3) {
        endingPoints.push_back(Point(j, i));
        puzzle2d[i][j] = 2;
      }
      if (t == -1) {
        blockPoints.push_back(Point(j, i));
        puzzle2d[i][j] = -1;
      }
      if (t == 0) {
        puzzle2d[i][j] = 0;
      }
    }
  }

  if (isPrintSolutionWithState) {
    printState(stateFromLocations(startingPoints), blockState);
  }

  endState = stateFromLocations(endingPoints);
  blockState = stateFromLocations(blockPoints);
  initCollideState();
  findMinimumDistanceFromAnyPointToAnyFinalPoint();
  
  if (aStar()) {
    printf("\ngame end\n");
  } else {
    printf("no possible move.\n");
  }

  return 0;
}
