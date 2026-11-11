#include "game.h"

void interagir(carro_t* carro_1, carro_t* carro_2, caixa_t* caixas, sons_t sons) {
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

int interagiu_item(carro_t* carro_1, carro_t* carro_2, caixa_t* caixas) {
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

void usa_poder(carro_t* carro_n, sons_t sons) {
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

void atualiza_poder(carro_t* carro_n) {
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

bool avalia_poder_ia(carro_t* inimigo, carro_t* meu_carro) {
    bool usar = false;

    if (inimigo->poder == NITRO && inimigo->tempo_nitro == 0) {
        usar = true;
    }

    else if (inimigo->poder == ESCUDO && inimigo->tempo_escudo == 0 && (meu_carro->tiro.ativo || meu_carro->bomba.ativo || meu_carro->tempo_escudo > 0)) {
        usar = true;
    }

    else if (inimigo->poder == ARMA && meu_carro->cont_checkpoint < inimigo->cont_checkpoint){
        usar = true;
    }

    else if (inimigo->poder == ARMA && meu_carro->tempo_escudo == 0 && !inimigo->tiro.ativo){
        for (int i = -5; i <= 5; i++) {
            float angulo_tiro = atan2f(meu_carro->posicao.y - inimigo->posicao.y, meu_carro->posicao.x - inimigo->posicao.x);
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
