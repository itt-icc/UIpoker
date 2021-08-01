#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QDialog>
#include "LordPanel.h"
#include <QResizeEvent>

namespace Ui {
class MainFrame;
}

class MainFrame : public QDialog
{
    Q_OBJECT

public:
    explicit MainFrame(QWidget *parent = nullptr);
    ~MainFrame();

protected:
    virtual void resizeEvent(QResizeEvent* event);

private:
    LordPanel* m_lordPanel;
    Ui::MainFrame *ui;
};

#endif // MAINFRAME_H
