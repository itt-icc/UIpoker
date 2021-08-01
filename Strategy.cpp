
#include "Cards.h"
#include "Hand.h"
#include "Player.h"
#include "Strategy.h"
/*

1.	如果有火箭，找出来；
2.	如果有炸弹，找出来（炸弹不拆）；
3.	如果有2，全部找出来（2不会参与顺子）；
4.	如果有飞机，找出来（飞机也不拆）；
5.	找出所有的顺子，每个顺子尽量长；
6.	处理顺子
    1.	顺子分裂，现有一个顺子345678910，发现手上还剩下一张6和7，那么顺子分裂成34567和678910；
    2.	顺子拆出连对，现有一个顺子345678910，发现手上还有345，变成334455和678910更好，就把顺子里面的345放回去；
    3.	顺子拆出三张，现有一个顺子345678，发现手上还有88，那么变成34567和888更好，就把顺子里面的8放回去；
    4.	顺子如果盖住对子、三张、连对，如果发现打散牌组数更少，则打散，比如7789910JJJQKK，拆散了更好；
    5.	顺子拆出两头的对子，现有一个顺子67890JQ，发现手上还有Q和6，那么把孙子里面的Q和6放回去；
    6.	反复进行1）到5），直到没有进一步变化；
7.	剩余的牌里面找出所有的连对；
8.	查看所有的连对，如果长度超过3，且一端有三条，把三条拆出来；
9.	剩余的牌里面找出所有的三张；
10.	延长顺子：如果一个顺子的两端外面有一个对子，如果这个对子小于10，则并入顺子，比如34567+88,那么变成345678+8;
11.	合并顺子：相同的顺子变成连对，首尾相接的顺子连成一个；
12.	剩余的牌里面找出所有对子和单张。


*/
Strategy::Strategy(Player* player, const Cards& cards)
{
    s_player = player;
    player_cards = cards;
}


