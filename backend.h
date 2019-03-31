#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include "serverstuff.h"

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);

signals:
    void smbConnected();
    void smbDisconnected();
    void newMessage(QString msg);

public slots:
    QString stopClicked();
    void smbConnectedToServer();
    void smbDisconnectedFromServer();
    void gotNewMesssage(QTcpSocket* clientSocket, QString msg);

private:
    ServerStuff *server;
    QList<QTcpSocket*> usersLobby;
    QList<QTcpSocket*> multiplayerGameUsers;
    QRegExp playerMoveRegex;
    QRegExp playerMoveMultiplayerRegex;
    QRegExp resetGameRegex;
    QRegExp closeGameRegex;

    QString startServer();
    void createGame(QTcpSocket* clientSocket);
    void doPlayerMove(QTcpSocket* clientSocket, QString msg);
    void doMultiplayerPlayerMove(QTcpSocket* clientSocket, QString msg);
    QString doAiMove(qint64 gameId, QString playerMark, QString gameLevel);
    void resetGame(QTcpSocket* clientSocket, QString msg);
    void closeGame(QString msg);
    void createMultiplayerGame();
};

#endif // BACKEND_H
