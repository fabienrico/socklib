#include <iostream>
#include <string>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>

// Inclusion des entete de la librairie
#include <socklib.hpp>
#include <bufferedreaderwriter.hpp>


using namespace std;

int main(int argc, char *argv[])
{
    // nom et port d'écoute du serveur
    string server = "localhost";
    string port = "8080";

    cout << "connection au serveur " << server << ":" << port << endl;

    // création de la socket d'écoute
    int s_srv = socklib::CreeSocketClient(server, port);

    // création du BufferedReaderWriter
    socklib::BufferedReaderWriter srv(s_srv);

    // on lit ce que tape l'utilisateur sur la console et on l'envoie au serveur
    while (true) {

        string ligne;
        getline(cin, ligne);
        // si la ligne est vide, c'est que la socket est fermée
        if (ligne == "bye") {
            // cette fermeture inatendue provoque un warning mais n'est pas une erreur
            break;
        }
        cout << "envoie de : " << ligne << endl;

        // on envoie la ligne lue en ajoutant bien un \n car le serveur
        // attend une ligne complète (voir le code du serveur)
        srv.write(ligne+"\n");
    }

    cout << "Fin du dialogue avec le serveur" << endl;

    // fermeture de la socket (c'est fait automatiquement à la destruction de l'objet, mais il vaut mieu le faire explicitement).
    srv.close();

    return 0;
}
