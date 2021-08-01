#include "Robot.h"
#include "CallLord.h"
#include "PlayHandThread.h"
#include "Strategy.h"
#include<QDebug>

/*
机器人主要是填充玩家的空白


1.	是否队友的牌；
2.	我的牌是否非常好；
3.	地主是我的上手还是下手；
4.	对方还剩多少牌；
5.	出牌大小；
6.	接牌的大小；


*/
void Robot::waitForCallLord()
{
    int weight = 0;
    Strategy st(this, player_cards);
    weight += st.getRangeCards(Card_SJ, Card_BJ).count() * 6;
    weight += player_cards.pointCount(Card_2) * 3;
    QVector<Cards> optSeq = st.pickOptimalStraight();
    weight += optSeq.size() * 5;
    Cards left = player_cards;
    left.remove(optSeq);
    Strategy stLeft(this, left);
    QVector<Cards> bombs = stLeft.findCardsByCount(4);
    weight += bombs.size() * 5;
    QVector<Cards> triples = stLeft.findCardsByCount(3);
    weight += triples.size() * 4;
    QVector<Cards> pairs = stLeft.findCardsByCount(2);
    weight += pairs.size() * 1;
    if (weight >= 22)CallLord(3);
    else if (weight < 22 && weight >= 18)CallLord(2);
    else if (weight > 18 && weight >= 10)CallLord(1);
    else CallLord(0);
}
void Robot::waitForPlayHand()
{Strategy strategy(this, player_cards);PlayHand(strategy.makeStrategy());}
Robot::Robot(QObject *parent){player_type = Player::Robot;}
void Robot::startCallLord()
{
    qDebug()<<"Bobot StartCallLord1";
    CallLordThread* thread = new CallLordThread(this);
    thread->start();
    qDebug()<<"Bobot StartCallLord2";
}

void Robot::startPlayHand()
{
    PlayHandThread* thread = new PlayHandThread(this);
    thread->start();
}


