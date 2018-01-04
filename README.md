# Documentation de la librairie SockLib

## Description générale

Inspirée par l'équivalent en JAVA, la librairie contient 2 classes :
  - la classe socklib::ReaderWriter qui est une classe **interne** dont le but est de faire un habillage C++ aux fonctions de la librairie C. La pluspart du temps  *ce n'est pas celle que vous souhaitez utiliser*.
  - la classe socklib::BufferedReaderWriter est une classe utilisant la première et un buffer pour crér des méthodes plus simples à manipuler.

Ces deux classes utilisent un descripteur de fichier *FD* qui peut être entre autre un fichier ou une socket connectée. Le `BufferedReaderWriter` est crée à partir d'un *FD* existant que l'on peut obtenir grâce au fonction habituelle de l'API C (`open` pour les fichiers et `socket` pour les socket).  Cependant, pour simplifier l'utilisation de socket, dans l'espace de nom socklib, il existe aussi les fonctions suivantes
  - pour le serveur :
    + socklib::CreeSocketServeur qui permet de créer un socket d'attente
    + socklib::AcceptConnexion qui permet d'acceptr un client
  - Pour le client :
    + socklib::CreeSocketClient qui permet de se connecter à un serveur

Ces fonctions crèent des sockets en mode connecté que vous pouvez ajouter à un  socklib::BufferedReaderWriter.


