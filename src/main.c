#include "game.h"

int main(void) {
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Kick Butowski's car ride");

    SetTargetFPS(60);

    InitAudioDevice();

    Music playlist[N_MUSICAS] = {
        LoadMusicStream("assets/music/Kick_Buttowski_Intro.mp3"),
        LoadMusicStream("assets/music/Bad_Boys.mp3"),
        LoadMusicStream("assets/music/California_Love.mp3"),
        LoadMusicStream("assets/music/Feel_Good_Inc.mp3"),
        LoadMusicStream("assets/music/Love_Me_Again.mp3"),
        LoadMusicStream("assets/music/Pump_It.mp3"),
        LoadMusicStream("assets/music/Six_Days.mp3"),
        LoadMusicStream("assets/music/Without_Me.mp3")
    };

    for (int i = 0; i < N_MUSICAS; i++)
        playlist[i].looping = false;

    int musica_atual = 0;
    PlayMusicStream(playlist[musica_atual]); //começa da intro do Kick butowski 

    texturas_t texturas = {
        .fundo_menu = LoadTexture("assets/textures/menu.jpg"),
        .car1_t = LoadTexture("assets/textures/car.png"),
        .car2_t = LoadTexture("assets/textures/car2.png"),
        .tiro_t = LoadTexture("assets/textures/tiro.png"),
        .bomba_t = LoadTexture("assets/textures/bomba.png"),
        .caixa_t = LoadTexture("assets/textures/moeda.png"),
        .carroescudo_t = LoadTexture("assets/textures/carroescudo.png"),
        .carro2escudo_t = LoadTexture("assets/textures/carro2escudo.png"),
        .grama_t = LoadTexture("assets/textures/gramav2.png"),
        .rua_t = LoadTexture("assets/textures/ruav2.png"),
        .largada_t = LoadTexture("assets/textures/largadav2.png"),
        .pneu_t = LoadTexture("assets/textures/pneuv2.png"),
        .carrodano_t = LoadTexture("assets/textures/queimado.png"),
        .barrainventario_t = LoadTexture("assets/textures/barra_inventario/barra_inventario.png"),
        .bomba_barra = LoadTexture("assets/textures/barra_inventario/bomba_barra.png"),
        .escudo_barra = LoadTexture("assets/textures/barra_inventario/escudo_barra.png"),
        .nitro_barra = LoadTexture("assets/textures/barra_inventario/nitro_barra.png"),
        .tiro_barra = LoadTexture("assets/textures/barra_inventario/tiro_barra.png")
    };

    sons_t sons = {
        .som_bomba = LoadSound("assets/sounds/som_bomba.mp3"),
        .som_tiro = LoadSound("assets/sounds/som_tiro.mp3"),
        .som_engatilhado = LoadSound("assets/sounds/som_engatilhado.mp3"),
        .som_escudo = LoadSound("assets/sounds/som_escudo.mp3"),
        .som_usoescudo = LoadSound("assets/sounds/som_usoescudo.mp3"),
        .som_nitro = LoadSound("assets/sounds/som_nitro.mp3"),
        .som_largada = LoadSound("assets/sounds/som_largada.mp3")
    };

    carro_t jogador;
    carro_t inimigo;

    caixa_t caixas[QUANT_CAIXAS];
    srand(time(NULL));

    FilePathList lista_arquivos = LoadDirectoryFilesEx("assets/maps", ".txt", false); // conta quantos arquivos .txt existem na pasta "assets/maps"
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
