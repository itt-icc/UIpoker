#include "LordPanel.h"
#include <QPainter>
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QHBoxLayout>
#include "Hand.h"
#include "Strategy.h"


void LordPanel::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    lp_markBoard->setGeometry(rect().right() - 150, 0, 130, 80);
    lp_baseCardPos = QPoint((width() - lp_cardSize.width()) / 2, height() / 2 - 100);
    lp_baseCard->move(lp_baseCardPos);
    lp_movingCard->move(lp_baseCardPos);
    QMap<Player*, PlayerContext>::Iterator it;
    it = lp_playerContextMap.find(lp_gameControl->getLeftRobot());
    it->cardsRect = QRect(10, 180, 100, height() - 200);
    it->playHandRect = QRect(180, 150, 100, 100);
    it->rolePic->move(it->cardsRect.left(), it->cardsRect.top() - 100);
    it = lp_playerContextMap.find(lp_gameControl->getRightRobot());
    it->cardsRect = QRect(rect().right() - 110, 180, 100, height() - 200);
    it->playHandRect = QRect(rect().right() - 280, 150, 100, 100);
    it->rolePic->move(it->cardsRect.left(), it->cardsRect.top() - 100);
    it = lp_playerContextMap.find(lp_gameControl->getUserPlayer());
    it->cardsRect = QRect(250, rect().bottom() - 120, width() - 500, 100);
    it->playHandRect = QRect(150, rect().bottom() - 280, width() - 300, 100);
    it->rolePic->move(it->cardsRect.right(), it->cardsRect.top() - 10);
    QRect rectUserTool(it->playHandRect.left(), it->playHandRect.bottom(),
        it->playHandRect.width(), it->cardsRect.top() - it->playHandRect.bottom());
    lp_userTool->setGeometry(rectUserTool);
    it = lp_playerContextMap.begin();
    while(it != lp_playerContextMap.end()){
        QRect playCardsRect = it->playHandRect;
        QLabel* infoLabel = it->info;
        QPoint pt( playCardsRect.left() + (playCardsRect.width() - infoLabel->width()) / 2,
            playCardsRect.top() + (playCardsRect.height() - infoLabel->height()) / 2 );
        infoLabel->move(pt);
        it++;
    }
    /*最后的三张地主的牌*/
    int base = (width() - 3 * lp_cardSize.width() - 2 * 10) / 2;
    for (int i = 0; i < lp_restThreeCards.size(); i++)
    {
        lp_restThreeCards[i]->move(base + (lp_cardSize.width() + 10) * i, 20);
    }
    updateCards(lp_gameControl->getUserPlayer());
    updateCards(lp_gameControl->getLeftRobot());
    updateCards(lp_gameControl->getRightRobot());
}
void LordPanel::initCardPicMap()
{
    QPixmap cardsPic(":/res/card.png");

    lp_cardBackPic = cardsPic.copy(2 * lp_cardSize.width(), 4 * lp_cardSize.height(),
        lp_cardSize.width(), lp_cardSize.height());
    int i = 0, j = 0;
    for (int suit = 1, i = 0; suit < 5; suit++, i++)
        for (int pt = 1, j = 0; pt < 14; pt++, j++)
        {
            Card card;
            card.point = (CardPoint)pt;
            card.suit = (CardType)suit;
            cutCardPic(cardsPic, j * lp_cardSize.width(), i * lp_cardSize.height(), card);
        }
    Card card;
    card.point = Card_SJ;
    card.suit = Suit_Begin;
    cutCardPic(cardsPic, 0, 4 * lp_cardSize.height(), card);
    card.point = Card_BJ;
    card.suit = Suit_Begin;
    cutCardPic(cardsPic, lp_cardSize.width(), 4 * lp_cardSize.height(), card);
}

void LordPanel::paintEvent(QPaintEvent* event)
{
    static QPixmap bk(":/res/table.png");
    QPainter painter(this);
    painter.drawPixmap(rect(), bk);
}


