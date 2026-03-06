#include "raylib.h"
#include "raymath.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"

typedef struct {
    Vector2 posicao;
    Vector2 direcao;
    bool ativo;
    Rectangle hitbox;
} Tiro;

typedef struct {
    bool ativo;
    Vector2 posicao;
    Rectangle hitbox;
} Bomba;

typedef struct {
    Vector2 velocidade;
    Vector2 posicao;
    Vector2 vertices[8];
    float rotacao;
    float aceleracao;
    int poder;
    int vida;
    Tiro tiro;
    Bomba bomba;
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
} Carro;

typedef struct {
    int tipo;
    Vector2 posicao;
    Color cor;
    Rectangle hitbox;
    bool colide;
} Bloco;

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
} Caixa;

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
} Texturas;

typedef struct {
    Sound som_bomba;
    Sound som_tiro;
    Sound som_engatilhado;
    Sound som_escudo;
    Sound som_usoescudo;
    Sound som_nitro;
    Sound som_largada;
} Sons;

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

int estado = 0;
#define MENU 0
#define JOGANDO 1
#define PAUSADO 2
#define PERDEU 3
#define GANHOU 4
#define NOVO_JOGO_MAIN 5
#define TELA_CONTAGEM 6

int opcao = 0;
#define NOVO_JOGO 0
#define SALVAR_JOGO 1
#define CARREGAR_JOGO 2
#define SAIR 3

int opcao_mapa = 1;
int total_mapas;
int frames_contagem = 3 * 60;

Bloco mapa[ALTURA][LARGURA];

void jogando(Carro *meuCarro, Carro *inimigo, Caixa* caixas, Texturas texturas, Sons sons);

void atualiza_jogador(Carro* meuCarro, Caixa* caixas, Carro* inimigo, Sons sons);
void atualiza_ia(Carro* inimigo, Caixa* caixas, Carro* meuCarro, Sons sons);
void virou_ia(Carro* inimigo, int indice_x, int indice_y);
void atualiza_carro_geral(Carro* carro_1, Carro* carro_2, Caixa* caixas, float v_max, float atrito, Sons sons);

void colisao_jogador(Carro* meuCarro, Carro* inimigo, bool tentou_rotacionar, float rotacao_antiga);
bool esta_colidindo(Carro* meuCarro, float temp_x, float temp_y);
bool carros_colidindo(Carro* carro_1, Carro* carro_2, float temp_x, float temp_y);
void atualiza_vertices(Carro* carro_n);

void zerar(Carro *meuCarro);
void carrega_mapa(const char* nome_arquivo, Carro* jogador, Carro* inimigo);

void menu(Carro* jogador, Carro* inimigo, Caixa* caixas, Texture2D background);
void novo_jogo(Carro* jogador, Carro* inimigo, Caixa* caixas, Texture2D background);
void salvar_jogo(Carro* jogador, Carro* inimigo, Caixa* caixas);
void carregar_jogo(Carro* jogador, Carro* inimigo, Caixa* caixas);
void atualiza_musica(Music* playlist, int* musica_atual);
void pause();
void ganhou();
void perdeu();
void tela_contagem(Sons sons);

void iniciar_caixa(Caixa* caixas, int i);
int interagiu_item(Carro* carro_1, Carro* carro_2, Caixa* caixas);

void usa_poder(Carro* carro_n, Sons sons);
void atualiza_poder(Carro* carro_n);
void interagir(Carro* carro_1, Carro* carro_2, Caixa* caixas, Sons sons);
bool avalia_poder_ia(Carro* inimigo, Carro* meuCarro);

void atualiza_checkp (Carro* carro_n);
void checkp_inicial(Carro* carro_n);
bool fim_de_jogo(Carro* carro_1, Carro* carro_2);

int main (void){
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Kick Butowski's car ride");

    SetTargetFPS(60);

    InitAudioDevice();

    Music playlist[N_MUSICAS] = {
        LoadMusicStream("musicas/Kick_Buttowski_Intro.mp3"),
        LoadMusicStream("musicas/Bad_Boys.mp3"),
        LoadMusicStream("musicas/California_Love.mp3"),
        LoadMusicStream("musicas/Feel_Good_Inc.mp3"),
        LoadMusicStream("musicas/Love_Me_Again.mp3"),
        LoadMusicStream("musicas/Pump_It.mp3"),
        LoadMusicStream("musicas/Six_Days.mp3"),
        LoadMusicStream("musicas/Without_Me.mp3")
    };

    for (int i = 0; i < N_MUSICAS; i++)
        playlist[i].looping = false;

    int musica_atual = 0;
    PlayMusicStream(playlist[musica_atual]); //começa da intro do Kick butowski 

    Texturas texturas = {
        .fundo_menu = LoadTexture("texturas/menu.jpg"),
        .car1_t = LoadTexture("texturas/car.png"),
        .car2_t = LoadTexture("texturas/car2.png"),
        .tiro_t = LoadTexture("texturas/tiro.png"),
        .bomba_t = LoadTexture("texturas/bomba.png"),
        .caixa_t = LoadTexture("texturas/moeda.png"),
        .carroescudo_t = LoadTexture("texturas/carroescudo.png"),
        .carro2escudo_t = LoadTexture("texturas/carro2escudo.png"),
        .grama_t = LoadTexture("texturas/gramav2.png"),
        .rua_t = LoadTexture("texturas/ruav2.png"),
        .largada_t = LoadTexture("texturas/largadav2.png"),
        .pneu_t = LoadTexture("texturas/pneuv2.png"),
        .carrodano_t = LoadTexture("texturas/queimado.png"),
        .barrainventario_t = LoadTexture("texturas/barra_inventario/barra_inventario.png"),
        .bomba_barra = LoadTexture("texturas/barra_inventario/bomba_barra.png"),
        .escudo_barra = LoadTexture("texturas/barra_inventario/escudo_barra.png"),
        .nitro_barra = LoadTexture("texturas/barra_inventario/nitro_barra.png"),
        .tiro_barra = LoadTexture("texturas/barra_inventario/tiro_barra.png")
    };

    Sons sons = {
        .som_bomba = LoadSound("sons/som_bomba.mp3"),
        .som_tiro = LoadSound("sons/som_tiro.mp3"),
        .som_engatilhado = LoadSound("sons/som_engatilhado.mp3"),
        .som_escudo = LoadSound("sons/som_escudo.mp3"),
        .som_usoescudo = LoadSound("sons/som_usoescudo.mp3"),
        .som_nitro = LoadSound("sons/som_nitro.mp3"),
        .som_largada = LoadSound("sons/som_largada.mp3")
    };

    Carro jogador;
    Carro inimigo;

    Caixa caixas[QUANT_CAIXAS];
    srand(time(NULL));

    FilePathList lista_arquivos = LoadDirectoryFilesEx("mapas", ".txt", false); // conta quantos arquivos .txt existem na pasta "mapas"
    total_mapas = lista_arquivos.count;
    UnloadDirectoryFiles(lista_arquivos); 

    while (!WindowShouldClose()){
        atualiza_musica(playlist, &musica_atual);
        switch (estado) {
            case MENU:
                menu(&jogador, &inimigo, caixas, texturas.fundo_menu);
                break;

            case TELA_CONTAGEM:
                tela_contagem(sons);
                break;

            case JOGANDO:
                jogando(&jogador, &inimigo, caixas, texturas, sons);
                break;

            case PAUSADO:
                pause();
                break;

            case GANHOU:
                ganhou();
                break;
    
            case PERDEU:
                perdeu();
                break;

            case NOVO_JOGO_MAIN:
                novo_jogo(&jogador, &inimigo, caixas, texturas.fundo_menu);
                break;
        }
    }
    UnloadMusicStream(playlist[musica_atual]);
    CloseAudioDevice();
    return 0;
}

