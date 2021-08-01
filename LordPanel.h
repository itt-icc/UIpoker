#ifndef LORDPANEL_H
#define LORDPANEL_H

#include <QWidget>
#include <QResizeEvent>
#include <QMap>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include "LordControl.h"
#include "CardPicture.h"
#include "MarkBoard.h"

class LordPanel : public QWidget
{
    Q_OBJECT
public:
    explicit LordPanel(QWidget *parent = 0);
protected:
    enum CardsAlign
    {
        Horizontal,
        Vertical
    };

    struct PlayerContext
    {
        QRect cardsRect;
        QRect playHandRect;
        CardsAlign cardsAlign;
        bool isFrontSide;
        Cards lastCards;
        QLabel* info;
        QLabel* rolePic;
    };
    QMap<Player*, PlayerContext> lp_playerContextMap;
private:
    LordControl* lp_gameControl;

    // 发牌动画时底牌和移动牌图片
    CardPicture* lp_baseCard;
    CardPicture* lp_movingCard;

    QVector<CardPicture*> lp_restThreeCards;

    // 牌背面的图像
    QPixmap lp_cardBackPic;

    QSize lp_cardSize;
    QPoint lp_baseCardPos;

    QMap<Card, CardPicture*> lp_cardPicMap;	// 每张卡片对应一个图片
    QSet<CardPicture*> lp_selectedCards;		// 用户出牌前选择的牌图片

    LordControl::GameStatus lp_gameStatus;

    QTimer* lp_pickCardTimer;	// 发牌计时器
    int lp_curMoveStep;

    QFrame* lp_userTool;
    QPushButton* lp_startButton;
    QPushButton* lp_continueButton;
    QPushButton* lp_playHandButton;
    QPushButton* lp_passButton;
    QPushButton* lp_bet0Button;
    QPushButton* lp_bet1Button;
    QPushButton* lp_bet2Button;
    QPushButton* lp_bet3Button;

    MarkBoard* lp_markBoard;
public slots:
    void OnStartBtn();
    void OnContinueBtn();
    void OnNoBet();
    void OnBet1();
    void OnBet2();
    void OnBet3();
    void OnPass();
    void OnPlayHand();
    void showPickingCardStep(Player* player, int step);
    void OnCardPicking();
    void OnCardSelected(Qt::MouseButton mouseButton);

    // 对接Player
    void OnPlayerCalledLord(Player* player, int bet);
    void OnPlayerPlayHand(Player* player, const Cards& cards);
    void OnPlayerPickCards(Player* player, const Cards& cards);
    void OnPlayerStatus(Player* player, LordControl::PlayerStatus playerStatus);
    void startPickCard();
    void hideLastCards(Player* player);
    void updateCards(Player* player);
    void processGameStatus(LordControl::GameStatus gameStatus);
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    void initCardPicMap();
    void cutCardPic(const QPixmap& cardsPic, int x, int y, Card card);	// 在卡片图左上角为(x, y)处抠下指定尺寸，与card对应起来
    void initControls();
    void initPlayerContext();
    void updateMarkBoard();

};

#endif // LORDPANEL_H