void LordPanel::updateMarkBoard()
{
    lp_markBoard->SetMarks(lp_gameControl->getUserPlayer()->getMark(),
                          lp_gameControl->getLeftRobot()->getMark(),
                          lp_gameControl->getRightRobot()->getMark());
}

void LordPanel::OnCardPicking()
{
    Player* curPlayer = lp_gameControl->getCurrentPlayer();
    if (lp_curMoveStep >= 100)	// 牌已移动到末端
    {
        Card card = lp_gameControl->pickOneCard();
        curPlayer->pickCard(card);

        showPickingCardStep(curPlayer, lp_curMoveStep);
        lp_gameControl->setCurrentPlayer(curPlayer->getNextPlayer());

        lp_curMoveStep = 0;

        if (lp_gameControl->getRestCards().count() == 3)	// 还剩下3张牌，摸牌结束
        {
            lp_pickCardTimer->stop();
            processGameStatus(LordControl::CallingLord);
        }
    }

    showPickingCardStep(curPlayer, lp_curMoveStep);
    lp_curMoveStep += 14;
}

void LordPanel::showPickingCardStep(Player* player, int step)
{
    QRect cardsRect = lp_playerContextMap[player].cardsRect;

    if (player == lp_gameControl->getLeftRobot())
    {
        int unit = (lp_baseCardPos.x() - cardsRect.right()) / 100;
        lp_movingCard->move(lp_baseCardPos.x() - step * unit, lp_baseCardPos.y());
    }
    else if (player == lp_gameControl->getRightRobot())
    {
        int unit = (cardsRect.left() - lp_baseCardPos.x()) / 100;
        lp_movingCard->move(lp_baseCardPos.x() + step * unit, lp_baseCardPos.y());
    }
    else if (player == lp_gameControl->getUserPlayer())
    {
        int unit = (cardsRect.top() - lp_baseCardPos.y()) / 100;
        lp_movingCard->move(lp_baseCardPos.x(), lp_baseCardPos.y() + step * unit);
    }

    if (step == 0) {lp_movingCard->show();}

    if (step >= 100) {lp_movingCard->hide();}

    update();
}

void LordPanel::OnCardSelected(Qt::MouseButton mouseButton)
{
    if (lp_gameStatus == LordControl::PickingCard) return;

    CardPicture* cardPic = (CardPicture*)sender();
    if (cardPic->getOwner() != lp_gameControl->getUserPlayer()) return;

    if (mouseButton == Qt::LeftButton)
    {
        cardPic->setSelected(!cardPic->isSelected());
        updateCards(cardPic->getOwner());

        QSet<CardPicture*>::Iterator it = lp_selectedCards.find(cardPic);
        if (it == lp_selectedCards.end())	{lp_selectedCards.insert(cardPic);}	// 选中牌
        else {lp_selectedCards.erase(it);}// 取消选中牌
    }
    else if (mouseButton == Qt::RightButton) {OnPlayHand();}	// 右击出牌
}

void LordPanel::hideLastCards(Player* player)
{
    QMap<Player*, PlayerContext>::Iterator it = lp_playerContextMap.find(player);
    if (it != lp_playerContextMap.end())
    {
        if (it->lastCards.isEmpty()) {it->info->hide();}	// 上一次打的空牌，即pass
        else
        {
            CardList lastCardList = it->lastCards.toCardList();
            CardList::ConstIterator itLast = lastCardList.constBegin();
            for (; itLast != lastCardList.constEnd(); itLast++)
            {
                lp_cardPicMap[*itLast]->hide();
            }
        }
    }
}