void tela_contagem(Sons sons){
    if (frames_contagem == 3*60)
         PlaySound(sons.som_largada);
    
    frames_contagem--;
    if (frames_contagem == 0)
        estado = JOGANDO;

    int segundo_tela = frames_contagem / 60;
    int tamanho = 300;
    int altura = 150;
    BeginDrawing();
        ClearBackground(BLACK);
        switch(segundo_tela){
            case 2:
                DrawText("3", LARGURA_TELA / 2 - 70, ALTURA_TELA / 2 - altura, tamanho, RED);
                break;
            case 1:
                DrawText("2", LARGURA_TELA / 2 - 70, ALTURA_TELA / 2 - altura, tamanho, WHITE);
                break;
            case 0:
                DrawText("1", LARGURA_TELA / 2 - 70, ALTURA_TELA / 2 - altura, tamanho, RED);
                break;
        }
    EndDrawing();
}

void atualiza_musica(Music* playlist, int* musica_atual){
    UpdateMusicStream(playlist[*musica_atual]);

    if (!IsMusicStreamPlaying(playlist[*musica_atual])){
        StopMusicStream(playlist[*musica_atual]);
        int musica_antiga = *musica_atual;
        do {
            *musica_atual = rand() % (N_MUSICAS -1) + 1; //para não repetir a intro
        } while (musica_antiga == *musica_atual);
        PlayMusicStream(playlist[*musica_atual]);
    }
}

