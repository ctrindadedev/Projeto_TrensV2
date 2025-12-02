#include "trem.h"
#include <QtCore>
#include <cmath>
#include <algorithm>

extern sem_t semaforos[7];

Trem::Trem(int ID, int x, int y) {
    this->ID = ID;
    this->x = x;
    this->y = y;
    this->velocidade = 100;
    this->parado = false;
    this->indiceAtual = 0;

    // Definição dos Pontos da Malha (Coordenadas Visuais)
    QPoint pTipEsq(120, 200);
    QPoint pTopEsq(260, 60);
    QPoint pTopDir(540, 60);
    QPoint pTipDir(680, 200);
    QPoint pBotDir(540, 340);
    QPoint pBotEsq(260, 340);
    QPoint pMidEsq(260, 200);
    QPoint pMidDir(540, 200);

    // Configuração dos trajetos - sentido Anti-Horári dessa vez
    switch(ID) {
        case 1: // Verde
            caminho << pTipEsq << pMidEsq << pTopEsq;
            this->x = pTipEsq.x(); this->y = pTipEsq.y();
            break;
        case 2: // Vermelho
            caminho << pTopEsq << pMidEsq << pMidDir << pTopDir;
            this->x = pTopEsq.x(); this->y = pTopEsq.y();
            break;
        case 3: // Azul
            caminho << pTopDir << pMidDir << pTipDir;
            this->x = pTopDir.x(); this->y = pTopDir.y();
            break;
        case 4: // Laranja
            caminho << pTipEsq << pBotEsq << pMidEsq;
            this->x = pTipEsq.x(); this->y = pTipEsq.y();
            break;
        case 5: // Amarelo
            caminho << pMidEsq << pBotEsq << pBotDir << pMidDir;
            this->x = pMidEsq.x(); this->y = pMidEsq.y();
            break;
        case 6: // Roxo
            caminho << pMidDir << pBotDir << pTipDir;
            this->x = pBotDir.x(); this->y = pBotDir.y();
            break;
    }
}

Trem::~Trem() {
    stop();
    wait();
}

void Trem::setVelocidade(int vel) {
    this->velocidade = vel;
}

void Trem::stop() {
    parado = true;
}

// Mapeia as coordenadas para o ID da região crítica
int Trem::obterRegiaoCritica(int i1, int i2) {
    QPoint p1 = caminho[i1];
    QPoint p2 = caminho[i2];

    // Região 0: Topo Esquerdo Vertical
    if ((p1.x() == 260 && p2.x() == 260) && ((p1.y() == 60 && p2.y() == 200) || (p1.y() == 200 && p2.y() == 60))) return 0;
    // Região 1: Topo Direito Vertical
    if ((p1.x() == 540 && p2.x() == 540) && ((p1.y() == 60 && p2.y() == 200) || (p1.y() == 200 && p2.y() == 60))) return 1;
    // Região 2: Ponta Esquerda Horizontal
    if ((p1.y() == 200 && p2.y() == 200) && ((p1.x() == 120 && p2.x() == 260) || (p1.x() == 260 && p2.x() == 120))) return 2;
    // Região 3: Centro Horizontal (HUB CRÍTICO)
    if ((p1.y() == 200 && p2.y() == 200) && ((p1.x() == 260 && p2.x() == 540) || (p1.x() == 540 && p2.x() == 260))) return 3;
    // Região 4: Ponta Direita Horizontal
    if ((p1.y() == 200 && p2.y() == 200) && ((p1.x() == 540 && p2.x() == 680) || (p1.x() == 680 && p2.x() == 540))) return 4;
    // Região 5: Baixo Esquerdo Vertical
    if ((p1.x() == 260 && p2.x() == 260) && ((p1.y() == 200 && p2.y() == 340) || (p1.y() == 340 && p2.y() == 200))) return 5;
    // Região 6: Baixo Direito Vertical
    if ((p1.x() == 540 && p2.x() == 540) && ((p1.y() == 200 && p2.y() == 340) || (p1.y() == 340 && p2.y() == 200))) return 6;

    return -1; // diagonais das pontas - caminhos que são livres
}

int Trem::calcularSleep() {
    if (velocidade == 0) return 200; 
    int sleep = 205 - velocidade;
    return (sleep < 2) ? 2 : sleep;
}

