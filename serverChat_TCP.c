#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#define port 5000


void Transfert_donnees(int newsocket);
int Ouverture_connexion_server(int newsocket) ;
int Occurence_envois(int nb_paquets ,int rcvwindow);
int Fermeture_Connexion_server(int newsocket);

int main(void) {
    struct sockaddr_in serveraddr, newaddr;
    int sersocket, newsocket, s;
    socklen_t size;
    srand(time(NULL));

    sersocket = socket(PF_INET, SOCK_STREAM, 0);
    if (sersocket > 0)
        printf("Server socket est crée\n");

    serveraddr.sin_family = PF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = bind(sersocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (s == 0)
        printf("Demarrage du server...\n");

    listen(sersocket, 1);
    size = sizeof(newaddr);
    printf("Démarré\n");
    printf("========== OUVERTURE CONNEXION ==========");

    newsocket = accept(sersocket, (struct sockaddr *)&newaddr, &size);
    if (newsocket > 0)
        printf("\n\n");

    while (Ouverture_connexion_server(newsocket) !=0 ){

        char buffer2[100]= {"Erreur sur ouverture de connexion.\n\t\t\t    Pour l'ouvrir, envoyer le paquet 'SYN'\n"};
        send(newsocket, buffer2, strlen(buffer2), 0);
    }
    Transfert_donnees(newsocket);

    printf("\n========== FERMETURE CONNEXION ==========\n");
    printf("\n");
    printf("Connexion en attente de fermeture...\n");

    while(Fermeture_Connexion_server( newsocket) != 0);

    close(newsocket);

    return 0;
}


int Ouverture_connexion_server(int newsocket)
{
    char buffer[100];
    recv(newsocket, buffer, sizeof(buffer), 0);
    printf("Envoyé par le client : %s\n", buffer);
    if (strcmp(buffer, "SYN\n") == 0){
        printf("\tPaquet SYN reçu !\n");
        printf("\nEnvoie du paquet SYN + ACK au client...\n\n");
        //Envoie ouverture de connction
        strcpy(buffer,"ACK");
        send(newsocket, buffer, strlen(buffer), 0);
        return 0;
    }
    return 1;
}



void Transfert_donnees(int newsocket) {
    char buffer[100],buffer2[100]; int nb_paquets = 0,rcvwindow = 0; int nb_envois;
    char paquet[15]= "PAQUET";int i = 0;
    int ttl;

    printf("========== TRANSFERT DES DONNEES ==========\n");

    recv(newsocket, buffer, sizeof(buffer), 0);
    printf("\nEnvoyé par le client :\n\tNombre de paquets à envoyer : %s", buffer);

    recv(newsocket, buffer2, sizeof(buffer2), 0);
    printf("\tTaille de la file d'attente : %s\n", buffer2);

    nb_paquets = atoi(buffer);
    rcvwindow = atoi(buffer2);
    nb_envois = Occurence_envois(nb_paquets, rcvwindow);

    printf("\tNombre d'envoie de PAQUET à faire au total : %d\n\n", nb_envois);

    int showAskMessage = 1; 

    while (i<nb_envois){
        
        if (showAskMessage) {
            printf("Demande du %s %d reçu !\n", paquet, i+1);
            printf("\tEnvoie du %s %d...\n",paquet,i+1);
        } else {
            showAskMessage = 1;
        }

        ttl = (rand()%2)+1;
        
        sleep(ttl);
        send(newsocket, paquet, strlen(paquet), 0);
        recv(newsocket, buffer2, sizeof(buffer2), 0);
        if(strcmp(buffer2,"V") == 0){
            i++;
        } else {
            printf("\tRenvoie du %s %d...\n",paquet,i+1);
            showAskMessage = 0;
        }
    }

    return;
}

int Occurence_envois(int nb_paquets ,int rcvwindow)
{
    int nb_envois;

    nb_envois = (nb_paquets/rcvwindow);
    if(nb_envois <= 0) {

        return 1;
    }
    else if(nb_paquets%rcvwindow != 0){
        //+1 pour le reste de la taille de données < rxvwindow
        return nb_envois +1;
    }
    else{
        return nb_envois ;
    }
}

int Fermeture_Connexion_server(int newsocket)
{
    char buffer[100];
    recv(newsocket, buffer, sizeof(buffer), 0);
    printf("\nEnvoyé par le client : %s", buffer);
    if (strcmp(buffer, "FIN\n") == 0){
        printf("\n\tPaquet FIN reçu !\n\n");

        printf("Envoie du paquet \"En cours de fermeture\"...\n");
        send(newsocket, "En cours de fermeture", strlen("En cours de fermeture"), 0);
        
        printf("Fermeture de la connexion en cours...\n\n");
        sleep(30);
        printf("\tConnexion femrée !\n");
        //Envoie femreture de connexion
        
        return 0;
    }
    return 1;
}