void jogando(Carro *meuCarro, Carro* inimigo, Caixa* caixas, Texturas texturas, Sons sons) {
    atualiza_jogador(meuCarro, caixas, inimigo, sons);

    atualiza_ia(inimigo, caixas, meuCarro, sons);

    BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < ALTURA; i++) {
            for (int j = 0; j < LARGURA; j++) {
                switch (mapa[i][j].tipo) {
                    case PISTA:
                        DrawTexture(texturas.rua_t, mapa[i][j].posicao.x, mapa[i][j].posicao.y, WHITE);
                        break;
                    case PAREDE:
                        DrawTexture(texturas.grama_t, mapa[i][j].posicao.x, mapa[i][j].posicao.y, WHITE);
                        break;
                    case PNEU:
                        DrawTexture(texturas.pneu_t, mapa[i][j].posicao.x, mapa[i][j].posicao.y, WHITE);
                        break;
                    case CHEGADA:
                        DrawTexture(texturas.largada_t, mapa[i][j].posicao.x, mapa[i][j].posicao.y, WHITE);
                        break;
                }  
            }
        }

        // renderização do carro do jogador
        if (meuCarro->tempo_escudo > 0) {
            DrawTexturePro(
                texturas.carroescudo_t,
                (Rectangle){ 0.0f, 0.0f, (float)texturas.car1_t.width, (float)texturas.car1_t.height },
                (Rectangle){ meuCarro->posicao.x, meuCarro->posicao.y, CARRO_X, CARRO_Y },
                (Vector2){ CARRO_X/2, CARRO_Y/2 },
                meuCarro->rotacao,
                WHITE
            );
        }
        else if (meuCarro->tempo_dano > 0) {
            DrawTexturePro(
                texturas.carrodano_t,
                (Rectangle){ 0.0f, 0.0f, (float)texturas.car1_t.width, (float)texturas.car1_t.height },
                (Rectangle){ meuCarro->posicao.x, meuCarro->posicao.y, CARRO_X, CARRO_Y },
                (Vector2){ CARRO_X/2, CARRO_Y/2 },
                meuCarro->rotacao,
                WHITE
            );
        }
        else {
            DrawTexturePro(
                texturas.car1_t,
                (Rectangle){ 0.0f, 0.0f, (float)texturas.car1_t.width, (float)texturas.car1_t.height },
                (Rectangle){ meuCarro->posicao.x, meuCarro->posicao.y, CARRO_X, CARRO_Y },
                (Vector2){ CARRO_X/2, CARRO_Y/2 },
                meuCarro->rotacao,
                WHITE
            );
        }

        // renderização do carro inimigo
        if (inimigo->tempo_escudo > 0) {
            DrawTexturePro(
                texturas.carro2escudo_t,
                (Rectangle){ 0.0f, 0.0f, (float)texturas.car2_t.width, (float)texturas.car2_t.height },
                (Rectangle){ inimigo->posicao.x, inimigo->posicao.y, CARRO_X, CARRO_Y },
                (Vector2){ CARRO_X/2, CARRO_Y/2 },
                inimigo->rotacao,
                WHITE
            );
        }
        else if (inimigo->tempo_dano > 0) {
            DrawTexturePro(
                texturas.carrodano_t,
                (Rectangle){ 0.0f, 0.0f, (float)texturas.car2_t.width, (float)texturas.car2_t.height },
                (Rectangle){ inimigo->posicao.x, inimigo->posicao.y, CARRO_X, CARRO_Y },
                (Vector2){ CARRO_X/2, CARRO_Y/2 },
                inimigo->rotacao,
                WHITE
            );
        }
        else {
            DrawTexturePro(
                texturas.car2_t,
                (Rectangle){ 0.0f, 0.0f, (float)texturas.car2_t.width, (float)texturas.car2_t.height },
                (Rectangle){ inimigo->posicao.x, inimigo->posicao.y, CARRO_X, CARRO_Y },
                (Vector2){ CARRO_X/2, CARRO_Y/2 },
                inimigo->rotacao,
                WHITE
            );
        }

        // renderização das caixas
        for (int i = 0; i < QUANT_CAIXAS; i++) {
            DrawTexture(texturas.caixa_t, caixas[i].posicao.x, caixas[i].posicao.y, WHITE);
        }

        // renderização dos poderes do jogador
        if (meuCarro->tiro.ativo) {
            DrawTexture(texturas.tiro_t, meuCarro->tiro.posicao.x - PROP/2, meuCarro->tiro.posicao.y - PROP/2, WHITE);
        }

        if (meuCarro->bomba.ativo) {
            DrawTexture(texturas.bomba_t, meuCarro->bomba.posicao.x - PROP/2, meuCarro->bomba.posicao.y - PROP/2, WHITE);
        }

        // renderização dos poderes do inimigo
        if (inimigo->tiro.ativo) {
            DrawTexture(texturas.tiro_t, inimigo->tiro.posicao.x - PROP/2, inimigo->tiro.posicao.y - PROP/2, WHITE);
        }

        if (inimigo->bomba.ativo) {
            DrawTexture(texturas.bomba_t, inimigo->bomba.posicao.x - PROP/2, inimigo->bomba.posicao.y - PROP/2, WHITE);
        }

        // renderização da barra de vida e itens do jogador, e vida do inimigo
        DrawTexture(texturas.barrainventario_t, 0, ALTURA_TELA - (2*(PROP + DIF_Y)), WHITE); //fundo base
        int tam = 37;
        DrawText(TextFormat("%i/3", meuCarro->voltas + 1), 18, ALTURA_TELA - 11 - tam, tam, WHITE);
        DrawText(TextFormat("%i", meuCarro->vida), 159, ALTURA_TELA - 11 - tam, tam, RED);
        DrawText(TextFormat("%i", inimigo->vida), 1021, ALTURA_TELA - 11 - tam, tam, BLUE);

        int x_item = 463;
        int altura_poder = 53;
        
        switch (meuCarro->poder){
            case ARMA:
                DrawTexture(texturas.tiro_barra, x_item, ALTURA_TELA - altura_poder, WHITE);
                break;
            case BOMBA:
                DrawTexture(texturas.bomba_barra, x_item, ALTURA_TELA - altura_poder, WHITE);
                break;
            case NITRO:
                DrawTexture(texturas.nitro_barra, x_item, ALTURA_TELA - altura_poder, WHITE);
                break;
            case ESCUDO:
                DrawTexture(texturas.escudo_barra, x_item, ALTURA_TELA - altura_poder, WHITE);
                break;
        }
    EndDrawing();
}

void atualiza_ia(Carro* inimigo, Caixa* caixas, Carro* meuCarro, Sons sons) {
    float temp_x = inimigo->posicao.x + inimigo->velocidade.x;
    float temp_y = inimigo->posicao.y + inimigo->velocidade.y;

    int original_y = (int)(inimigo->posicao.y / (PROP + DIF_Y));
    int original_x = (int)(inimigo->posicao.x / PROP);

    int indice_x = (int)(temp_x / PROP);
    int indice_y = (int)(temp_y / (PROP + DIF_Y));

    int dist = 4; //distância para verificação de colisão

    if (inimigo->direcao_x) {
        if (!mapa[indice_y][indice_x + (dist * inimigo->fator)].colide) {
            inimigo->velocidade.x += inimigo->aceleracao * inimigo->fator;
        } 
        else { 
            virou_ia(inimigo, indice_x, indice_y);
            inimigo->velocidade.x *= 0.3f;
            if (inimigo->tempo_nitro > 0) inimigo->velocidade.x *= 0.1f;
        }
    } 
    else {
        if (!mapa[indice_y + ((dist / 2) * inimigo->fator)][indice_x].colide) {
            inimigo->velocidade.y += inimigo->aceleracao * inimigo->fator;
        } 
        else {
            virou_ia(inimigo, indice_x, indice_y);
            inimigo->velocidade.y *= 0.3f;
            if (inimigo->tempo_nitro > 0) inimigo->velocidade.y *= 0.1f;
        }
    }

    //sistema de colisões simples

    if (mapa[indice_y][original_x].colide) {
        inimigo->velocidade.y *= -0.3f;
        temp_y = inimigo->posicao.y;
    }

    if (mapa[original_y][indice_x].colide) {
        inimigo->velocidade.x *= -0.3f;
        temp_x = inimigo->posicao.x;
    }

    if (carros_colidindo(inimigo, meuCarro, temp_x, temp_y)) {
        temp_x = inimigo->posicao.x;
        temp_y = inimigo->posicao.y;
        inimigo->velocidade.x = 0;
        inimigo->velocidade.y = 0;
    }


    inimigo->posicao.x = temp_x;
    inimigo->posicao.y = temp_y;

    if (avalia_poder_ia(inimigo, meuCarro)) { //avalia se deve usar o poder
        usa_poder(inimigo, sons);
    }

    atualiza_carro_geral(inimigo, meuCarro, caixas, 12.0f, 0.94f, sons);

    if (fim_de_jogo(inimigo, meuCarro))
        estado = PERDEU;
}

