#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>

# define port 5000

int Ouverture_connexion_client(int clisocket);
int Occurence_envois(int nb_paquets ,int rcvwindow);
void Demande_de_donnees(int clisocket,int timeOut);
int Fermeture_Connexion_clients(int clisocket);
int randInt(int min, int max);        // Choisir une valeur aléatoirement entre min et max
int validitySimulator(int randMin, int randMax, int iterationBeforeStop);  // Simuler qu'on ai reçu un élément (OU) Simuler qu'un élément reçu soit correct ou non


int  main(void)
{
    int clisocket;int timeOut;
    struct sockaddr_in serveraddr;
    clisocket=socket(PF_INET,SOCK_STREAM,0);
    if(clisocket>0)
    printf("Server socket est crée\n");
    printf("========== OUVERTURE CONNEXION ==========\n");

    serveraddr.sin_family= PF_INET;
    serveraddr.sin_port= htons(port);
    serveraddr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
    connect(clisocket,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    while (Ouverture_connexion_client(clisocket)!=0);
    printf("\tOuverture de la connexion ...\n\n");
    timeOut = 3;

     Demande_de_donnees( clisocket,timeOut);

    printf("\n========== FERMETURE CONNEXION ==========\n");

    printf("\n");
    printf("Connexion en attente de fermeture...\n");

    while (Fermeture_Connexion_clients(clisocket) !=0){
        printf("\n\tRéponse du serveur : Pour lancer la fermeture envoyer le paquet 'FIN'\n");
    }

    printf("Fermeture de la connexion en cours...\n");

    sleep(5);

    close(clisocket);

    printf("\n\tConnexion fermée !\n");

    return 0;
}


int Ouverture_connexion_client(int clisocket)
{
    char buffer[100];
    printf("\n> Envoyer au serveur : ");
    fgets(buffer,sizeof(buffer),stdin);
    send(clisocket,buffer,sizeof(buffer),0);
    recv(clisocket,buffer,sizeof(buffer),0);

    if(strcmp(buffer, "ACK\n") == 0){
        printf("\tPaquet SYN + ACK reçu !\n");
        return 0;
    }
    printf("\tRéponse du serveur: %s",buffer);
    return 1;
}

void Demande_de_donnees(int clisocket,int timeOut) {
    char buffer[100], buffer2[100];
    double ttl;
    int nb_paquets, rcvwindow, nb_paquet_toSend, i = 0;

    printf("========== TRANSFERT DES DONNEES ==========\n\n");

    printf("Envoyer au serveur : \n");

    printf("\t> Nombre de paquets demandés : ");
    fgets(buffer,sizeof(buffer),stdin);
    send(clisocket, buffer, sizeof(buffer), 0);


    printf("\t> Taille de la file d'attente : ");
    fgets(buffer2,sizeof(buffer2),stdin);
    send(clisocket, buffer2, sizeof(buffer2), 0);

    nb_paquets = atoi(buffer);
    rcvwindow = atoi(buffer2);
    nb_paquet_toSend = Occurence_envois(nb_paquets, rcvwindow);

    printf("\n\tNombre de demande de PAQUET à faire au total : %d\n\n", nb_paquet_toSend);

    while (i< nb_paquet_toSend) {

        printf("Demande du paquet %d...\n", i+1);

        clock_t timeAtFctStart = clock();
        recv(clisocket, buffer, sizeof(buffer), 0);
        clock_t timeAtFctEnd = clock();
        ttl = ((double) (timeAtFctEnd - timeAtFctStart)) / 10;

        if (ttl <= timeOut) {

            int isPacketValid = validitySimulator(1, 5, 10);

            if (isPacketValid == 1) {   // Si le paquet reçu est valide

                printf("\t%s %d reçu !\n",buffer,i+1);
                send(clisocket, "V", sizeof("V"), 0);
                i++;

            } else {       // Si le paquet reçu est invalide
                printf("\tAcquittement négatif !!!\n\tRedemande du %s %d...\n", buffer,i+1);
                send(clisocket, "F", sizeof("F"), 0);            }
        }
        else{
            printf("\tTime Out !!!\n\tRedemande du %s %d...\n", buffer,i+1);
            send(clisocket, "F", sizeof("F"), 0);

        }
    }
}

int Occurence_envois(int nb_paquets ,int rcvwindow)
{
    int nb_envois;

    nb_envois = (nb_paquets/rcvwindow);
    if(nb_envois <= 0) {

        return 1;
    }
    else if(nb_paquets%rcvwindow != 0){
        //+1 pour le reste de la tille de données < rxvwindow
        return nb_envois +1;
    }
    else{
        return nb_envois ;
    }
}


int Fermeture_Connexion_clients(int clisocket)
{
    char buffer[100];
    printf("\n> Envoyer au serveur : ");
    fgets(buffer,sizeof(buffer),stdin);
    send(clisocket,buffer,sizeof(buffer),0);
    if(strcmp(buffer,"FIN\n")==0){
        //send(clisocket,buffer,sizeof(buffer),0);
        recv(clisocket,buffer,sizeof(buffer),0);
        printf("\tPaquet \"En cours de fermeture\" reçu!\n");
        return 0;
    }
    return 1;
}


int validitySimulator(int randMin, int randMax, int iterationBeforeStop) {

    int randVal = 0;
    int valToGet = randInt(randMin, randMax);
    int iteration = 0;

    // Simulation de la validité (Recherche d'une valeur aléatoire)
    do {
        randVal = randInt(randMin, randMax);
        iteration++;
    } while (randVal != valToGet && iteration < iterationBeforeStop);


    if (randVal == valToGet) {    // Si la valeur aléatoire est trouvée
        return 1;
    } else {
        return 0;
    }
}


int randInt(int min, int max) {
    int number = (rand() % (max - min + 1)) + min;
    return number;
}
