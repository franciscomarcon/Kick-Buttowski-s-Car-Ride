#include "game.h"

void carrega_mapa(const char* nome_arquivo, carro_t* jogador, carro_t* inimigo) {
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

void iniciar_caixa(caixa_t* caixas, int i) {
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
    // caixa_t tem PROP de largura e PROP de altura, e está no meio do bloco verticalmente
    caixas[i].tipo = rand() % 4;
}

void zerar(carro_t *carro_n) {
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

bool esta_colidindo(carro_t* meu_carro, float temp_x, float temp_y) {
    const float inset_fator = 0.98f; // folga ao redor do carro
    atualiza_vertices(meu_carro);

    Vector2 centro = meu_carro->posicao;

    for (int i = 0; i < 8; i++) {
        // calcula a posição relativa do vértice em relação ao centro e aplica inset + deslocamento
        Vector2 rel = { meu_carro->vertices[i].x - centro.x, meu_carro->vertices[i].y - centro.y };
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

void atualiza_vertices(carro_t* carro_n) {
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

    float cos_rotacao = cosf(ang);
    float sin_rotacao = sinf(ang);
    
    for (int i = 0; i < 8; i++) {
        float x_rot = v_temp[i].x * cos_rotacao - v_temp[i].y * sin_rotacao;
        float y_rot = v_temp[i].x * sin_rotacao + v_temp[i].y * cos_rotacao;
        carro_n->vertices[i].x = carro_n->posicao.x + x_rot;
        carro_n->vertices[i].y = carro_n->posicao.y + y_rot;
    }
}
