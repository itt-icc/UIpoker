#include "LordControl.h"
#include <QMetaType>
#include "Hand.h"
#include <QMessageBox>
#include<QDebug>

Player* LordControl::getUserPlayer() {return lc_user;}
Player* LordControl::getLeftRobot() {return lc_Lrobot;}
Player* LordControl::getRightRobot() {return lc_Rrobot;}
Player* LordControl::getCurrentPlayer() {return lc_curPlayer;}
void LordControl::setCurrentPlayer(Player* curPlayer) {lc_curPlayer = curPlayer;}
Cards LordControl::getRestCards() {return lc_allCards;}
Player* LordControl::getPunchPlayer() {return lc_punchPlayer;}
Cards LordControl::getPunchCards() {return lc_punchCards;}
int LordControl::getCurBet() {return lc_curBet;}
void LordControl::setCurBet(int curBet) {lc_curBet = curBet;}
Card LordControl::pickOneCard()
{
    Card card = lc_allCards.takeRandomCard();
    return card;
}

void LordControl::clearPlayerMarks()
{
    lc_Lrobot->setMark(0);
    lc_Rrobot->setMark(0);
    lc_user->setMark(0);
}

LordControl::LordControl(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<Hand>("Hand");
    qRegisterMetaType<Card>("Card");
    qRegisterMetaType<Cards>("Cards");
    lc_punchPlayer = nullptr;
    lc_punchCards = Cards();
    lc_curBet = 0;
}

void LordControl::init()
{
    lc_Lrobot = new Robot(this);
    lc_Rrobot = new Robot(this);
    lc_user = new UserPlayer(this);
    lc_Lrobot->setName(tr("玩家2"));
    lc_Rrobot->setName(tr("玩家3"));
    lc_user->setName(tr("自己"));
    connect(lc_Lrobot, &Robot::yesCallLord, this, &LordControl::OnCalledLord);
    connect(lc_Rrobot,  &Robot::yesCallLord, this, &LordControl::OnCalledLord);
    connect(lc_user, &Robot::yesCallLord, this, &LordControl::OnCalledLord);
    connect(lc_Lrobot, &Robot::yesPlayHand, this, &LordControl::OnPlayedHand);
    connect(lc_Rrobot, &Robot::yesPlayHand, this, &LordControl::OnPlayedHand);
    connect(lc_user, &Robot::yesPlayHand, this, &LordControl::OnPlayedHand);
    connect(lc_Lrobot, &Robot::yesPickCards, this, &LordControl::OnPickCards);
    connect(lc_Rrobot, &Robot::yesPickCards, this, &LordControl::OnPickCards);
    connect(lc_user, &Robot::yesPickCards, this, &LordControl::OnPickCards);
    connect(this, &LordControl::tellPlayerPunch, lc_Lrobot, &Robot::OnPlayerPunch);
    connect(this, &LordControl::tellPlayerPunch, lc_Rrobot, &Robot::OnPlayerPunch);
    connect(this, &LordControl::tellPlayerPunch, lc_user, &Robot::OnPlayerPunch);
    lc_Lrobot->setPrevPlayer(lc_Rrobot);
    lc_Lrobot->setNextPlayer(lc_user);
    lc_Rrobot->setPrevPlayer(lc_user);
    lc_Rrobot->setNextPlayer(lc_Lrobot);
    lc_user->setPrevPlayer(lc_Lrobot);
    lc_user->setNextPlayer(lc_Rrobot);
    lc_curPlayer = lc_user;
}

void LordControl::addCard(CardType suit, CardPoint point)
{
    Card card;
    card.suit = suit;
    card.point = point;

    lc_allCards.add(card);
}

void LordControl::initCards()
{
    lc_allCards.clear();

    for (int p = Card_Begin + 1; p < Card_SJ; p++)
    {
        for (int s = Suit_Begin + 1; s < Suit_End; s++)
        {

            addCard((CardType)s, (CardPoint)p);
        }
    }

    addCard(Suit_Begin, Card_SJ);
    addCard(Suit_Begin, Card_BJ);
}

void LordControl::shuffleCards()
{

}

void LordControl::StartCallLord()
{
    lc_betList.clear();

    emit tellPlayerStatus(lc_curPlayer, LordControl::waitCallLord);
    lc_curPlayer->startCallLord();
}