void virou_ia(Carro* inimigo, int indice_x, int indice_y) {
    inimigo->direcao_x = !inimigo->direcao_x;
    int teste_x = 0;
    int teste_y = 0;
    bool achou = false;

    while (!achou) {
        if (mapa[indice_y + teste_y][indice_x + teste_x].colide) {
            achou = true;
            inimigo->fator = -1;
        } 
        else if (mapa[indice_y - teste_y][indice_x - teste_x].colide) {
            achou = true;
            inimigo->fator = 1;
        } 
        if (inimigo->direcao_x) teste_x++;
        else teste_y++;
    }

    if (inimigo->direcao_x) inimigo->rotacao = 0;
    else inimigo->rotacao = 90;

    if (inimigo->fator == -1) {
        inimigo->rotacao += 180;
    }
        
}

void atualiza_jogador(Carro* meuCarro, Caixa* caixas, Carro* inimigo, Sons sons) {
    if (!meuCarro) return;

    if (IsKeyDown(KEY_W)) {
        meuCarro->velocidade.x += meuCarro->aceleracao * cosf(DEG2RAD * meuCarro->rotacao);
        meuCarro->velocidade.y += meuCarro->aceleracao * sinf(DEG2RAD * meuCarro->rotacao);
    }
    if (IsKeyDown(KEY_S)) {
        meuCarro->velocidade.x -= meuCarro->aceleracao * cosf(DEG2RAD * meuCarro->rotacao);
        meuCarro->velocidade.y -= meuCarro->aceleracao * sinf(DEG2RAD * meuCarro->rotacao);
    }

    float rotacao_antiga = meuCarro->rotacao;
    bool tentou_rotacionar = false;

    if (IsKeyDown(KEY_A)) {
        meuCarro->rotacao -= 3.5f;
        tentou_rotacionar = true;
    }
    if (IsKeyDown(KEY_D)) {
        meuCarro->rotacao += 3.5f;
        tentou_rotacionar = true;
    }

    if (IsKeyPressed(KEY_P)) {
        estado = PAUSADO;
    }

    if (IsKeyDown(KEY_SPACE)){
        usa_poder(meuCarro, sons);
    }

    colisao_jogador(meuCarro, inimigo, tentou_rotacionar, rotacao_antiga);

    float v_max = 15.0f;
    float atrito = 0.93f;
    atualiza_carro_geral(meuCarro, inimigo, caixas, v_max, atrito, sons);

    if (fim_de_jogo(meuCarro, inimigo))
        estado = GANHOU;
}

void colisao_jogador(Carro* meuCarro, Carro* inimigo, bool tentou_rotacionar, float rotacao_antiga) {
    int original_y = (int)(meuCarro->posicao.y / (PROP + DIF_Y));
    int original_x = (int)(meuCarro->posicao.x / PROP);
    
    if (tentou_rotacionar) {
        // Verifique se a NOVA rotação, na POSIÇÃO ATUAL, causa colisão
        if (esta_colidindo(meuCarro, meuCarro->posicao.x, meuCarro->posicao.y)) {
            // A rotação prendeu o carro na parede 
            meuCarro->rotacao = rotacao_antiga;
            if (mapa[original_y + 1][original_x].colide)
                meuCarro->posicao.y -= PROP / 10; // Afasta para cima
            if (mapa[original_y - 1][original_x].colide)
                meuCarro->posicao.y += PROP / 10; // Afasta para baixo
            if (mapa[original_y][original_x + 1].colide) 
                meuCarro->posicao.x -= PROP / 10; // Afasta para esquerda
            if (mapa[original_y][original_x - 1].colide)
                meuCarro->posicao.x += PROP / 10; // Afasta para direita
        }

        if (carros_colidindo(meuCarro, inimigo, meuCarro->posicao.x, meuCarro->posicao.y)) {
            meuCarro->rotacao = rotacao_antiga;
        }
    }

    float temp_x = meuCarro->posicao.x + meuCarro->velocidade.x;
    float temp_y = meuCarro->posicao.y + meuCarro->velocidade.y;

    if (esta_colidindo(meuCarro, temp_x, temp_y)) {
        if (esta_colidindo(meuCarro, temp_x, meuCarro->posicao.y)){
            meuCarro->velocidade.x *= -0.6f;
            temp_x = meuCarro->posicao.x;
        }
        if (esta_colidindo(meuCarro, meuCarro->posicao.x, temp_y)){
            meuCarro->velocidade.y *= -0.6f;
            temp_y = meuCarro->posicao.y;
        }
    }

    if (carros_colidindo(meuCarro, inimigo, temp_x, temp_y)) {
        if (carros_colidindo(meuCarro, inimigo, temp_x, meuCarro->posicao.y)){
            temp_x = meuCarro->posicao.x;
            meuCarro->velocidade.x = 0;
        }
        if (carros_colidindo(meuCarro, inimigo, meuCarro->posicao.x, temp_y)){
            temp_y = meuCarro->posicao.y;
            meuCarro->velocidade.y = 0;
        }
    }

    
    meuCarro->posicao.x = temp_x;
    meuCarro->posicao.y = temp_y;
}

bool carros_colidindo(Carro* carro_1, Carro* carro_2, float temp_x, float temp_y) {
    atualiza_vertices(carro_2);

    Vector2 pos_futura = { temp_x, temp_y };
    if (Vector2Distance(pos_futura, carro_2->posicao) > 150.0f) {
        return false;
    }

    Vector2 c1_futuros[8];
    Vector2 metade = { CARRO_X / 2.0f, CARRO_Y / 2.0f };

    // Mesma ordem da sua função atualiza_vertices
    Vector2 locais[8] = {
        { -metade.x, -metade.y }, // 0: Top-Esq
        {  metade.x, -metade.y }, // 1: Top-Dir
        {  metade.x,  metade.y }, // 2: Inf-Dir 
        { -metade.x,  metade.y }, // 3: Inf-Esq
        { 0, -metade.y },         // 4: Meio-Cima
        { metade.x, 0 },          // 5: Meio-Dir
        { 0, metade.y },          // 6: Meio-Baixo
        { -metade.x, 0 }          // 7: Meio-Esq
    };

    float ang = DEG2RAD * carro_1->rotacao;
    float cosR = cosf(ang);
    float sinR = sinf(ang);

    for(int i = 0; i < 8; i++) {
        float x_rot = locais[i].x * cosR - locais[i].y * sinR;
        float y_rot = locais[i].x * sinR + locais[i].y * cosR;
        
        c1_futuros[i].x = temp_x + x_rot;
        c1_futuros[i].y = temp_y + y_rot;
    }

    for (int i = 0; i < 8; i++) {
        if (CheckCollisionPointPoly(carro_2->vertices[i], c1_futuros, 4)) {
            return true;
        }
    }

    for (int i = 0; i < 8; i++) {
        if (CheckCollisionPointPoly(c1_futuros[i], carro_2->vertices, 4)) {
            return true;
        }
    }

    return false;
}

