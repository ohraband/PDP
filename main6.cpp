#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <omp.h>
#include <chrono>
#include <iostream>



using namespace std;
int minTurns;
int lb;
int done = 0;

struct PointDistance {
    int startX, startY;
    int endX, endY;
    int distance;
    PointDistance(int startX, int startY, int endX, int endY, int distance)
            : startX(startX), startY(startY), endX(endX), endY(endY), distance(distance) {}
};


bool isValid(int x, int y, int m, int n) {
    return (x >= 0 && x < m && y >= 0 && y < n);
}
struct Rectangle{
    int x1,y1,x2,y2;
};


bool isInRect(Rectangle rect, int x, int y) {
    return x >= rect.x1 && x <= rect.x2 && y >= rect.y1 && y <= rect.y2;
}


int calculateLowerBound(const vector<pair<int, int>>& whiteKnights, const vector<pair<int, int>>& blackKnights, const vector<PointDistance>& pointDistances, const Rectangle& whiteRect, const Rectangle& blackRect) {
    int lowerBound = 0;
    for (int x = whiteRect.x1; x <= whiteRect.x2; ++x) {
        for (int y = whiteRect.y1; y <= whiteRect.y2; ++y) {
            int minDistanceToBlack = numeric_limits<int>::max();
            for (auto blackKnight : blackKnights) {
                for (auto pd : pointDistances) {
                    if ((pd.startX == x && pd.startY == y) &&
                        (pd.endX == blackKnight.first && pd.endY == blackKnight.second)) {
                        minDistanceToBlack = min(minDistanceToBlack, pd.distance);
                        break;
                    }
                }
            }
            lowerBound += minDistanceToBlack;
        }
    }


    for (int x = blackRect.x1; x <= blackRect.x2; ++x) {
        for (int y = blackRect.y1; y <= blackRect.y2; ++y) {
            int minDistanceToWhite = numeric_limits<int>::max();
            for (auto whiteKnight : whiteKnights) {
                for (auto pd : pointDistances) {
                    if ((pd.startX == x && pd.startY == y) &&
                        (pd.endX == whiteKnight.first && pd.endY == whiteKnight.second)) {
                        minDistanceToWhite = min(minDistanceToWhite, pd.distance);
                        break;
                    }
                }
            }
            lowerBound += minDistanceToWhite;
        }
    }
    return lowerBound;
}


int calculateUpperBound(const vector<pair<int, int>>& whiteKnights, const vector<pair<int, int>>& blackKnights, const vector<PointDistance>& pointDistances, const Rectangle& whiteRect, const Rectangle& blackRect) {
    int upperBound = 0;

    for (int x = whiteRect.x1; x <= whiteRect.x2; ++x) {
        for (int y = whiteRect.y1; y <= whiteRect.y2; ++y) {
            int maxDistanceToBlack = 0;
            for (auto blackKnight : blackKnights) {
                for (auto pd : pointDistances) {
                    if ((pd.startX == x && pd.startY == y) &&
                        (pd.endX == blackKnight.first && pd.endY == blackKnight.second)) {
                        maxDistanceToBlack = max(maxDistanceToBlack, pd.distance);
                        break;
                    }
                }
            }
            upperBound += maxDistanceToBlack;
        }
    }

    for (int x = blackRect.x1; x <= blackRect.x2; ++x) {
        for (int y = blackRect.y1; y <= blackRect.y2; ++y) {
            int maxDistanceToWhite = 0;
            for (auto whiteKnight : whiteKnights) {
                for (auto pd : pointDistances) {
                    if ((pd.startX == x && pd.startY == y) &&
                        (pd.endX == whiteKnight.first && pd.endY == whiteKnight.second)) {
                        maxDistanceToWhite = max(maxDistanceToWhite, pd.distance);
                        break;
                    }
                }
            }
            upperBound += maxDistanceToWhite;
        }
    }

    return upperBound;
}

