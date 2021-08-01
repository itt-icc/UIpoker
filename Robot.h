#ifndef ROBOT_H
#define ROBOT_H
#include "Player.h"
class Robot : public Player
{
    Q_OBJECT
public:
    Robot(QObject* parent = nullptr);
public:
    virtual void waitForCallLord();
    virtual void waitForPlayHand();
    virtual void startCallLord();
    virtual void startPlayHand();

};

#endif
