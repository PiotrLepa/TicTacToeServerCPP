#ifndef GAMEUTILS_H
#define GAMEUTILS_H

#include <QDateTime>
#include <QTcpSocket>

class GameUtils
{
public:
    GameUtils();
    static void createNewGame(qint64 gameId);
    static QString* loadGameData(qint64 gameId);
    static void savePlayerMove(qint64 gameId, QString mark, int index);
    static void resetGame(qint64 gameId);
    static void deleteGame(qint64 gameId);

private:
    static const QString FILE_PATH;
    static const QString FILE_CLIENT_SOCKET_PATH;
};

#endif // GAMEUTILS_H
