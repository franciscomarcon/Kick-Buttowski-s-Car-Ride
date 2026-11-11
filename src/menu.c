#include "game.h"

void menu(carro_t* jogador, carro_t* inimigo, caixa_t* caixas, Texture2D background) {
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

void novo_jogo(carro_t* jogador, carro_t* inimigo, caixa_t* caixas, Texture2D background) {
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
        carrega_mapa(TextFormat("assets/maps/pista%i.txt", opcao_mapa), jogador, inimigo);
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

void salvar_jogo(carro_t* jogador, carro_t* inimigo, caixa_t* caixas) {
    FILE* arquivo = fopen("salvamento.bin", "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para salvar o jogo.\n");
        return;
    }

    fwrite(&opcao_mapa, sizeof(int), 1, arquivo);
    fwrite(jogador, sizeof(carro_t), 1, arquivo);
    fwrite(inimigo, sizeof(carro_t), 1, arquivo);
    fwrite(caixas, sizeof(caixa_t), QUANT_CAIXAS, arquivo);

    fclose(arquivo);
    printf("Jogo salvo\n");
}

void carregar_jogo(carro_t* jogador, carro_t* inimigo, caixa_t* caixas) {
    FILE* arquivo = fopen("salvamento.bin", "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para carregar o jogo.\n");
        return;
    }

    int carrega_map;
    fread(&carrega_map, sizeof(int), 1, arquivo);
    carrega_mapa(TextFormat("assets/maps/pista%i.txt", carrega_map), jogador, inimigo);

    fread(jogador, sizeof(carro_t), 1, arquivo);
    fread(inimigo, sizeof(carro_t), 1, arquivo);
    fread(caixas, sizeof(caixa_t), QUANT_CAIXAS, arquivo);

    fclose(arquivo);
    estado = TELA_CONTAGEM;
    frames_contagem = 3 * 60;

    printf("Jogo carregado\n");
}

void pause(void) {
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