void LordPanel::updateCards(Player* player)
{
    Cards restCards = player->getCards();
    CardList restCardList = restCards.toCardList(Cards::Desc);

    const int cardSpacing = 20;		// 牌间隔

    // 显示剩下的牌
    QRect cardsRect = lp_playerContextMap[player].cardsRect;
    CardList::ConstIterator itRest = restCardList.constBegin();
    for (int i = 0; itRest != restCardList.constEnd(); itRest++, i++)
    {
        CardPicture* cardPic = lp_cardPicMap[*itRest];
        cardPic->setFrontSide(lp_playerContextMap[player].isFrontSide);
        cardPic->show();
        cardPic->raise();

        if (lp_playerContextMap[player].cardsAlign == Horizontal)
        {
            int leftBase = cardsRect.left() + (cardsRect.width() - (restCardList.size() - 1) * cardSpacing - cardPic->width()) / 2;
            int top = cardsRect.top() + (cardsRect.height() - cardPic->height()) / 2;
            if (cardPic->isSelected()) top -= 10;

            cardPic->move(leftBase + i * cardSpacing, top);
        }
        else
        {
            int left = cardsRect.left() + (cardsRect.width() - cardPic->width()) / 2;
            if (cardPic->isSelected()) left += 10;
            int topBase = cardsRect.top() + (cardsRect.height() - (restCardList.size() - 1) * cardSpacing - cardPic->height()) / 2;
            cardPic->move(left, topBase + i * cardSpacing);
        }
    }

    // 显示上一次打出去的牌
    QRect playCardsRect = lp_playerContextMap[player].playHandRect;
    if (!lp_playerContextMap[player].lastCards.isEmpty())		// 不是空牌
    {
        int playSpacing = 24;
        CardList lastCardList = lp_playerContextMap[player].lastCards.toCardList();
        CardList::ConstIterator itPlayed = lastCardList.constBegin();
        for (int i = 0; itPlayed != lastCardList.constEnd(); itPlayed++, i++)
        {
            CardPicture* cardPic = lp_cardPicMap[*itPlayed];
            cardPic->setFrontSide(true);
            cardPic->raise();

            if (lp_playerContextMap[player].cardsAlign == Horizontal)
            {
                int leftBase = playCardsRect.left () +
                    (playCardsRect.width() - (lastCardList.size() - 1) * playSpacing - cardPic->width()) / 2;
                int top = playCardsRect.top() + (playCardsRect.height() - cardPic->height()) / 2;
                cardPic->move(leftBase + i * playSpacing, top);
            }
            else
            {
                int left = playCardsRect.left() + (playCardsRect.width() - cardPic->width()) / 2;
                int topBase = playCardsRect.top();
                cardPic->move(left, topBase + i * playSpacing);
            }
        }
    }
}

void LordPanel::OnPlayerCalledLord(Player* player, int bet)
{
    QString betInfo;
    if (bet == 0) {betInfo = tr("不叫");}

    else if (bet == 1) {betInfo = tr("一分");}

    else if (bet == 2) {betInfo = tr("两分");}

    else if (bet == 3) {betInfo = tr("三分");}


    QLabel* infoLabel = lp_playerContextMap[player].info;
    infoLabel->setText(betInfo);
    infoLabel->show();

    // 用户叫完分，隐藏叫分按钮
    if (player == lp_gameControl->getUserPlayer())
    {
        lp_bet0Button->hide();
        lp_bet1Button->hide();
        lp_bet2Button->hide();
        lp_bet3Button->hide();
    }
}

void LordPanel::OnPlayerPlayHand(Player* player, const Cards& cards)
{
    hideLastCards(player);

    // 记录上一次打的牌
    QMap<Player*, PlayerContext>::Iterator itContext = lp_playerContextMap.find(player);
    itContext->lastCards = cards;

    // 打空牌，显示“不要”
    if (cards.isEmpty())
    {
        itContext->info->setText(tr("要不起啊"));
        itContext->info->show();
    }

    updateCards(player);
}

