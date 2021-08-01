#include "PlayHandThread.h"
#include "Player.h"

PlayHandThread::PlayHandThread(Player* player) :
    QThread(player)
{
    pht_player = player;
}

void PlayHandThread::run()
{
    msleep(300);
    pht_player->waitForPlayHand();
}
