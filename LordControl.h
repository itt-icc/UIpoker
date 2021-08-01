#ifndef LORDCONTROL_H
#define LORDCONTROL_H

#include <QObject>
#include "Robot.h"
#include "UserPlayer.h"
#include <QTimer>
#include <QVector>

class LordControl : public QObject
{
    Q_OBJECT
public:
    explicit LordControl(QObject *parent = nullptr);

protected:
    struct BetRecrod
    {
        Player* player;
        int bet;
    };
protected:
    Robot* lc_Lrobot;
    Robot* lc_Rrobot;
    UserPlayer* lc_user;
    Player* lc_curPlayer;
    int lc_curBet;
    Cards lc_allCards;
    // 当前出有效牌的玩家和牌
    Player* lc_punchPlayer;
    Cards lc_punchCards;
    QVector<BetRecrod> lc_betList;
public:
    enum GameStatus
    {
        PickingCard,
        CallingLord,
        PlayingHand,
    };

    enum PlayerStatus
    {
        waitCallLord,
        waitPlayHand,
        WinningStatus
    };
    void initCards();		// 初始化所有牌，填充到allCards
    void addCard(CardType suit, CardPoint point);
    void shuffleCards();	// 洗牌
    void init();		// 初始化玩家，信号槽
    void clear();
    Player* getUserPlayer();
    Player* getLeftRobot();
    Player* getRightRobot();
    Player* getCurrentPlayer();
    void setCurrentPlayer(Player* curPlayer);
    Cards getRestCards();
    Card pickOneCard();
    Player* getPunchPlayer();
    Cards getPunchCards();
    int getCurBet();
    void setCurBet(int curBet);
    void clearPlayerMarks();
signals:
    void tellPlayerCalledLord(Player*, int);	// 玩家叫分
    void tellPlayerPlayHand(Player*, const Cards& cards);
    void tellPlayerPickCards(Player*, const Cards& cards);
    void tellPlayerPunch(Player*, const Cards& cards);
    void tellGameStatus(LordControl::GameStatus);
    void tellPlayerStatus(Player*, LordControl::PlayerStatus);
public slots:
    void OnCalledLord(Player* player, int bet);
    void OnPlayedHand(Player* player, const Cards& cards);
    void OnPickCards(Player* player, const Cards& cards);
    void StartCallLord();	// 开始叫分
    void StartPlayHand();	// 开始出牌
};

#endif // LORDCONTROL_H
