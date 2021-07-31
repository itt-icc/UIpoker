#ifndef CALLLORD_H
#define CALLLORD_H

#include <QThread>

class Player;

class CallLordThread : public QThread
{
    Q_OBJECT
public:
    explicit CallLordThread(Player* player);

protected:
    virtual void run();

private:
    Player* m_player;
};

#endif