Cards Strategy::playBeatHand(Hand hand)
{
    Cards left = player_cards;
    left.remove(Strategy(s_player, left).pickOptimalStraight());

    if (hand.getType() == card_Single)
    {
        QVector<Cards> singleArray = Strategy(s_player, left).findCardsByCount(1);
        for (int i = 0; i < singleArray.size(); i++)
        {
            if (Hand(singleArray[i]).canBeat(hand))
            {
                return singleArray[i];
            }
        }
    }
    else if (hand.getType() == card_Pair)
    {
        QVector<Cards> pairArray = Strategy(s_player, left).findCardsByCount(2);
        for (int i = 0; i < pairArray.size(); i++)
        {
            if (Hand(pairArray[i]).canBeat(hand))
            {
                return pairArray[i];
            }
        }
    }

    Player* nextPlayer = s_player->getNextPlayer();
    QVector<Cards> beatCardsArray = Strategy(s_player, left).findHand(hand, true);
    if (!beatCardsArray.isEmpty())
    {
        if (s_player->getRole() != nextPlayer->getRole() &&
            nextPlayer->getCards().count() <= 2)
        {
            return beatCardsArray.back();
        }
        else
        {
            return beatCardsArray.front();
        }
    }
    else
    {
        beatCardsArray = Strategy(s_player, player_cards).findHand(hand, true);
        if (!beatCardsArray.isEmpty())
        {
            if (s_player->getRole() != nextPlayer->getRole() &&
                nextPlayer->getCards().count() <= 2)
            {
                return beatCardsArray.back();
            }
            else
            {
                return beatCardsArray.front();
            }
        }
    }
    Player* punchPlayer = s_player->getPunchPlayer();
    if (s_player->getRole() != punchPlayer->getRole())
    {
        if (punchPlayer->getCards().count() <= 3)
        {
            QVector<Cards> bombs = findCardsByCount(4);
            if (!bombs.isEmpty())
            {
                return bombs[0];
            }
        }
    }
    return Cards();
}
QVector<Cards> Strategy::findHand(Hand hand, bool beat)
{
    HandType handType = hand.getType();
    CardPoint basePoint = hand.getBasePoint();
    int extra = hand.getExtra();

    if (handType == card_Pass){}
    else if (handType == card_Single)
    {
        QVector<Cards> findCardsArray;
        CardPoint beginPoint = beat ? CardPoint(basePoint + 1) : CardPoint(Card_Begin + 1);
        for (CardPoint point = beginPoint; point < Card_End; point = CardPoint(point + 1))
        {
            Cards findCards = findSamePoint(point, 1);
            if (!findCards.isEmpty())findCardsArray << findCards;
        }
        return findCardsArray;
    }
    else if (handType == card_Pair)
    {
        QVector<Cards> findCardsArray;

        CardPoint beginPoint = beat ? CardPoint(basePoint + 1) : CardPoint(Card_Begin + 1);
        for (CardPoint point = beginPoint; point < Card_End; point = CardPoint(point + 1))
        {
            Cards findCards = findSamePoint(point, 2);
            if (!findCards.isEmpty())
            {
                findCardsArray << findCards;
            }
        }

        return findCardsArray;
    }
    else if (handType == card_Triple)
    {
        QVector<Cards> findCardsArray;

        CardPoint beginPoint = beat ? CardPoint(basePoint + 1) : CardPoint(Card_Begin + 1);
        for (CardPoint point = beginPoint; point < Card_End; point = CardPoint(point + 1))
        {
            Cards findCards = findSamePoint(point, 3);
            if (!findCards.isEmpty())
            {
                findCardsArray << findCards;
            }
        }

        return findCardsArray;
    }
    else if (handType == card_Triple_with_Single)
    {
        QVector<Cards> findCardsArray;

        CardPoint beginPoint = beat ? CardPoint(basePoint + 1) : CardPoint(Card_Begin + 1);
        for (CardPoint point = beginPoint; point < Card_End; point = CardPoint(point + 1))
        {
            Cards findCards = findSamePoint(point, 3);
            if (!findCards.isEmpty())
            {
                findCardsArray << findCards;
            }
        }

        if (!findCardsArray.isEmpty())
        {
            Cards remainCards = player_cards;
            remainCards.remove(findCardsArray);

            Strategy st(s_player, remainCards);
            QVector<Cards> oneCardsArray = st.findHand(Hand(card_Single, Card_Begin, 0), false);
            if (!oneCardsArray.isEmpty())
            {
                for (int i = 0; i < findCardsArray.size(); i++)
                {
                    findCardsArray[i] << oneCardsArray[0];
                }
            }
            else
            {
                findCardsArray.clear();
            }
        }

        return findCardsArray;
    }
    else if (handType == card_Triple_with_Pair)
    {
        QVector<Cards> findCardsArray;

        CardPoint beginPoint = beat ? CardPoint(basePoint + 1) : CardPoint(Card_Begin + 1);
        for (CardPoint point = beginPoint; point < Card_End; point = CardPoint(point + 1))
        {
            Cards findCards = findSamePoint(point, 3);
            if (!findCards.isEmpty())
            {
                findCardsArray << findCards;
            }
        }

        if (!findCardsArray.isEmpty())
        {
            Cards remainCards = player_cards;
            remainCards.remove(findCardsArray);

            Strategy st(s_player, remainCards);
            QVector<Cards> pairCardsArray = st.findHand(Hand(card_Pair, Card_Begin, 0), false);
            if (!pairCardsArray.isEmpty())
            {
                for (int i = 0; i < findCardsArray.size(); i++)
                {
                    findCardsArray[i] << pairCardsArray[0];
                }
            }
            else
            {
                findCardsArray.clear();
            }
        }

        return findCardsArray;
    }
    else if (handType == card_Plane)
    {
        QVector<Cards> findCardsArray;

        CardPoint beginPoint = beat ? CardPoint(basePoint + 1) : CardPoint(Card_Begin + 1);
        for (CardPoint point = beginPoint; point <= Card_K; point = CardPoint(point + 1))
        {
            Cards prevCards = findSamePoint(point, 3);
            Cards nextCards = findSamePoint(CardPoint(point+1), 3);
            if (!prevCards.isEmpty() && !nextCards.isEmpty())
            {
                findCardsArray << prevCards << nextCards;
            }
        }
    }
    else if (handType == card_Plane_with_2Single)
    {
        QVector<Cards> findCardsArray;

        CardPoint beginPoint = beat ? CardPoint(basePoint + 1) : CardPoint(Card_Begin + 1);
        for (CardPoint point = beginPoint; point <= Card_K; point = CardPoint(point + 1))
        {
            Cards prevCards = findSamePoint(point, 3);
            Cards nextCards = findSamePoint(CardPoint(point+1), 3);
            if (!prevCards.isEmpty() && !nextCards.isEmpty())
            {
                Cards findCards;
                findCards << prevCards << nextCards;
                findCardsArray << findCards;
            }
        }

        if (!findCardsArray.isEmpty())
        {
            Cards remainCards = player_cards;
            remainCards.remove(findCardsArray);

            Strategy st(s_player, remainCards);
            QVector<Cards> oneCardsArray = st.findHand(Hand(card_Single, Card_Begin, 0), false);
            if (oneCardsArray.size() >= 2)
            {
                for (int i = 0; i < findCardsArray.size(); i++)
                {
                    Cards oneCards;
                    oneCards << oneCardsArray[0] << oneCardsArray[1];
                    findCardsArray[i] << oneCards;
                }
            }
            else
            {
                findCardsArray.clear();
            }
        }

        return findCardsArray;
    }
    else if (handType == card_Plane_with_2Pair)
    {
        QVector<Cards> findCardsArray;

        CardPoint beginPoint = beat ? CardPoint(basePoint + 1) : CardPoint(Card_Begin + 1);
        for (CardPoint point = beginPoint; point <= Card_K; point = CardPoint(point + 1))
        {
            Cards prevCards = findSamePoint(point, 3);
            Cards nextCards = findSamePoint(CardPoint(point+1), 3);
            if (!prevCards.isEmpty() && !nextCards.isEmpty())
            {
                Cards findCards;
                findCards << prevCards << nextCards;
                findCardsArray << findCards;
            }
        }

        if (!findCardsArray.isEmpty())
        {
            Cards remainCards = player_cards;
            remainCards.remove(findCardsArray);

            Strategy st(s_player, remainCards);
            QVector<Cards> pairCardsArray = st.findHand(Hand(card_Pair, Card_Begin, 0), false);
            if (pairCardsArray.size() >= 2)
            {
                for (int i = 0; i < findCardsArray.size(); i++)
                {
                    Cards pairCards;
                    pairCards << pairCardsArray[0] << pairCardsArray[1];
                    findCardsArray[i] << pairCards;
                }
            }
            else
            {
                findCardsArray.clear();
            }
        }

        return findCardsArray;
    }
    else if (handType == card_Pair_Straight)
    {
        if (beat)
        {
            QVector<Cards> findCardsArray;
            for (CardPoint point = CardPoint(basePoint + 1); point <= Card_Q; point = CardPoint(point + 1))
            {
                bool seqPairFound = true;
                Cards seqPair;

                for (int i = 0; i < extra; i++)
                {
                    Cards cards = findSamePoint(CardPoint(point + i), 2);
                    // 连对中断，或顶到2了
                    if (cards.isEmpty() || (point + i >= Card_2))
                    {
                        seqPairFound = false;
                        seqPair.clear();
                        break;
                    }
                    else
                    {
                        seqPair << cards;
                    }
                }

                if (seqPairFound)
                {
                    findCardsArray << seqPair;
                    return findCardsArray;
                }
            }

            return findCardsArray;
        }
        else
        {
            QVector<Cards> findCardsArray;

            for (CardPoint point = Card_3; point <= Card_Q; point = CardPoint(point + 1))
            {
                Cards cards0 = findSamePoint(point, 2);
                Cards cards1 = findSamePoint(CardPoint(point + 1), 2);
                Cards cards2 = findSamePoint(CardPoint(point + 2), 2);

                if (cards0.isEmpty() || cards1.isEmpty() || cards2.isEmpty()) continue;

                Cards baseSeq;
                baseSeq << cards0 << cards1 << cards2;
                findCardsArray << baseSeq;

                int followed = 3;
                Cards alreadyFollowedCards;

                while (true)
                {
                    CardPoint followedPoint = CardPoint(point + followed);
                    if (followedPoint >= Card_2)
                    {
                        break;
                    }

                    Cards followedCards = findSamePoint(followedPoint, 2);
                    if (followedCards.isEmpty())
                    {
                        break;
                    }
                    else
                    {
                        alreadyFollowedCards << followedCards;
                        Cards newSeq = baseSeq;
                        newSeq << alreadyFollowedCards;

                        findCardsArray << newSeq;
                        followed++;
                    }
                }
            }

            return findCardsArray;
        }
    }
    else if (handType == card_Straight)
    {
        if (beat)
        {
            QVector<Cards> findCardsArray;
            for (CardPoint point = CardPoint(basePoint + 1); point <= Card_10; point = CardPoint(point + 1))
            {
                bool seqSingleFound = true;
                Cards seqSingle;

                for (int i = 0; i < extra; i++)
                {
                    Cards cards = findSamePoint(CardPoint(point + i), 1);
                    if (cards.isEmpty() || (point + extra >= Card_2))
                    {
                        seqSingleFound = false;
                        seqSingle.clear();
                        break;
                    }
                    else
                    {
                        seqSingle << cards;
                    }
                }

                if (seqSingleFound)
                {
                    findCardsArray << seqSingle;
                    return findCardsArray;
                }
            }
        }
        else
        {
            QVector<Cards> findCardsArray;

            for (CardPoint point = Card_3; point <= Card_10; point = CardPoint(point + 1))
            {
                Cards cards0 = findSamePoint(point, 1);
                Cards cards1 = findSamePoint(CardPoint(point + 1), 1);
                Cards cards2 = findSamePoint(CardPoint(point + 2), 1);
                Cards cards3 = findSamePoint(CardPoint(point + 3), 1);
                Cards cards4 = findSamePoint(CardPoint(point + 4), 1);

                if (cards0.isEmpty() || cards1.isEmpty() || cards2.isEmpty()
                    || cards3.isEmpty() || cards4.isEmpty())
                {
                    continue;
                }

                Cards baseSeq;
                baseSeq << cards0 << cards1 << cards2 << cards3 << cards4;
                findCardsArray << baseSeq;

                int followed = 5;
                Cards alreadyFollowedCards;

                while (true)
                {
                    CardPoint followedPoint = CardPoint(point + followed);
                    if (followedPoint >= Card_2)
                    {
                        break;
                    }

                    Cards followedCards = findSamePoint(followedPoint, 1);
                    if (followedCards.isEmpty())
                    {
                        break;
                    }
                    else
                    {
                        alreadyFollowedCards << followedCards;
                        Cards newSeq = baseSeq;
                        newSeq << alreadyFollowedCards;

                        findCardsArray << newSeq;
                        followed++;
                    }
                }
            }

            return findCardsArray;
        }
    }
    else if (handType == card_Bomb)
    {
        QVector<Cards> findCardsArray;

        CardPoint beginPoint = beat ? CardPoint(basePoint + 1) : CardPoint(Card_Begin + 1);
        for (CardPoint point = beginPoint; point < Card_End; point = CardPoint(point + 1))
        {
            Cards findCards = findSamePoint(point, 4);
            if (!findCards.isEmpty())
            {
                findCardsArray << findCards;
            }
        }

        return findCardsArray;
    }
    else if (handType == card_Bomb_with_Single)
    {

    }
    else if (handType == card_Bomb_with_Pair)
    {

    }
    else if (handType == card_Bomb_with_2Single)
    {

    }

    return QVector<Cards>();
}

