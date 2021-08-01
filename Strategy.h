#ifndef STRATEGY_H
#define STRATEGY_H

#include <QVector>
#include"Hand.h"

class Cards;
class MyAI;
class Player;


class Strategy
{
protected:
    Player* s_player;
    Cards player_cards;
public:
    Strategy(Player* player, const Cards& cards);
    Cards makeStrategy();
public:
    Cards getFirstStraight();
    QVector<Cards> findHand(Hand hand, bool beat);
    Cards playFirst();
    Cards playBeatHand(Hand hand);
    bool whetherToBeat(const Cards& myCards);
    void pickStraight(QVector<QVector<Cards> >& allSeqRecord,
                      const QVector<Cards>& seqInherited, Cards cards);
    QVector<Cards> pickOptimalStraight();
    Cards findSamePoint(CardPoint point, int count);
    QVector<Cards> findCardsByCount(int count);
    Cards getRangeCards(CardPoint beginPoint, CardPoint endPoint);

};
class MyAI{};

#endif







