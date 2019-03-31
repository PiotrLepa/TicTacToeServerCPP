#include "gameutils.h"
#include <QDebug>
#include <QtCore>

const QString GameUtils::FILE_PATH = "/home/piotr/Pulpit/TicTacToeSaves/";
const QString GameUtils::FILE_CLIENT_SOCKET_PATH = FILE_PATH + "ClientSockets/";

void GameUtils::createNewGame(qint64 gameId)
{
    qDebug() << "gameId: " << gameId;
    QString emptyBoard[9] = {"", "", "", "", "", "", "", "", ""};
    QString fileName = FILE_PATH + QString::number(gameId) + ".txt";
    QFile fout(fileName);
    if(fout.open(QIODevice::WriteOnly)){
        QDataStream out(&fout);
        out<<emptyBoard[0]<<emptyBoard[1]<<emptyBoard[2]<<emptyBoard[3]<<emptyBoard[4]<<emptyBoard[5]<<emptyBoard[6]<<emptyBoard[7]<<emptyBoard[8];
    }
    fout.close(); // or fout.flush();
}

QString* GameUtils::loadGameData(qint64 gameId)
{
    QString fileName = FILE_PATH + QString::number(gameId) + ".txt";
    QString *board = new QString[9];
    QFile fin(fileName);
    if (fin.open(QIODevice::ReadOnly)) {
        QDataStream in(&fin);
        in>>board[0]>>board[1]>>board[2]>>board[3]>>board[4]>>board[5]>>board[6]>>board[7]>>board[8];
    }
    fin.close();
    return board;
}

void GameUtils::savePlayerMove(qint64 gameId, QString mark, int index)
{
    QString * board = loadGameData(gameId);
    board[index] = mark;
    QString fileName = FILE_PATH + QString::number(gameId) + ".txt";
    QFile fout(fileName);
    if(fout.open(QIODevice::WriteOnly)){
        QDataStream out(&fout);
        out<<board[0]<<board[1]<<board[2]<<board[3]<<board[4]<<board[5]<<board[6]<<board[7]<<board[8];
    }
    fout.close();
}

void GameUtils::resetGame(qint64 gameId)
{
    createNewGame(gameId);
}

void GameUtils::deleteGame(qint64 gameId)
{
    QString fileName = FILE_PATH + QString::number(gameId) + ".txt";
    QFile file (fileName);
    file.remove();
}
