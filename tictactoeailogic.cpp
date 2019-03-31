#include "tictactoeailogic.h"
#include "QtDebug"

TicTacToeAiLogic::TicTacToeAiLogic(QString board[], QString aiPlayer)
{
    this->board = board;
    this->aiPlayer = aiPlayer;
    this->huPlayer = aiPlayer == "O" ? "X" : "O";
}

int TicTacToeAiLogic::getBestMove(QString gameLevel)
{
    qDebug() << "gameLevel: " << gameLevel;
    QString* boardWithIndexes = changeEmptyToIndexes(board);
    Move bestMove;
    if (gameLevel == "EASY") bestMove = minMax(boardWithIndexes, this->aiPlayer, 2);
    else if (gameLevel == "MEDIUM") bestMove = minMax(boardWithIndexes, this->aiPlayer, 3);
    else bestMove = minMax(boardWithIndexes, this->aiPlayer, 9999999);

    qDebug() << "bestMove index: " << bestMove.index;
    return bestMove.index.toInt();
}

TicTacToeAiLogic::Move TicTacToeAiLogic::minMax(QString newBoard[], QString player, int maxCalls)
{
    if (maxCalls == 0)
    {
        return Move(0, "");
    }
    QList<int> availSpots = getEmptyFieldsIndexes(newBoard);

    if (checkWin(newBoard, this->huPlayer))
    {
        return Move(-1, "");
    }
    else if (checkWin(newBoard, this->aiPlayer))
    {
        return Move(1, "");
    }
    else if (availSpots.size() == 0)
    {
        return Move(0, "");
    }


    QList<TicTacToeAiLogic::Move> moves;
    for (int i=0; i<availSpots.size(); i++)
    {
        Move move;
        move.index = newBoard[availSpots[i]];
        newBoard[availSpots[i]] = player;

        if (player == this->aiPlayer)
        {
            move.score = minMax(newBoard, this->huPlayer, maxCalls-1).score;
        }
        else
        {
            move.score = minMax(newBoard, this->aiPlayer, maxCalls-1).score;
        }

        newBoard[availSpots[i]] = move.index;
        moves.append(move);
    }

    int bestMove = 0;
    if (player == this->aiPlayer)
    {
        int bestScore = -10000;
        for (int i=0; i<moves.size(); i++)
        {
            if (moves[i].score > bestScore)
            {
                bestScore = moves[i].score;
                bestMove = i;
            }
        }
    }
    else
    {
        int bestScore = 10000;
        for (int i=0; i<moves.size(); i++)
        {
            if (moves[i].score < bestScore)
            {
                bestScore = moves[i].score;
                bestMove = i;
            }
        }
    }


    return moves[bestMove];
}


QString* TicTacToeAiLogic::changeEmptyToIndexes(QString* board)
{
    QString* boardWithIndexes = new QString[9];
    for (int i = 0; i < 9; i++)
    {
        if (board[i] == "") boardWithIndexes[i] = QString::number(i);
        else  boardWithIndexes[i] = board[i];
    }

    return boardWithIndexes;
}

QList<int> TicTacToeAiLogic::getEmptyFieldsIndexes(QString board[])
{
    QList<int> list;
    for (int i = 0; i < 9; i++)
    {
        if (board[i] != "" && board[i] != this->huPlayer && board[i] != this->aiPlayer)
        {
            list.append(i);
        }
    }
    return list;
}

bool TicTacToeAiLogic::checkWin(QString board[], QString mark)
{
    int winningCombinations[8][3] =
                    {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};

    for (int i=0; i<8; i++)
    {
        if((board[winningCombinations[i][0]] == mark)
                && (board[winningCombinations[i][1]] == mark)
                && (board[winningCombinations[i][2]] == mark))
        {
            return true;
        }
    }

    return false;
}


bool TicTacToeAiLogic::checkWinOrDraw(QString board[])
{
    int winningCombinations[8][3] =
                    {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};

    for (int i=0; i<8; i++)
    {
        if((board[winningCombinations[i][0]] == "O")
                && (board[winningCombinations[i][1]] == "O")
                && (board[winningCombinations[i][2]] == "O"))
        {
            return true;
        }
    }

    for (int i=0; i<8; i++)
    {
        if((board[winningCombinations[i][0]] == "X")
                && (board[winningCombinations[i][1]] == "X")
                && (board[winningCombinations[i][2]] == "X"))
        {
            return true;
        }
    }

    bool isDraw = true;
    for (int i=0; i<8; i++)
    {
        if (board[i] == "")
        {
            isDraw = false;
        }
    }

    return isDraw;
}