Cards Strategy::findSamePoint(CardPoint point, int count)
{
    if (point == Card_SJ || point == Card_BJ)
    {
        if (count > 1) return Cards();

        Card oneCard;
        oneCard.point = point;
        oneCard.suit = Suit_Begin;
        if (player_cards.contains(oneCard))
        {
            Cards cards;
            cards.add(oneCard);
            return cards;
        }

        return Cards();
    }

    Cards findCards;
    int findCount = 0;
    for (int suit = Suit_Begin + 1; suit < Suit_End; suit++)
    {
        Card oneCard;
        oneCard.point = point;
        oneCard.suit = (CardType)suit;

        if (player_cards.contains(oneCard))
        {
            findCount++;
            findCards << oneCard;

            if (findCount == count)
            {
                return findCards;
            }
        }
    }

    return Cards();
}

QVector<Cards> Strategy::findCardsByCount(int count)
{
    QVector<Cards> cardsArray;
    if (count < 1 || count > 4) return cardsArray;

    for (CardPoint point = Card_3; point < Card_End; point = CardPoint(point + 1))
    {
        if (player_cards.pointCount(point) == count)
        {
            Cards cards;
            cards << findSamePoint(point, count);
            cardsArray << cards;
        }
    }

    return cardsArray;
}

Cards Strategy::getRangeCards(CardPoint beginPoint, CardPoint endPoint)
{
    Cards rangeCards;
    for (CardPoint point = beginPoint; point <= endPoint; point = CardPoint(point + 1))
    {
        int count = player_cards.pointCount(point);
        Cards cards = findSamePoint(point, count);
        rangeCards << cards;
    }
    return rangeCards;
}

