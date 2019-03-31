#ifndef TICTACTOEAILOGIC_H
#define TICTACTOEAILOGIC_H

#include <QString>
#include <QList>

class TicTacToeAiLogic
{
public:
    class Move
    {
    public:
        int score;
        QString index;

        Move() {}
        Move(int score, QString index) {this->score = score; this->index = index;}
    };

    TicTacToeAiLogic(QString board[], QString aiPlayer);
    int getBestMove(QString gameLevel);

private:
    QString* board;
    QString aiPlayer;
    QString huPlayer;

    QString* changeEmptyToIndexes(QString* board);
    Move minMax(QString newBoard[], QString player, int maxCalls);
    QList<int> getEmptyFieldsIndexes(QString board[]);
    bool checkWin(QString board[], QString mark);
    static bool checkWinOrDraw(QString board[]);
};

#endif // TICTACTOEAILOGIC_H
