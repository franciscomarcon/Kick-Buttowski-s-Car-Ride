#include "game.h"

int main(void) {
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

    texturas_t texturas = {
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

    sons_t sons = {
        .som_bomba = LoadSound("sons/som_bomba.mp3"),
        .som_tiro = LoadSound("sons/som_tiro.mp3"),
        .som_engatilhado = LoadSound("sons/som_engatilhado.mp3"),
        .som_escudo = LoadSound("sons/som_escudo.mp3"),
        .som_usoescudo = LoadSound("sons/som_usoescudo.mp3"),
        .som_nitro = LoadSound("sons/som_nitro.mp3"),
        .som_largada = LoadSound("sons/som_largada.mp3")
    };

    carro_t jogador;
    carro_t inimigo;

    caixa_t caixas[QUANT_CAIXAS];
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