void LordPanel::OnPlayerPickCards(Player* player, const Cards& cards)
{
    CardList cardList = cards.toCardList();
    CardList::ConstIterator it = cardList.constBegin();
    for (; it != cardList.constEnd(); it++)
    {
        CardPicture* pickedCard = lp_cardPicMap[*it];
        pickedCard->setOwner(player);
    }

    updateCards(player);
}

void LordPanel::OnNoBet()
{
    lp_gameControl->getUserPlayer()->CallLord(0);
}

void LordPanel::OnBet1()
{
    lp_gameControl->getUserPlayer()->CallLord(1);
}

void LordPanel::OnBet2()
{
    lp_gameControl->getUserPlayer()->CallLord(2);
}

void LordPanel::OnBet3()
{
    lp_gameControl->getUserPlayer()->CallLord(3);
}

void LordPanel::OnPass()
{
    if (lp_gameControl->getCurrentPlayer() != lp_gameControl->getUserPlayer()) return;

    Player* punchPlayer = lp_gameControl->getPunchPlayer();
    if (punchPlayer == lp_gameControl->getUserPlayer() || punchPlayer == NULL) return;

    lp_gameControl->getUserPlayer()->PlayHand(Cards());

    QSet<CardPicture*>::ConstIterator it = lp_selectedCards.constBegin();
    for (; it != lp_selectedCards.constEnd(); it++)
    {
        (*it)->setSelected(false);
    }
    lp_selectedCards.clear();

    updateCards(lp_gameControl->getUserPlayer());
}

void LordPanel::OnPlayHand()
{
    if (lp_gameStatus != LordControl::PlayingHand)
    {
        return;
    }

    if (lp_gameControl->getCurrentPlayer() != lp_gameControl->getUserPlayer())
    {
        return;
    }

    if (lp_selectedCards.isEmpty()) return;

    Cards playCards;
    QSet<CardPicture*>::const_iterator it = lp_selectedCards.constBegin();
    for (; it != lp_selectedCards.constEnd(); it++)
    {
        playCards.add((*it)->getCard());
    }

    Hand hand(playCards);
    HandType ht = hand.getType();
    CardPoint bp = hand.getBasePoint();
    int ex = hand.getExtra();

    if (ht == card_Unknown)	{return;}	// 未知牌型，不允许出


    // 管不住其他人的牌
    if (lp_gameControl->getPunchPlayer() != lp_gameControl->getUserPlayer())
    {
        if (!hand.canBeat(Hand(lp_gameControl->getPunchCards()))){return;}
    }

    lp_gameControl->getUserPlayer()->PlayHand(playCards);
    lp_selectedCards.clear();
}

void LordPanel::OnStartBtn()
{
    lp_startButton->hide();

    lp_gameControl->clearPlayerMarks();
    updateMarkBoard();

    processGameStatus(LordControl::PickingCard);
}

void LordPanel::OnContinueBtn()
{
    lp_continueButton->hide();
    processGameStatus(LordControl::PickingCard);
}

void LordPanel::startPickCard()
{
    QMap<Card, CardPicture*>::const_iterator it = lp_cardPicMap.constBegin();
    for (; it != lp_cardPicMap.constEnd(); it++)
    {
        it.value()->setSelected(false);
        it.value()->setFrontSide(true);
        it.value()->hide();
    }

    for (int i = 0; i < lp_restThreeCards.size(); i++)
    {
        lp_restThreeCards[i]->hide();
    }

    lp_selectedCards.clear();
    lp_playerContextMap[lp_gameControl->getLeftRobot()].lastCards.clear();
    lp_playerContextMap[lp_gameControl->getRightRobot()].lastCards.clear();
    lp_playerContextMap[lp_gameControl->getUserPlayer()].lastCards.clear();

    lp_playerContextMap[lp_gameControl->getLeftRobot()].info->hide();
    lp_playerContextMap[lp_gameControl->getRightRobot()].info->hide();
    lp_playerContextMap[lp_gameControl->getUserPlayer()].info->hide();

    lp_playerContextMap[lp_gameControl->getLeftRobot()].isFrontSide = false;
    lp_playerContextMap[lp_gameControl->getRightRobot()].isFrontSide = false;
    lp_playerContextMap[lp_gameControl->getUserPlayer()].isFrontSide = true;

    lp_playerContextMap[lp_gameControl->getLeftRobot()].rolePic->hide();
    lp_playerContextMap[lp_gameControl->getRightRobot()].rolePic->hide();
    lp_playerContextMap[lp_gameControl->getUserPlayer()].rolePic->hide();

    lp_gameControl->clear();

    lp_baseCard->show();

    lp_bet0Button->hide();
    lp_bet1Button->hide();
    lp_bet2Button->hide();
    lp_bet3Button->hide();

    lp_playHandButton->hide();
    lp_passButton->hide();

    lp_pickCardTimer->start();
}