void printPointDistances(const vector<PointDistance>& pointDistances) {
    cout << "Distances between each pair of points:" << endl;
    for (const auto& pd : pointDistances) {
        cout << "Start: (" << pd.startX << "," << pd.startY << "), "
             << "End: (" << pd.endX << "," << pd.endY << "), "
             << "Distance: " << pd.distance << endl;
    }
}


vector<vector<int>> shortestDistances(int m, int n, int a, int b) {
    vector<pair<int, int>> moves = {
            {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
            {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
    };

    vector<vector<int>> distances(m, vector<int>(n, -1));

    queue<pair<int, int>> q;
    q.push({a, b});
    distances[a][b] = 0;

    while (!q.empty()) {
        auto current = q.front();
        q.pop();

        for (auto move : moves) {
            int newX = current.first + move.first;
            int newY = current.second + move.second;

            if (isValid(newX, newY, m, n) && distances[newX][newY] == -1) {
                distances[newX][newY] = distances[current.first][current.second] + 1;
                q.push({newX, newY});
            }
        }
    }

    return distances;
}

struct GameState {
    vector<pair<int, int>> whiteKnights;
    vector<pair<int, int>> blackKnights;
};


bool isGoalState(const GameState& state, Rectangle black, Rectangle white) {
    for (const auto& whiteKnight : state.whiteKnights) {
        if (!isInRect(black, whiteKnight.first, whiteKnight.second)) {
            return false;
        }
    }

    for (const auto& blackKnight : state.blackKnights) {
        if (!isInRect(white, blackKnight.first, blackKnight.second)) {
            return false;
        }
    }

    return true;
}

bool isKnightAt(int x, int y, const vector<pair<int, int>>& knights) {
    for (const auto& knight : knights) {
        if (knight.first == x && knight.second == y) {
            return true;
        }
    }
    return false;
}

vector<GameState> pathsoultion;

vector<GameState> generateSuccessorStates(const GameState& state, int m, int n, const Rectangle& white, const Rectangle& black, int last_move) {
    vector<GameState> successors;

    vector<pair<int, int>> moves = {
            {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
            {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
    };

    int white_move = 0;

    for (const auto& whiteKnight : state.whiteKnights) {
        if (!isInRect(black, whiteKnight.first, whiteKnight.second) && last_move == 2) {
            white_move = 1;
            break;
        }
    }


    if(white_move) {
        for (const auto &whiteKnight: state.whiteKnights) {
            for (const auto &move: moves) {
                int newX = whiteKnight.first + move.first;
                int newY = whiteKnight.second + move.second;
                if (newX >= 0 && newX < m && newY >= 0 && newY < n && !isKnightAt(newX, newY, state.whiteKnights) &&
                    !isKnightAt(newX, newY, state.blackKnights)) {
                    GameState newState = state;
                    newState.whiteKnights.clear();
                    for (const auto &knight: state.whiteKnights) {
                        if (knight != whiteKnight) {
                            newState.whiteKnights.push_back(knight);
                        } else {
                            newState.whiteKnights.emplace_back(newX, newY);
                        }
                    }
                    successors.push_back(newState);
                }
            }
        }
    }


    int black_move = 0;

    for (const auto& blackKnight : state.blackKnights) {
        if (!isInRect(white, blackKnight.first, blackKnight.second) && last_move == 1){
            black_move = 1;
            break;
        }
    }


    if(black_move) {
        for (const auto &blackKnight: state.blackKnights) {
            for (const auto &move: moves) {
                int newX = blackKnight.first + move.first;
                int newY = blackKnight.second + move.second;
                if (newX >= 0 && newX < m && newY >= 0 && newY < n && !isKnightAt(newX, newY, state.whiteKnights) &&
                    !isKnightAt(newX, newY, state.blackKnights)) {
                    GameState newState = state;
                    newState.blackKnights.clear();
                    for (const auto &knight: state.blackKnights) {
                        if (knight != blackKnight) {
                            newState.blackKnights.push_back(knight);
                        } else {
                            newState.blackKnights.emplace_back(newX, newY);
                        }
                    }
                    successors.push_back(newState);
                }
            }
        }
    }

    return successors;
}
void dfsseq(const GameState& state, int depth, int m, int n, const Rectangle& black, const Rectangle& white, const vector<PointDistance>& pointDistances, vector<GameState> solution, int last_move) {
    solution.emplace_back(state);
    if ((depth >= minTurns) or done) {
        return;
    }

    if (isGoalState(state, black, white)) {
#pragma omp critical
        {
            if (depth < minTurns) {
                minTurns = depth;
                pathsoultion = solution;
            }
        }
        cout << "turns needed: " << minTurns << endl;
        if (minTurns == lb) {
            done = 1;
        }
        return;
    }

    vector<GameState> successors = generateSuccessorStates(state, m, n, white, black, last_move);
    depth = depth + 1;

    if (last_move == 1) {
        for (const auto &whiteKnight: state.whiteKnights) {
            if (!isInRect(black, whiteKnight.first, whiteKnight.second)) {
                last_move = 2;
                break;
            }
        }
    } else {
        for (const auto &blackKnight: state.blackKnights) {
            if (!isInRect(white, blackKnight.first, blackKnight.second)) {
                last_move = 1;
                break;
            }
        }
    }

    vector<pair<int, GameState>> lbSuccessors;
    for (const auto &successor: successors) {
        int lowerBound = calculateLowerBound(successor.whiteKnights, successor.blackKnights, pointDistances, white,
                                             black);
        if ((lowerBound + depth + 1) <= minTurns) {
            lbSuccessors.emplace_back(lowerBound, successor);
        }
    }

    sort(lbSuccessors.begin(), lbSuccessors.end(), [](const auto &a, const auto &b) {
        return a.first < b.first;
    });

    vector<GameState> sortedSuccessors;
    sortedSuccessors.reserve(lbSuccessors.size());
    for (const auto &lbSuccessor: lbSuccessors) {
        sortedSuccessors.push_back(lbSuccessor.second);
    }


    for (const auto &successor: sortedSuccessors) {
        const vector<GameState> &path = solution;
        dfsseq(successor, depth, m, n,  black, white,
            pointDistances, path, last_move);
    }
}


void dfs(const GameState& state, int depth, int m, int n, const Rectangle& black, const Rectangle& white, const vector<PointDistance>& pointDistances, vector<GameState> solution, int last_move) {
    solution.emplace_back(state);
    if ((depth >= minTurns) or done) {
        return;
    }

    if (isGoalState(state, black, white)) {
#pragma omp critical
        {
            if (depth < minTurns) {
                minTurns = depth;
                pathsoultion = solution;
            }
        }
        cout << "turns needed: " << minTurns << endl;
        if (minTurns == lb) {
            done = 1;
        }
        return;
    }

    vector<GameState> successors = generateSuccessorStates(state, m, n, white, black, last_move);
    depth = depth + 1;

    if (last_move == 1) {
        for (const auto &whiteKnight: state.whiteKnights) {
            if (!isInRect(black, whiteKnight.first, whiteKnight.second)) {
                last_move = 2;
                break;
            }
        }
    } else {
        for (const auto &blackKnight: state.blackKnights) {
            if (!isInRect(white, blackKnight.first, blackKnight.second)) {
                last_move = 1;
                break;
            }
        }
    }

    vector<pair<int, GameState>> lbSuccessors;
    for (const auto &successor: successors) {
        int lowerBound = calculateLowerBound(successor.whiteKnights, successor.blackKnights, pointDistances, white,
                                             black);
        if ((lowerBound + depth + 1) <= minTurns) {
            lbSuccessors.emplace_back(lowerBound, successor);
        }
    }

    sort(lbSuccessors.begin(), lbSuccessors.end(), [](const auto &a, const auto &b) {
        return a.first < b.first;
    });

    vector<GameState> sortedSuccessors;
    sortedSuccessors.reserve(lbSuccessors.size());
    for (const auto &lbSuccessor: lbSuccessors) {
        sortedSuccessors.push_back(lbSuccessor.second);
    }


            for (const auto &successor: sortedSuccessors) {
                const vector<GameState> &path = solution;
                if(depth < 2) {
                #pragma omp task
                    {
                        dfs(successor, depth, m, n, black, white,
                            pointDistances, path, last_move);
                    }
                }
                else{
                    dfsseq(successor, depth, m, n, black, white,
                        pointDistances, path, last_move);
                }
            }
            #pragma omp taskwait
}


int main() {
    omp_set_num_threads(16);
    ifstream inputFile("input.txt");
    if (!inputFile) {
        cerr << "Error opening input file." << endl;
        return 1;
    }

    int m, n, k1, k2;
    inputFile >> m >> n >> k1 >> k2;

    vector<pair<int, int>> whiteKnights;
    vector<pair<int, int>> blackKnights;


    int x1, y1, x2, y2;
    inputFile >> x1 >> y1 >> x2 >> y2;
    Rectangle white = {x1,y1,x2,y2};
    for (int x = x1; x <= x2; ++x) {
        for (int y = y1; y <= y2; ++y) {
            whiteKnights.emplace_back(x, y);
        }
    }


    inputFile >> x1 >> y1 >> x2 >> y2;
    Rectangle black = {x1,y1,x2,y2};
    for (int x = x1; x <= x2; ++x) {
        for (int y = y1; y <= y2; ++y) {
            blackKnights.emplace_back(x, y);
        }
    }



    vector<PointDistance> pointDistances;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            vector<vector<int>> distances = shortestDistances(m, n, i, j);
            for (int k = 0; k < m; ++k) {
                for (int l = 0; l < n; ++l) {
                    pointDistances.emplace_back(i, j, k, l, distances[k][l]);
                }
            }
        }
    }


    int lowerBound = calculateLowerBound(whiteKnights, blackKnights, pointDistances, white, black);
    cout << "Lower bound: " << lowerBound << endl;
    lb = calculateLowerBound(whiteKnights, blackKnights, pointDistances, white, black);

    int upperBound = calculateUpperBound(whiteKnights, blackKnights, pointDistances, white, black);
    cout << "Upper bound: " << upperBound << endl;

    minTurns = upperBound;

    GameState initialState = {whiteKnights, blackKnights};
    vector<GameState> solution;
    pair<int, vector<GameState>> result;
    const auto start{std::chrono::steady_clock::now()};
    # pragma omp parallel
    {
    #pragma omp single
       {
            dfs(initialState, 0, m, n, black, white, pointDistances, solution, 2);
      }
   }
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};

    cout << "Optimal number of turns: " << minTurns << endl;

    int step = 0;
    for (const auto& state : pathsoultion) {
        cout << "Step " << step << endl;
        cout << "  ";
        for (int j = 0; j < m; ++j) {
            cout << j << " ";
        }
        cout << endl;
        for (int i = 0; i < n; ++i) {
            cout << i << " ";
            for (int j = 0; j < m; ++j) {
                bool whiteKnightFound = false;
                bool blackKnightFound = false;
                for (const auto& whiteKnight : state.whiteKnights) {
                    if (whiteKnight.second == i && whiteKnight.first == j) {
                        cout << "W ";
                        whiteKnightFound = true;
                        break;
                    }
                }
                for (const auto& blackKnight : state.blackKnights) {
                    if (blackKnight.second == i && blackKnight.first == j) {
                        cout << "B ";
                        blackKnightFound = true;
                        break;
                    }
                }
                if (!whiteKnightFound && !blackKnightFound) {
                    cout << ". ";
                }
            }
            cout << endl;
        }
        cout << endl;
        step++;
    }

    cout << "Optimal number of turns: " << minTurns << endl;

    inputFile.close();

    std::cout << elapsed_seconds.count() << endl;

    return 0;
}