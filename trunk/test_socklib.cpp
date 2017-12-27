#include "socklib.hpp"
#include "bufferedreaderwriter.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <system_error>
#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>

bool test_rw_with_pipe() {
  int pi[2];
  int r;

  std::cerr << "test de lecture ecriture via un pipe" << std::endl;
      
  r = pipe(pi);
  exit_error("Erreur lors de la création du pipe", r==-1, errno);

  socklib::ReaderWriter in(pi[1]);
  socklib::ReaderWriter out(pi[0]);
 

  std::cerr << "Envoie d'une chaine de caractère" << std::endl;
  std::string message = "pipe()  crée un tube, un canal unidirectionnel de données qui peut être utilisé pour la communication entre processus.";
  int taille = message.size();
  char recu[taille+1];
  
  
  in.write(message.data(), 0, taille);
  out.read(recu, 0, taille);
  recu[taille] = 0;
  //  std::string srecu(recu);

  if (message != recu) {
    std::cerr << "Les deux messages sont différents '" << message
	      << "'!='" << recu << "'" << std::endl;
    return false;
  }
  
  std::cerr << "Réussit !!" << std::endl;  
  std::vector<char> b1, b2;
  
  return true;
}

bool test_rw_with_pipe_and_fork() {
  int pi[2];
  int r;

  std::cerr << "test de lecture ecriture via un pipe" << std::endl;
      
  r = pipe(pi);
  exit_error("Erreur lors de la création du pipe", r==-1, errno);

  socklib::ReaderWriter in(pi[1]);
  socklib::ReaderWriter out(pi[0]);
 

  std::cerr << "Envoie d'une chaine de caractère" << std::endl;
  std::string message = "pipe()  crée un tube, un canal unidirectionnel de données qui peut être utilisé pour la communication entre processus.";
  int taille = message.size();
  
  pid_t pid = fork();
  exit_error("Pb avec le fork", pid == -1, errno);

  if (pid == 0) {
    // je suis le fils
    out.close();
    // envoie caractère par caractère
    for (auto c : message) {
      // std::cerr << "fils envoie " << c << std::endl;
      in.write(&c, 0, 1);
      usleep(10);
      
    }
    in.close();
    exit(1);
  }
  // je suis le père
  in.close();
  char recu[taille+1];
  out.read(recu, 0, taille, true);
  recu[taille] = 0;
  //  std::string srecu(recu);
  
  if (message != recu) {
    std::cerr << "Les deux messages sont différents '" << message
	      << "'!='" << recu << "'" << std::endl;
    return false;
  }
  out.close();
  std::cerr << "Réussit !!" << std::endl;  

  pid_t wr = waitpid(pid, NULL, 0);
  exit_error("Waitpid", wr == -1, errno);
  
  return true;
}

bool test_brw_with_pipe_and_fork() {
  int pi[2];
  int r;

  std::cerr << "test de lecture ecriture via un pipe sur un brw" << std::endl;
      
  r = pipe(pi);
  exit_error("Erreur lors de la création du pipe", r==-1, errno);

  socklib::BufferedReaderWriter in(pi[1]);
  socklib::BufferedReaderWriter out(pi[0]);
 

  std::cerr << "Envoie d'une chaine de caractère" << std::endl;

  int nbl = 7;
  std::string tab[nbl];
  int i = 0;
  tab[i++] = "un\n";
  tab[i++] = "deux\n";
  tab[i++] = "trois\n";
  tab[i++] = "quatre\n";
  tab[i++] = "cinq\n";
  tab[i++] = "six\n";
  tab[i++] = "sept";
  
  std::string message1 = "";
  std::string message2 = "";
  for (i=0; i<3; i++) {
    message1 += tab[i];
  }
  message1 += "qua";
  message2 += "tre\n";
  for (i=4; i<nbl; i++) {
    message2 += tab[i];
  }

  
  pid_t pid = fork();
  exit_error("Pb avec le fork", pid == -1, errno);

  if (pid == 0) {
    // je suis le fils
    out.close();
    // envoie en 2 parties
    
    in.write(message1);
    usleep(500);
    in.write(message2);
    in.close();
    exit(1);
  }
  // je suis le père
  in.close();

  for (i=0; i<nbl; i++) {
    std::string recu = out.read_line();
    std::cerr << "Lu '" << recu << "' : ";
    if (recu == tab[i]) {
      std::cerr << "OK" << std::endl;
    } else {
      std::cerr << "KO !!!" << std::endl;
      return false;
    }
  }
  out.close();
  std::cerr << "Réussit !!" << std::endl;  

  pid_t wr = waitpid(pid, NULL, 0);
  exit_error("Waitpid", wr == -1, errno);

  return true;
}

