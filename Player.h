#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "Cards.h"

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
public:
    enum Role {Lord,Farmer};
    enum Type {Unknow,User,Robot};
    void CallLord(int bet);
    void PlayHand(const Cards& cards);
signals:
    void yesCallLord(Player*, int);
    void yesPlayHand(Player*, const Cards&);
    void yesPickCards(Player*, const Cards&);
public slots:
    void OnPlayerPunch(Player* player, const Cards& cards);
public:
    void setPrevPlayer(Player* player);
    Player* getPrevPlayer();
    void setNextPlayer(Player* player);
    Player* getNextPlayer();
    void clearCards();
    void pickCards(const Cards& cards);
    void pickCard(const Card& card);
    virtual void startCallLord();
    virtual void startPlayHand();
    virtual void waitForCallLord();
    virtual void waitForPlayHand();
    Cards getCards();
    Player* getPunchPlayer();
    Cards getPunchCards();
    void setType(Type type);
    Type getType();
    void setRole(Role role);
    Role getRole();
    void setName(const QString& name);
    QString getName();
    int getMark();
    void setMark(int mark);
protected:
    Player* player_punchPlayer;
    Cards player_punchCards;
    Player* player_nextPlayer;
    Player* player_prevPlayer;
    QString player_name;
    Cards player_cards;
    Role player_role;
    Type player_type;
    int player_mark;
};

#endif // PLAYER_H
