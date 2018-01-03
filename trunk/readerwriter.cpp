#include "readerwriter.hpp"
#include <sstream>
#include <iostream>
#include <cerrno>
#include <fcntl.h>


/**
 * @namespace codeC
 * @brief espace de nom interne pour éviter une ambiguité
 *
 * Ce namespace sert à eviter la confusion entre
 * la méthode socklib::ReaderWriter::close() et la fonction close la libc
 */
namespace codeC {
  #include <unistd.h>
}
#include "socklib.hpp"

socklib::ReaderWriter::ReaderWriter(int fd) : fd(fd) {
}

void
socklib::ReaderWriter::close() {
  int r = codeC::close(this->fd);
  exit_error("Erreur à la fermeture du file descriptor", r==-1, errno);
}

void
std::swap(socklib::ReaderWriter &a, socklib::ReaderWriter &b) {
  int temp;
  temp = a.fd;
  a.fd = b.fd;
  b.fd = temp;
}


int socklib::ReaderWriter::read(std::vector<char> &buf, bool wait_all) {
  int taille = buf.size();
  if (taille == 0) {
    std::ostringstream c;
    c << "ATTENTION fichier " << __FILE__ << " ligne " <<  __LINE__ << " : vous essayez de lire des données dans un buffer de taille 0" << std::endl;
    std::cerr << c.str();
  }

  // on utilise la fonction read de base sur le tableau de donnée du vector
  int lu = this->read(buf.data(), 0, buf.size(), wait_all);

  return lu;
}

int socklib::ReaderWriter::read(char *buf, int offset, int len, bool wait_all) {
  int deja = 0;

  while (deja < len) {
    // on fait une lecture
    // mais si la demande de lecture est trop importante, on lit par morceau
    unsigned long alire = len-deja;
    if (alire > SIZE_MAX) {
      alire = SIZE_MAX;
    }
    int nblus = codeC::read(this->fd, buf+offset+deja, alire);
    exit_error("erreur lors de la lecture", nblus==-1, errno);
    if (nblus == 0) {
      // la socket vient d'être fermée
      // si on a déjà lu les données cela ne change rien,
      // si il en manque cela risque de poser des problèmes alors
      // je met des 0 et je leve une exception
      if (deja < len) {
	std::ostringstream c;
	c << "Le flux de lecture est fermé après " << deja
			<< " octets alors que vous en attendez " << len;
	//	std::cerr << "Attention fichier " << __FILE__ << " ligne " << __LINE__
	//		  << " " << c.str() << std::endl;
	if (wait_all) {
	  exit_error(c.str().c_str(), true, 0);
	  //	  throw std::runtime_error(c.str());
	} else {
	  warning_error(c.str().c_str(), true, 0);
	}
      }
      break;
    }
    deja += nblus;

    //    std::cerr << "lu : " << nblus << "caractères " << 

    if (!wait_all) {
      // si on ne demande pas d'attendre, on sort qu'en même
      break;
    }
  }

  // on renvoie le nombre d'octets lus
  return deja;
}

int
socklib::ReaderWriter::test_and_read(char *buff, int offset, int len) {
  int r;
  // on récupère les anciennes options
  int oldattr = fcntl(fd, F_GETFL);
  exit_error("fcntl impossible", oldattr == -1, errno);
  // on ajoute l'option non blocante
  r = fcntl(fd, F_SETFL, oldattr | O_NONBLOCK);
  exit_error("fcntl impossible", r == -1, errno);
  int nblus = codeC::read(fd, buff+offset, len);
  if ((nblus == -1)
      &&((errno==EAGAIN)||(errno==EWOULDBLOCK))
      )
    {
    nblus = 0;
  }
  exit_error("read impossible", nblus == -1, errno);
  r = fcntl(fd, F_SETFL, oldattr);
  exit_error("fcntl impossible", r == -1, errno);

  return nblus;
}

int socklib::ReaderWriter::write(const std::vector<char> &buf) {
  int taille = buf.size();
  if (taille == 0) {
    std::ostringstream c;
    c << "ATTENTION fichier " << __FILE__ << " ligne " <<  __LINE__ << " : vous essayez d'écrire des données depuis un buffer de taille 0" << std::endl;
    std::cerr << c.str();
  }

  // on utilise la fonction read de base sur le tableau de donnée du vector
  int ecrit = this->write(buf.data(), 0, buf.size());

  return ecrit;  
}

int socklib::ReaderWriter::write(const char* buf, int offset, int len) {
  int deja = 0;

  while (deja < len) {
    // on fait une ecriture
    int nbecr = codeC::write(this->fd, buf+offset, len-deja);
    exit_error("erreur lors de la lecture", nbecr==-1, errno);
    deja += nbecr;
  }

  // on renvoie le nombre d'octets lus
  return deja;
}