bool test_brw_with_pipe_and_fork_long_message() {
  int pi[2];
  int r;

  std::cerr << "test de lecture ecriture via un pipe sur un brw" << std::endl;
      
  r = pipe(pi);
  exit_error("Erreur lors de la création du pipe", r==-1, errno);

  socklib::BufferedReaderWriter in(pi[1]);
  socklib::BufferedReaderWriter out(pi[0]);
 

  std::cerr << "Envoie d'une chaine de caractère" << std::endl;

  std::string phr = "Stnavius Semot Sel Zetehca Lani Milbus Egassem.";
  
  std::vector<char> message;
  while(message.size() < 201 ) {
    message.insert(message.end(),phr.begin(), phr.end());
  }
  message.push_back('\0');
  int taille = message.size();
  
  pid_t pid = fork();
  exit_error("Pb avec le fork", pid == -1, errno);

  if (pid == 0) {
    // je suis le fils
    out.close();
    
    in.write(message);
    usleep(1000);
    in.write(message);
    usleep(1000);
    in.write(message);
    in.close();
    exit(1);
  }
  // je suis le père
  in.close();

  {
    // première lecture comme une ligne
    std::cerr << "premier envoie lecture avec read until ... ";
    std::vector<char> recu = out.read_until('\0');
    if (recu == message) {
      std::cerr << "reussit !!!" << std::endl;
    } else {
      std::cerr << "manqué !!! " << std::endl;
      std::cerr << "'";
      std::cerr.write(recu.data(), recu.size());
      std::cerr << "' != '" << message.data() << std::endl;
      return false;
    }
  }

  {
    // seconde lecture en demandant la taille
    std::cerr << "2eme envoie lecture avec read_all ... ";
    std::vector<char> recu = out.read_all(taille);
    if (recu == message) {
      std::cerr << "reussit !!!" << std::endl;
    } else {
      std::cerr << "manqué !!! " << std::endl;
      std::cerr << "'";
      std::cerr.write(recu.data(), recu.size());
      std::cerr << "' != '" << message.data() << std::endl;
      return false;
    }
  }
  
  try {
    // seconde lecture avec une taille erronée
    std::cerr << "troisieme envoie lecture avec read_all et une mauvaise taille. Cela devrait faire une exception ... ";
    std::vector<char> recu = out.read_all(taille+1);
    std::cerr << "manqué !!! " << std::endl;
    return false;
  } catch (std::runtime_error) {
    std::cerr << "Réussit !!!" << std::endl;
  }
  out.close();

  pid_t wr = waitpid(pid, NULL, 0);
  exit_error("Waitpid", wr == -1, errno);

  return true;
}

bool test_brw_test_read() {
  int pi[2];
  int r;

  std::cerr << "test de lecture ecriture via un pipe sur un brw" << std::endl;
      
  r = pipe(pi);
  exit_error("Erreur lors de la création du pipe", r==-1, errno);

  socklib::BufferedReaderWriter in(pi[1]);
  socklib::BufferedReaderWriter out(pi[0]);
 

  std::cerr << "Envoie d'une chaine de caractère" << std::endl;

  std::string phr = "Stnavius Semot Sel Zetehca Lani Milbus Egassem.";
  
  std::vector<char> message;
  while(message.size() < 201 ) {
    message.insert(message.end(),phr.begin(), phr.end());
  }
  message.push_back('\0');
  int taille = message.size();
  
  pid_t pid = fork();
  exit_error("Pb avec le fork", pid == -1, errno);

  if (pid == 0) {
    // je suis le fils
    out.close();
    
    in.write(message);
    usleep(2000);
    in.write(message);
    in.close();
    exit(1);
  }
  // je suis le père
  in.close();

  {
    // première lecture comme une ligne
    std::cerr << "premier envoie lecture avec read until ... ";
    std::vector<char> recu = out.read_until('\0');
    if (recu == message) {
      std::cerr << "reussit !!!" << std::endl;
    } else {
      std::cerr << "manqué !!! " << std::endl;
      std::cerr << "'";
      std::cerr.write(recu.data(), recu.size());
      std::cerr << "' != '" << message.data() << std::endl;
      return false;
    }
  }

  std::cerr << "Le processus fait un test de lecture tout les 50ms alors qu'il n'y a pas d'écriture avant 2s. Si ça fonctionne, il faut plus de 10 tests échoués" << std::endl;
  int i=0;
  while(!out.test_read()) {
    std::cerr << "nothing to read " << ++i << std::endl;
    usleep(50);
  }
  // s'il n'y a pas eu plusieurs boucle c'est un problème
  if (i < 10) {
    std::cerr << "manqué !!" << std::endl;
    return false;
  } else {
    std::cerr << "réussit !!" << std::endl;
  }
  {
    // seconde lecture en demandant la taille
    std::cerr << "2eme envoie lecture avec read_all ... ";
    std::vector<char> recu = out.read_all(taille);
    if (recu == message) {
      std::cerr << "reussit !!!" << std::endl;
    } else {
      std::cerr << "manqué !!! " << std::endl;
      std::cerr << "'";
      std::cerr.write(recu.data(), recu.size());
      std::cerr << "' != '" << message.data() << std::endl;
      return false;
    }
  }

  std::cerr << "A priori, le flux est fermé, il faut que la fonction de test renvoie qu'il n'y a rien à lire" << std::endl;
  if (out.test_read()) {
    std::cerr << "manqué !!" << std::endl;
    return false;
  } else {
    std::cerr << "réussit !!" << std::endl;
  }
    
  
  out.close();

  pid_t wr = waitpid(pid, NULL, 0);
  exit_error("Waitpid", wr == -1, errno);

  return true;
}

