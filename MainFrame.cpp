#include "MainFrame.h"
#include "ui_MainFrame.h"

MainFrame::~MainFrame(){delete ui;}
void MainFrame::resizeEvent(QResizeEvent* event)
{m_lordPanel->setGeometry(rect());QDialog::resizeEvent(event);}
MainFrame::MainFrame(QWidget *parent) : QDialog(parent),ui(new Ui::MainFrame)
{
    ui->setupUi(this);
    m_lordPanel = new LordPanel(this);
    setFixedSize(1280, 720);
    setWindowTitle(tr("Lord Card Game"));
}
