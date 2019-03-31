#include "backend.h"
#include "gameutils.h"
#include "tictactoeailogic.h"
#include <QDateTime>

Backend::Backend(QObject *parent) : QObject(parent),
    playerMoveRegex("PLAYER_MOVE:\\d{13}:[XO]:\\d:(EASY|MEDIUM|HARD)"),
    playerMoveMultiplayerRegex("PLAYER_MOVE_MULTIPLAYER:\\d{13}:[XO]:\\d:OPPONENT_INDEX:\\d"),
    resetGameRegex("RESET_GAME:\\d{13}"),
    closeGameRegex("DELETE_GAME:\\d{13}")
{
    server = new ServerStuff();
    connect(server, &ServerStuff::gotNewMesssage, this, &Backend::gotNewMesssage);
    connect(server->tcpServer, &QTcpServer::newConnection, this, &Backend::smbConnectedToServer);
    connect(server, &ServerStuff::smbDisconnected, this, &Backend::smbDisconnectedFromServer);
    startServer();
}

QString Backend::startServer()
{
    if (!server->tcpServer->listen(QHostAddress::Any, 6547))
    {
        return "Error! The port is taken by some other service";
    }
    else
    {
        connect(server->tcpServer, &QTcpServer::newConnection, server, &ServerStuff::newConnection);
        return "Server started, port is openned";
    }
}

QString Backend::stopClicked()
{
    if(server->tcpServer->isListening())
    {
        disconnect(server->tcpServer, &QTcpServer::newConnection, server, &ServerStuff::newConnection);
        
        QList<QTcpSocket *> clients = server->getClients();
        for(int i = 0; i < clients.count(); i++)
        {
            //server->sendToClient(clients.at(i), "Connection closed");
            server->sendToClient(clients.at(i), "0");
        }
        
        server->tcpServer->close();
        return "Server stopped, post is closed";
    }
    else
    {
        return "Error! Server was not running";
    }
}

void Backend::smbConnectedToServer()
{
    emit smbConnected();
}

void Backend::smbDisconnectedFromServer()
{
    emit smbDisconnected();
}

void Backend::gotNewMesssage(QTcpSocket* clientSocket, QString msg)
{
    emit newMessage(msg);
    if (msg == "CREATE_NEW_GAME") createGame(clientSocket);
    else if (playerMoveRegex.exactMatch(msg)) doPlayerMove(clientSocket, msg);
    else if (playerMoveMultiplayerRegex.exactMatch(msg)) doMultiplayerPlayerMove(clientSocket, msg);
    else if (resetGameRegex.exactMatch(msg)) resetGame(clientSocket, msg);
    else if (closeGameRegex.exactMatch(msg))closeGame(msg);
    else if (msg == "SEARCH_OPPONENT")
    {
        usersLobby.append(clientSocket);
        if (usersLobby.size() == 2)
        {
            createMultiplayerGame();
        }
    }
    
};

void Backend::createGame(QTcpSocket* clientSocket)
{
    qDebug() << "createGame";
    qint64 gameId = QDateTime::currentMSecsSinceEpoch();
    GameUtils::createNewGame(gameId);
    if (gameId %2 == 0) {
        QString playerMark = "X";
        QString msgToSend = "GAME_CREATED:" + QString::number(gameId) + ":" + playerMark;
        server->sendToClient(clientSocket, msgToSend);
    }
    else {
        QString playerMark = "O";
        QString aiMove = doAiMove(gameId, playerMark, "HARD");
        QString msgToSend = "GAME_CREATED_AI_MOVE:" + QString::number(gameId) + +":" + playerMark + ":" + aiMove;
        server->sendToClient(clientSocket, msgToSend);
    }
}

void Backend::doPlayerMove(QTcpSocket *clientSocket, QString msg)
{
    qDebug() << "doPlayerMove PLAYER_MOVE";
    QStringList list = msg.split(":");
    qint64 gameId = list[1].toLongLong();
    QString playerMark = list[2];
    int index = list[3].toInt();
    QString gameLevel = list[4];
    GameUtils::savePlayerMove(gameId, playerMark, index);

    QString msgToSend = doAiMove(gameId, playerMark, gameLevel);
    server->sendToClient(clientSocket, msgToSend);
}

void Backend::doMultiplayerPlayerMove(QTcpSocket *clientSocket, QString msg)
{
    qDebug() << "doMultiplayerPlayerMove PLAYER_MOVE_MULTIPLAYER";
    QStringList list = msg.split(":");
    qint64 gameId = list[1].toLongLong();
    QString playerMark = list[2];
    int index = list[3].toInt();
    int opponentIndex= list[5].toInt();
    qDebug() << "doMultiplayerPlayerMove PLAYER_MOVE_MULTIPLAYER: opponentIndex: " << opponentIndex;
    GameUtils::savePlayerMove(gameId, playerMark, index);

    QString msgToSend = "MULTIPLAYER_OPPONENT_MOVE:" + playerMark + ":" + QString::number(index);
    server->sendToClient(multiplayerGameUsers[opponentIndex], msgToSend);
}

QString Backend::doAiMove(qint64 gameId, QString playerMark, QString gameLevel)
{
    QString* board = GameUtils::loadGameData(gameId);
    QString aiMark = playerMark == "O" ? "X" : "O";
    int aiMoveIndex = TicTacToeAiLogic(board, aiMark).getBestMove(gameLevel);
    GameUtils::savePlayerMove(gameId, aiMark, aiMoveIndex);
    return "AI_MOVE:" + aiMark + ":" + QString::number(aiMoveIndex);
}

void Backend::resetGame(QTcpSocket *clientSocket, QString msg)
{
    qDebug() << "resetGame resetGameRegex";
    QStringList list = msg.split(":");
    qint64 gameId = list[1].toLongLong();
    GameUtils::resetGame(gameId);

    QString msgToSend = "GAME_RESETED";
    server->sendToClient(clientSocket, msgToSend);
}

void Backend::closeGame(QString msg)
{
    qDebug() << "gotNewMesssage CLOSE_GAME_ID";
    QStringList list = msg.split(":");
    qint64 gameId = list[1].toLongLong();
    GameUtils::deleteGame(gameId);
}

void Backend::createMultiplayerGame()
{
    qDebug() << "gotNewMesssage SEARCH_OPPONENT";
    qint64 gameId = QDateTime::currentMSecsSinceEpoch();
    GameUtils::createNewGame(gameId);
    QString firstPlayerMark;
    QString secondPlayerMark;
    if (gameId %2 == 0)
    {
        firstPlayerMark = "X";
        secondPlayerMark = "O";
    }
    else
    {
        firstPlayerMark = "O";
        secondPlayerMark = "X";
    }

    QString msgToSend = "MULTIPLAYER_GAME_CREATED:" + QString::number(gameId) + ":";
    multiplayerGameUsers.append(usersLobby[0]);
    multiplayerGameUsers.append(usersLobby[1]);

    server->sendToClient(usersLobby[0], msgToSend + firstPlayerMark + ":OPPONENT_INDEX:" + QString::number(multiplayerGameUsers.size()-1));
    server->sendToClient(usersLobby[1], msgToSend + secondPlayerMark + ":OPPONENT_INDEX:" + QString::number(multiplayerGameUsers.size()-2));

    usersLobby.removeFirst();
    usersLobby.removeFirst();
}