bool test_brw_socket_long_message() {
  std::cerr << "test de lecture ecriture via une socket sur un brw" << std::endl;
      
   std::string phr = "Stnavius Semot Sel Zetehca Lani Milbus Egassem.";
  
  std::vector<char> message;
  while(message.size() < 201 ) {
    message.insert(message.end(),phr.begin(), phr.end());
  }
  message.push_back('\0');
  int taille = message.size();
  
  pid_t pid = fork();
  exit_error("Pb avec le fork", pid == -1, errno);

  if (pid == 0) {
    // je suis le fils je serait client
    usleep(500);
    socklib::BufferedReaderWriter brw(socklib::CreeSocketClient("localhost", "23451"));
    
    brw.write(message);
    usleep(1000);
    brw.write(message);
    usleep(1000);
    brw.write(message);
    brw.close();
    exit(1);
  }
  // je suis le père je serait serveur
  int at = socklib::CreeSocketServeur("23451");
  socklib::BufferedReaderWriter out(socklib::AcceptConnexion(at));
  {
    // première lecture comme une ligne
    std::cerr << "premier envoie lecture avec read until ... ";
    std::vector<char> recu = out.read_until('\0');
    if (recu == message) {
      std::cerr << "reussit !!!" << std::endl;
    } else {
      std::cerr << "manqué !!! " << std::endl;
      std::cerr << "'";
      std::cerr.write(recu.data(), recu.size());
      std::cerr << "' != '" << message.data() << std::endl;
      return false;
    }
  }

  {
    // seconde lecture en demandant la taille
    std::cerr << "2eme envoie lecture avec read_all ... ";
    std::vector<char> recu = out.read_all(taille);
    if (recu == message) {
      std::cerr << "reussit !!!" << std::endl;
    } else {
      std::cerr << "manqué !!! " << std::endl;
      std::cerr << "'";
      std::cerr.write(recu.data(), recu.size());
      std::cerr << "' != '" << message.data() << std::endl;
      return false;
    }
  }
  
  try {
    // seconde lecture avec une taille erronée
    std::cerr << "troisieme envoie lecture avec read_all et une mauvaise taille. Cela devrait faire une exception ... ";
    std::vector<char> recu = out.read_all(taille+1);
    std::cerr << "manqué !!! " << std::endl;
    return false;
  } catch (std::runtime_error) {
    std::cerr << "Réussit !!!" << std::endl;
  }
  out.close();

  pid_t wr = waitpid(pid, NULL, 0);
  exit_error("Waitpid", wr == -1, errno);

  return true;
}