void atualiza_carro_geral(Carro* carro_1, Carro* carro_2, Caixa* caixas, float v_max, float atrito, Sons sons) {
    if (carro_1->velocidade.x > v_max) carro_1->velocidade.x = v_max;
    if (carro_1->velocidade.y > v_max) carro_1->velocidade.y = v_max;
    if (carro_1->velocidade.x < -v_max) carro_1->velocidade.x = -v_max;
    if (carro_1->velocidade.y < -v_max) carro_1->velocidade.y = -v_max;

    interagir(carro_1, carro_2, caixas, sons);
    atualiza_poder(carro_1);

    int indice_y = (int)(carro_1->posicao.y / (PROP + DIF_Y));
    int indice_x = (int)(carro_1->posicao.x / PROP);
    
    if (carro_1->comecou) // só conta checkpoints depois de começar a corrida
        atualiza_checkp(carro_1);
    else {
        if (mapa[indice_y][indice_x].tipo == CHEGADA){
            carro_1->comecou = true;
            carro_1->cont_checkpoint = 0;
            carro_1->cooldown_chegada = 60;
        }
    }

    if (carro_1->tempo_dano > 0) {
        carro_1->tempo_dano--;
        carro_1->aceleracao = 0.2f; // Reduz a aceleração ao sofrer dano
        if (carro_1->tempo_dano == 0)
            carro_1->aceleracao = 0.35f; // Restaura a aceleração normal
    }

    if (carro_1->cooldown_chegada > 0) 
        carro_1->cooldown_chegada--;

    carro_1->velocidade.x *= atrito; // atrito
    carro_1->velocidade.y *= atrito;
}

void atualiza_checkp (Carro* carro_n){
    int indice_x = (int)(carro_n->posicao.x / PROP);
    int indice_y = (int)(carro_n->posicao.y / (PROP + DIF_Y));

    //(indice_x == (LARGURA / 2) || indice_x == (LARGURA / 2) + carro_n->fator) && (indice_y >= (ALTURA / 2) || indice_y >= (ALTURA / 2) + carro_n->fator)


    if (indice_y == (ALTURA / 2) && indice_x < (LARGURA / 2) && carro_n->n_checkpoint == 1){
        carro_n->n_checkpoint = 0;
        carro_n->cont_checkpoint++;
    }
    else if (indice_y == (ALTURA / 2) && indice_x >= (LARGURA / 2) && carro_n->n_checkpoint == 0){
        carro_n->n_checkpoint = 1;
        carro_n->cont_checkpoint++;
    }


    if (carro_n->cont_checkpoint % 2 == 0 && carro_n->cont_checkpoint != 0 && 
        mapa[indice_y][indice_x].tipo == CHEGADA && 
        carro_n->cooldown_chegada == 0){
        
        printf("Volta %d completa!\n", (carro_n->cont_checkpoint / 4));
        carro_n->voltas++;
        carro_n->cooldown_chegada = 60;
    }
}

bool fim_de_jogo(Carro* carro_1, Carro* carro_2) {
    if (carro_1->voltas >= N_VOLTAS) {
        return true;
    }
    if (carro_2->vida <= 0) {
        return true;
    }
    return false;
}

void interagir(Carro* carro_1, Carro* carro_2, Caixa* caixas, Sons sons) {
    int interação = interagiu_item(carro_1, carro_2, caixas);

    if (interação == DANO_TIRO || interação == DANO_BOMBA) {
        if (carro_1->tempo_escudo == 0) {
            carro_1->vida -= 50;
            printf("Vida do jogador: %d\n", carro_1->vida);
            carro_1->tempo_dano = TEMPO_DANO * 60;
            carro_1->tempo_nitro = 0;
            carro_1->velocidade.x = 0;
            carro_1->velocidade.y = 0;
            if (interação == DANO_BOMBA)
                PlaySound(sons.som_bomba);
        } 
        else {
            printf("Escudo absorveu o dano!\n");
            carro_1->tempo_escudo = 0;
            carro_1->vida += 50;
            if (carro_1->vida > 100) carro_1->vida = 100;
            printf("Vida do jogador: %d\n", carro_1->vida);
            PlaySound(sons.som_usoescudo);
        }
        carro_2->tiro.ativo = false;
        carro_2->bomba.ativo = false;
    }
    else if (interação != -1 && carro_1->poder == -1) {
        carro_1->poder = caixas[interação].tipo;
        iniciar_caixa(caixas, interação);
        if (carro_1->poder == ARMA)
            PlaySound(sons.som_engatilhado);
    } 
}

int interagiu_item(Carro* carro_1, Carro* carro_2, Caixa* caixas) {
    atualiza_vertices(carro_1);
    
    for (int i = 0; i < 8; i++) {
        if (carro_2->tiro.ativo && CheckCollisionPointRec(carro_1->vertices[i], carro_2->tiro.hitbox)) {
            return DANO_TIRO;
        }
        
        if (carro_2->bomba.ativo && CheckCollisionPointRec(carro_1->vertices[i], carro_2->bomba.hitbox)) {
            return DANO_BOMBA;
        }

        if (carro_1->poder == -1){
            for (int j = 0; j < QUANT_CAIXAS; j++) {
                if (CheckCollisionPointRec(carro_1->vertices[i], caixas[j].hitbox)) {
                    return j;
                }
            }
        }
    }
    return -1;
}

