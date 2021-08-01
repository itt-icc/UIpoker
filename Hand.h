#ifndef HAND_H
#define HAND_H

#include "Cards.h"
#include <QVector>

class Hand
{
public:
    Hand();
    Hand(HandType type, CardPoint base, int extra);
    Hand(const Cards& cards);

    bool operator ==(const Hand& hand);

public:
    void fromCards(const Cards& cards);
    void judgeHand();

    bool canBeat(const Hand& other);
    HandType getType();
    CardPoint getBasePoint();
    int getExtra();

protected:
    HandType hand_handType;
    CardPoint hand_basePoint;
    int hand_extra;

    QVector<CardPoint> hand_oneCard;
    QVector<CardPoint> hand_twoCards;
    QVector<CardPoint> hand_threeCards;
    QVector<CardPoint> hand_fourCards;
};

#endif // HAND_H
