#pragma once
#include "readerwriter.hpp"
#include "string"

namespace std {
    /**
     * @brief échange de 2 `BufferedReaderWriter`
     * @param a,b : les deux `BufferedReaderWriter` à échanger
     */
  void swap(socklib::BufferedReaderWriter &a, socklib::BufferedReaderWriter &b);
}

namespace socklib {

  /**
   * @class BufferedReaderWriter
   * @brief classe de gestion des entrées sorties bufferisées
   *
   * C'est la classe principale qui gère les entrée sortie de haut niveau.
   * Pour la lecture, elle utilise un buffer pour stocker les données afin
   * de pouvoir gérer les lecture sur plusieurs octets comme :
   * - lecture d'une ligne read_line()
   * - lecture de données jusqu'à un fanion read_until()
   *
   * @warning A cause du buffer, le descripteur de fichier (FD) associé à un objet
   * est forcement lié à lui. Cela signifie, qu'il est impossible de copier un
   * `BufferedReaderWriter` et que sa destruction ferme le FD associé.
   **/
  class BufferedReaderWriter {
    /// la taille du buffer
    static const int TAILLE_BUFF=500;
    /// l'outil de lecture interne
    ReaderWriter rw;
    /// debut de ce qu'il reste à lire (si une lecture a déjà eu lieu sans vider le buffer et
    /// qu'on veut éviter de déplacer les données
    int deb;
    /// fin de ce qui reste à lire
    int fin;
    /// le buffer
    std::vector<char> buf;
    /// le fd est-il valide
    bool valid;

    /**
     * @brief fonction **interne** de lecture du buffer
     * @param deja : à partir de quel caractère doit-t-on considérer la lecture.
     * @return false si la socke test fermée
     * 
     * cette fonction retourne imédiatement si le buffer contient déjà plus de d'octet que demandé.
     * Sinon, il appelle la lecture sur le fd et tente de compléter le buffer. Dans ce cas elle peut être
     * blocante.
     */
    bool _intern_complete_buffer(int deja=0);

    /**
     * @brief fonction **interne** utilitaire de nettoyage du buffer
     * @param nboctet : le nombre d'octet à retirer
     * 
     */
    void _intern_retire_buffer(int nboctet);

  public:
    /**
     * @brief Constructeur à partir d'un descripteur de fichier.
     * @param fd : le *file descriptor* associé.
     * 
     * Ce constructeur initialise le buffer, il s'utilise à partir d'un FD existant.
     *
     * Par exemple ce constructeur s'utilise comme cela :
     * @code
     * int sock_client = socklib::CreeSocketClient("localhost", "23351");
     * BufferedReaderWriter rw(sock_client);
     * @endcode
     */    
    explicit BufferedReaderWriter(int fd);

    /**
     *  @brief Destructeur il fermera automatiquement le FD
     */    
    ~BufferedReaderWriter();

    /**
     * @brief Le constructeur de copie par défaut est interdit.
     *
     * Il est impossible de partager un descripteur de fichier dans plusieurs
     * objet bufferisé. Pour empêcher cela, le constructeur de copie est
     * interdit.
     * L'appel à ce constructeur par erreur provoquera une erreur de
     * compilation : `use of deleted function`
     */
    BufferedReaderWriter(const BufferedReaderWriter &brw) = delete;

    /**
     * @brief La copie de `BufferedReadrWriter` donc l'affectation est interdite
     *
     * Il est impossible de partager un descripteur de fichier dans plusieurs
     * objet bufferisé. Pour empêcher cela, l'opérateur d'affectation est
     * interdit.
     * L'appel à ce constructeur par erreur provoquera une erreur de
     * compilation : `use of deleted function`
     */
    BufferedReaderWriter &operator=(const BufferedReaderWriter &brw) = delete;

    /**
     * @brief Le constructeur de copie d'une valeur temporaire.
     * @param brw : l'origine de la copie
     */
    BufferedReaderWriter(BufferedReaderWriter &&brw);
    /**
     * @brief L'affectation d'une valeur temporaire
     * @param brw : l'origine de la copie
     *
     * Cette fonction échange les 2 objets pour que la valeur écrasée
     * par l'afectation soit détruite durant la destruction de l'objet
     * affecté.
     */
    BufferedReaderWriter &operator=(BufferedReaderWriter &&brw);