void usa_poder(Carro* carro_n, Sons sons) {
    switch (carro_n->poder){
        case ARMA:
            printf("Atirou!\n");
            carro_n->tiro.ativo = true;
            carro_n->tiro.posicao.x = carro_n->posicao.x;
            carro_n->tiro.posicao.y = carro_n->posicao.y;
            carro_n->tiro.direcao.x = cosf(DEG2RAD * carro_n->rotacao);
            carro_n->tiro.direcao.y = sinf(DEG2RAD * carro_n->rotacao);
            carro_n->tiro.hitbox = (Rectangle){ carro_n->tiro.posicao.x - PROP/2.0f, carro_n->tiro.posicao.y - PROP/2.0f, PROP, PROP };
            PlaySound(sons.som_tiro);
            break;
        case ESCUDO:
            printf("Ativou escudo!\n");
            carro_n->tempo_escudo = TEMPO_PODER * 60; // em frames
            PlaySound(sons.som_escudo);
            break;
        case BOMBA:
            printf("Lançou bomba!\n");
            carro_n->bomba.ativo = true;
            carro_n->bomba.posicao.x = carro_n->posicao.x - cosf(DEG2RAD * carro_n->rotacao) * 25.0f;
            carro_n->bomba.posicao.y = carro_n->posicao.y - sinf(DEG2RAD * carro_n->rotacao) * 25.0f;
            carro_n->bomba.hitbox = (Rectangle){ carro_n->bomba.posicao.x - PROP/2.0f, carro_n->bomba.posicao.y - PROP/2.0f, PROP, PROP };
            break;
        case NITRO:
            printf("Usou nitro!\n");
            carro_n->tempo_nitro = TEMPO_PODER * 60; // em frames
            PlaySound(sons.som_nitro);
            break;    
        }
        carro_n->poder = -1;
}

void atualiza_poder(Carro* carro_n) {
    if (carro_n->tempo_escudo > 0) {
        carro_n->tempo_escudo--;
        if (carro_n->tempo_escudo == 0) {
            printf("Escudo acabou!\n");
        }
    }
    if (carro_n->tempo_nitro > 0) {
        carro_n->tempo_nitro--;
        if (carro_n->tempo_nitro == 0) {
            printf("Nitro acabou!\n");
            carro_n->aceleracao = 0.35f;
        } else {
            carro_n->aceleracao = 0.6f; // Aceleração dobrada com nitro
        }
    }
    if (carro_n->tiro.ativo) {
        carro_n->tiro.posicao.x += carro_n->tiro.direcao.x * 15.0f;
        carro_n->tiro.posicao.y += carro_n->tiro.direcao.y * 15.0f;
        carro_n->tiro.hitbox = (Rectangle){ carro_n->tiro.posicao.x - PROP/2.0f, carro_n->tiro.posicao.y - PROP/2.0f, PROP, PROP };

        // Desativar o tiro se bater em uma parede
        int indice_x = (int)(carro_n->tiro.posicao.x / PROP);
        int indice_y = (int)(carro_n->tiro.posicao.y / (PROP + DIF_Y));

        if (mapa[indice_y][indice_x].colide)
            carro_n->tiro.ativo = false;
    }
}

bool avalia_poder_ia(Carro* inimigo, Carro* meuCarro) {
    bool usar = false;

    if (inimigo->poder == NITRO && inimigo->tempo_nitro == 0) {
        usar = true;
    }

    else if (inimigo->poder == ESCUDO && inimigo->tempo_escudo == 0 && (meuCarro->tiro.ativo || meuCarro->bomba.ativo || meuCarro->tempo_escudo > 0)) {
        usar = true;
    }

    else if (inimigo->poder == ARMA && meuCarro->cont_checkpoint < inimigo->cont_checkpoint){
        usar = true;
    }

    else if (inimigo->poder == ARMA && meuCarro->tempo_escudo == 0 && !inimigo->tiro.ativo){
        for (int i = -5; i <= 5; i++) {
            float angulo_tiro = atan2f(meuCarro->posicao.y - inimigo->posicao.y, meuCarro->posicao.x - inimigo->posicao.x);
            float angulo_inimigo = DEG2RAD * inimigo->rotacao;
            float diferenca_angulo = angulo_tiro - angulo_inimigo;

            if (fabs(diferenca_angulo) < DEG2RAD * (15 + i)) {
                usar = true;
                break;
            }
        }
    }

    else if (inimigo->poder == BOMBA && !inimigo->bomba.ativo) {
        usar = true;
    }

    return usar;
}

void menu(Carro* jogador, Carro* inimigo, Caixa* caixas, Texture2D background) {
    if (IsKeyPressed(KEY_DOWN)) {
        if (opcao == 3)
            opcao = 0;
        else
            opcao++;
    }
    if (IsKeyPressed(KEY_UP)) {
        if (opcao == 0)
            opcao = 3;
        else
            opcao--;
    }

    // Seleciona a opção com ENTER
    if (IsKeyPressed(KEY_ENTER)) {
        switch (opcao) {
            case NOVO_JOGO:
                estado = NOVO_JOGO_MAIN;
                WaitTime(0.1f); // Pequena pausa para evitar múltiplas seleções
                break;
            case SALVAR_JOGO:
                salvar_jogo(jogador, inimigo, caixas);
                break;
            case CARREGAR_JOGO:
                carregar_jogo(jogador, inimigo, caixas);            
                break;
            case SAIR:
                CloseWindow();
                return;
        }
    }

    // Desenho do menu (opcional)
    BeginDrawing();
        ClearBackground(BLACK);

        Vector2 scale = {
            (float)LARGURA_TELA / background.width,
            (float)ALTURA_TELA / background.height
        };
        DrawTextureEx(background, (Vector2){ 0, 0 }, 0.0f, scale.x, WHITE);

        DrawText("MENU", 100, 50, 30, WHITE);
        DrawText("Novo Jogo", 100, 100, 20, (opcao == 0) ? RED : WHITE);
        DrawText("Salvar Jogo", 100, 130, 20, (opcao == 1) ? RED : WHITE);
        DrawText("Carregar Jogo", 100, 160, 20, (opcao == 2) ? RED : WHITE);
        DrawText("Sair", 100, 190, 20, (opcao == 3) ? RED : WHITE);

    EndDrawing();
}