Cards Strategy::getFirstStraight()
{
    int seqCount = 0;
    CardPoint beginPoint, endPoint;
    beginPoint = endPoint = Card_Begin;

    for (CardPoint point = Card_3; point <= Card_10; point = CardPoint(point + 1))
    {
        while (true)
        {
            CardPoint findPoint = CardPoint(point + seqCount);
            Cards oneCard = findSamePoint(findPoint, 1);
            if (oneCard.isEmpty() || findPoint > Card_A)
            {
                if (seqCount >= 5)
                {
                    endPoint = CardPoint(point + seqCount - 1);
                    return getRangeCards(beginPoint, endPoint);
                }

                point = findPoint;
                beginPoint = endPoint = Card_Begin;
                seqCount = 0;

                break;
            }
            else
            {
                if (seqCount == 0)
                {
                    beginPoint = point;
                }

                seqCount++;
            }
        }
    }

    return Cards();
}

void Strategy::pickStraight(QVector<QVector<Cards> > & allSeqRecord, const QVector<Cards>& seqInherited, Cards cards)
{
    QVector<Cards> allSeqScheme = Strategy(s_player, cards).findHand(Hand(card_Straight, Card_Begin, 0), false);
    if (allSeqScheme.isEmpty())
    {
        allSeqRecord << seqInherited;
    }
    else
    {
        Cards savedCards = cards;
        for (int i = 0; i < allSeqScheme.size(); i++)
        {
            Cards aScheme = allSeqScheme[i];
            Cards leftCards = savedCards;
            leftCards.remove(aScheme);

            QVector<Cards> seqArray = seqInherited;
            seqArray << aScheme;

            pickStraight(allSeqRecord, seqArray, leftCards);
        }
    }
}

