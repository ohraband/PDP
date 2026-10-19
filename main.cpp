#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>

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

vector<GameState> generateSuccessorStates(const GameState& state, int m, int n) {
    vector<GameState> successors;

    vector<pair<int, int>> moves = {
            {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
            {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
    };

    for (const auto& whiteKnight : state.whiteKnights) {
        for (const auto& move : moves) {
            int newX = whiteKnight.first + move.first;
            int newY = whiteKnight.second + move.second;
            if (newX >= 0 && newX < m && newY >= 0 && newY < n && !isKnightAt(newX, newY, state.whiteKnights) && !isKnightAt(newX, newY, state.blackKnights)) {
                GameState newState = state;
                newState.whiteKnights.clear();
                for (const auto& knight : state.whiteKnights) {
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

    for (const auto& blackKnight : state.blackKnights) {
        for (const auto& move : moves) {
            int newX = blackKnight.first + move.first;
            int newY = blackKnight.second + move.second;
            if (newX >= 0 && newX < m && newY >= 0 && newY < n && !isKnightAt(newX, newY, state.whiteKnights) && !isKnightAt(newX, newY, state.blackKnights)) {
                GameState newState = state;
                newState.blackKnights.clear();
                for (const auto& knight : state.blackKnights) {
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
    return successors;
}


pair<int, vector<GameState>> dfs(const GameState& state, int depth, int m, int n, int x1, int y1, int x2, int y2, const Rectangle& black, const Rectangle& white, const vector<PointDistance>& pointDistances, vector<GameState> solution) {
    solution.emplace_back(state);
    vector<GameState> pathback;
    if ((depth >= minTurns) or done) {
        return make_pair(999999999, solution);
    }

    if (isGoalState(state, black, white)) {
        minTurns = depth;
        cout << "turns needed: " <<  minTurns << endl;
        if(minTurns == lb){
            done = 1;
        }
        return make_pair(depth, solution);
    }


    vector<GameState> successors = generateSuccessorStates(state, m, n);
    vector<pair<int, GameState>> lbSuccessors;
    for (const auto& successor : successors) {
        int lowerBound = calculateLowerBound(successor.whiteKnights, successor.blackKnights, pointDistances, white, black);
        if ((lowerBound + depth + 2) <= minTurns) {
            lbSuccessors.emplace_back(lowerBound, successor);
        }
    }

    sort(lbSuccessors.begin(), lbSuccessors.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    vector<GameState> sortedSuccessors;
    sortedSuccessors.reserve(lbSuccessors.size());
    for (const auto& lbSuccessor : lbSuccessors) {
        sortedSuccessors.push_back(lbSuccessor.second);
    }

    int result = 999999999;
    for (const auto& successor : sortedSuccessors) {
        const vector<GameState>& path = solution;
        pair<int, vector<GameState>> res = dfs(successor, depth + 1, m, n, x1, y1, x2, y2, black, white, pointDistances, path);
        if (res.first < result) {
            result = res.first;
            pathback = res.second;
        }
    }
    return make_pair(result, pathback);
}

int main() {
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
    pair<int, vector<GameState>> result = dfs(initialState, 0, m, n, x1, y1, x2, y2, black, white, pointDistances, solution);

    cout << "Optimal number of turns: " << result.first << endl;

    int step = 0;
    for (const auto& i: result.second){
        cout << "Step " << step << " " << endl;
        for (auto o: i.whiteKnights) {
            std::cout << o.first << o.second << " ";
        }
        cout << endl;
        for (auto j: i.blackKnights) {
            std::cout << j.first << j.second << " ";
        }
        cout << endl;
        step +=1;
    }

    inputFile.close();
    return 0;
}