void novo_jogo(Carro* jogador, Carro* inimigo, Caixa* caixas, Texture2D background) {
    if (IsKeyPressed(KEY_DOWN)) {
        if (opcao_mapa == total_mapas)
            opcao_mapa = 1;
        else
            opcao_mapa++;
    }
    if (IsKeyPressed(KEY_UP)) {
        if (opcao_mapa == 1)
            opcao_mapa = total_mapas;
        else
            opcao_mapa--;
    }

    // Seleciona a opção com ENTER
    if (IsKeyPressed(KEY_ENTER)) {
        zerar(jogador);
        zerar(inimigo);
        carrega_mapa(TextFormat("mapas/pista%i.txt", opcao_mapa), jogador, inimigo);
        for (int i = 0; i < QUANT_CAIXAS; i++)
            iniciar_caixa(caixas, i);
        estado = TELA_CONTAGEM;
        frames_contagem = 3 * 60;
    }
    
    BeginDrawing();
        ClearBackground(BLACK);

        Vector2 scale = {
            (float)LARGURA_TELA / background.width,
            (float)ALTURA_TELA / background.height
        };
        DrawTextureEx(background, (Vector2){ 0, 0 }, 0.0f, scale.x, WHITE);

        DrawText("PISTA", 100, 50, 30, WHITE);
        for (int i = 1; i <= total_mapas; i++){
            DrawText(TextFormat("Pista %i", i), 100, 70 + 30*i, 20, (opcao_mapa == i) ? RED : WHITE);
        }
    EndDrawing();
}

void salvar_jogo(Carro* jogador, Carro* inimigo, Caixa* caixas) {
    FILE* arquivo = fopen("salvamento.bin", "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para salvar o jogo.\n");
        return;
    }

    fwrite(&opcao_mapa, sizeof(int), 1, arquivo);
    fwrite(jogador, sizeof(Carro), 1, arquivo);
    fwrite(inimigo, sizeof(Carro), 1, arquivo);
    fwrite(caixas, sizeof(Caixa), QUANT_CAIXAS, arquivo);

    fclose(arquivo);
    printf("Jogo salvo\n");
}

void carregar_jogo(Carro* jogador, Carro* inimigo, Caixa* caixas) {
    FILE* arquivo = fopen("salvamento.bin", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para carregar o jogo.\n");
        return;
    }

    int carrega_map;
    fread(&carrega_map, sizeof(int), 1, arquivo);
    carrega_mapa(TextFormat("mapas/pista%i.txt", carrega_map), jogador, inimigo);

    fread(jogador, sizeof(Carro), 1, arquivo);
    fread(inimigo, sizeof(Carro), 1, arquivo);
    fread(caixas, sizeof(Caixa), QUANT_CAIXAS, arquivo);

    fclose(arquivo);
    estado = TELA_CONTAGEM;
    frames_contagem = 3 * 60;

    printf("Jogo carregado\n");
}