QVector<Cards> Strategy::pickOptimalStraight()
{
    QVector<QVector<Cards> > seqRecord;
    QVector<Cards> seqInherited;
    Strategy(s_player, player_cards).pickStraight(seqRecord, seqInherited, player_cards);
    if (seqRecord.isEmpty()) return QVector<Cards>();

    QMap<int, int> seqMarks;
    for (int i = 0; i < seqRecord.size(); i++)
    {
        Cards backupCards = player_cards;
        QVector<Cards> seqArray = seqRecord[i];
        backupCards.remove(seqArray);

        QVector<Cards> singleArray = Strategy(s_player, backupCards).findCardsByCount(1);

        CardList cardList;
        for (int j = 0; j < singleArray.size(); j++)
        {
            cardList << singleArray[j].toCardList();
        }

        int mark = 0;
        for (int j = 0; j < cardList.size(); j++)
        {
            mark += cardList[j].point + 15;
        }
        seqMarks.insert(i, mark);
    }

    int index = 0;
    int compMark = 1000000;
    QMap<int, int>::ConstIterator it = seqMarks.constBegin();
    for (; it != seqMarks.constEnd(); it++)
    {
        if (it.value() < compMark)
        {
            compMark = it.value();
            index = it.key();
        }
    }

    return seqRecord[index];
}