void LordControl::OnCalledLord(Player* player, int bet)
{
    tellPlayerCalledLord(player, bet);

    if (bet == 3)	// 叫3分，直接当地主
    {
        lc_curBet = 3;

        player->setRole(Player::Lord);
        player->getPrevPlayer()->setRole(Player::Farmer);
        player->getNextPlayer()->setRole(Player::Farmer);

        lc_curPlayer = player;
        player->pickCards(lc_allCards);
        lc_allCards.clear();

        // 延迟一下，开始出牌过程
        QTimer::singleShot(1000, this, SLOT(StartPlayHand()));

        return;
    }

    BetRecrod record;
    record.player = player;
    record.bet = bet;
    lc_betList.push_back(record);
    if (lc_betList.size() == 3)
    {
        if (lc_betList[0].bet == lc_betList[1].bet &&
            lc_betList[1].bet == lc_betList[2].bet)	{emit tellGameStatus(PickingCard);}	// 三人叫分一样，重新发牌
        else
        {
            BetRecrod record;
            if (lc_betList[0].bet >= lc_betList[1].bet){record = lc_betList[0];}
            else {record = lc_betList[1];}
            if (record.bet < lc_betList[2].bet){record = lc_betList[2];}
            record.player->setRole(Player::Lord);
            record.player->getPrevPlayer()->setRole(Player::Farmer);
            record.player->getNextPlayer()->setRole(Player::Farmer);
            lc_curPlayer = record.player;
            lc_curBet = record.bet;
            lc_curPlayer->pickCards(lc_allCards);
            lc_allCards.clear();
            QTimer::singleShot(1000, this, SLOT(StartPlayHand()));
        }
        return;
    }
    Player* nextPlayer = player->getNextPlayer();
    lc_curPlayer = nextPlayer;
    emit tellPlayerStatus(lc_curPlayer, LordControl::waitCallLord);
    lc_curPlayer->startCallLord();
}

void LordControl::StartPlayHand()
{
    qDebug()<<"出牌";
    emit tellGameStatus(PlayingHand);
    emit tellPlayerStatus(lc_curPlayer, LordControl::waitPlayHand);
    lc_curPlayer->startPlayHand();
}

void LordControl::OnPlayedHand(Player* player, const Cards& cards)
{
    if (!cards.isEmpty())
    {
        lc_punchPlayer = player;
        lc_punchCards = cards;
        tellPlayerPunch(lc_punchPlayer, lc_punchCards);
    }
    HandType handType = Hand(cards).getType();
    if (handType == card_Bomb || handType == card_Rocket) {lc_curBet = lc_curBet * 2;}
    emit tellPlayerPlayHand(lc_curPlayer, cards);
    // player已把牌都出完，计算三方得分
    if (player->getCards().isEmpty())
    {
        Player* prev = player->getPrevPlayer();
        Player* next = player->getNextPlayer();
        if (player->getRole() == Player::Lord)
        {
            player->setMark(player->getMark() + 2 * lc_curBet);
            prev->setMark(prev->getMark() - lc_curBet);
            next->setMark(next->getMark() - lc_curBet);
        }
        else
        {
            player->setMark(player->getMark() + lc_curBet);
            if (prev->getRole() == Player::Lord)
            {
                prev->setMark(prev->getMark() - 2 * lc_curBet);
                next->setMark(next->getMark() + lc_curBet);
            }
            else
            {
                prev->setMark(prev->getMark() + lc_curBet);
                next->setMark(next->getMark() - 2 * lc_curBet);
            }
        }
        emit tellPlayerStatus(player, LordControl::WinningStatus);
        return;
    }
    Player* nextPlayer = player->getNextPlayer();
    lc_curPlayer = nextPlayer;
    emit tellPlayerStatus(lc_curPlayer, LordControl::waitPlayHand);
    lc_curPlayer->startPlayHand();
}
void LordControl::OnPickCards(Player* player, const Cards& cards)
{
    emit tellPlayerPickCards(player, cards);
}

void LordControl::clear()
{
    initCards();
    lc_Lrobot->clearCards();
    lc_Rrobot->clearCards();
    lc_user->clearCards();
    lc_punchPlayer = nullptr;
    lc_punchCards.clear();
    tellPlayerPunch(lc_punchPlayer, lc_punchCards);
    lc_betList.clear();
}

