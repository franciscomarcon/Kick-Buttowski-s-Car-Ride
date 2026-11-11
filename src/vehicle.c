#include "game.h"

void atualiza_ia(carro_t* inimigo, caixa_t* caixas, carro_t* meu_carro, sons_t sons) {
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

    if (carros_colidindo(inimigo, meu_carro, temp_x, temp_y)) {
        temp_x = inimigo->posicao.x;
        temp_y = inimigo->posicao.y;
        inimigo->velocidade.x = 0;
        inimigo->velocidade.y = 0;
    }


    inimigo->posicao.x = temp_x;
    inimigo->posicao.y = temp_y;

    if (avalia_poder_ia(inimigo, meu_carro)) { //avalia se deve usar o poder
        usa_poder(inimigo, sons);
    }

    atualiza_carro_geral(inimigo, meu_carro, caixas, 12.0f, 0.94f, sons);

    if (fim_de_jogo(inimigo, meu_carro))
        estado = PERDEU;
}

void virou_ia(carro_t* inimigo, int indice_x, int indice_y) {
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

void atualiza_jogador(carro_t* meu_carro, caixa_t* caixas, carro_t* inimigo, sons_t sons) {
    if (!meu_carro) return;

    if (IsKeyDown(KEY_W)) {
        meu_carro->velocidade.x += meu_carro->aceleracao * cosf(DEG2RAD * meu_carro->rotacao);
        meu_carro->velocidade.y += meu_carro->aceleracao * sinf(DEG2RAD * meu_carro->rotacao);
    }
    if (IsKeyDown(KEY_S)) {
        meu_carro->velocidade.x -= meu_carro->aceleracao * cosf(DEG2RAD * meu_carro->rotacao);
        meu_carro->velocidade.y -= meu_carro->aceleracao * sinf(DEG2RAD * meu_carro->rotacao);
    }

    float rotacao_antiga = meu_carro->rotacao;
    bool tentou_rotacionar = false;

    if (IsKeyDown(KEY_A)) {
        meu_carro->rotacao -= 3.5f;
        tentou_rotacionar = true;
    }
    if (IsKeyDown(KEY_D)) {
        meu_carro->rotacao += 3.5f;
        tentou_rotacionar = true;
    }

    if (IsKeyPressed(KEY_P)) {
        estado = PAUSADO;
    }

    if (IsKeyDown(KEY_SPACE)){
        usa_poder(meu_carro, sons);
    }

    colisao_jogador(meu_carro, inimigo, tentou_rotacionar, rotacao_antiga);

    float v_max = 15.0f;
    float atrito = 0.93f;
    atualiza_carro_geral(meu_carro, inimigo, caixas, v_max, atrito, sons);

    if (fim_de_jogo(meu_carro, inimigo))
        estado = GANHOU;
}

void colisao_jogador(carro_t* meu_carro, carro_t* inimigo, bool tentou_rotacionar, float rotacao_antiga) {
    int original_y = (int)(meu_carro->posicao.y / (PROP + DIF_Y));
    int original_x = (int)(meu_carro->posicao.x / PROP);
    
    if (tentou_rotacionar) {
        // Verifique se a NOVA rotação, na POSIÇÃO ATUAL, causa colisão
        if (esta_colidindo(meu_carro, meu_carro->posicao.x, meu_carro->posicao.y)) {
            // A rotação prendeu o carro na parede 
            meu_carro->rotacao = rotacao_antiga;
            if (mapa[original_y + 1][original_x].colide)
                meu_carro->posicao.y -= PROP / 10; // Afasta para cima
            if (mapa[original_y - 1][original_x].colide)
                meu_carro->posicao.y += PROP / 10; // Afasta para baixo
            if (mapa[original_y][original_x + 1].colide) 
                meu_carro->posicao.x -= PROP / 10; // Afasta para esquerda
            if (mapa[original_y][original_x - 1].colide)
                meu_carro->posicao.x += PROP / 10; // Afasta para direita
        }

        if (carros_colidindo(meu_carro, inimigo, meu_carro->posicao.x, meu_carro->posicao.y)) {
            meu_carro->rotacao = rotacao_antiga;
        }
    }

    float temp_x = meu_carro->posicao.x + meu_carro->velocidade.x;
    float temp_y = meu_carro->posicao.y + meu_carro->velocidade.y;

    if (esta_colidindo(meu_carro, temp_x, temp_y)) {
        if (esta_colidindo(meu_carro, temp_x, meu_carro->posicao.y)){
            meu_carro->velocidade.x *= -0.6f;
            temp_x = meu_carro->posicao.x;
        }
        if (esta_colidindo(meu_carro, meu_carro->posicao.x, temp_y)){
            meu_carro->velocidade.y *= -0.6f;
            temp_y = meu_carro->posicao.y;
        }
    }

    if (carros_colidindo(meu_carro, inimigo, temp_x, temp_y)) {
        if (carros_colidindo(meu_carro, inimigo, temp_x, meu_carro->posicao.y)){
            temp_x = meu_carro->posicao.x;
            meu_carro->velocidade.x = 0;
        }
        if (carros_colidindo(meu_carro, inimigo, meu_carro->posicao.x, temp_y)){
            temp_y = meu_carro->posicao.y;
            meu_carro->velocidade.y = 0;
        }
    }

    
    meu_carro->posicao.x = temp_x;
    meu_carro->posicao.y = temp_y;
}

bool carros_colidindo(carro_t* carro_1, carro_t* carro_2, float temp_x, float temp_y) {
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
    float cos_rotacao = cosf(ang);
    float sin_rotacao = sinf(ang);

    for(int i = 0; i < 8; i++) {
        float x_rot = locais[i].x * cos_rotacao - locais[i].y * sin_rotacao;
        float y_rot = locais[i].x * sin_rotacao + locais[i].y * cos_rotacao;
        
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

void atualiza_carro_geral(carro_t* carro_1, carro_t* carro_2, caixa_t* caixas, float v_max, float atrito, sons_t sons) {
    if (carro_1->velocidade.x > v_max) carro_1->velocidade.x = v_max;
    if (carro_1->velocidade.y > v_max) carro_1->velocidade.y = v_max;
    if (carro_1->velocidade.x < -v_max) carro_1->velocidade.x = -v_max;
    if (carro_1->velocidade.y < -v_max) carro_1->velocidade.y = -v_max;

    interagir(carro_1, carro_2, caixas, sons);
    atualiza_poder(carro_1);

    int indice_y = (int)(carro_1->posicao.y / (PROP + DIF_Y));
    int indice_x = (int)(carro_1->posicao.x / PROP);
    
    if (carro_1->comecou) // só conta checkpoints depois de começar a corrida
        atualiza_checkpoint(carro_1);
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

void atualiza_checkpoint(carro_t* carro_n){
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

bool fim_de_jogo(carro_t* carro_1, carro_t* carro_2) {
    if (carro_1->voltas >= N_VOLTAS) {
        return true;
    }
    if (carro_2->vida <= 0) {
        return true;
    }
    return false;
}