void Trem::run() {
    if (caminho.isEmpty()) return;

    // Onde o trem nasce
    int proximoIndiceInicial = (indiceAtual + 1) % caminho.size();
    int regiaoInicial = obterRegiaoCritica(indiceAtual, proximoIndiceInicial);
    
    if (regiaoInicial != -1) {
        sem_wait(&semaforos[regiaoInicial]);
        regioesAlocadas.append(regiaoInicial);
    }

    // Distância para começar a frear e pedir passagem
    const int DISTANCIA_FRENAGEM = 60; 

    while (!parado) {
        while (velocidade == 0 && !parado) { msleep(100); }
        if (parado) break;

        // Definindo o alvo atual (Fim do trilho onde o trem está)
        int proximoIndice = (indiceAtual + 1) % caminho.size();
        QPoint alvo = caminho[proximoIndice];

        // Definindo o PRÓXIMO trilho (Onde o trem vai entrar depois de chegar no alvo)
        int indiceAposAlvo = (proximoIndice + 1) % caminho.size();
        int regiaoFutura = obterRegiaoCritica(proximoIndice, indiceAposAlvo);

        // Lógica de "olhar para frente" para evitar deadlock
        int indiceAposFuturo = (indiceAposAlvo + 1) % caminho.size();
        int regiaoAposFutura = obterRegiaoCritica(indiceAposAlvo, indiceAposFuturo);

        bool podeMover = true;

        // Loop de movimento até chegar no alvo
        while ((x != alvo.x() || y != alvo.y()) && !parado) {
            
            int dx = alvo.x() - x;
            int dy = alvo.y() - y;
            double distancia = std::sqrt(dx*dx + dy*dy);

            // Se aproximar do fim do trilho atual
            if (distancia <= DISTANCIA_FRENAGEM) {
                // Se existe uma região crítica à frente e ainda não foi reservada
                if (regiaoFutura != -1 && !regioesAlocadas.contains(regiaoFutura)) {
                    
                    bool tentativaSucesso = false;
                    if (regiaoAposFutura != -1) {
                        if (sem_trywait(&semaforos[regiaoFutura]) == 0) {
                            if (sem_trywait(&semaforos[regiaoAposFutura]) == 0) {
                                regioesAlocadas.append(regiaoFutura);
                                regioesAlocadas.append(regiaoAposFutura);
                                tentativaSucesso = true;
                            } else {
                                //  impede que o trem fique parado bloqueando a primeira região
                                sem_post(&semaforos[regiaoFutura]);
                                tentativaSucesso = false;
                            }
                        }
                    } else {
                        // Só tem uma região crítica à frente (depois vem zona livre)
                        if (sem_trywait(&semaforos[regiaoFutura]) == 0) {
                            regioesAlocadas.append(regiaoFutura);
                            tentativaSucesso = true;
                        }
                    }

                    if (!tentativaSucesso) {
                        // Se não conseguiu os recursos necessários, o trem freia
                        podeMover = false;
                    }
                }
            }
            if (!podeMover) {
                msleep(2); 
                podeMover = true; 
                continue; 
            }

            moverPara(alvo.x(), alvo.y());
            emit updateGUI(ID, x, y);
            msleep(calcularSleep());
        }

        // Se chegar no fim do trilho atual, libera o trilho que ficou para trás (Região Passada)
        int regiaoPassada = obterRegiaoCritica(indiceAtual, proximoIndice);

        if (regiaoPassada != -1 && regioesAlocadas.contains(regiaoPassada)) {    
            if (regiaoPassada != regiaoFutura) {
                sem_post(&semaforos[regiaoPassada]);
                regioesAlocadas.removeAll(regiaoPassada);
            }
        }

        indiceAtual = proximoIndice;
    }

    //Se a thread for encerrada, libera tudo
    for(int r : regioesAlocadas) {
        sem_post(&semaforos[r]);
    }
}

void Trem::moverPara(int targetX, int targetY) {
  
    int step = 3; 
    
    if (x < targetX) x += std::min(step, targetX - x);
    else if (x > targetX) x -= std::min(step, x - targetX);
    
    if (y < targetY) y += std::min(step, targetY - y);
    else if (y > targetY) y -= std::min(step, y - targetY);
}