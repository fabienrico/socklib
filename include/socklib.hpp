#pragma once
#include <cerrno>
#include <string>

/**
 * @file socklib.hpp
 * @brief fichier contenant les fonctions de créations de socket et les fonctions de gestion d'erreur
 */


/**
 * @namespace socklib
 * @brief espace de nom général
 **/

namespace socklib {
  class ReaderWriter;
  class BufferedReaderWriter;
  
  /**
     @brief fonction générique de la gestion d'erreur
     @param file,ligne : le nom et la ligne du fichier où a lieu l'erreur
     @param msg : le message à afficher en cas d'erreur
     @param cond : la condition (l'erreure est déclanchée si elle est vraie)
     @param errnum : code d'erreur système
     @param exception : si vrai envoie une exception en cas d'erreur,
     sinon, c'est just un warning
  */
  void
  __error_(const char *file, const int ligne, const std::string &msg,
	   bool cond, int errnum, bool exception);
  
  /**
     @brief fonction de test et de génération d'erreur
     @param msg : message à afficher
     @param cond : condition d'erreur
     @param errnum : numéro de l'erreur système ou 0 s'il n'y en a pas.

     Cette fonction affiche le texte d'erreur `msg` sur la sortie d'erreur
     et lance une exception.
     Si un code d'erreur `errnum` différent de 0 est fourni, l'exception
     générée est une erreur système (std::system_error) dont `errnum` est
     le code d'erreur.
     Sinon, l'exception est une std::runtime_error dont `msg` est le message.

     Par exemple le code suivant ouvre un FD en écriture en créant le
     fichier `toto.txt` et teste sa création et génère une exception
     système en cas d'erreur
     @code{.cpp}
     fdout = open("toto.txt", O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
     exit_error("Création du fichier", fdout==-1, errno);
     @endcode
  */
#define exit_error(msg, cond, errnum) socklib::__error_(__FILE__, __LINE__, (msg), cond, errnum, true)
  /**
     @brief fonction de test provoquant un warning
     @param msg : message à afficher
     @param cond : condition d'erreur
     @param errnum : numéro de l'erreur système ou 0 s'il n'y en a pas.

     teste la condition cond et affiche un warning.

     Cette fonction affiche le texte d'erreur `msg` sur la sortie d'erreur.
     Si un code d'erreur `errnum` différent de 0 est fourni, l'exception
     générée est une erreur système (std::system_error) dont `errnum` est
     le code d'erreur.
     Sinon, l'execption est une std::runtime_error dont `msg` est le message.

  */
#define warning_error(msg, cond, errnum) socklib::__error_(__FILE__, __LINE__, (msg), cond, errnum, false)


  /**
   * @brief Cree une socket d'attente pour le serveur sur le port ''port''
   * @param port : le port utilisé
   * @return la socket d'attente
   * @throws une system_error s'il y a un soucis à la création ou une runtime_error
   * si le port est occupé ou ne peux pas être pris
   *
   * En cas d'erreur, un message explicatif est affiché sur la sortie d'erreur 
   * standart
   */
  int CreeSocketServeur(const std::string &port);
  
  /**
   * @brief Crée une socket de client en se connectant à un serveur
   * @param server, port : le serveur et le port à contacter.
   * @return la socket de discution
   * @throws une system_error s'il y a un souci à la création ou une runtime_error
   * si le serveur ne peut pas être contacté
   */
  int CreeSocketClient(const std::string &server, const std::string &port);

  
  /**
   * @brief Accepte un nouveau client
   * @param s : la socket d'attente du serveur
   * @return la socket de discution
   * @throws une système error s'il y a un problème
   */
  int AcceptConnexion(int s);

}
