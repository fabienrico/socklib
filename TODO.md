# TODO list

### A faire
	- faire des warnings à la fermeture de la socket
	- faire des niveaux de log d'exécution pour une utilisation moin verbeuse.
	- faire une implementation compatible windows.
	- faire une lecture avec temps max
	- vérifier le fonctionnement de do_copy, bizarrement il utilise des RW
	  non bufferisés et du code de lecture propre au lieu de la méthode
	  générale

### Fait

	- corriger le string param de write
	  commit 4c26eea55fecf7e4715fc7de38f4a0cf5bc34f11
	- faire qqchose sur le constructeur de reférences temporaire et
      vérifier qu'on peut ajouter des bufferedRW dans un tableau
	  commit e858ef8ebae7872dd48bc45ad3faf6ad37cfd87f
	- utilisation de codeblocks
	  commit 0b031929423aab034c39b8e414055fbc32fd511f