bool test_brw_socket_test_read() {
  std::cerr << "test de lecture ecriture via un pipe sur un brw" << std::endl;
      
  std::cerr << "Envoie d'une chaine de caractère" << std::endl;

  std::string phr = "Stnavius Semot Sel Zetehca Lani Milbus Egassem.";
  
  std::vector<char> message;
  while(message.size() < 201 ) {
    message.insert(message.end(),phr.begin(), phr.end());
  }
  message.push_back('\0');
  int taille = message.size();
  
  pid_t pid = fork();
  exit_error("Pb avec le fork", pid == -1, errno);

  if (pid == 0) {
    // je suis le fils je serait client
    usleep(500);
    socklib::BufferedReaderWriter brw(socklib::CreeSocketClient("localhost", "23351"));
    
    brw.write(message);
    usleep(2000);
    brw.write(message);
    brw.close();
    exit(1);
  }
  // je suis le père
  int at = socklib::CreeSocketServeur("23351");
  socklib::BufferedReaderWriter out(socklib::AcceptConnexion(at));

  
  {
    // première lecture comme une ligne
    std::cerr << "premier envoie lecture avec read until ... ";
    std::vector<char> recu = out.read_until('\0');
    if (recu == message) {
      std::cerr << "reussit !!!" << std::endl;
    } else {
      std::cerr << "manqué !!! " << std::endl;
      std::cerr << "'";
      std::cerr.write(recu.data(), recu.size());
      std::cerr << "' != '" << message.data() << std::endl;
      return false;
    }
  }

  std::cerr << "Le processus fait un test de lecture tout les 50ms alors qu'il n'y a pas d'écriture avant 2s. Si ça fonctionne, il faut plus de 10 tests échoués" << std::endl;
  int i=0;
  while(!out.test_read()) {
    std::cerr << "nothing to read " << ++i << std::endl;
    usleep(50);
  }
  // s'il n'y a pas eu plusieurs boucle c'est un problème
  if (i < 10) {
    std::cerr << "manqué !!" << std::endl;
    return false;
  } else {
    std::cerr << "réussit !!" << std::endl;
  }
  {
    // seconde lecture en demandant la taille
    std::cerr << "2eme envoie lecture avec read_all ... ";
    std::vector<char> recu = out.read_all(taille);
    if (recu == message) {
      std::cerr << "reussit !!!" << std::endl;
    } else {
      std::cerr << "manqué !!! " << std::endl;
      std::cerr << "'";
      std::cerr.write(recu.data(), recu.size());
      std::cerr << "' != '" << message.data() << std::endl;
      return false;
    }
  }

  std::cerr << "A priori, le flux est fermé, il faut que la fonction de test renvoie qu'il n'y a rien à lire" << std::endl;
  if (out.test_read()) {
    std::cerr << "manqué !!" << std::endl;
    return false;
  } else {
    std::cerr << "réussit !!" << std::endl;
  }
    
  
  out.close();

  pid_t wr = waitpid(pid, NULL, 0);
  exit_error("Waitpid", wr == -1, errno);

  return true;
}

bool test_rw_with_read_by_part() {
  int pi[2];
  int r;

  std::cerr << "test de lecture en morceau, ce test doit faire une exception" << std::endl;
      
  r = pipe(pi);
  exit_error("Erreur lors de la création du pipe", r==-1, errno);

  socklib::ReaderWriter in(pi[1]);
  socklib::ReaderWriter out(pi[0]);
 

  std::cerr << "Envoie d'une chaine de caractère" << std::endl;
  std::string message = "pipe()  crée un tube, un canal unidirectionnel de données qui peut être utilisé pour la communication entre processus.";
  int taille = message.size();

  std::vector<char> vmess(taille);
  vmess.assign(message.begin(), message.end());
  in.write(vmess);
  in.close();
  
  char recu[11];
  std::string srecu;
  while(1) {
    try{
      int nbr;
      nbr = out.read(recu, 0, 10, true);
      recu[nbr] = '\0';
      std::cerr << "recu : " << recu << std::endl;
      srecu.append(recu);
    } catch(std::runtime_error &e) {
      std::cerr << "Le runtime error est bien recu" << std::endl;
      break;
    }
  }
  // a priori, il manque 2 caractère à la phrase.
  srecu.append("s.");
  if (message != srecu) {
    std::cerr << "Les deux messages sont différents '" << message
	      << "'!='" << recu << "'" << std::endl;
    return false;
  }
  out.close();
  std::cerr << "Réussit !!" << std::endl;  

  return true;
}

