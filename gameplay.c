#include "game.h"

void tela_contagem(sons_t sons){
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

void jogando(carro_t *meu_carro, carro_t* inimigo, caixa_t* caixas, texturas_t texturas, sons_t sons) {
    atualiza_jogador(meu_carro, caixas, inimigo, sons);

    atualiza_ia(inimigo, caixas, meu_carro, sons);

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
        if (meu_carro->tempo_escudo > 0) {
            DrawTexturePro(
                texturas.carroescudo_t,
                (Rectangle){ 0.0f, 0.0f, (float)texturas.car1_t.width, (float)texturas.car1_t.height },
                (Rectangle){ meu_carro->posicao.x, meu_carro->posicao.y, CARRO_X, CARRO_Y },
                (Vector2){ CARRO_X/2, CARRO_Y/2 },
                meu_carro->rotacao,
                WHITE
            );
        }
        else if (meu_carro->tempo_dano > 0) {
            DrawTexturePro(
                texturas.carrodano_t,
                (Rectangle){ 0.0f, 0.0f, (float)texturas.car1_t.width, (float)texturas.car1_t.height },
                (Rectangle){ meu_carro->posicao.x, meu_carro->posicao.y, CARRO_X, CARRO_Y },
                (Vector2){ CARRO_X/2, CARRO_Y/2 },
                meu_carro->rotacao,
                WHITE
            );
        }
        else {
            DrawTexturePro(
                texturas.car1_t,
                (Rectangle){ 0.0f, 0.0f, (float)texturas.car1_t.width, (float)texturas.car1_t.height },
                (Rectangle){ meu_carro->posicao.x, meu_carro->posicao.y, CARRO_X, CARRO_Y },
                (Vector2){ CARRO_X/2, CARRO_Y/2 },
                meu_carro->rotacao,
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
        if (meu_carro->tiro.ativo) {
            DrawTexture(texturas.tiro_t, meu_carro->tiro.posicao.x - PROP/2, meu_carro->tiro.posicao.y - PROP/2, WHITE);
        }

        if (meu_carro->bomba.ativo) {
            DrawTexture(texturas.bomba_t, meu_carro->bomba.posicao.x - PROP/2, meu_carro->bomba.posicao.y - PROP/2, WHITE);
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
        DrawText(TextFormat("%i/3", meu_carro->voltas + 1), 18, ALTURA_TELA - 11 - tam, tam, WHITE);
        DrawText(TextFormat("%i", meu_carro->vida), 159, ALTURA_TELA - 11 - tam, tam, RED);
        DrawText(TextFormat("%i", inimigo->vida), 1021, ALTURA_TELA - 11 - tam, tam, BLUE);

        int x_item = 463;
        int altura_poder = 53;
        
        switch (meu_carro->poder){
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
