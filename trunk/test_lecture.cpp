#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "socklib.h"
#include "bufferedreaderwriter.hpp"


/// les mode de lecture
const int MODE_NONE = 0;
const int MODE_FICHIER = 1;
const int MODE_LIGNE = 2;
const int MODE_TAILLEFIXE = 3;
const int MODE_TAILLEFIXE_ATTENDUE = 4;

const int SORTIE_PRINT = 0;
const int SORTIE_FICHIER = 1;

extern int optind, opterr, optopt;

void usage(const string &nomprog) {
  std::cerr << "Usage "
	    << nomprog
	    << " [-p port] [-w <temps attente en seconde>] (-s|-l|-t <taille>|-a <taille>) (<nom fichier>|PRINT)"
	    << std::endl;
}



int main(int argc, char *argv[]) {
  int fdout;
  std::string nomfichier;
  std::string port = "8080";
  int mode = MODE_NONE;
  int sortie = SORTIE_PRINT;
  int taillelect = 0;
  int wait = 0;

  int option;
  while ((option = getopt(argc, argv, "p:slt:a:w:")) != -1) {
    switch (option) {
    case 'p' :
      port = optarg;
      break;
    case 's':
      mode = MODE_FICHIER;
      std::cerr << "lecture jusquà la fin de la socket" << std::endl;
      break;
    case 'l':
      mode = MODE_LIGNE;
      std::cerr << "lecture ligne par ligne" << std::endl;
      break;
    case 't':
      mode = MODE_TAILLEFIXE;
      taillelect = atoi(optarg);
      if (taillelect == 0) {
	std::ostringstream c;
	c << "!!!la taille " << optarg
	  << " n'est pas correcte !!!";
	std::cerr << c.str() << std::endl;
	usage(argv[0]);
	exit_error(c.str().c_str(), true, 0);
      }
      std::cerr << "lecture de taille fixe " << taillelect << " octets"
		<< std::endl;
      break;
    case 'a':
      mode = MODE_TAILLEFIXE_ATTENDUE;
      taillelect = atoi(optarg);
      if (taillelect == 0) {
	std::ostringstream c;
	c << "!!!la taille " << optarg
	  << " n'est pas correcte !!!";
	std::cerr << c.str() << std::endl;
	usage(argv[0]);
	exit_error(c.str().c_str(), true, 0);
      }
      std::cerr << "lecture de taille fixe "
		<< taillelect << " octet en attendant le message complet"
		<< std::endl;
      break;
    case 'w':
      wait = atoi(optarg);
      if (wait == 0) {
	std::ostringstream c;
	c << "!!!le temps d'attente " << optarg
	  << " n'est pas correcte !!!";
	std::cerr << c.str() << std::endl;
	usage(argv[0]);
	exit_error(c.str().c_str(), true, 0);
      }
      std::cerr << "avant de commencer, attend " << wait << " secondes"
		<< std::endl;
      break;
    default :
	std::ostringstream c;
	c << "!!!l'option " << optopt
	  << " est inconnue ou mal utilisée !!!";
	std::cerr << c.str() << std::endl;
	usage(argv[0]);
	exit_error(c.str().c_str(), true, 0);
    }
  }
  
  if (mode == MODE_NONE) {
	std::ostringstream c;
	c << "!!!pas de mode de lecture choisi !!!";
	std::cerr << c.str() << std::endl;
	usage(argv[0]);
	exit_error(c.str().c_str(), true, 0);
  }

  if (optind == argc) {
	std::ostringstream c;
	c << "!!! il manque le nom du fichier de sauvegarde !!!";
	std::cerr << c.str() << std::endl;
	usage(argv[0]);
	exit_error(c.str().c_str(), true, 0);
  }

  nomfichier = argv[optind];
  if (nomfichier == "PRINT") {
    std::cerr << "les données seront affichées" << std::endl;
    fdout = STDOUT_FILENO;
    sortie = SORTIE_PRINT;
  } else {
    fprintf(stderr, "les données seronts sauvegardées dans le fichier %s\n", nomfichier);
    fdout = open(nomfichier, O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
    exit_error("Création du fichier", fd==-1, errno);
    sortie = SORTIE_FICHIER;
  }

  std::cerr << "port d'écoute : " << port << std::endl;

  // on cree une socket d'attente
  int sock_attente = socklib::CreeSocketServeur(port);

  // on recoit un nouveau client
  int sock = socklib::AcceptConnexion(sock_attente);
  socklib::BufferedReaderWriter in(sock);

  
  int cont = 1;
  while (cont) {
    if (wait != 0) {
      sleep(wait);
    }

    switch (mode) {
    case MODE_FICHIER: {
      ReaderWriter out(fd);
      int nb = in.recopie(out);
      std::cerr << "lecture jusqu'à fermeture de " << nb << " octets." << std::endl;
      cont = 0;
      break;
    }
    case MODE_LIGNE: {
      string ligne = in.read_line();
      if (ligne == "") {
	// il y a eu une erreur ou c'est la fin de la socket
	cont = 0;
	break;
      }
      int res;

      int taille = ligne.size();
      std::cerr << "lecture d'une ligne de " << taille << " octets." << std::endl;

      if (sortie == SORTIE_PRINT) {
	std::cout << "#" << ligne << "#" << std::endl;
      } else {
	BufferedReaderWriter out(fd);
	out.write(ligne);
      }
      
      break;
    }
    case MODE_TAILLEFIXE: {
      int res;
      std::vector<char> data = in.read_all(taillelect, false);
      if (data.size() == 0) {
	std::cerr << "La socket a été fermée" << std::endl;
	cont = 0;
	break;
      }
      std::cerr << "Lecture de " << data.size() << " octets." << std::endl;
      if (sortie == SORTIE_PRINT) { 
	std::cout << "#" << data << "#" << std::endl;
      } else {
	BufferedReaderWriter out(fd);
	out.write(data);
      }
      break;
    }
    case MODE_TAILLEFIXE_ATTENDUE: {
      int res;
      std::vector<char> data = in.read_all(taillelect, true);
      if (data.size() == 0) {
	std::cerr << "La socket a été fermée" << std::endl;
	cont = 0;
	break;
      }
      std::cerr << "Lecture de " << data.size() << " octets." << std::endl;
      if (sortie == SORTIE_PRINT) { 
	std::cout << "#" << data << "#" << std::endl;
      } else {
	BufferedReaderWriter out(fd);
	out.write(data);
      }
      break;
    }
    default :
      exit_error("Mode de lecture non prévu", true, 0);
    }
  }
  std::cout << "Fin de la socket du client" << std::endl;
  close(fd);
  close(socket_attente);
  // la socket est fermée directement par le BufferedReaderWriter
  return 0;
}