bool test_copie_fichier() {
  int res;
  
  std::cerr << "Test de la copie d'un fichier par la fonction recopie" << std::endl;

  const char *inputname = "./test_socklib.cpp";
  // const char *inputname = "./toto.txt";
  char *outputname = tmpnam(NULL);

  int fdinput = open(inputname, O_RDONLY);
  exit_error("Ouverture du fichier à lire", fdinput==-1, errno);
  int fdoutput = open(outputname, O_CREAT|O_EXCL|O_WRONLY, S_IRUSR|S_IWUSR);
  exit_error("Ouverture du fichier à écrire", fdoutput==-1, errno);

  socklib::BufferedReaderWriter input(fdinput);

  input.do_copy(fdoutput);

  res = close(fdoutput);
  exit_error(std::string("Fermeture du fichier ")+outputname, res==-1, errno);
  input.close();

  // on vérifie que les fichiers sont identique
  std::ifstream verif_in(inputname);
  std::ifstream verif_out(outputname);

  if (!(verif_in)) {
    exit_error(std::string("Impossible d'ouvrir le fichier ")+inputname, true, errno);
  }
  if (!(verif_out)) {
    exit_error(std::string("Impossible d'ouvrir le fichier ")+outputname, true, errno);
  }
  
  bool equal = std::equal(std::istream_iterator<char>(verif_in),
		     std::istream_iterator<char>(),
		     std::istream_iterator<char>(verif_out)		     );

  res = unlink(outputname);
  exit_error("Suppression du fichier temporaire", res==-1, errno);
  
  if (equal) {
    std::cerr << "Réussit !!" << std::endl;
    return true;
  } else {
    std::cerr << "Manqué !!" << std::endl;
    return false;
  }
}

bool test_rw_close() {
  
  std::cerr << "Test de la fonction close des ReaderWriter" << std::endl;

  std::cerr << "fermetur du fd 1" << std::endl;
  socklib::ReaderWriter r(1);
  r.close();
  std::cerr << "Réussit !!" << std::endl;
  
  std::cerr << "fermeture d'un fd inexistant (ce test doit générer une exception)" << std::endl; 
  try {
    socklib::ReaderWriter r(3);
    r.close();
    std::cerr << "Échoué !!" << std::endl;
    return false;
  } catch ( std::exception &e) {
    std::cerr << "Réussit !!" << std::endl;
  }

  return true;
}

bool test_socket_error() {
  std::cerr << "test d'erreur à la création de socket" << std::endl;

  std::cerr << "Ouverture d'une socket de serveur sur un port inexistant ... ";
  try {
    socklib::CreeSocketServeur("toto");
    std::cerr << "Manquée !!" << std::endl;
    return false;
  } catch (std::runtime_error &e) {
    std::cerr << "Réussit !!" << std::endl;
  }

  std::cerr << "Ouverture d'une socket de client sur un port inconnu ";
  try {
    socklib::CreeSocketClient("localhost", "toto");
    std::cerr << "Manquée !!" << std::endl;
    return false;
  } catch (std::runtime_error &e) {
    std::cerr << "Réussit !!" << std::endl;
  }

  std::cerr << "Ouverture d'une socket de client sur un serveur inconnu ";
  try {
    socklib::CreeSocketClient("existepas.loin.tcm", "80");
    std::cerr << "Manquée !!" << std::endl;
    return false;
  } catch (std::runtime_error &e) {
    std::cerr << "Réussit !!" << std::endl;
  }

  std::cerr << "Ouverture d'une socket de client sur un serveur local et un port non utilisé "
	    << std::endl;
  try {
    std::ostringstream p;
    srand(time(NULL));
    int dp;
    dp = rand();
    p << dp;
    socklib::CreeSocketClient("localhost", p.str());
    // si ça passe une fois, cela ne devrait pas la 2eme
    dp = rand();
    p << dp;
    socklib::CreeSocketClient("localhost", p.str());
    
    std::cerr << "Manquée !!" << std::endl;
    return false;
  } catch (std::runtime_error &e) {
    std::cerr << "Réussit !!" << std::endl;
  }

  

  
  return true;
}

void test(bool (fct)()) {
  try {
    std::cerr << "\033[32;11m#####################################\033[00m" << std::endl;
    if (fct()) {
      std::cerr << "\033[32;11m## Réussit ##########################\033[00m" << std::endl;    
    } else {
      std::cerr << "\033[31;11mLoupé#############################\033[00m" << std::endl;
      exit(1);
    }
  } catch (std::exception &e) {
    std::cerr << "\033[31;11m## Le test à généré une exception ###\033[00m" << std::endl;
    std::cerr << "\033[31;11m  " << e.what() << " ###\033[00m" << std::endl;
    exit(1);
  } 

}

int main(int argc, char *argv[]) {

  test(test_rw_close);
  test(test_rw_with_pipe);
  test(test_rw_with_pipe_and_fork);
  test(test_rw_with_read_by_part);

  test(test_brw_with_pipe_and_fork);
  test(test_brw_with_pipe_and_fork_long_message);
  test(test_brw_test_read);

  test(test_socket_error);
  test(test_brw_socket_long_message);
  test(test_brw_socket_test_read);

  test(test_copie_fichier);
  
  return 0;
}