void LordPanel::processGameStatus(LordControl::GameStatus gameStatus)
{
    lp_gameStatus = gameStatus;

    if (gameStatus == LordControl::PickingCard) {startPickCard();}

    else if (gameStatus == LordControl::CallingLord)
    {
        CardList restThreeCards = lp_gameControl->getRestCards().toCardList();

        for (int i = 0; i < lp_restThreeCards.size(); i++)
        {
            lp_restThreeCards[i]->setPic(lp_cardPicMap[restThreeCards[i]]->getPic(), lp_cardBackPic);
            lp_restThreeCards[i]->hide();
        }

        lp_gameControl->StartCallLord();
    }
    else if (gameStatus == LordControl::PlayingHand)
    {
        lp_baseCard->hide();
        lp_movingCard->hide();

        for (int i = 0; i < lp_restThreeCards.size(); i++)
        {
            lp_restThreeCards[i]->show();
        }

        QMap<Player*, PlayerContext>::ConstIterator it = lp_playerContextMap.constBegin();
        for (; it != lp_playerContextMap.constEnd(); it++)
        {
            it.value().info->hide();

            if (it.key()->getRole() == Player::Lord)
            {
                it.value().rolePic->setPixmap(QPixmap(":/res/lord.png"));
                it.value().rolePic->show();
            }
            else
            {
                it.value().rolePic->setPixmap(QPixmap(":/res/farmer.png"));
                it.value().rolePic->show();
            }
        }
    }
}

void LordPanel::OnPlayerStatus(Player* player, LordControl::PlayerStatus playerStatus)
{
    if (playerStatus == LordControl::waitCallLord)
    {
        if (player == lp_gameControl->getUserPlayer())	// 用户叫分，显示叫分按钮
        {
            lp_bet0Button->show();
            lp_bet1Button->show();
            lp_bet2Button->show();
            lp_bet3Button->show();
        }
    }
    else if (playerStatus == LordControl::waitPlayHand)
    {
        hideLastCards(player);

        if (player == lp_gameControl->getUserPlayer())
        {
            lp_playHandButton->show();

            Player* punchPlayer = lp_gameControl->getPunchPlayer();
            if (punchPlayer == lp_gameControl->getUserPlayer() || punchPlayer == nullptr)
            {
                lp_passButton->hide();
            }
            else
            {
                lp_passButton->show();
            }
        }
        else
        {
            lp_playHandButton->hide();
            lp_passButton->hide();
        }
    }
    else if (lp_gameStatus == LordControl::WinningStatus)
    {
        lp_playerContextMap[lp_gameControl->getLeftRobot()].isFrontSide = true;
        lp_playerContextMap[lp_gameControl->getRightRobot()].isFrontSide = true;
        updateCards(lp_gameControl->getLeftRobot());
        updateCards(lp_gameControl->getRightRobot());
        updateMarkBoard();
        QString str = tr("%1 赢得了游戏.");
        str = str.arg(player->getName());
        QMessageBox::information(this, tr("提示"), str);
        lp_playHandButton->hide();
        lp_passButton->hide();
        lp_continueButton->show();
        // 赢者下一局先叫分
        lp_gameControl->setCurrentPlayer(player);
    }
}