Cards Strategy::playFirst()
{
    Hand hand(player_cards);
    if (hand.getType() != card_Unknown)
    {
        return player_cards;
    }

    Cards seqSingleRange = getFirstStraight();
    if (!seqSingleRange.isEmpty())
    {
        Cards left = seqSingleRange;
        left.remove(Strategy(s_player, left).findCardsByCount(4));
        left.remove(Strategy(s_player, left).findCardsByCount(3));
        QVector<Cards> optimalSeq = Strategy(s_player, left).pickOptimalStraight();
        if (!optimalSeq.isEmpty())
        {
            int oriSingleCount = Strategy(s_player, left).findCardsByCount(1).size();
            left.remove(optimalSeq);
            int leftSingleCount = Strategy(s_player, left).findCardsByCount(1).size();

            if (leftSingleCount < oriSingleCount)
            {
                return optimalSeq[0];
            }
        }
    }

    bool hasPlane, hasTriple, hasSeqPair;
    hasPlane = hasTriple = hasSeqPair = false;
    Cards leftCards = player_cards;

    QVector<Cards> bombArray = Strategy(s_player, leftCards).findHand(Hand(card_Bomb, Card_Begin, 0), false);
    leftCards.remove(bombArray);

    QVector<Cards> planeArray = Strategy(s_player, leftCards).findHand(Hand(card_Plane, Card_Begin, 0), false);
    if (!planeArray.isEmpty())
    {
        hasPlane = true;
        leftCards.remove(planeArray);
    }

    QVector<Cards> tripleArray = Strategy(s_player, leftCards).findHand(Hand(card_Triple, Card_Begin, 0), false);
    if (!tripleArray.isEmpty())
    {
        hasTriple = true;
        leftCards.remove(tripleArray);
    }

    QVector<Cards> seqPairArray = Strategy(s_player, leftCards).findHand(Hand(card_Pair_Straight, Card_Begin, 0), false);
    if (!seqPairArray.isEmpty())
    {
        hasSeqPair = true;
        leftCards.remove(seqPairArray);
    }

    if (hasPlane)
    {
        bool twoPairFound = false;
        QVector<Cards> pairArray;
        for (CardPoint point = Card_3; point <= Card_A; point = CardPoint(point + 1))
        {
            Cards pair = Strategy(s_player, leftCards).findSamePoint(point, 2);
            if (!pair.isEmpty())
            {
                pairArray << pair;
                if (pairArray.size() == 2)
                {
                    twoPairFound = true;
                    break;
                }
            }
        }

        if (twoPairFound)
        {
            Cards playCards = planeArray[0];
            playCards.add(pairArray);

            return playCards;
        }
        else
        {
            bool twoSingleFound = false;
            QVector<Cards> singleArray;
            for (CardPoint point = Card_3; point <= Card_A; point = CardPoint(point + 1))
            {
                if (leftCards.pointCount(point) == 1)
                {
                    Cards single = Strategy(s_player, leftCards).findSamePoint(point, 1);
                    if (!single.isEmpty())
                    {
                        singleArray << single;
                        if (singleArray.size() == 2)
                        {
                            twoSingleFound = true;
                            break;
                        }
                    }
                }
            }

            if (twoSingleFound)
            {
                Cards playCards = planeArray[0];
                playCards.add(singleArray);

                return playCards;
            }
            else
            {
                return planeArray[0];
            }
        }
    }

    if (hasTriple)
    {
        if (Hand(tripleArray[0]).getBasePoint() < Card_A)
        {
            for (CardPoint point = Card_3; point <= Card_A; point = CardPoint(point + 1))
            {
                int pointCount = leftCards.pointCount(point);
                if (pointCount == 1)
                {
                    Cards single = Strategy(s_player, leftCards).findSamePoint(point, 1);
                    if (!single.isEmpty())
                    {
                        Cards playCards = tripleArray[0];
                        playCards.add(single);

                        return playCards;
                    }
                }
                else if (pointCount == 2)
                {
                    Cards pair = Strategy(s_player, leftCards).findSamePoint(point, 2);
                    if (!pair.isEmpty())
                    {
                        Cards playCards = tripleArray[0];
                        playCards.add(pair);

                        return playCards;
                    }
                }

            }

            return tripleArray[0];
        }
    }

    if (hasSeqPair)
    {
        Cards maxSeqPair;
        for (int i = 0; i < seqPairArray.size(); i++)
        {
            if (seqPairArray[i].count() > maxSeqPair.count())
            {
                maxSeqPair = seqPairArray[i];
            }
        }

        return maxSeqPair;
    }

    Player* nextPlayer = s_player->getNextPlayer();

    if (s_player->getRole() != nextPlayer->getRole() &&
        nextPlayer->getCards().count() == 1)
    {
        for (CardPoint point = CardPoint(Card_End - 1); point >= Card_3; point = CardPoint(point - 1))
        {
            int pointCount = leftCards.pointCount(point);
            if (pointCount == 1)
            {
                Cards single = Strategy(s_player, leftCards).findSamePoint(point, 1);
                return single;
            }
            else if (pointCount == 2)
            {
                Cards pair = Strategy(s_player, leftCards).findSamePoint(point, 2);
                return pair;
            }
        }
    }
    else
    {
        for (CardPoint point = Card_3; point < Card_End; point = CardPoint(point + 1))
        {
            int pointCount = leftCards.pointCount(point);
            if (pointCount == 1)
            {
                Cards single = Strategy(s_player, leftCards).findSamePoint(point, 1);
                return single;
            }
            else if (pointCount == 2)
            {
                Cards pair = Strategy(s_player, leftCards).findSamePoint(point, 2);
                return pair;
            }
        }
    }

    return Cards();
}


