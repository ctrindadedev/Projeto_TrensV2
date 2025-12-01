#ifndef TREM_H
#define TREM_H

#include <QThread>
#include <QVector>
#include <QPoint>
#include <semaphore.h>

class Trem : public QThread {
    Q_OBJECT
public:
    Trem(int ID, int x, int y);
    ~Trem();
    void run();
    void setVelocidade(int vel);
    void stop();

signals:
    void updateGUI(int ID, int x, int y);

private:
    int x;
    int y;
    int ID;
    int velocidade;
    bool parado;
    
    // Controle de Trajeto
    QVector<QPoint> caminho;
    int indiceAtual;

    // Regiões críticas alocadas pelo trem
    QVector<int> regioesAlocadas;

    // Métodos Auxiliares
    void moverPara(int targetX, int targetY);
    int obterRegiaoCritica(int indiceDe, int indicePara);
    int calcularSleep();
};

#endif // TREM_H