LordPanel::LordPanel(QWidget *parent) :
    QWidget(parent)
{
    lp_gameControl = new LordControl(this);
    lp_gameControl->init();

    lp_cardSize = QSize(80, 105);

    lp_markBoard = new MarkBoard(this);
    lp_markBoard->show();
    updateMarkBoard();

    initCardPicMap();
    initControls();
    initPlayerContext();

    connect(lp_gameControl, &LordControl::tellPlayerCalledLord, this, &LordPanel::OnPlayerCalledLord);
    connect(lp_gameControl, &LordControl::tellPlayerPlayHand, this, &LordPanel::OnPlayerPlayHand);
    connect(lp_gameControl, &LordControl::tellPlayerPickCards, this, &LordPanel::OnPlayerPickCards);
    connect(lp_gameControl, &LordControl::tellGameStatus, this, &LordPanel::processGameStatus);
    connect(lp_gameControl, &LordControl::tellPlayerStatus, this, &LordPanel::OnPlayerStatus);

    lp_baseCard = new CardPicture(this);
    lp_movingCard = new CardPicture(this);
    lp_baseCard->setPic(lp_cardBackPic, lp_cardBackPic);
    lp_movingCard->setPic(lp_cardBackPic, lp_cardBackPic);

    // 剩下3张牌
    CardPicture* restCardPic = new CardPicture(this);
    lp_restThreeCards << restCardPic;
    restCardPic = new CardPicture(this);
    lp_restThreeCards << restCardPic;
    restCardPic = new CardPicture(this);
    lp_restThreeCards << restCardPic;

    // 发牌效果所用的计时器
    lp_curMoveStep = 0;
    lp_pickCardTimer = new QTimer(this);
    lp_pickCardTimer->setInterval(5);
    connect(lp_pickCardTimer, &QTimer::timeout, this, &LordPanel::OnCardPicking);
}

void LordPanel::initPlayerContext()
{
    PlayerContext context;

    context.cardsAlign = Horizontal;
    context.isFrontSide = true;
    lp_playerContextMap.insert(lp_gameControl->getUserPlayer(), context);

    context.cardsAlign = Vertical;
    context.isFrontSide = false;
    lp_playerContextMap.insert(lp_gameControl->getLeftRobot(), context);

    context.cardsAlign = Vertical;
    context.isFrontSide = false;
    lp_playerContextMap.insert(lp_gameControl->getRightRobot(), context);

    QMap<Player*, PlayerContext>::Iterator it = lp_playerContextMap.begin();
    for (; it != lp_playerContextMap.end(); it++)
    {
        it->info = new QLabel(this);
        it->info->resize(100, 50);
        it->info->setObjectName("info");
        it->info->hide();
        it->rolePic = new QLabel(this);
        it->rolePic->resize(84, 120);
        it->rolePic->hide();
    }
}

