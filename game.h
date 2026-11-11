#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    Vector2 posicao;
    Vector2 direcao;
    bool ativo;
    Rectangle hitbox;
} tiro_t;

typedef struct {
    bool ativo;
    Vector2 posicao;
    Rectangle hitbox;
} bomba_t;

typedef struct {
    Vector2 velocidade;
    Vector2 posicao;
    Vector2 vertices[8];
    float rotacao;
    float aceleracao;
    int poder;
    int vida;
    tiro_t tiro;
    bomba_t bomba;
    int tempo_escudo;
    int tempo_nitro;
    int tempo_dano;
    bool direcao_x;
    int fator;
    int n_checkpoint;
    int cont_checkpoint;
    int voltas;
    bool comecou;
    int cooldown_chegada;
} carro_t;

typedef struct {
    int tipo;
    Vector2 posicao;
    Color cor;
    Rectangle hitbox;
    bool colide;
} bloco_t;

#define PISTA 0
#define PAREDE 1
#define CHEGADA 2
#define PNEU 3

typedef struct {
    Vector2 posicao;
    Color cor;
    Rectangle hitbox;
    Texture2D textura;  
    int tipo;
} caixa_t;

typedef struct {
    Texture2D fundo_menu;
    Texture2D car1_t;
    Texture2D car2_t;
    Texture2D tiro_t;
    Texture2D bomba_t;
    Texture2D caixa_t;
    Texture2D carroescudo_t;
    Texture2D carro2escudo_t;
    Texture2D grama_t;
    Texture2D rua_t;
    Texture2D largada_t;
    Texture2D pneu_t;
    Texture2D carrodano_t;
    Texture2D barrainventario_t;
    Texture2D bomba_barra;
    Texture2D escudo_barra;
    Texture2D nitro_barra;
    Texture2D tiro_barra;
} texturas_t;

typedef struct {
    Sound som_bomba;
    Sound som_tiro;
    Sound som_engatilhado;
    Sound som_escudo;
    Sound som_usoescudo;
    Sound som_nitro;
    Sound som_largada;
} sons_t;

#define N_MUSICAS 8

#define QUANT_CAIXAS 3
#define ARMA 0
#define ESCUDO 1
#define BOMBA 2
#define NITRO 3
#define DANO_TIRO 4
#define DANO_BOMBA 5

#define TEMPO_PODER 5 // em segundos
#define TEMPO_DANO 1 // em segundos

#define PROP 16
#define DIF_Y (PROP)

#define LARGURA 80
#define ALTURA 20

#define CARRO_X  (3*(PROP))
#define CARRO_Y (PROP + DIF_Y)

#define LARGURA_TELA (LARGURA * PROP)
#define ALTURA_TELA (ALTURA * (PROP + DIF_Y) + 2*(PROP + DIF_Y))

#define N_VOLTAS 3


#define MENU 0
#define JOGANDO 1
#define PAUSADO 2
#define PERDEU 3
#define GANHOU 4
#define NOVO_JOGO_MAIN 5
#define TELA_CONTAGEM 6


#define NOVO_JOGO 0
#define SALVAR_JOGO 1
#define CARREGAR_JOGO 2
#define SAIR 3







void jogando(carro_t *meu_carro, carro_t *inimigo, caixa_t* caixas, texturas_t texturas, sons_t sons);

void atualiza_jogador(carro_t* meu_carro, caixa_t* caixas, carro_t* inimigo, sons_t sons);
void atualiza_ia(carro_t* inimigo, caixa_t* caixas, carro_t* meu_carro, sons_t sons);
void virou_ia(carro_t* inimigo, int indice_x, int indice_y);
void atualiza_carro_geral(carro_t* carro_1, carro_t* carro_2, caixa_t* caixas, float v_max, float atrito, sons_t sons);

void colisao_jogador(carro_t* meu_carro, carro_t* inimigo, bool tentou_rotacionar, float rotacao_antiga);
bool esta_colidindo(carro_t* meu_carro, float temp_x, float temp_y);
bool carros_colidindo(carro_t* carro_1, carro_t* carro_2, float temp_x, float temp_y);
void atualiza_vertices(carro_t* carro_n);

void zerar(carro_t *meu_carro);
void carrega_mapa(const char* nome_arquivo, carro_t* jogador, carro_t* inimigo);

void menu(carro_t* jogador, carro_t* inimigo, caixa_t* caixas, Texture2D background);
void novo_jogo(carro_t* jogador, carro_t* inimigo, caixa_t* caixas, Texture2D background);
void salvar_jogo(carro_t* jogador, carro_t* inimigo, caixa_t* caixas);
void carregar_jogo(carro_t* jogador, carro_t* inimigo, caixa_t* caixas);
void atualiza_musica(Music* playlist, int* musica_atual);
void pause(void);
void ganhou();
void perdeu();
void tela_contagem(sons_t sons);

void iniciar_caixa(caixa_t* caixas, int i);
int interagiu_item(carro_t* carro_1, carro_t* carro_2, caixa_t* caixas);

void usa_poder(carro_t* carro_n, sons_t sons);
void atualiza_poder(carro_t* carro_n);
void interagir(carro_t* carro_1, carro_t* carro_2, caixa_t* caixas, sons_t sons);
bool avalia_poder_ia(carro_t* inimigo, carro_t* meu_carro);

void atualiza_checkpoint(carro_t* carro_n);
void checkpoint_inicial(carro_t* carro_n);
bool fim_de_jogo(carro_t* carro_1, carro_t* carro_2);

extern int estado;
extern int opcao;
extern int opcao_mapa;
extern int total_mapas;
extern int frames_contagem;
extern bloco_t mapa[ALTURA][LARGURA];

#endif