void carrega_mapa(const char* nome_arquivo, Carro* jogador, Carro* inimigo) {
    FILE* arquivo = fopen(nome_arquivo, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", nome_arquivo);
        return;
    }

    int buffer;
    int x = 0;
    int y = 0;

    int x_jogador, y_jogador, x_inimigo, y_inimigo;
    
    while ((buffer = fgetc(arquivo)) != EOF) {
        if (buffer == '\n') {
            y++;
            x = 0;
        }
        else {
            switch (buffer) {
                case 'p':
                    mapa[y][x].tipo = PAREDE;
                    mapa[y][x].colide = true;
                    break;

                case '\t':
                case ' ':
                    mapa[y][x].tipo = PISTA;
                    mapa[y][x].colide = false;
                    break;
                case 'j':
                    mapa[y][x].tipo = PISTA;
                    jogador->posicao.x = x * PROP + PROP / 2;
                    jogador->posicao.y = y * (PROP + DIF_Y) + (PROP + DIF_Y) / 2;

                    x_jogador = x;
                    y_jogador = y;

                    mapa[y][x].colide = false;
                    break;
                case 'i':
                    mapa[y][x].tipo = PISTA;
                    
                    inimigo->posicao.x = x * PROP + PROP / 2;
                    inimigo->posicao.y = y * (PROP + DIF_Y) + (PROP + DIF_Y) / 2;

                    x_inimigo = x;
                    y_inimigo = y;

                    mapa[y][x].colide = false;

                    break;
                case 'L':
                    mapa[y][x].tipo = CHEGADA;
                    mapa[y][x].colide = false;
                    break;
                default:
                    // caractere desconhecido -> trata como pista (ou ignore)
                    mapa[y][x].tipo = PISTA;
                    mapa[y][x].colide = false;
                    break;
            }

            mapa[y][x].posicao.x = x * PROP;
            mapa[y][x].posicao.y = y * (PROP + DIF_Y);  
            mapa[y][x].hitbox = (Rectangle){ mapa[y][x].posicao.x, mapa[y][x].posicao.y, PROP, PROP + DIF_Y };
            x++;
        }
    }
    fclose(arquivo);

    // Carrega texturas especiais para paredes próximas a pistas
    for (int i = 0; i < ALTURA; i++){
        for (int j = 0; j < LARGURA; j++){
            if (mapa[i][j].tipo == PAREDE){
                bool encontrou_pista = false;
                for (int a = -1; a <= 1 && !encontrou_pista; a++){
                    for (int b = -1; b <= 1; b++){
                        int vizinho_x = j + a;
                        int vizinho_y = i + b;
                        if (vizinho_x >= 0 && vizinho_x < LARGURA && vizinho_y >= 0 && vizinho_y < ALTURA){
                            if (mapa[vizinho_y][vizinho_x].tipo == PISTA){
                                mapa[i][j].tipo = PNEU;
                                encontrou_pista = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    for (int off = -1; off <= 1; off += 2) { // afasta de paredes próximas para conseguir girar
        int nx = x_jogador + off;
        int ny = y_jogador + off;
        if (nx >= 0 && nx < LARGURA && mapa[y_jogador][nx].tipo == PAREDE)
            jogador->posicao.x += (PROP / 3) * -off;
        if (ny >= 0 && ny < ALTURA && mapa[ny][x_jogador].tipo == PAREDE)
            jogador->posicao.y += (PROP / 3) * -off;

        nx = x_inimigo + off;
        ny = y_inimigo + off;
        if (nx >= 0 && nx < LARGURA && mapa[y_inimigo][nx].tipo == PAREDE)
            inimigo->posicao.x += (PROP / 3) * -off;
        if (ny >= 0 && ny < ALTURA && mapa[ny][x_inimigo].tipo == PAREDE)
            inimigo->posicao.y += (PROP / 3) * -off;
    }

    printf("carregou mapa\n");
    printf("Jogador em: (%d, %d)\n", x_jogador, y_jogador);
    printf("Inimigo em: (%d, %d)\n", x_inimigo, y_inimigo);
}

void iniciar_caixa(Caixa* caixas, int i) {
    bool pos_valida = false;
    while (!pos_valida) {
        int bloco_x = rand() % LARGURA;
        int bloco_y = rand() % ALTURA;
        if (mapa[bloco_y][bloco_x].tipo == PISTA) {
            pos_valida = true;
            caixas[i].posicao.x = bloco_x * PROP;
            caixas[i].posicao.y = (bloco_y * (PROP + DIF_Y)) + (DIF_Y/2);
        }
    }
    caixas[i].cor = YELLOW;
    caixas[i].hitbox = (Rectangle){ caixas[i].posicao.x, caixas[i].posicao.y, PROP, PROP};
    // Caixa tem PROP de largura e PROP de altura, e está no meio do bloco verticalmente
    caixas[i].tipo = rand() % 4;
}

void zerar(Carro *carro_n) {
    carro_n->velocidade.x = 0.0f;
    carro_n->velocidade.y = 0.0f;
    carro_n->rotacao = 0.0f;
    carro_n->aceleracao = 0.35f;
    carro_n->vida = 100;
    carro_n->poder = -1;
    carro_n->tempo_escudo = 0;
    carro_n->tempo_nitro = 0;
    carro_n->tiro.ativo = false;
    carro_n->bomba.ativo = false;
    carro_n->tempo_dano = 0;
    carro_n->direcao_x = true;
    carro_n->fator = 1;
    carro_n->comecou = false;
    carro_n->cont_checkpoint = 0;
    carro_n->voltas = 0;
    carro_n->cooldown_chegada = 0;
    carro_n->n_checkpoint = 0;
}

bool esta_colidindo(Carro* meuCarro, float temp_x, float temp_y) {
    const float inset_fator = 0.98f; // folga ao redor do carro
    atualiza_vertices(meuCarro);

    Vector2 centro = meuCarro->posicao;

    for (int i = 0; i < 8; i++) {
        // calcula a posição relativa do vértice em relação ao centro e aplica inset + deslocamento
        Vector2 rel = { meuCarro->vertices[i].x - centro.x, meuCarro->vertices[i].y - centro.y };
        Vector2 v = { temp_x + rel.x * inset_fator, temp_y + rel.y * inset_fator };

        int bloco_x = (int)(v.x / PROP);
        int bloco_y = (int)(v.y / (PROP + DIF_Y));

        if (bloco_x < 0 || bloco_x >= LARGURA || bloco_y < 0 || bloco_y >= ALTURA)
            return true;

        if (mapa[bloco_y][bloco_x].colide)
            return true;
    }

    return false;
}

void atualiza_vertices(Carro* carro_n) {
    float ang = DEG2RAD * carro_n->rotacao;
    Vector2 metade = { CARRO_X / 2.0f, CARRO_Y / 2.0f };

    Vector2 v_temp[8];
    v_temp[0] = (Vector2){ -metade.x, -metade.y };
    v_temp[1] = (Vector2){  metade.x, -metade.y };
    v_temp[2] = (Vector2){  metade.x,  metade.y }; 
    v_temp[3] = (Vector2){ -metade.x,  metade.y };
    v_temp[4] = (Vector2){ 0, -metade.y };
    v_temp[5] = (Vector2){ metade.x, 0 };
    v_temp[6] = (Vector2){ 0, metade.y };
    v_temp[7] = (Vector2){ -metade.x, 0 };

    float cosR = cosf(ang);
    float sinR = sinf(ang);
    
    for (int i = 0; i < 8; i++) {
        float x_rot = v_temp[i].x * cosR - v_temp[i].y * sinR;
        float y_rot = v_temp[i].x * sinR + v_temp[i].y * cosR;
        carro_n->vertices[i].x = carro_n->posicao.x + x_rot;
        carro_n->vertices[i].y = carro_n->posicao.y + y_rot;
    }
}

void pause() {
    if (IsKeyPressed(KEY_P)) {
        estado = JOGANDO;
    }
    if (IsKeyPressed(KEY_M)) {
        estado = MENU;
    }

    BeginDrawing();
        ClearBackground(BLACK);
        DrawText("PAUSADO", LARGURA_TELA / 2 - 50, ALTURA_TELA / 2 - 20, 30, WHITE);
        DrawText("Pressione P para continuar", LARGURA_TELA / 2 - 150, ALTURA_TELA / 2 + 20, 20, WHITE);
        DrawText("Pressione M para voltar ao menu", LARGURA_TELA / 2 - 170, ALTURA_TELA / 2 + 50, 20, WHITE);
    EndDrawing();
}

void ganhou() {
    if (IsKeyPressed(KEY_ENTER)) {
        estado = MENU;
    }

    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("GANHOU", 100, ALTURA_TELA / 2 - 150, 250, RED);
    DrawText("Pressione ENTER para voltar ao menu", LARGURA_TELA / 2 - 10, ALTURA_TELA - 50, 20, WHITE);
    EndDrawing();
}

void perdeu() {
    if (IsKeyPressed(KEY_ENTER)) {
        estado = MENU;
    }

    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("PERDEU", 100, ALTURA_TELA / 2 - 150, 250, BLUE); 
    DrawText("Pressione ENTER para voltar ao menu", LARGURA_TELA / 2 - 150, ALTURA_TELA - 50, 20, WHITE);
    EndDrawing();
}