void LordPanel::initControls()
{
    lp_userTool = new QFrame(this);
    QHBoxLayout* horLayout = new QHBoxLayout(lp_userTool);
    horLayout->setSpacing(9);
    horLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    lp_startButton = new QPushButton(lp_userTool);
    lp_startButton->setObjectName("button");
    lp_startButton->setFocusPolicy(Qt::NoFocus);
    lp_startButton->setText(tr("游戏开始"));
    lp_startButton->show();
    lp_startButton->setFixedSize(89, 32);
    horLayout->addWidget(lp_startButton);
    connect(lp_startButton, &QPushButton::clicked, this, &LordPanel::OnStartBtn);

    lp_continueButton = new QPushButton(lp_userTool);
    lp_continueButton->setObjectName("button");
    lp_continueButton->setFocusPolicy(Qt::NoFocus);
    lp_continueButton->setText(tr("继续游戏"));
    lp_continueButton->hide();
    lp_continueButton->setFixedSize(89, 32);
    horLayout->addWidget(lp_continueButton);
    connect(lp_continueButton, &QPushButton::clicked, this, &LordPanel::OnContinueBtn);

    lp_passButton = new QPushButton(lp_userTool);
    lp_passButton->setObjectName("button");
    lp_passButton->setFocusPolicy(Qt::NoFocus);
    lp_passButton->setText(tr("过牌"));
    lp_passButton->hide();
    lp_passButton->setFixedSize(89, 32);
    horLayout->addWidget(lp_passButton);
    connect(lp_passButton, &QPushButton::clicked, this, &LordPanel::OnPass);

    lp_playHandButton = new QPushButton(lp_userTool);
    lp_playHandButton->setObjectName("button");
    lp_playHandButton->setFocusPolicy(Qt::NoFocus);
    lp_playHandButton->setText(tr("发牌"));
    lp_playHandButton->hide();
    lp_playHandButton->setFixedSize(89, 32);
    QFont font = lp_playHandButton->font();
    font.setBold(true);
    font.setPixelSize(14);
    lp_playHandButton->setFont(font);
    horLayout->addWidget(lp_playHandButton);
    connect(lp_playHandButton, &QPushButton::clicked, this, &LordPanel::OnPlayHand);

    lp_bet0Button = new QPushButton(lp_userTool);
    lp_bet0Button->setObjectName("button");
    lp_bet0Button->setFocusPolicy(Qt::NoFocus);
    lp_bet0Button->setText(tr("不叫"));
    lp_bet0Button->hide();
    lp_bet0Button->setFixedSize(89, 32);
    horLayout->addWidget(lp_bet0Button);
    connect(lp_bet0Button, &QPushButton::clicked, this, &LordPanel::OnNoBet);

    lp_bet1Button = new QPushButton(lp_userTool);
    lp_bet1Button->setObjectName("button");
    lp_bet1Button->setFocusPolicy(Qt::NoFocus);
    lp_bet1Button->setText(tr("一分"));
    lp_bet1Button->hide();
    lp_bet1Button->setFixedSize(89, 32);
    horLayout->addWidget(lp_bet1Button);
    connect(lp_bet1Button, &QPushButton::clicked, this, &LordPanel::OnBet1);

    lp_bet2Button = new QPushButton(lp_userTool);
    lp_bet2Button->setObjectName("button");
    lp_bet2Button->setFocusPolicy(Qt::NoFocus);
    lp_bet2Button->setText(tr("两分"));
    lp_bet2Button->hide();
    lp_bet2Button->setFixedSize(89, 32);
    horLayout->addWidget(lp_bet2Button);
    connect(lp_bet2Button, &QPushButton::clicked, this, &LordPanel::OnBet2);

    lp_bet3Button = new QPushButton(lp_userTool);
    lp_bet3Button->setObjectName("button");
    lp_bet3Button->setFocusPolicy(Qt::NoFocus);
    lp_bet3Button->setText(tr("三分"));
    lp_bet3Button->hide();
    lp_bet3Button->setFixedSize(89, 32);
    horLayout->addWidget(lp_bet3Button);
    connect(lp_bet3Button, &QPushButton::clicked, this, &LordPanel::OnBet3);

    horLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
}

void LordPanel::cutCardPic(const QPixmap& cardsPic,int x, int y, Card card)
{
    QPixmap pic = cardsPic.copy(x, y, lp_cardSize.width(), lp_cardSize.height());

    CardPicture* cardPic = new CardPicture(this);
    cardPic->hide();
    cardPic->setPic(pic, lp_cardBackPic);

    cardPic->setCard(card);
    lp_cardPicMap.insert(card, cardPic);

    connect(cardPic, &CardPicture::showSelected, this, &LordPanel::OnCardSelected);
}

