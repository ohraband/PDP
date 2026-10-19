#include "mpi.h"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <omp.h>
#include <chrono>
#include <iostream>

#define KNIGHTS_SIZE 50
#define SOLUTION_SIZE 100

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
    GameState(vector<pair<int, int>> vector1, vector<pair<int, int>> vector2) :
            whiteKnights(std::move(vector1)), blackKnights(std::move(vector2)) {}
    vector<pair<int, int>> whiteKnights;
    vector<pair<int, int>> blackKnights;
};

struct DfsState {
    DfsState(GameState state, int depth, int m, int n, const vector<struct GameState>& solution, int last_move)
            : gameState(std::move(state)), depth(depth), m(m), n(n), solution(solution), last_move(last_move) {}
    struct GameState gameState;
    int depth;
    int m;
    int n;
    vector<GameState> solution;
    int last_move;
};

struct GameStateStatic {
    pair<int, int> whiteKnights[KNIGHTS_SIZE];
    pair<int, int> blackKnights[KNIGHTS_SIZE];
    int whiteKnightsSize;
    int blackKnightsSize;
};

struct DfsStateStatic {
    struct GameStateStatic gameState;
    int depth;
    int m;
    int n;
    GameStateStatic solution[SOLUTION_SIZE];
    int solutionSize;
    int last_move;
    int bestRouteLength = 9999999;
};

// Conversion functions

GameStateStatic GameStateConvertToStatic(const GameState& gameState) {
    GameStateStatic staticGameState;
    staticGameState.whiteKnightsSize = gameState.whiteKnights.size();
    staticGameState.blackKnightsSize = gameState.blackKnights.size();
    for (int i = 0; i < KNIGHTS_SIZE; ++i) {
        if (i < staticGameState.whiteKnightsSize) {
            staticGameState.whiteKnights[i] = gameState.whiteKnights[i];
        } else {
            staticGameState.whiteKnights[i] = std::make_pair(0, 0); // Default initialization
        }
        if (i < staticGameState.blackKnightsSize) {
            staticGameState.blackKnights[i] = gameState.blackKnights[i];
        } else {
            staticGameState.blackKnights[i] = std::make_pair(0, 0); // Default initialization
        }
    }
    return staticGameState;
}

GameState GameStateConvertFromStatic(const GameStateStatic& staticGameState) {
    std::vector<std::pair<int, int>> whiteKnights;
    std::vector<std::pair<int, int>> blackKnights;
    whiteKnights.reserve(staticGameState.whiteKnightsSize);
    for (int i = 0; i < staticGameState.whiteKnightsSize; ++i) {
            whiteKnights.push_back(staticGameState.whiteKnights[i]);
        }

    blackKnights.reserve(staticGameState.blackKnightsSize);
    for (int i = 0; i < staticGameState.blackKnightsSize; ++i) {
            blackKnights.push_back(staticGameState.blackKnights[i]);
        }

    return {whiteKnights, blackKnights};
}


// Conversion functions

DfsStateStatic ConvertToStatic(const DfsState& dfsState) {
    DfsStateStatic staticDfsState;
    staticDfsState.gameState = GameStateConvertToStatic(dfsState.gameState);
    staticDfsState.depth = dfsState.depth;
    staticDfsState.m = dfsState.m;
    staticDfsState.n = dfsState.n;
    staticDfsState.solutionSize = dfsState.solution.size();
    for (int i = 0; i < dfsState.solution.size(); ++i) {
        staticDfsState.solution[i] = GameStateConvertToStatic(dfsState.solution[i]);
    }
    staticDfsState.last_move = dfsState.last_move;
    return staticDfsState;
}

DfsState ConvertFromStatic(const DfsStateStatic& staticDfsState) {
    DfsState dfsState(
            GameStateConvertFromStatic(staticDfsState.gameState),
            staticDfsState.depth,
            staticDfsState.m,
            staticDfsState.n,
            std::vector<GameState>(),
            staticDfsState.last_move
    );

    for (int i = 0; i < staticDfsState.solutionSize; ++i) {
        if (staticDfsState.solution[i].whiteKnightsSize > 0 || staticDfsState.solution[i].blackKnightsSize > 0) {
            dfsState.solution.push_back(GameStateConvertFromStatic(staticDfsState.solution[i]));
        }
    }
    return dfsState;
}

vector<DfsState> dfs_states_vector;


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
void bfsseq(const GameState& state, int depth, int m, int n, const Rectangle& black, const Rectangle& white, const vector<PointDistance>& pointDistances, vector<GameState> solution, int last_move) {
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
        dfs_states_vector.emplace_back(successor, depth, m, n, path, last_move);
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
                    dfs(successor, depth, m, n, black, white,
                        pointDistances, path, last_move);
            }
}


