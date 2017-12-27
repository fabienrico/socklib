#pragma once
#include <vector>

namespace socklib {
  /**
     @brief classe de lecteur sur un file descriptor sans buffer.
     
     Cette classe est simplifiée plutot basée sur le fonctionnement des socker C. Elle est définie
     pour proposer une lecture et une écriture même sur les fd qui ne permettent que l'un des deux
     (fichier ouvert en lecture, pipe, ...). Dans le cas de l'appel à la fonction lecture ou ériture
     alors que c'est impossible, cela génèrera des exceptions.
     Cette classe n'est prévue que pour être utilisée dans le la classe de donnée bufferisées
  */
  
  class ReaderWriter{
  private :
    int fd;
    
  public:
    /**
       @brief constructeur à partir d'un file descriptor
    */
    ReaderWriter(int fd);
    
    /**
       @brief fermeture du flux
    */
    void close();

    /**
     * @brief test s'il y a qqchose à lire en modifiant le comportement
     * pour le rendre non blocant.
     * @param buff : le char * où stocker les données lues s'il ty en a,
     * @param offset : la position où commencer de mettre les données
     * @param len : la taille maximum des données à lire.
     * @return le nombre d'octet lu s'il y en a ou 0 si le fd est
     *  terminé ou s'il n'y a rien à lire
     * @throws un error_system s'il y a une erreur de lecture.
     * 
     * Cette méthode est forcement non blocante, par contre s'il y
     * a des caractères à lire elle les lit. Dans les man, il est dit que
     * si la longueur demandée est 0, cela PEUT permettre de détecter
     * les erreur. Dans ce cas, cela permettrait de tester sans lire. Mais
     * ce n'est pas dans la norme.
     */
    int test_and_read(char *buff, int offset, int len);
    
    /**
       @brief lire des données arrivées
       @param buff : le vector où stocker les données,
       @param wait_all : attendre que toutes les données 
       @throws une std::runtime_error si le fd se ferme alors qu'on attend encore des données ou
       une std::error_condition s'il y a une erreur d'entrée sortie
       
       Cette méthode lit les données par un seul read, le buffer doit avoir une taille de départ,
       qui sera le nombre d'octets lus au maximum. Si wait_all est ''true'' la foctione attend que tout
       les octets soient arrivés.
    */
    int read(std::vector<char> &buff, bool wait_all=false);

    /**
       @brief lire des données arrivées dans un buffer
       @param buff : le char * où stocker les données,
       @param offset : la position où commencer de mettre les données
       @param len : la taille maximum des données à lire
       @param wait_all : attendre que toutes les données
       @throws une std::runtime_error si le fd se ferme alors qu'on attend encore des données ou
       une std::error_condition s'il y a une erreur d'entrée sortie
       
       Cette méthode lit les données par un seul read, le buffer doit avoir une taille de départ,
       qui sera le nombre d'octets lus au maximum. Si wait_all est ''true'' la foctione attend que tout
       les octets soient arrivés.
    */
    int read(char *buff, int offset, int len, bool wait_all=false);

    /**
       @brief ecriture de données
       @param buff : le vecteur de données à écrires
       @throw une erreur si l'ecriture est impossible
    */
    int write(const std::vector<char> &buff);

    /**
       @brief ecriture de données
       @param buff : le tableau des données à écrire
       @param offset, len : le début des données et leur taille
       @throw une erreur si l'ecriture est impossible
    */
    int write(const char* buff, int offset, int len);

  };
  
}

