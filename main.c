//puxa bibliotecas padroes
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

//puxa a biblioteca allegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

//constantes
//fixo de tamanho de tela
#define LAR 1370
#define ALT 835
//FPS
#define FPS 30

//estruturas

struct tiro{
    double x;
    double y;
    int vel;
    ALLEGRO_BITMAP *Laser;
    int astID;
};

struct errei{
    char *ex;
    char *es;
    char *tr;
};

struct Asteroides {
    double x;
    double y;
    int vel;
    ALLEGRO_BITMAP *Img;
    int id;
    char palex[50];
    char pales[50];
    char paltr[50];
    int delay;
};
//define elemento
struct elemento {
    struct Asteroides dados;
    struct elemento *Prox;
};
//define um tipo de elemento auxiliar
typedef struct elemento Elem;
//um tipo de variavel lista que eh um ponteiro
typedef struct elemento* Lista;
//cria a cabeca da lista
Lista *li;

//declaracao de funcoes
bool inicializar();
bool carrega();
void desenha(int score);
void destruir();
void loadScreen();
void ins_list(int num, Lista* li);
void anima(Lista* li, int frame, int agFramex, int agFramey);
bool telaInicial();
bool telaMenu();
void input(ALLEGRO_EVENT letra);
bool pausar(int score);
void compara(Lista* li);
short strcompar(char vet[], char vit[]);
bool set();
bool continuar();



//variaveis globais
//declaracao da janela
ALLEGRO_DISPLAY *Janela = NULL;
    
//declaracao de imagens
ALLEGRO_BITMAP *Nave = NULL;
ALLEGRO_BITMAP *Fundo = NULL;
ALLEGRO_BITMAP *Vida[3];
ALLEGRO_BITMAP *Asteroide = NULL;
ALLEGRO_BITMAP *fundoStart = NULL;
ALLEGRO_BITMAP *Preto = NULL;
ALLEGRO_BITMAP *Tutor = NULL;

    
//declaracao de eventos
ALLEGRO_EVENT_QUEUE *Fila_eventos = NULL;

//declaracao da fonte
ALLEGRO_FONT *Fonte = NULL;
ALLEGRO_FONT *Fload = NULL;
ALLEGRO_FONT *Exb = NULL;
ALLEGRO_FONT *Fscore = NULL;
ALLEGRO_FONT *Fini = NULL;

//declaracao de timer
ALLEGRO_TIMER *Fps = NULL;

//declara sample
ALLEGRO_SAMPLE *Mfundo = NULL;
ALLEGRO_SAMPLE *Msplash = NULL;
ALLEGRO_SAMPLE *Merro = NULL;
ALLEGRO_SAMPLE *Mcerto = NULL;

//variaveis do input
char let[50];
short pos = 0;
ALLEGRO_FONT *Inf = NULL;
bool agudo = 0;
bool circunflexo = 0;
bool crase = 0;
bool til = 0;
bool valida = 1;


//vidas do jogador!
short lifes = 0;
short acerto = 4;

//tipo de jogo
int tipo = 1;

//rotação
double angulo;

//diretorio
char *Usfont;
char *Nome;
int max;
bool tipJog;
char *Ajuda;

//verificação de som
bool foiMus = true;

//tiro
struct tiro tiros;
bool atirar = false;

//salvar as palavras erradas pelo jogador
struct errei palser[3];

//delay do buffer
int bdl = 0;