bool Strategy::whetherToBeat(const Cards& myCards)
{
    if (myCards.isEmpty()) return true;

    Player* punchPlayer = s_player->getPunchPlayer();
    Cards punchCards = s_player->getPunchCards();

    if (s_player->getRole() == punchPlayer->getRole())
    {
        Cards left = player_cards;
        left.remove(myCards);
        if (Hand(left).getType() != card_Unknown) return true;

        CardPoint basePoint = Hand(myCards).getBasePoint();
        if (basePoint == Card_2 || basePoint == Card_SJ || basePoint == Card_BJ)
        {
            return false;
        }
    }
    else
    {
        Hand myHand(myCards);

        if ( (myHand.getType() == card_Triple_with_Single || myHand.getType() == card_Triple_with_Pair) &&
             (myHand.getBasePoint() == Card_2) )
        {
            return false;
        }

        if (myHand.getType() == card_Pair && myHand.getBasePoint() == Card_2 &&
            punchPlayer->getCards().count() >= 10 && s_player->getCards().count() >= 5)
        {
            return false;
        }
    }

    return true;
}

Cards Strategy::makeStrategy()
{
    Player* punchPlayer = s_player->getPunchPlayer();
    Cards punchCards = s_player->getPunchCards();

    if (punchPlayer == s_player || punchPlayer == nullptr)
    {
        return playFirst();
    }
    else
    {
        Hand punchHand(punchCards);
        Cards beatCards = playBeatHand(punchHand);

        bool shouldBeat = whetherToBeat(beatCards);
        if (shouldBeat)
        {
            return beatCards;
        }
        else
        {
            return Cards();
        }
    }

    return Cards();
}
