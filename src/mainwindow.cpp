#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <semaphore.h>
#include <QPainter>
#include <QPen>

sem_t semaforos[7];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for(int i = 0; i < 7; i++){
        sem_init(&semaforos[i], 0, 1);
    }

    trem1 = new Trem(1, 0, 0); 
    trem2 = new Trem(2, 0, 0);
    trem3 = new Trem(3, 0, 0);
    trem4 = new Trem(4, 0, 0);
    trem5 = new Trem(5, 0, 0);
    trem6 = new Trem(6, 0, 0);

    connect(trem1, SIGNAL(updateGUI(int,int,int)), SLOT(updateInterface(int,int,int)));
    connect(trem2, SIGNAL(updateGUI(int,int,int)), SLOT(updateInterface(int,int,int)));
    connect(trem3, SIGNAL(updateGUI(int,int,int)), SLOT(updateInterface(int,int,int)));
    connect(trem4, SIGNAL(updateGUI(int,int,int)), SLOT(updateInterface(int,int,int)));
    connect(trem5, SIGNAL(updateGUI(int,int,int)), SLOT(updateInterface(int,int,int)));
    connect(trem6, SIGNAL(updateGUI(int,int,int)), SLOT(updateInterface(int,int,int)));

    trem1->start();
    trem2->start();
    trem3->start();
    trem4->start();
    trem5->start();
    trem6->start();
}

MainWindow::~MainWindow()
{
    trem1->stop(); trem2->stop(); trem3->stop();
    trem4->stop(); trem5->stop(); trem6->stop();
    
    trem1->wait(); trem2->wait(); trem3->wait();
    trem4->wait(); trem5->wait(); trem6->wait();

    delete trem1; delete trem2; delete trem3;
    delete trem4; delete trem5; delete trem6;

    for(int i = 0; i < 7; i++){
        sem_destroy(&semaforos[i]);
    }
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    QPen pen;
    pen.setWidth(8);
    pen.setColor(Qt::black);
    painter.setPen(pen);

    painter.drawLine(260, 60, 540, 60);   
    painter.drawLine(260, 200, 540, 200); 
    painter.drawLine(260, 340, 540, 340); 
    painter.drawLine(260, 60, 260, 340);  
    painter.drawLine(540, 60, 540, 340);  
    painter.drawLine(120, 200, 260, 60);  
    painter.drawLine(120, 200, 260, 340); 
    painter.drawLine(540, 60, 680, 200);  
    painter.drawLine(540, 340, 680, 200); 
    painter.drawLine(120, 200, 260, 200); 
    painter.drawLine(540, 200, 680, 200); 
}

void MainWindow::updateInterface(int id, int x, int y){
    switch(id){
        case 1: ui->label_trem1->setGeometry(x-10, y-10, 20, 20); break;
        case 2: ui->label_trem2->setGeometry(x-10, y-10, 20, 20); break;
        case 3: ui->label_trem3->setGeometry(x-10, y-10, 20, 20); break;
        case 4: ui->label_trem4->setGeometry(x-10, y-10, 20, 20); break;
        case 5: ui->label_trem5->setGeometry(x-10, y-10, 20, 20); break;
        case 6: ui->label_trem6->setGeometry(x-10, y-10, 20, 20); break;
    }
}

void MainWindow::on_horizontalSlider_1_valueChanged(int value){ trem1->setVelocidade(value); }
void MainWindow::on_horizontalSlider_2_valueChanged(int value){ trem2->setVelocidade(value); }
void MainWindow::on_horizontalSlider_3_valueChanged(int value){ trem3->setVelocidade(value); }
void MainWindow::on_horizontalSlider_4_valueChanged(int value){ trem4->setVelocidade(value); }
void MainWindow::on_horizontalSlider_5_valueChanged(int value){ trem5->setVelocidade(value); }
void MainWindow::on_horizontalSlider_6_valueChanged(int value){ trem6->setVelocidade(value); }