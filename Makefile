CC=gcc
CFLAGS=-Wall


all: clean bin exec

# Compilation de l'exécutable
exec: clientChat_TCP.c serverChat_TCP.c
	$(CC) $(CFLAGS) clientChat_TCP.c -o bin/cli
	$(CC) $(CFLAGS) serverChat_TCP.c -o bin/ser


# Création des dossiers nécessaires
bin:
	mkdir -p bin


# Nettoyage
clean:
	rm -rf bin
