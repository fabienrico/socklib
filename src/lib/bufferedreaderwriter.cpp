#include <socklib.hpp>
#include <bufferedreaderwriter.hpp>
#include <sstream>
#include <cstring>
#include <algorithm>

socklib::BufferedReaderWriter::BufferedReaderWriter(int fd) :
  rw(fd), deb(0), fin(0), buf(TAILLE_BUFF), valid(true) {
}

socklib::BufferedReaderWriter::BufferedReaderWriter(BufferedReaderWriter &&brw) :
  rw(brw.rw), deb(brw.deb), fin(brw.fin), buf(brw.buf), valid(brw.valid)
{
  brw.valid = false;
}

void std::swap(socklib::BufferedReaderWriter &a, socklib::BufferedReaderWriter &b){
  int temp;
  bool tempb;
  std::swap(a.rw, b.rw);

  temp = a.deb;
  a.deb = b.deb;
  b.deb = temp;

  temp = a.fin;
  a.fin = b.fin;
  b.fin = temp;

  tempb = a.valid;
  a.valid = b.valid;
  b.valid = tempb;

  std::swap(a.buf, b.buf);
}

socklib::BufferedReaderWriter &
socklib::BufferedReaderWriter::operator=(socklib::BufferedReaderWriter &&brw) {
  
  std::swap(*this, brw);
  
  return *this;
}

socklib::BufferedReaderWriter::~BufferedReaderWriter() {
  if  (this->valid) {
    this->close();
  }
}

bool socklib::BufferedReaderWriter::_intern_complete_buffer(int deja) {
  if (fin-deb>deja) {
    return true;
  }
  // on teste pour savoir s'il est possiblde de lire qqchose
  warning_error("On ne peut pas compléter le buffer qui est plein. Cela ne devrait pas arriver, prévener le programmeur de la librairie", buf.size()-fin <= 0, 0);

  int lus = rw.read(buf.data(), fin, buf.size()-fin, false);
  fin += lus;

  return (lus != 0);
}

void socklib::BufferedReaderWriter::_intern_retire_buffer(int nboctet) {
  std::ostringstream msg;
  int arrives = fin-deb;
  
  if (arrives < nboctet) {
    msg  << "On ne peut pas retirer du buffer " << nboctet
	 << " alors qu'il a " << arrives << " octets";
    exit_error(msg.str().c_str(), true, 0);
  } else if (arrives == nboctet) {
    // le buffer est totalement consomé, on le vide
    fin = 0;
    deb = 0;
  } else {
    // le buffer n'est pas totaltement consomé, on change seulement la valeur de deb
    deb += nboctet;
  }
}

void socklib::BufferedReaderWriter::close() {
  rw.close();
  valid = false;
}

std::vector<char>
socklib::BufferedReaderWriter::read() {
  bool r = _intern_complete_buffer(0);

  if (!r) {
    return std::vector<char>();
  }

  std::vector<char> ret(fin-deb);

  memcpy(ret.data(), buf.data()+deb, fin-deb);
  fin = 0;
  deb = 0;

  return ret;
}

int
socklib::BufferedReaderWriter::read_data(char *buff, int offset, int len, bool wait) {
  _intern_complete_buffer(0);
  if (fin-deb == 0) {
    // si après une lecture, fin-deb est nul c'est que la socket est fermée
    return 0;
  }
  
  if (fin-deb>=len) {
    // il y a déjà dans le buffer suffisamment de données
    memcpy(buff, this->buf.data()+deb, len);
    _intern_retire_buffer(len);
    return len;
  } else {
    // Il faut récupérer les données et peut-être en attendre de nouvelles
    // d'abord on vide le buffer
    int deja = fin-deb;
    if (deja > 0) {
      memcpy(buff, this->buf.data()+deb, deja);
      _intern_retire_buffer(deja);
    }
    int nblus = deja;
    if (wait) {
      nblus += rw.read(buff, deja, len-deja, wait);
    }
    return nblus;
  }
}

