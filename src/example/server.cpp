#include <iostream>
#include <string>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>

// Inclusion des entete de la librairie
#include <socklib.hpp>
#include <bufferedreaderwriter.hpp>

using namespace std;

int main()
{
    int res;

    // port d'écoute du srveur
    string port = "8080";

    cout << "ouverture d'un serveur en écoute sur le port " << port << endl;

    // création de la socket d'écoute
    int s_ec = socklib::CreeSocketServeur(port);

    // attente du client
    int s_cli = socklib::AcceptConnexion(s_ec);

    // la socket d'écoute n'est plus utile
    res = close(s_ec);
    exit_error("Fermeture socket", res == -1, errno);

    // création du BufferedReaderWriter
    socklib::BufferedReaderWriter cli(s_cli);

    // on lit la valeur ligne par ligne jusquà la fermeture de la socket par l client
    while (true) {
        string ligne = cli.read_line();
        // si la ligne est vide, c'est que la socket est fermée
        if (ligne == "") {
            // cette fermeture inatendue provoque un warning mais n'est pas une erreur
            break;
        }
        cout << "reçu : " << ligne;
    }

    cout << "Fin du dialogue avec le client" << endl;

    // fermeture de la socket (c'est fait automatiquement à la destruction de l'objet, mais il vaut mieu le faire explicitement).
    cli.close();

    return 0;
}