int main(int argc, char *argv[]) {
    omp_set_num_threads(4);
    ifstream inputFile("input.txt");
    if (!inputFile) {
        cerr << "Error opening input file." << endl;
        return 1;
    }

    int my_rank;
    int p;

    /* start up MPI */
    MPI_Init( &argc, &argv );

    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    cout << "MPI says rank and number of processes is: " << my_rank << " " << p << endl;


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

    //generating states with bfs
    if(my_rank == 0) {
        dfs_states_vector.emplace_back(initialState, 0, m, n, solution, 2);
        while (dfs_states_vector.size() < 1000) {
            DfsState current = dfs_states_vector[0];
            dfs_states_vector.erase(dfs_states_vector.begin());
            bfsseq(current.gameState, current.depth, current.m, current.n, black, white, pointDistances,
                   current.solution, current.last_move);
        }
    }

    cout << "States generated: " << dfs_states_vector.size() << endl;

    struct AnswerReturn{
        int bestRouteLength{};
        GameStateStatic solution[SOLUTION_SIZE];
    };



    //MPI structs
    const int nitems1 = 4; // Number of items in the struct
    int blocklengths1[4] = {KNIGHTS_SIZE , KNIGHTS_SIZE , 1, 1}; // Number of elements in each block
    MPI_Datatype types1[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT}; // Datatype of each block
    MPI_Datatype gameStateStaticType;
    MPI_Aint offsets1[4];

    offsets1[0] = offsetof(GameStateStatic, whiteKnights); // Offset of whiteKnights
    offsets1[1] = offsetof(GameStateStatic, blackKnights); // Offset of blackKnights
    offsets1[2] = offsetof(GameStateStatic, whiteKnightsSize); // Offset of whiteKnightsSize
    offsets1[3] = offsetof(GameStateStatic, blackKnightsSize); // Offset of blackKnightsSize

    MPI_Type_create_struct(nitems1, blocklengths1, offsets1, types1, &gameStateStaticType);
    MPI_Type_commit(&gameStateStaticType);

    const int nitems = 8; // Number of items in the struct
    int blocklengths[8] = {1, 1, 1, 1, SOLUTION_SIZE, 1, 1, 1}; // Number of elements in each block
    MPI_Datatype types[8] = {gameStateStaticType, MPI_INT, MPI_INT, MPI_INT, gameStateStaticType, MPI_INT, MPI_INT, MPI_INT}; // Datatype of each block
    MPI_Datatype dfsStateStaticType;
    MPI_Aint offsets[8];

    offsets[0] = offsetof(DfsStateStatic, gameState); // Offset of gameState
    offsets[1] = offsetof(DfsStateStatic, depth); // Offset of depth
    offsets[2] = offsetof(DfsStateStatic, m); // Offset of m
    offsets[3] = offsetof(DfsStateStatic, n); // Offset of n
    offsets[4] = offsetof(DfsStateStatic, solution); // Offset of solution
    offsets[5] = offsetof(DfsStateStatic, solutionSize); // Offset of solutionSize
    offsets[6] = offsetof(DfsStateStatic, last_move); // Offset of lastmove
    offsets[7] = offsetof(DfsStateStatic, bestRouteLength); // Offset of bestRouteLength

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &dfsStateStaticType);
    MPI_Type_commit(&dfsStateStaticType);



    const int nitems2 = 2; // Number of items in the struct
    int blocklengths2[2] = {1, SOLUTION_SIZE}; // Number of elements in each block
    MPI_Datatype types2[2] = {MPI_INT, gameStateStaticType}; // Datatype of each block
    MPI_Datatype answerReturnType;
    MPI_Aint offsets2[2];

    offsets2[0] = offsetof(AnswerReturn, bestRouteLength); // Offset of bestRouteLength
    offsets2[1] = offsetof(AnswerReturn, solution); // Offset of solution

    MPI_Type_create_struct(nitems2, blocklengths2, offsets2, types2, &answerReturnType);
    MPI_Type_commit(&answerReturnType);

    //master
    if (my_rank == 0) {
        DfsState first_one = dfs_states_vector[0];
        MPI_Status status;
        int free_slaves = p-1;
        bool free_slave_status[p];
        for (int i = 0; i < p; ++i) {
            free_slave_status[i] = true;
        }
        int i = 0;
        int responses_received = 0;

        while(i < dfs_states_vector.size()) {

            for(int j = 1; j < p; j++) {
                if(free_slave_status[j]){
                    DfsStateStatic dfsStateStatic = ConvertToStatic(dfs_states_vector[i]);
                    i++;
                    dfsStateStatic.bestRouteLength = minTurns;
                    cout << "Sending work to rank: " << j << endl;
                    MPI_Send(&dfsStateStatic, 1, dfsStateStaticType, j, 1, MPI_COMM_WORLD);
                    cout << "Sent work to rank: " << j << endl;
                    free_slaves = free_slaves - 1;
                    free_slave_status[j] = false;
                }
            }

            if(free_slaves <= 0) {
                AnswerReturn answerReturn{};
                cout << "Waiting for response from a slave: " << endl;
                MPI_Recv(&answerReturn, 1, answerReturnType, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
                cout << "Received response from rank: " << status.MPI_SOURCE << endl;
                responses_received++;
                int source_rank = status.MPI_SOURCE;
                free_slave_status[source_rank] = true;
                if (minTurns > answerReturn.bestRouteLength) {
#pragma omp critical
                    if (minTurns > answerReturn.bestRouteLength) {
                        minTurns = answerReturn.bestRouteLength;
                        pathsoultion.clear();
                        for (int j = 0; j < answerReturn.bestRouteLength+1; j++) {
                            pathsoultion.push_back(GameStateConvertFromStatic(answerReturn.solution[j]));
                        }
                    }
                }
                free_slaves = free_slaves + 1;
            }
        }

        while(responses_received < dfs_states_vector.size()) {
            AnswerReturn answerReturn{};
            cout << "Waiting for response from a slave: " << endl;
            MPI_Recv(&answerReturn, 1, answerReturnType, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
            cout << "Received response from rank: " << status.MPI_SOURCE << endl;
            responses_received++;
            int source_rank = status.MPI_SOURCE;
            free_slave_status[source_rank] = true;
            if (minTurns > answerReturn.bestRouteLength) {
#pragma omp critical
                if (minTurns > answerReturn.bestRouteLength) {
                    minTurns = answerReturn.bestRouteLength;
                    pathsoultion.clear();
                    for (int j = 0; j < answerReturn.bestRouteLength+1; j++) {
                        pathsoultion.push_back(GameStateConvertFromStatic(answerReturn.solution[j]));
                    }
                }
            }
            free_slaves = free_slaves + 1;
        }

        //ukonci
        DfsStateStatic dfsStateStaticnew = ConvertToStatic(first_one);
        for (int j = 1; j < p; j++) {
            cout << "Sending end signal to rank: " << j << endl;
            MPI_Send(&dfsStateStaticnew, 1, dfsStateStaticType, j, 2, MPI_COMM_WORLD);
            cout << "Sent end signal to rank: " << j << endl;
        }


    }

    //slave
    else{
        DfsStateStatic dfsStateStatic{};
        MPI_Status status;

        while(true) {
            MPI_Recv(&dfsStateStatic, 1, dfsStateStaticType, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            cout << "Slave: " << my_rank << " received work" << endl;
            // Check the tag
            if (status.MPI_TAG == 2) {
                std::cout << "Received end signal. Exiting loop. Rank: " << my_rank << std::endl;
                break;
            }

            if (minTurns > dfsStateStatic.bestRouteLength) {
#pragma omp critical
                if (minTurns > dfsStateStatic.bestRouteLength) {
                    { minTurns = dfsStateStatic.bestRouteLength; }
                }
            }
            DfsState dfsState = ConvertFromStatic(dfsStateStatic);
# pragma omp parallel
            {
# pragma omp single
                {
                    dfs(dfsState.gameState,dfsState.depth, dfsState.m,dfsState.n, black, white, pointDistances, dfsState.solution,dfsState.last_move);
                }
            }

            AnswerReturn answerReturn{};
            answerReturn.bestRouteLength = minTurns;
            for(int i = 0; i < answerReturn.bestRouteLength+1; i++){
                answerReturn.solution[i] = GameStateConvertToStatic(pathsoultion[i]);
            }

            cout << "Sending response with best turns: " << minTurns << " from slave rank: " << my_rank << endl;
            MPI_Send(&answerReturn, 1, answerReturnType, 0, 1, MPI_COMM_WORLD);
            cout << "Sent response from rank: " << my_rank<< endl;
        }
    }


    //vypis sachovnice
    if(my_rank == 0) {
        const auto end{std::chrono::steady_clock::now()};
        const std::chrono::duration<double> elapsed_seconds{end - start};

        cout << "Optimal number of turns: " << minTurns << endl;

        int step = 0;
        for (const auto &state: pathsoultion) {
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
                    for (const auto &whiteKnight: state.whiteKnights) {
                        if (whiteKnight.second == i && whiteKnight.first == j) {
                            cout << "W ";
                            whiteKnightFound = true;
                            break;
                        }
                    }
                    for (const auto &blackKnight: state.blackKnights) {
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
    }


    MPI_Type_free(&dfsStateStaticType);
    MPI_Type_free(&answerReturnType);
    MPI_Type_free(&gameStateStaticType);
    MPI_Finalize();
    return 0;
}