std::vector<char>
socklib::BufferedReaderWriter::read_all(int len) {
  std::vector<char> ret(len);
  _intern_complete_buffer(0);

  if (fin-deb>=len) {
    // il y a déjà dans le buffer suffisamment de données
    memcpy(ret.data(), buf.data()+deb, len);
    _intern_retire_buffer(len);
  } else {
    // Il faut lire des données en plus
    // d'abord on vide le buffer
    int deja = fin - deb;
    if (deja > 0) {
      memcpy(ret.data(), buf.data()+deb, deja);
      _intern_retire_buffer(deja);
    }
    int nblus = rw.read(ret.data(), deja, len-deja, true);
    ret.resize(nblus+deja);
  }

  return  ret;
}

std::vector<char>
socklib::BufferedReaderWriter::read_until(char end) {
  std::vector<char> ret;

  while (true) {
    // on lit qq données (sort imédiatement s'il y en a déjà)
    bool ouvert = _intern_complete_buffer();
    if (!ouvert) {
      // le fd est fermé brutalement alors que le fanion n'a pas été trouvé. On prévient et on sort
      warning_error("Fermeture du fd sans avoir vu le fanion", true, 0);
      break;
    }
    // on test si le fanion est présent
    auto trouve = std::find(buf.begin()+deb, buf.begin()+fin, end);
    if (trouve != buf.begin()+fin) {
      // le fanion a été trouvé
      int taille = trouve-(buf.begin()+deb)+1;
      int actuel = ret.size();
      
      ret.resize(actuel+taille);
      memcpy(ret.data()+actuel, buf.data()+deb, taille);
      // on vide le buffer
      _intern_retire_buffer(taille);

      return ret;
    } else {
      // sinon il faut récupérer les données et continuer à lire
      int taille = fin-deb;
      int actuel = ret.size();

      //      ret.insert(ret.size(), buf.data()+deb, buf.data()+fin);
      ret.resize(actuel+taille);
      memcpy(ret.data()+actuel, buf.data()+deb, taille);
      // on vide le buffer
      deb = 0;
      fin = 0;
    }
  }
  return ret;
}

std::string
socklib::BufferedReaderWriter::read_line(char end) {
  std::vector<char> data = read_until(end);

  std::string ret;
  ret.append(data.data(), data.size());

  return ret;
}

bool
socklib::BufferedReaderWriter::test_read() {
  if (fin-deb>0) {
    // le buffer n'est pas vide
    return true;
  } else {
    // le buffer est vide on essaye de lire qqchose
    int nbl = rw.test_and_read(buf.data(), 0, buf.size());
    deb = 0;
    fin = nbl;
    return (nbl != 0);
  }
}

void
socklib::BufferedReaderWriter::write(const std::vector<char> &data,
				     int offset, int len) {
  int env = len;
  if (len == -1) {
    env = data.size();
  }
  rw.write(data.data(), offset, env);
}

void
socklib::BufferedReaderWriter::write(const std::string &data,
				     int offset, int len) {
  int env = len;
  if (len == -1) {
    env = data.size();
  }
  rw.write(data.data(), offset, env);
}

int
socklib::BufferedReaderWriter::do_copy(int fd) {
  socklib::ReaderWriter temp(fd);
  int res = do_copy(temp);
  return res;
}
int
socklib::BufferedReaderWriter::do_copy(socklib::ReaderWriter &dest) {
  int nb_recu;
  char buff[TAILLE_BUFF];

  // on commence par vider le buffer
  if (fin-deb > 0) {
    dest.write(this->buf.data(), deb, fin-deb);
    _intern_retire_buffer(fin-deb);
  }

  // tant que le fd n'est pas fermé, on lit et on écrit
  nb_recu = 0;
  while(true) {
    int res1, res2;
    res1 = this->rw.read(buff, 0, TAILLE_BUFF, false);
    if (res1 == 0) {
      // Le fichier/socket est terminé
      // on stoppe la fonction
      break;
    }

    nb_recu += res1;

    //    fprintf(stdout, "Recu %d oct total %d oct\n", res1, nb_recu);

    res2 = dest.write(buff, 0, res1);
    exit_error("Probleme ecriture fichier", res1!=res2, errno);
  }

  return nb_recu;
}
