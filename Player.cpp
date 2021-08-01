#include "Player.h"


/*

1.	首先通过搜索找到一个能接牌的最佳牌组，如果找不到，就结束了；
2.	如果是队友的牌，相对还比较大，我不接
3.	如果是队友的牌，我肯定不用大牌接；
4.	如果是地主的牌，我的牌绝对得好，肯定接；
5.	如果是地主的牌，接了之后我的牌不变差太多，接；
6.	如果是地主的牌，我的接牌比他大得不多（比如王对2），接；
7.	如果是地主的牌，接牌是顺子、飞机、连对这类，接；
8.	如果是地主的牌，我要动用炸弹，如果他牌还很多，出的又不是王和2之类，不接；
9.	如果是地主的牌，他的牌很少，或者他出大牌了，接。

*/


Player::Player(QObject *parent) :
    QObject(parent)
{
    player_type = Robot;
    player_role = Farmer;
    player_punchPlayer = nullptr;
    player_mark = 0;
}

void Player::setType(Player::Type type){player_type = type;}
Player::Type Player::getType() {return player_type;}
void Player::setRole(Player::Role role) {player_role = role;}
Player::Role Player::getRole() {return player_role;}
void Player::setName(const QString& name) {player_name = name;}
void Player::setNextPlayer(Player* player) {player_nextPlayer = player;}
Player* Player::getNextPlayer() {return player_nextPlayer;}
QString Player::getName() {return player_name;}
int Player::getMark() {return player_mark;}
void Player::setMark(int mark) {player_mark = mark;}
void Player::setPrevPlayer(Player* player) {player_prevPlayer = player;}
Player* Player::getPrevPlayer() {return player_prevPlayer;}
void Player::clearCards() {player_cards.clear();}
void Player::pickCards(const Cards& cards)
{
    player_cards.add(cards);
    emit yesPickCards(this, cards);
}
void Player::pickCard(const Card& card)
{
    Cards cards;
    cards << card;
    pickCards(cards);
}
void Player::startCallLord(){}
void Player::startPlayHand() {}
void Player::waitForCallLord(){}
void Player::waitForPlayHand() {}
void Player::CallLord(int bet) {emit yesCallLord(this, bet);}
void Player::PlayHand(const Cards& cards)
{
    player_cards.remove(cards);
    emit yesPlayHand(this, cards);
}
Cards Player::getCards() {return player_cards;}
void Player::OnPlayerPunch(Player* player, const Cards& cards)
{
    player_punchPlayer = player;
    player_punchCards = cards;
}
Player* Player::getPunchPlayer() {return player_punchPlayer;}
Cards Player::getPunchCards() {return player_punchCards;}
