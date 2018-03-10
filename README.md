# Documentation de la librairie SockLib

## Description générale

Inspirée par l'équivalent en JAVA, la librairie contient 2 classes :
  - la classe socklib::ReaderWriter qui est une classe **interne** dont le but est de faire un habillage C++ aux fonctions de la librairie C. La pluspart du temps  *ce n'est pas celle que vous souhaitez utiliser*.
  - la classe socklib::BufferedReaderWriter est une classe utilisant la première et un buffer pour crér des méthodes plus simples à manipuler.

## Mise en place du client ou du serveur

Ces deux classes utilisent un descripteur de fichier *FD* qui peut être entre autre un fichier ou une socket connectée. Le `BufferedReaderWriter` est crée à partir d'un *FD* existant que l'on peut obtenir grâce aux fonctions habituelles de l'API C (`open()` pour les fichiers et `socket()` pour les sockets).  Cependant, pour simplifier l'utilisation des sockets, dans l'espace de nom socklib, il existe aussi les fonctions suivantes
  - pour le serveur :
    + socklib::CreeSocketServeur qui permet de créer une socket d'attente
    + socklib::AcceptConnexion qui permet d'accepter un client
  - Pour le client :
    + socklib::CreeSocketClient qui permet de se connecter à un serveur

Exemple pour un serveur

    // création de la socket d'écoute
    int s_ec = socklib::CreeSocketServeur("8080");
    // attente du client
    int sock = socklib::AcceptConnexion(s_ec);

Exemple pour un client

	// création de la socket connectée avec le serveur
    int sock = socklib::CreeSocketClient("localhost", "8080");

Ces fonctions crèent des sockets en mode connecté `sock` que vous pouvez ajouter à un  socklib::BufferedReaderWriter.


## Fonctionnement

Le `BufferedReaderWriter` est une classe qui contient une socket de dialogue et un buffer pour la lecture. Ces objets ne peuvent être copiés et leur desctruction entraine la fermeture automatique de la socket. Il proposent des primitives de lecture évoluées :

  - socklib::BufferedReaderWriter::read() : pour lire des données arrivées sur la socket 
  - socklib::BufferedReaderWriter::read_all(int l) : pour lire exactement `l` octets
  - socklib::BufferedReaderWriter::read_line() : pour lire une ligne de texte terlinée par un `\n`
  - socklib::BufferedReaderWriter::read_line(char end) : pour lire une série d'octets jusqu'à un caractère spécial : `end`.
  - socklib::BufferedReaderWriter::test_read() : qui permet de savoir s'il y a des octets à lire dans la socket.

La classe propose aussi des primitive d'écriture :

  - \link socklib::BufferedReaderWriter::write(const vector<char> &data, int offset=0, int len=-1) socklib::BufferedReaderWriter::write(const vector<char> &data)\endlink : pour écrire des octets
  - \link socklib::BufferedReaderWriter::write(const std::string &data, int offset=0, int len=-1) socklib::BufferedReaderWriter::write(const string &data)\endlink : pour écrire du texte.

Par exemple après la création de la socket `sock` on peut lire et afficher les lignes envoyées par :

	// création du BufferedReaderWriter
    socklib::BufferedReaderWriter rw(sock);
	
    // on lit la valeur ligne par ligne jusquà la fermeture de la
	// socket
    while (true) {
        string ligne = rw.read_line();
        // si la ligne est vide, c'est que la socket est fermée
        if (ligne == "") {
        // cette fermeture inatendue provoque un warning mais
		// ce n'est pas une erreur
            break;
        }
        cout << "reçu : " << ligne;
    }
    cout << "Fin du dialogue" << endl;

    // fermeture de la socket (c'est fait automatiquement
	// à la destruction de l'objet, mais il vaut mieu le faire
	// explicitement).
    rw.close();

De l'autre coté Pour envoyer des lignes de texte :

	// création du BufferedReaderWriter
    socklib::BufferedReaderWriter rw(sock);

	// on lit ce que tape l'utilisateur sur la console et on
	// l'envoie
    while (true) {
        string ligne;
        getline(cin, ligne);
        // si l'utilisateur tape bye, on sort
        if (ligne == "bye") {
            // cette fermeture inatendue provoque un warning mais
		    // n'est pas une erreur
            break;
        }
        cout << "envoie de : " << ligne << endl;

        // on envoie la ligne lue en ajoutant bien un \n car le pair
        // attend une ligne complète
        rw.write(ligne+"\n");
    }

    // fermeture de la socket (c'est fait automatiquement
	// à la destruction de l'objet, mais il vaut mieu le faire
	// explicitement).
    rw.close();