int main(int argc, char *argv[]){
    
    //declaração de variaveis
    //desenho e jogo
    bool fim_jogo = false;
    bool redesenha = true;
    
    //variaveis dos meus frames de aimação do asteroide
    int agFramex = 0;
    int agFramey = 0;
    int conFrame = 0;
    int delayFrame = 2;
    int frame = 128;
    
    //pontuação
    int score = 0;
    
    //numero de asteroides minimo
    int numAst;
    
    //variavel que diz se tem ou não uma nova wave
    bool wave = false;
    
    //contagem de rounds
    int round = 0;
    
    //starta o srand
    srand(time(NULL));
    
    //variavel de continuar o jogo
    bool continua = 1;
    
    //inicializa tudo a ser inicializado
    if(!inicializar()){
        return -1;
    }
    
    //esconde o mouse da tela
    al_hide_mouse_cursor(Janela);
    
    //loadscreen
    loadScreen();
    
    //carrega tudo a ser caregado
    if(!carrega()){
        return -1;
    }
    
    //tela inicial
    if(!telaInicial()){
        destruir();
        return 0;
    }
    
    while(!fim_jogo){
        
        if(foiMus == false){
            //starto a música de fundo
            al_play_sample(Msplash, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
        }
        
        //chama a funcao de menu e sai caso o retorno seja 0
        if(!telaMenu()){
            destruir();
            return 0;
        }
        
        //inicializa o vetor de input
        for(int i = 0; i < 50; i++){
            let[i] = '\0';
        }

        //lista de asteroides iniciada
        Lista* li = (Lista*) malloc(sizeof(Lista));
        if(li != NULL){
            *li = NULL;
        }

        //start timer
        al_start_timer(Fps);
    
        fprintf(stderr, "nave esta em x %i e y %i", (LAR/2)-(al_get_bitmap_width(Nave)/2), (ALT-200)+al_get_bitmap_height(Nave)/2);
    
        //paro todas as musicas em execussão
        al_stop_samples();
    
        continua = 1;
    
        while(continua){
            
            //inicia a execussao da musica
            al_play_sample(Mfundo, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
            
            //resetando valores para continue
            fim_jogo = false;
            redesenha = true;
            agFramex = 0;
            agFramey = 0;
            conFrame = 0;
            delayFrame = 2;
            frame = 128;
            score = 0;
            wave = false;
            round = 0;
            angulo = -90*3.14159/180;
            lifes = 0;
            acerto = 4;
            while (pos > 0) {
                pos--;
                let[pos] = '\0';
            }
            if(pos < 0)
                pos = 0;
            foiMus = false;
            continua = 1;
            tiros.x = 650;
            tiros.y = 680;
            tiros.Laser = al_load_bitmap("Imagens/Laserball.png");
            if(!tiros.Laser){
                fprintf(stderr, "falha ao carregar imagem de tiro\n");
            }
            tiros.vel = 40;
            
            //loop do jogo
            while(!fim_jogo){
        
                //cria uma variavel de eventos e espera um evento
                ALLEGRO_EVENT evento;
                al_wait_for_event(Fila_eventos, &evento);
        
                //acontece um evento a cada 1/30 segundo
                if(evento.type == ALLEGRO_EVENT_TIMER){
            
                    //verifico se a lista está vazia, se estiver libero para uma nova wave e somo o round
                    if(*li == NULL){
                        wave = true;
                        round++;
                    }
        
                    //falo que já pode redesenhar
                    redesenha = true;
            
                    //codigo da velocidade de animacao do asteroide
                    if(conFrame >= delayFrame){
                        if(agFramex >= 7 && agFramey >= 3){
                            agFramex = 1;
                            agFramey = 0;
                        } else if(agFramex >= 7){
                            agFramey++;
                            agFramex = 0;
                        } else {
                            agFramex++;
                        }
                        conFrame = 0;
                    }
            
                    //limpa a tela e desenha o novo frame
                    al_clear_to_color(al_map_rgb(0,0,0));
                    desenha(score);
            
                    //verifica se existe uma nova wave, se existir, eu reencho a lista de asteroides
                    if(wave){
                        numAst = (rand()%round) + 1;
                        ins_list(numAst, li);
                        wave = false;
                    }
                
                    //faço a comparação para ver se o que foi digitado é pertinente a algo da lista
                    compara(li);
            
                    //verifico acerto, caso == 0 é considerado um erro, caso == 1 é considerado que acertou
                    if(acerto == 0){
                        acerto = 4;
                        bdl = 45;
                        al_play_sample(Merro, 1.39, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                    while (pos > 0) {
                        pos--;
                        let[pos] = '\0';
                    }
                    if(pos < 0)
                        pos = 0;
                    } else if(acerto == 1){
                        score++;
                        acerto = 4;
                        al_play_sample(Mcerto, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                        while (pos > 0) {
                            pos--;
                            let[pos] = '\0';
                        }
                        if(pos < 0)
                            pos = 0;
                    }
            
                    //desenha a animacao do asteroide
                    anima(li, frame, agFramex, agFramey);
                    conFrame++;
                    
                    bdl--;
                    if(bdl < 0){
                        bdl = 0;
                    }
            
                    //condição para o jogo acontecer
                    if(lifes >= 3){
                        Elem* no = *li;
                        while (no != NULL) {
                            if(no == *li){
                                *li = no->Prox;
                                free(no);
                                no = *li;
                            }
                        }
                        if(continuar()){
                            fim_jogo = true;
                            continua = 0;
                        } else {
                            fim_jogo = true;
                        }
                    }
            
                    //botões que pertencem ao jogo
                } else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
                    switch (evento.keyboard.keycode) {
                        case ALLEGRO_KEY_BACKSPACE:
                            while (pos > 0) {
                                pos--;
                                let[pos] = '\0';
                            }
                            if(pos < 0)
                                pos = 0;
                            break;
                        case ALLEGRO_KEY_1:
                            agudo = true;
                            break;
                        case ALLEGRO_KEY_2:
                            circunflexo = true;
                            break;
                        case ALLEGRO_KEY_3:
                            til = true;
                            break;
                        case ALLEGRO_KEY_4:
                            crase = true;
                            break;
                        case ALLEGRO_KEY_ESCAPE:
                            fim_jogo = pausar(score);
                            continua = 0;
                            break;
                        default:
                            if(pos < 50 && bdl <= 0)
                                input(evento);
                            break;
                    }
                }
            
                //verifica se deve redesenhar a tela
                if(redesenha && al_is_event_queue_empty(Fila_eventos)){
                    redesenha = false;
                    al_flip_display();
                }
            }
        }
        al_stop_samples();
        fim_jogo = false;
    }
    
    //destroi tudo
    destruir();
   
    return 0;
}

//inicializa tudo que deve ser inicializado
bool inicializar(){

    //inicia allegro, caso erro da a msg
    if(!al_init()) {
        fprintf(stderr, "failed to initialize allegro!\n");
        return 0;
    }
    //Inicializa o add-on para utilização de imagens
    if (!al_init_image_addon()){
        fprintf(stderr, "Falha ao inicializar add-on allegro_image.\n");
        return 0;
    }

    //inicia o add-on para uso de fontes
    al_init_font_addon();

    //inicia addon para uso de fontes TRUE TYPE
    if(!al_init_ttf_addon()){
        fprintf(stderr, "Falha ao inicializar add-on al_init_ttf_addon!.\n");
        return 0;
    }
    
    //codigo para fazer fullscreen
    ALLEGRO_MONITOR_INFO info;
    int res_x_comp, res_y_comp;
    al_get_monitor_info(0, &info);
    res_x_comp = info.x2 - info.x1;
    res_y_comp = info.y2 - info.y1;
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    
    //cria a janela
    Janela = al_create_display(res_x_comp, res_y_comp);
    if(!Janela) {
        fprintf(stderr, "failed to create display!\n");
        return 0;
    }
    
    //continua o codigo para fullscreen
    float red_x = res_x_comp / (float) LAR;
    float red_y = res_y_comp / (float) ALT;
    ALLEGRO_TRANSFORM transformar;
    al_identity_transform(&transformar);
    al_scale_transform(&transformar, red_x, red_y);
    al_use_transform(&transformar);
    
    //inicia timer
    Fps = al_create_timer(1.0/FPS);
    
    //cria fila de eventos
    Fila_eventos = al_create_event_queue();
    if (!Fila_eventos) {
        fprintf(stderr, "Falha ao criar fila de eventos!\n");
        return 0;
    }

    //instala o uso de audio
    al_install_audio();

    //inicia o addon de codec
    al_init_acodec_addon();

    //reservando espaco para numero de sons
    al_reserve_samples(3);
    
    //instala os inputs
    al_install_keyboard();
    al_install_mouse();

    //registra fonte dos eventos
    al_register_event_source(Fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(Fila_eventos, al_get_mouse_event_source());
    al_register_event_source(Fila_eventos, al_get_display_event_source(Janela));
    al_register_event_source(Fila_eventos, al_get_timer_event_source(Fps));

    return 1;
}

//funcao que carrega tudo a ser carregado
bool carrega(){

    //carrega imagem fundo
    Fundo = al_load_bitmap("Imagens/Fundo.jpg");
    if(!Fundo){
        fprintf(stderr, "Falha ao carregar imagem fundo!\n");
        return 0;
    }
    
    //chamo uma imagem meio transparente
    Preto = al_load_bitmap("Imagens/preto.png");
    
    //carrega imagem nave
    Nave = al_load_bitmap("Imagens/Nave.png");
    if (!Nave){
        fprintf(stderr, "Falha ao carregar imagem nave!\n");
        return 0;
    }

    //carregar imagens vida1, vida2, vida3
    Vida[0] = al_load_bitmap("Imagens/Vida.png");
    if(!Vida[0]){
        fprintf(stderr, "Falha ao carregar imagem vida1!\n");
        return 0;
    }
    Vida[1] = al_load_bitmap("Imagens/Vida.png");
    if(!Vida[1]){
        fprintf(stderr, "Falha ao carregar imagem vida2!\n");
        return 0;
    }
    Vida[2] = al_load_bitmap("Imagens/Vida.png");
    if(!Vida[2]){
        fprintf(stderr, "Falha ao carregar imagem vida3!\n");
        return 0;
    }

    //carrega imagem asteroide
    Asteroide = al_load_bitmap("Imagens/Asteroide.png");
    if(!Asteroide){
        fprintf(stderr, "Falha ao carregar imagem asteroide!\n");
        return 0;
    }

    //carrega as musicas
    Mfundo = al_load_sample("Musicas/Mjogo.ogg");
    if(!Mfundo){
        fprintf(stderr, "Falha ao carregar audio!\n");
        return 0;
    }
    
    //musica da splashscreen
    Msplash = al_load_sample("Musicas/Splash.ogg");
    if(!Msplash){
        fprintf(stderr, "Falha ao carregar audio msplash!\n");
        return 0;
    }
    
    //som erro
    Merro = al_load_sample("Musicas/erro.ogg");
    if(!Merro){
        fprintf(stderr, "Falha ao carregar audio de erro!\n");
        return 0;
    }
    
    //carrega imagem inicial
    fundoStart = al_load_bitmap("Imagens/fundoStart.jpg");
    if(!fundoStart){
        fprintf(stderr, "Falha ao carregar fundoStart!\n");
        return 0;
    }
    
    //carrega fonte br
    Inf = al_load_font("Fontes/Arial.ttf", 37, 0);
    if(!Inf){
        al_destroy_display(Janela);
        fprintf(stderr, "Falha ao carregar fonte br");
        return -1;
    }
    
    //fonte score
    Fscore = al_load_font("Fontes/Space.TTF", 24, 0);
    
    Mcerto = al_load_sample("Musicas/laser.ogg");

    
    return 1;
}

//desenha as coisas estaticas na tela
void desenha(int score){
    
    //desenha a imagem do fundo
    al_draw_bitmap(Fundo, -5, -5, 0);
    
    //desenha laser
    if(atirar == true)
        al_draw_bitmap(tiros.Laser, tiros.x, tiros.y, 0);
    
    //desenha a nave rotacionada
    al_draw_rotated_bitmap(Nave, al_get_bitmap_width(Nave)/2, al_get_bitmap_height(Nave)/2, LAR/2, ALT-150, angulo, 0);

    //desenha input
    if (pos > 49) {
        al_draw_text(Inf, al_map_rgb(255, 255, 255), LAR/2, ALT/2, ALLEGRO_ALIGN_CENTER, "Não pode mais caracteres");
    }
    
    
    if(pos == 0 && bdl <= 0){
        al_draw_text(Inf, al_map_rgb(255, 255, 255), (LAR/2 - (al_get_bitmap_width(Nave)/2))+60, ALT-75, ALLEGRO_ALIGN_CENTER, "Digite!");
    } else {
    al_draw_textf(Inf, al_map_rgb(255, 255, 255), (LAR/2 - (al_get_bitmap_width(Nave)/2))+60, ALT-75, ALLEGRO_ALIGN_CENTER, "%s", let);
    }
    
    //escreve na tela
    al_draw_textf(Fscore, al_map_rgb(255, 255, 255), 27, 27, ALLEGRO_ALIGN_LEFT, "Score: %i", score);

    //desenha as vidas na tela
    if(lifes == 0){
    al_draw_bitmap(Vida[0], LAR-136, 27, 0);
    al_draw_bitmap(Vida[1], LAR-99, 27, 0);
    al_draw_bitmap(Vida[2], LAR-62, 27, 0);
    } else if(lifes == 1){
        al_draw_bitmap(Vida[1], LAR-99, 27, 0);
        al_draw_bitmap(Vida[2], LAR-62, 27, 0);
    } else if(lifes == 2){
        al_draw_bitmap(Vida[2], LAR-62, 27, 0);
    }
}

//destroi as coisas estaticas na tela
void destruir(){

    //destroi fonte
    al_destroy_font(Fonte);
    al_destroy_font(Fload);
    
    //Destroi a fila de eventos
    al_destroy_event_queue(Fila_eventos);
    
    //destroi imagens
    al_destroy_bitmap(Nave);
    al_destroy_bitmap(Fundo);
    al_destroy_bitmap(Vida[0]);
    al_destroy_bitmap(Vida[1]);
    al_destroy_bitmap(Vida[2]);
    al_destroy_bitmap(Asteroide);
    al_destroy_bitmap(fundoStart);
    
    //destroi variavel janela
    al_destroy_display(Janela);

    //destroi sample
    al_destroy_sample(Mfundo);
    
    //destroi fps
    al_destroy_timer(Fps);
}

//cria load screen
void loadScreen(){
    //limpo para preto
    al_clear_to_color(al_map_rgb(0,0,0));
    //chamo a nova fonte
    Fload = al_load_font("Fontes/Space.TTF", 48, 0);
    //desenho na tela que esta carregando
    al_draw_text(Fload, al_map_rgb(255, 255, 255), LAR/2, ALT/2, ALLEGRO_ALIGN_CENTER, "Now Loading...");
    al_flip_display();
}

//insere na lista
void ins_list(int num, Lista* li){
    
    //reinicio o random baseado no time
    srand(time(NULL));
    fprintf(stderr, "tem %i asteroides na lista!\n", num);
    
    //crio minha variavel de arquivo
    FILE *arquivo;

    //digo que é no modo leitura no arquivo especificado
    arquivo = fopen(Nome, "r");
    
    //pospal = posicao palavra, palpar = palavra parametro, locpal = localizador de palavra, vari = pariação de delays entre asteroides
    int pospal, palpar = -1, locpal = 0, j = 0, vari = 0, contg = 0;
    
    //caractere pego na string
    char palar, chart;
    
    //loop de preenchimento do das variaveis dos vetores
    for(int i = 0; i < num; i++){
    
        //crio um elemento auxiliar
        Elem* no = (Elem*) malloc(sizeof(Elem));
        
        //caso der erro, para
        if(no == NULL) break;
        
        //atribuo algumas coisas ao meu novo nó
        no->dados.Img = Asteroide;
        no->dados.x = (rand()%1242);
        no->dados.y = -200;
        no->dados.id = i;
        
        //a velocidade não pode ser 0
        do{
            no->dados.vel = (rand()%5);
        }while (no->dados.vel <= 0);
        
        //seto o delay entre asteroides e somo com mais delay
        no->dados.delay = (rand()%(46+vari));
        vari += 50;
        
        //codigo de inicialização de pales, paltr, palex (palavra exibição, escrita e tradução)
        for (int zera = 0; zera < 50; zera++) {
            no->dados.palex[zera] = '\0';
        }
        for (int zera = 0; zera < 50; zera++) {
            no->dados.pales[zera] = '\0';
        }
        for (int zera = 0; zera < 50; zera++) {
            no->dados.paltr[zera] = '\0';
        }
        
        //defino uma nova palavra random com teto no maximo que meu arquivo permite
        pospal = (rand()%max);
        fprintf(stderr, "pospal vale %i\n", pospal);
        
        while(contg < 3){
            chart = fgetc(arquivo);
            if(chart == '\n')
                contg++;
        }
        
        //codigo de preenchimento de pales, paltr, palex (palavra exibição, escrita e tradução)
        while (palpar != pospal) {
        
            //leio o numero na frente
            fscanf(arquivo, "%d", &palpar);
            fgetc(arquivo);
        }
        
        //codigo de colocar as palavras nas variaveis
        while (palar != '\n' && palar != EOF) {
            if (locpal == 0) {
                palar = fgetc(arquivo);
                no->dados.palex[j] = palar;
            }else if(locpal == 1){
                palar = fgetc(arquivo);
                if(palar == '_')
                    palar = ' ';
                no->dados.pales[j] = palar;
            } else if(locpal == 2){
                palar = fgetc(arquivo);
                if(palar == '_')
                    palar = ' ';
                no->dados.paltr[j] = palar;
            }
            if(palar == ' ' || palar == '\n'){
                if(locpal == 0){
                    no->dados.palex[j] = '\0';
                } else if(locpal == 1){
                    no->dados.pales[j] = '\0';
                } else if(locpal == 2){
                    no->dados.paltr[j] = '\0';
                }
                locpal++;
                j = -1;
            }
            j++;
        }
        
        //-3 para não ser um valor que teria no txt
        palpar = -3;
        
        //volto para o inicio do meu txt
        fseek(arquivo, 0, SEEK_SET);
        
        //reseta o palar para que ele não valha nem \0 nem EOF
        palar = 0;
        
        //reseto a localização da palavra
        locpal = 0;
        
        //no->dados.Flist = pathFind(no->dados.x, no->dados.y, no->dados.vel);
        no->Prox = (*li);
        *li = no;
    }
    
    //por via das duvidas zerar a variação
    vari = 0;
    
    //fechando o arquivo
    fclose(arquivo);
}

//anima asteroides
void anima(Lista* li, int frame, int agFramex, int agFramey){
    
    //crio elementos auxiliares
    Elem* no = *li;
    Elem* ant = NULL;
    
    //variaveis para descobrir a reta tangente
    double x, y;
    double xl, yl;
    
    //variavel para saber o angulo da reta
    double tang, tg;
    
    //loop para andar a lista
    while(no != NULL){
        if(no->dados.id == tiros.astID && atirar == true){
            xl = (no->dados.x+50) - tiros.x;
            yl = (no->dados.y+50) - tiros.y;
            tg = atan2(-yl, -xl);
            tiros.x -= tiros.vel*cos(tg);
            tiros.y -= tiros.vel*sin(tg);
        }
        
        //caso a distancia dele para a nave seja menor que 100, destroi o asteroide e tira uma vida
        if(sqrt(pow((LAR/2-al_get_bitmap_width(Nave)/2)-no->dados.x, 2)+pow((ALT-200)-no->dados.y, 2)) < 100){
            
            //alocação de memoria para colocar as palavras que o jogador errou
            palser[lifes].ex = (char*) malloc(sizeof(char)*50);
            strcat(palser[lifes].ex, no->dados.palex);
            palser[lifes].es = (char*) malloc(sizeof(char)*50);
            strcat(palser[lifes].es, no->dados.pales);
            palser[lifes].tr = (char*) malloc(sizeof(char)*50);
            strcat(palser[lifes].tr, no->dados.paltr);
            
            if(no == *li){
                *li = no->Prox;
                free(no);
                no = *li;
            } else {
                ant->Prox = no->Prox;
                free(no);
                no = ant->Prox;
            }
            lifes++;
            //verificação de destruição de asteroide pelo projetil
        } else if(sqrt(pow((tiros.x)-(no->dados.x), 2)+pow((tiros.y)-(no->dados.y), 2)) < 100 && no->dados.id == tiros.astID){
            
            if(no == *li){
                *li = no->Prox;
                free(no);
                no = *li;
            } else {
                ant->Prox = no->Prox;
                free(no);
                no = ant->Prox;
            }
            atirar = false;
            acerto = 4;
            tiros.x = 650;
            tiros.y = 680;
            
            //se o asteroide ja puder ser mandado é mandado
        }else if(no->dados.delay <= 0){
        
            //caso o delay seja menor ou igual a 0
            
            //variaveis de pontos da tangente
            x = no->dados.x - 621;
            y = no->dados.y - 699;
            
            //descubro a tangente
            tang = atan2(-y, -x);
            
            //faço com que o asteroide ande em seno e cosseno da velocidade natual dele
            no->dados.x += no->dados.vel*cos(tang);
            no->dados.y += no->dados.vel*sin(tang);
            
            //atualizo a posição do asteroide
            al_draw_bitmap_region(no->dados.Img, agFramex*frame, agFramey*frame, frame, frame, no->dados.x, no->dados.y, 0);
            
            //desenho a palavra de exibição do asteroide
            al_draw_textf(Exb, al_map_rgb(255, 255, 255), no->dados.x+70, no->dados.y+120,ALLEGRO_ALIGN_CENTER, "%s", no->dados.palex);
            
            //ando na lista
            ant = no;
            no = no->Prox;
            
        } else {
            
            //caso ainda está no delay
            no->dados.delay--;
            ant = no;
            no = no->Prox;
        }
    }
}

//tela inicial
bool telaInicial(){
    
    //limpo a tela para preto
    al_clear_to_color(al_map_rgb(0,0,0));
    
    //contagem de frames
    int cont = 0;
    
    //start timer
    al_start_timer(Fps);
    
    //carrego a fonte
    Fini = al_load_font("Fontes/Space.TTF", 40, 0);
    
    //crio a variavel para o logo
    ALLEGRO_FONT *Logo = NULL;
    
    //chamo a fonte do logo
    Logo = al_load_font("Fontes/Space.TTF", 60, 0);
    
    //desenho o fundo
    al_draw_bitmap(fundoStart, (LAR/2 - (al_get_bitmap_width(fundoStart)/2))+30, ALT/2 - (al_get_bitmap_height(fundoStart)/2), 0);
    
    //atualizo a tela
    al_flip_display();
    
    //falo que é pra desenhar
    bool redesenha = true;
    
    //condição de saida da tela
    bool sai = false;
    
    //starto a música de fundo
    al_play_sample(Msplash, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
    
    //loop para fazer a animação do press enter
    while(!sai){
        
        //crio um evento
        ALLEGRO_EVENT evento;
        al_wait_for_event(Fila_eventos, &evento);
       
        //acontece um evento a cada 1/30 segundo
        if(evento.type == ALLEGRO_EVENT_TIMER){
            
            //somo na contagem de frames
            cont++;
            
            //caso o frame seja maior que 60 zera
            if(cont > 60)
                cont = 0;
            
            //maior que 30 desenha o press enter
            if(cont >= 30){
                al_draw_text(Fini, al_map_rgb(0, 0, 0),LAR/2, (ALT/2)+80,ALLEGRO_ALIGN_CENTER,"PRESS ENTER");
                al_flip_display();
            } else {
        
                //menor que 30 não senha o press enter
                al_clear_to_color(al_map_rgb(0,0,0));
                al_draw_bitmap(fundoStart, (LAR/2 - (al_get_bitmap_width(fundoStart)/2))+30, ALT/2 - (al_get_bitmap_height(fundoStart)/2), 0);
                al_draw_text(Logo, al_map_rgb(255,255,255),LAR/2, 200,ALLEGRO_ALIGN_CENTER,"WORLD WORDS");
                al_flip_display();
            }
        }
        
        //verifica se uma tecla foi pressionada, caso enter = entre no menu, caso esc =  sai do jogo
        if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
            switch (evento.keyboard.keycode){
                case ALLEGRO_KEY_ESCAPE:
                    return 0;
                case ALLEGRO_KEY_ENTER:
                    return 1;
                    break;
            }
        }
    }
    
    //teoricamente se chegar aqui algo deu muito errado
    fprintf(stderr, "Algo errado não esta certo!\n");
    return 0;
}

//menu
bool telaMenu(){
    bool sai =  false;
    FILE *tempor;
    char *ww = ".ww";
    bool show = false;
    char save[30];
    char *prass;
    bool ntem = true;
    bool show0 = false;
    
    //limpa o vetor
    for(int i = 0; i < 30; i++){
        save[i] = '\0';
    }
    
    while(!sai){
        
        //cria uma variavel de eventos e espera um evento
        ALLEGRO_EVENT evento;
        al_wait_for_event(Fila_eventos, &evento);
        if(evento.type == ALLEGRO_EVENT_KEY_DOWN){
            switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_BACKSPACE:
                    while (pos > 0) {
                        pos--;
                        let[pos] = '\0';
                    }
                    if(pos < 0)
                        pos = 0;
                    break;
                case ALLEGRO_KEY_1:
                    agudo = true;
                    break;
                case ALLEGRO_KEY_2:
                    circunflexo = true;
                    break;
                case ALLEGRO_KEY_3:
                    til = true;
                    break;
                case ALLEGRO_KEY_4:
                    crase = true;
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    sai = true;
                    break;
                case ALLEGRO_KEY_ENTER:
                    if(pos > 30){
                        show0 = true;
                        while (pos > 0) {
                            pos--;
                            let[pos] = '\0';
                        }
                        if(pos < 0)
                            pos = 0;
                        for(int i = 0; i < 30; i++){
                            save[i] = '\0';
                        }
                        break;
                    }
                    prass = (char*) malloc(sizeof(char)*30);
                    Nome = (char *) malloc(sizeof(let)+sizeof(ww)+sizeof("Linguas/"));
                    strcat(Nome, "Linguas/");
                    strcat(Nome, let);
                    strcat(Nome, ww);
                    strcat(save, let);
                    tempor = fopen(Nome, "r");
                    if(!tempor){
                        ntem = false;
                        show = true;
                        while (pos > 0) {
                            pos--;
                            let[pos] = '\0';
                        }
                        if(pos < 0)
                            pos = 0;
                        for(int i = 0; i < 30; i++){
                            save[i] = '\0';
                        }
                        free(Nome);
                        free(prass);
                        break;
                    } else {
                        tempor = fopen("lista.ww", "r");
                        do{
                            fscanf(tempor, "%s", prass);
                            fprintf(stderr, "palavra do arquivo = %s      palavra do buffer = %s\n", prass, save);
                            if(strcmp(prass, save) == 0){
                                ntem = false;
                            }
                        }while(fgetc(tempor) != EOF);
                        tempor = fopen("lista.ww", "a");

                        if(ntem){
                            fprintf(tempor, "%s\n", save);
                        }
                        for(int i = 0; i < 29; i++){
                            save[i] = '\0';
                        }
                        free(prass);
                        fclose(tempor);
                        fprintf(stderr, "a string eh =  %s   3\n", Nome);
                        if(!set())
                            return 0;
                        while (pos > 0) {
                            pos--;
                            let[pos] = '\0';
                        }
                        if(pos < 0)
                            pos = 0;
                        return 1;
                    }
                default:
                    if(pos < 50)
                        input(evento);
                    break;
            }
        }
        ntem = true;
        
        //toda a parte escrita e de imagem do menu esta aqui
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(fundoStart, (LAR/2 - (al_get_bitmap_width(fundoStart)/2))+30, ALT/2 - (al_get_bitmap_height(fundoStart)/2), 0);
        al_draw_bitmap(Preto, 0, 0, 0);
        al_draw_text(Inf, al_map_rgb(255, 255, 255), 100, 100, ALLEGRO_ALIGN_LEFT, "Pressione ESC para sair");
        al_draw_text(Inf, al_map_rgb(255, 255, 255), LAR/2, (ALT/2)-150, ALLEGRO_ALIGN_CENTER, "Digite o nome do arquivo de lingua. (O arquivo .ww)");
        al_draw_text(Inf, al_map_rgb(255, 255, 255), LAR/2, (ALT/2)-100, ALLEGRO_ALIGN_CENTER, "Ex: ingles, japones, etc");
        al_draw_textf(Inf, al_map_rgb(255, 255, 255), LAR/2, ALT/2, ALLEGRO_ALIGN_CENTER, "%s", let);
        if(show){
            al_draw_text(Inf, al_map_rgb(255, 255, 255), LAR/2, (ALT/2)+100, ALLEGRO_ALIGN_CENTER, "Não foram encontrados arquivos com este nome na pasta Linguas");
        }
        if(show0){
            al_draw_text(Inf, al_map_rgb(255, 255, 255), LAR/2, (ALT/2)+150, ALLEGRO_ALIGN_CENTER, "O nome deve conter no máximo 30 caracteres");
        }
        al_flip_display();
    }
    return 0;
}

//input, aqui eu defino todas as letras que podem ser pressionadas com seus respectivos acentos
void input(ALLEGRO_EVENT letra){
    switch (letra.keyboard.keycode) {
        case ALLEGRO_KEY_A:
            if (agudo) {
                let[pos] = -61;
                pos++;
                let[pos] = -95;
                pos++;
            }else if(circunflexo){
                let[pos] = -61;
                pos++;
                let[pos] = -94;
                pos++;
            } else if(crase){
                let[pos] = -61;
                pos++;
                let[pos] = -96;
                pos++;
            } else if(til){
                let[pos] = -61;
                pos++;
                let[pos] = -93;
                pos++;
            } else {
            let[pos] = 'a';
            pos++;
            }
            til = false;
            crase = false;
            circunflexo = false;
            agudo = false;
            break;
            
        case ALLEGRO_KEY_B:
            let[pos] = 'b';
            pos++;
            break;
            
        case ALLEGRO_KEY_C:
            if(agudo){
                let[pos] = -61;
                pos++;
                let[pos] = -89;
                pos++;
            } else {
            let[pos] = 'c';
            pos++;
            }
            agudo = false;
            break;
            
        case ALLEGRO_KEY_D:
            let[pos] = 'd';
            pos++;
            break;
            
        case ALLEGRO_KEY_E:
            if (agudo) {
                let[pos] = -61;
                pos++;
                let[pos] = -87;
                pos++;
            } else if(circunflexo){
                let[pos] = -61;
                pos++;
                let[pos] = -86;
                pos++;
            } else if(crase){
                let[pos] = -61;
                pos++;
                let[pos] = -88;
                pos++;
            } else {
            let[pos] = 'e';
            pos++;
            }
            crase = false;
            circunflexo = false;
            agudo = false;
            break;
            
        case ALLEGRO_KEY_F:
            let[pos] = 'f';
            pos++;
            break;
            
        case ALLEGRO_KEY_G:
            let[pos] = 'g';
            pos++;
            break;
            
        case ALLEGRO_KEY_H:
            let[pos] = 'h';
            pos++;
            break;
            
        case ALLEGRO_KEY_I:
            if(agudo){
                let[pos] = -61;
                pos++;
                let[pos] = -83;
                pos++;
            } else if(circunflexo){
                let[pos] = -61;
                pos++;
                let[pos] = -82;
                pos++;
            } else if(crase){
                let[pos] = -61;
                pos++;
                let[pos] = -84;
                pos++;
            } else {
            let[pos] = 'i';
            pos++;
            }
            crase = false;
            circunflexo = false;
            agudo = false;
            break;
            
        case ALLEGRO_KEY_J:
            let[pos] = 'j';
            pos++;
            break;
            
        case ALLEGRO_KEY_K:
            let[pos] = 'k';
            pos++;
            break;
            
        case ALLEGRO_KEY_L:
            let[pos] = 'l';
            pos++;
            break;
            
        case ALLEGRO_KEY_M:
            let[pos] = 'm';
            pos++;
            break;
            
        case ALLEGRO_KEY_N:
            let[pos] = 'n';
            pos++;
            break;
            
        case ALLEGRO_KEY_O:
            if(agudo){
                let[pos] = -61;
                pos++;
                let[pos] = -77;
                pos++;
            } else if(circunflexo){
                let[pos] = -61;
                pos++;
                let[pos] = -76;
                pos++;
            } else if(crase){
                let[pos] = -61;
                pos++;
                let[pos] = -78;
                pos++;
            } else if(til){
                let[pos] = -61;
                pos++;
                let[pos] = -75;
                pos++;
            } else {
            let[pos] = 'o';
            pos++;
            }
            til = false;
            crase = false;
            circunflexo = false;
            agudo = false;
            break;
            
        case ALLEGRO_KEY_P:
            let[pos] = 'p';
            pos++;
            break;
            
        case ALLEGRO_KEY_Q:
            let[pos] = 'q';
            pos++;
            break;
            
        case ALLEGRO_KEY_R:
            let[pos] = 'r';
            pos++;
            break;
            
        case ALLEGRO_KEY_S:
            let[pos] = 's';
            pos++;
            break;
            
        case ALLEGRO_KEY_T:
            let[pos] = 't';
            pos++;
            break;
            
        case ALLEGRO_KEY_U:
            if(agudo){
                let[pos] = -61;
                pos++;
                let[pos] = -70;
                pos++;
            } else if(circunflexo){
                let[pos] = -61;
                pos++;
                let[pos] = -69;
                pos++;
            } else if(crase){
                let[pos] = -61;
                pos++;
                let[pos] = -71;
                pos++;
            } else {
            let[pos] = 'u';
            pos++;
            }
            crase = false;
            circunflexo = false;
            agudo = false;
            break;
            
        case ALLEGRO_KEY_V:
            let[pos] = 'v';
            pos++;
            break;
            
        case ALLEGRO_KEY_W:
            let[pos] = 'w';
            pos++;
            break;
            
        case ALLEGRO_KEY_X:
            let[pos] = 'x';
            pos++;
            break;
            
        case ALLEGRO_KEY_Y:
            let[pos] = 'y';
            pos++;
            break;
            
        case ALLEGRO_KEY_Z:
            let[pos] = 'z';
            pos++;
            break;
        case ALLEGRO_KEY_MINUS:
            let[pos] = '-';
            pos++;
            break;
        case ALLEGRO_KEY_SPACE:
            let[pos] = ' ';
            pos++;
            break;
        default:
            
            break;
    }
}

//funcao pause
bool pausar(int score){
    
    //defino uma opção padrão
    bool op = true;
    
    bool exbtut = false;
    
    //variaveis que mudam a cor do item selecionado
    short x1 = 255, x2 = 255, x3 = 255;
    short y1 = 255, y2 = 255, y3 = 255;
    short z1 = 255, z2 = 255, z3 = 255;
    
    //defino a posição em relação ao menu
    short posic = 0;
    
    //loop do menu pause
    while (op) {
        
        //defino um evento
        ALLEGRO_EVENT pp;
        
        //espero por um evento
        al_wait_for_event(Fila_eventos, &pp);
        
        //caso aperte para cima ou para baixo
        if(pp.type == ALLEGRO_EVENT_KEY_DOWN){
            switch (pp.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    posic--;
                    break;
                    
                case ALLEGRO_KEY_DOWN:
                    posic++;
                    break;
                    
                case ALLEGRO_KEY_ENTER:
                    if (posic == 0) {
                        return 0;
                    } else if(posic == 1){
                        exbtut = true;
                    } else if(posic == 2){
                        return 1;
                    }
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    exbtut = false;
            }
        }
        
        //previno bugs
        if(posic > 2)
            posic = 2;
        if (posic < 0) {
            posic = 0;
        }
        
        //seto tudo branco
        x1 = x2 = x3 = y1 = y2 = y3 = z1 = z2 = z3 = 255;
        
        //caso posic valha algo, as variaveis certas mudarão de cor
        if(posic == 0){
            x1 = 43;
            x2 = x3 = 255;
        } else if (posic == 1){
            y1 = 43;
            y2 = y3 = 255;
        } else if(posic == 2){
            z1 = 43;
            z2 = z3 = 255;
        }
        
        //limpo tudo para preto
        al_clear_to_color(al_map_rgb(0, 0, 0));
        
        //desenho tudo por traz por questoes de efeito(o asteroide não é desenhado)
        desenha(score);
        
        //desenho o fundo transparente
        al_draw_bitmap(Preto, 0, 0, 0);
        
        //desenho as opções com suas cores respectivas
        al_draw_text(Fload, al_map_rgb(x1, x2, x3),LAR/2, (ALT/2)-100,ALLEGRO_ALIGN_CENTER,"Resume Game");
        al_draw_text(Fload, al_map_rgb(y1, y2, y3),LAR/2, (ALT/2),ALLEGRO_ALIGN_CENTER,"Tutorial");
        al_draw_text(Fload, al_map_rgb(z1, z2, z3),LAR/2, (ALT/2)+100,ALLEGRO_ALIGN_CENTER,"Quit to Menu");
        if(exbtut){
            al_draw_bitmap(Tutor, (LAR/2)-(al_get_bitmap_width(Tutor)/2), (ALT/2)-(al_get_bitmap_height(Tutor)/2), 0);
             al_draw_text(Fscore, al_map_rgb(255, 255, 255),LAR/2, ALT-50,ALLEGRO_ALIGN_CENTER,"Esc para sair");
        }
        al_flip_display();
    }
    return 0;
}

//compara as palavras
void compara(Lista *li){
    
    float xt, yt;
    
    //crio elementos auxiliares
    Elem* no = *li;
    Elem* ant = no;
    
    //seto acerto como 0 para fazer a mudança caso isso não seja verdade
    acerto = 0;
    
    //percorro a lista
    while (no != NULL) {
        
        if(strcmp(no->dados.paltr, " ") == 0 || strcmp(no->dados.pales, " ") == 0 || strcmp(no->dados.paltr, "\n") == 0 || strcmp(no->dados.pales, "\n") == 0 || strcmp(no->dados.paltr, "") == 0 || strcmp(no->dados.pales, "") == 0){
            if(no == *li){
                *li = no->Prox;
                free(no);
                no = *li;
            } else {
                ant->Prox = no->Prox;
                free(no);
                no = ant->Prox;
            }
        }
        
        //se o jogo for do tipo 1
        if(tipo == 1){
            
            //verifico se a palavra no buffer e a de algum asteroide é exatamente igual
            if(strcmp(no->dados.paltr, let) == 0 && pos != 0){
                xt = no->dados.x - 640;
                yt = no->dados.y - 699;
                angulo = atan2(yt, xt);
                fprintf(stderr, "angulo = %f\n", angulo);
                acerto = 1;
                atirar = true;
                tiros.x = 650;
                tiros.y = 680;
                tiros.astID = no->dados.id;
                return ;
                
                //caso não eu verifico se é igual pelo menos até certo ponto ou se o buffer ta zerado
            } else if(strcompar(no->dados.paltr, let) == 0 || pos == 0){
                acerto = 4;
            }
        //caso o tipo de jogo seja 0
        } else if(tipo == 0){
            
            //verifico se a palavra no buffer e a de algum asteroide é exatamente igual
            if(strcmp(no->dados.pales, let) == 0 && pos != 0){
                xt = (no->dados.x+60) - 640;
                yt = (no->dados.y+60) - 699;
                angulo = atan2(yt, xt);
                fprintf(stderr, "angulo = %f\n", angulo);
                acerto = 1;
                atirar = true;
                tiros.x = 650;
                tiros.y = 680;
                tiros.astID = no->dados.id;
                return ;
                
                //caso não eu verifico se é igual pelo menos até certo ponto ou se o buffer ta zerado
            } else if(strcompar(no->dados.pales, let) == 0 || pos == 0){
                acerto = 4;
            }
        }
        
        //ando na lista
        if(no != NULL){
            ant = no;
            no = no->Prox;
        }
    }
}

//compara string
short strcompar(char vet[], char vit[]){
    
    //verifico se até onde o buffer está existe uma palavra que atende
    for(int i = 0; i < pos; i++){
        if(vet[i] != vit[i]){
            return 1;
        }
    }
    return 0;
}

bool set(){
    
    //defino duas variaveis para pegar a palavra do txt
    char fontt[30];
    char ajudaa[30];
    
    //crio uma variavel de arquivo
    FILE *arqt;
    
    //abro no modo read
    arqt = fopen(Nome, "r");
    
    //pego os valores
    fscanf(arqt, "%d\n%d\n%s\n%s", &tipo, &max, ajudaa, fontt);
    fprintf(stderr, "tipo = %d \nmaximo = %d \nAjuda = %s \nnomefonte = %s \n", tipo, max, ajudaa, fontt);
    
    //aloco memoria para o nome da fonte e da imagem de tutorial
    Usfont = malloc(sizeof("Fontes/")+sizeof(fontt));
    strcat(Usfont, "Fontes/");
    strcat(Usfont, fontt);
    if(ajudaa[0] == '-')
        strcat(ajudaa, ".png");
    Ajuda = malloc(sizeof("Imagens/")+sizeof(ajudaa));
    strcat(Ajuda, "Imagens/");
    strcat(Ajuda, ajudaa);
    Tutor = al_load_bitmap(Ajuda);
    
    if(!Tutor){
        al_destroy_display(Janela);
        fprintf(stderr, "Falha ao carregar imagem tutorial\n");
    }
    
    //fonte para palavras exibição
    Exb = al_load_font(Usfont, 30, 0);
    if(!Exb){
        al_destroy_display(Janela);
        fprintf(stderr, "Falha ao carregar fonte exibição\n");
    }
    if(tipo == 0){
        short x1, x2, x3, y1, y2, y3;
        bool opc = true;
        tipo = 1;
        //loop do menu pause
        while (opc) {
            
            //defino um evento
            ALLEGRO_EVENT even;
            
            //espero por um evento
            al_wait_for_event(Fila_eventos, &even);
            
            //caso aperte para cima ou para baixo
            if(even.type == ALLEGRO_EVENT_KEY_DOWN){
                switch (even.keyboard.keycode) {
                    case ALLEGRO_KEY_UP:
                        tipo = 1;
                        break;
                        
                    case ALLEGRO_KEY_DOWN:
                        tipo = 0;
                        break;
                        
                    case ALLEGRO_KEY_ENTER:
                        opc = false;
                        break;
                    case ALLEGRO_KEY_ESCAPE:
                        return 0;
                        break;
                }
            }
            
            x1 = x2 = x3 = y1 = y2 = y3 = 255;
            
            if(tipo == 1){
                x1 = 43;
                x2 = x3 = 255;
            } else if(tipo == 0) {
                y1 = 43;
                y2 = y3 = 255;
            }
            al_clear_to_color(al_map_rgb(0,0,0));
            al_draw_bitmap(fundoStart, (LAR/2 - (al_get_bitmap_width(fundoStart)/2))+30, ALT/2 - (al_get_bitmap_height(fundoStart)/2), 0);
            al_draw_bitmap(Preto, 0, 0, 0);
            al_draw_text(Fload, al_map_rgb(x1, x2, x3),LAR/2, (ALT/2)-100,ALLEGRO_ALIGN_CENTER,"Translate mode");
            al_draw_text(Fload, al_map_rgb(y1, y2, y3),LAR/2, (ALT/2)+100,ALLEGRO_ALIGN_CENTER,"Write mode");
            al_flip_display();
        }
    fclose(arqt);
    }
    return 1;
}

//função de continue do jogo
bool continuar(){
    bool sai = true;
    bool opcao = 0;
    short x1, x2, x3, y1, y2, y3;
    
    al_stop_samples();
    while(sai){
        
        //exibo as palavras erradas e mostro o game over
        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_text(Fscore, al_map_rgb(255, 255, 255), LAR/2, (ALT/2)-200, ALLEGRO_ALIGN_CENTER, "Game Over");
        al_draw_text(Fscore, al_map_rgb(50, 50, 50), LAR/2, (ALT/2)-150, ALLEGRO_ALIGN_CENTER, "Press enter");
        al_draw_text(Fscore, al_map_rgb(255, 255, 255), LAR/2, (ALT/2)-100, ALLEGRO_ALIGN_CENTER, "Suas palavras erradas foram: ");
        al_draw_text(Inf, al_map_rgb(255, 255, 255), (LAR/2)-300, (ALT/2)-50, ALLEGRO_ALIGN_CENTER, "exibição");
        al_draw_textf(Exb, al_map_rgb(255, 255, 255), (LAR/2)-300, (ALT/2), ALLEGRO_ALIGN_CENTER, "%s", palser[0].ex);
        al_draw_textf(Exb, al_map_rgb(255, 255, 255), (LAR/2)-300, (ALT/2)+50, ALLEGRO_ALIGN_CENTER, "%s", palser[1].ex);
        al_draw_textf(Exb, al_map_rgb(255, 255, 255), (LAR/2)-300, (ALT/2)+100, ALLEGRO_ALIGN_CENTER, "%s", palser[2].ex);
        al_draw_text(Inf, al_map_rgb(255, 255, 255), (LAR/2), (ALT/2)-50, ALLEGRO_ALIGN_CENTER, "escrita");
        al_draw_textf(Inf, al_map_rgb(255, 255, 255), (LAR/2), (ALT/2), ALLEGRO_ALIGN_CENTER, "%s", palser[0].es);
        al_draw_textf(Inf, al_map_rgb(255, 255, 255), (LAR/2), (ALT/2)+50, ALLEGRO_ALIGN_CENTER, "%s", palser[1].es);
        al_draw_textf(Inf, al_map_rgb(255, 255, 255), (LAR/2), (ALT/2)+100, ALLEGRO_ALIGN_CENTER, "%s", palser[2].es);
        al_draw_text(Inf, al_map_rgb(255, 255, 255), (LAR/2)+300, (ALT/2)-50, ALLEGRO_ALIGN_CENTER, "tradução");
        al_draw_textf(Inf, al_map_rgb(255, 255, 255), (LAR/2)+300, (ALT/2), ALLEGRO_ALIGN_CENTER, "%s", palser[0].tr);
        al_draw_textf(Inf, al_map_rgb(255, 255, 255), (LAR/2)+300, (ALT/2)+50, ALLEGRO_ALIGN_CENTER, "%s", palser[1].tr);
        al_draw_textf(Inf, al_map_rgb(255, 255, 255), (LAR/2)+300, (ALT/2)+100, ALLEGRO_ALIGN_CENTER, "%s", palser[2].tr);
        al_flip_display();
        //defino um evento
        ALLEGRO_EVENT even;
        
        //espero por um evento
        al_wait_for_event(Fila_eventos, &even);
        
        //caso aperte para cima ou para baixo
        if(even.type == ALLEGRO_EVENT_KEY_DOWN){
            switch (even.keyboard.keycode) {
                case ALLEGRO_KEY_ENTER:
                    sai = false;
                    break;
            }
        }
    }
    
    //libero o espaço das palavras erradas
    free(palser[0].ex);
    free(palser[0].es);
    free(palser[0].tr);
    free(palser[1].ex);
    free(palser[1].es);
    free(palser[1].tr);
    free(palser[2].ex);
    free(palser[2].es);
    free(palser[2].tr);
    sai = true;
    
    opcao = 0;
    
    while(sai){
        
        //defino um evento
        ALLEGRO_EVENT even;
        
        //espero por um evento
        al_wait_for_event(Fila_eventos, &even);
        
        //caso aperte para cima ou para baixo
        if(even.type == ALLEGRO_EVENT_KEY_DOWN){
            switch (even.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    opcao = 0;
                    break;
                    
                case ALLEGRO_KEY_DOWN:
                    opcao = 1;
                    break;
                
                case ALLEGRO_KEY_ENTER:
                    return opcao;
                    break;
            }
        }
        x1 = x2 = x3 = y1 = y2 = y3 = 255;
        if(opcao == 0){
            x1 = 43;
            x2 = x3 = 255;
        } else if(opcao == 1){
            y1 = 43;
            y2 = y3 = 255;
        }
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_text(Fload, al_map_rgb(x1, x2, x3),LAR/2, (ALT/2)-50,ALLEGRO_ALIGN_CENTER,"Restart game");
        al_draw_text(Fload, al_map_rgb(y1, y2, y3),LAR/2, (ALT/2)+50,ALLEGRO_ALIGN_CENTER,"Return to menu");
        al_flip_display();
    }
    fprintf(stderr, "Algo errado não está certo, continuar!");
    return 0;
}