    /**
     * @brief échange de 2 `BufferedReaderWriter`
     * @param a,b : les deux `BufferedReaderWriter` à échanger
     */
    friend void std::swap(BufferedReaderWriter &a, BufferedReaderWriter &b);
    
    /**
     * @brief fermeture du flux
     */
    void close();
    
    /**
     * @brief récupération de donnée
     * @return un vector avec les données lues
     *
     * Cette fonction retourne soit les données du buffer soit fait une
     * requète sur le fd sous-jascent (ce qui peut être blocant). 
     */
    std::vector<char> read();

    /**
     * @brief récupération d'un nombre de données connu
     * @param len : le nombre d'octets attendus
     * dès qu'une partie des donnés st arrivée.
     * @return le vector des données lues
     *
     * cette fonction fait toujours une lecture sur le fd interne.
     */
    std::vector<char> read_all(int len);

    /**
     * @brief récupération dans un tableau de char
     * @param buff : le tableau de données à remplir, il doit avoir
     * une taille suffisante
     * @param offset : où commencer le remplissage
     * @param len : le nombre d'octets attendus
     * @param wait : doit-t-on attendre toutes les données ou s'arréter
     * dès qu'une partie des données est arrivée.
     * @return le nombre de données lues (0 si la socket est fermé ou
     * le fichié est terminé).
     *
     * cette fonction fait toujours une lecture sur le fd interne.
     */
    int read_data(char *buff, int offset, int len, bool wait=true);

    
    /**
     * @brief récupération jusqu'à un fanion ou la fermeture
     * @param end : le caractère attendu (il est concervé dans le tableau lu)
     * @return le vector des données lues
     *
     * Cette fonction renvoie les données lues jusqu'au fanion. Le tableau
     * renvoyé contien le fanion sauf si la fonction c'est terminée sur la fin
     * du fd sous-jascent. Dans ce cas la fonction renvoie les données. Si le
     * tableau rendu est de taille 0, c'est que le fd est fermé.
     */
    std::vector<char> read_until(char end);

    /**
     * @brief récupération jusqu'à un fanion sous la forme de chaine de
     * caractères
     * @param end : le caractère attendu (il est concervé dans le tableau
     * lu sauf si c'est '\0')
     * @return la chaine des données lues
     *
     * Cette fonction renvoie les données lues jusqu'au fanion. Le tableau
     * renvoyé contien le fanion sauf si la fonction c'est terminée sur la fin
     * du fd sous-jascent. Dans ce cas la fonction renvoie les données.
     * Si le tableau rendu est de taille 0, c'est que le fd est fermé.
     */
    std::string read_line(char end = '\n');

    /**
     * @brief test s'il y a qqchose à lire
     * @return true s'il y a des octets à lire, false sinon 
     */
    bool test_read();
    
    /**
     * @brief Envoie/Écrit des données sur le flux
     * @param data : les données
     * @param offset, len : le début et la longueur des données à envoyer. Si len est -1 toutes les
     * données sont envoyée.
     */
    void write(const std::vector<char> &data, int offset=0, int len=-1);

    /**
     * @brief Envoie/Écrit des données sur le flux
     * @param data : les données sous forme de string
     * @param offset, len : le début et la longueur des données à envoyer. Si len est -1 toutes les
     * données sont envoyée.
     */
    void write(const std::string &data, int offset=0, int len=-1);

    /**
     * @brief copie le contenu d'un fichier ou d'une socket dans une autre
     * jusqu'à la fermeture du premier
     * @param dest : le fd où copier le contenu de l'objet appelant
     * @return le nombre d'octets écrit
     *
     * Cette fonction crée un ReaderWriter temporaire pour faire le
     * travail, mais elle ne ferme pas le FD qui lui a été transmis.
     */
    int do_copy(int dest);
    
    /**
     * @brief copie le contenu d'un fichier ou d'une socket dans un autre jusqu'à la fermeture du premier
     * @param dest : le ReaderWriter où copier le contenu de l'objet appelant
     * @return le nombre d'octets écrit
     */
    int do_copy(socklib::ReaderWriter &dest);
  };
}

