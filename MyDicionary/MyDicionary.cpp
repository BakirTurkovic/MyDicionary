#pragma once
#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>
using namespace std;

template<class Tip>
class Cvor {
public:
    Cvor* next;
    Cvor* prev;
    Tip info;

    Cvor(Tip info, Cvor* next = nullptr, Cvor* prev = nullptr) {
        this->info = info;
        this->next = next;
        this->prev = prev;
    }
};

template <class Tip>
class MyList {
public:
    Cvor<Tip>* prvi = nullptr;
    Cvor<Tip>* zadnji = nullptr;

    ~MyList() { clear(); }

    void clear() {
        while (prvi != nullptr) {
            Cvor<Tip>* t = prvi;
            prvi = prvi->next;
            delete t;
        }
        zadnji = nullptr;
    }

    bool dodaj(const Tip& v) {
        Cvor<Tip>* novi = new Cvor<Tip>(v, nullptr, zadnji);
        if (zadnji != nullptr) zadnji->next = novi;
        else prvi = novi;
        zadnji = novi;
        return true;
    }

    Tip remove() {
        if (prvi == nullptr) throw runtime_error("Prazna lista");

        Cvor<Tip>* t = prvi;
        prvi = prvi->next;
        if (prvi != nullptr) prvi->prev = nullptr;
        else zadnji = nullptr;

        Tip x = t->info;
        delete t;
        return x;
    }

    bool obrisi_po_cvoru(Cvor<Tip>*& x) {
        if (x == nullptr) return false;

        if (x->prev != nullptr) x->prev->next = x->next;
        else prvi = x->next;

        if (x->next != nullptr) x->next->prev = x->prev;
        else zadnji = x->prev;

        delete x;
        x = nullptr;
        return true;
    }

    bool jelPrazna() const { return prvi == nullptr; }
};

template <typename Key, typename Value>
ostream& operator<<(ostream& os, const pair<Key, Value>& p) {
    os << "{" << p.first << ": " << p.second << "}";
    return os;
}

template <class Key, class Value>
class MyDictionary {
private:
    // OVDJE SAM RADIO: Dodat pokazivac na kante i bucketSize
    MyList<pair<Key, Value>>* buckets = nullptr;
    int bucketSize;
    int brojac = 0;

    int hashFunction(const Key& key) const {
        // OVDJE SAM RADIO: Izracunavanje heš indeksa
        return abs((int)key) % bucketSize;
    }

    void rehash() {
        // OVDJE SAM RADIO: Proširenje niza kanti i preraspodjela elemenata
        MyList<pair<Key, Value>>* oldBuckets = buckets;
        int oldBucketSize = bucketSize;
        bucketSize *= 2;
        buckets = new MyList<pair<Key, Value>>[bucketSize];
        brojac = 0;

        for (int i = 0; i < oldBucketSize; ++i) {
            while (!oldBuckets[i].jelPrazna()) {
                auto kv = oldBuckets[i].remove();
                dodaj(kv.first, kv.second);
            }
        }
        delete[] oldBuckets;
    }

    void checkLoadFactor() {
        // OVDJE SAM RADIO: Provjera opterećenja (70%) za poziv rehash-a
        if (brojac > bucketSize * 0.7) {
            rehash();
        }
    }

public:
    MyDictionary(int bucketSize = 100) {
        // OVDJE SAM RADIO: Inicijalizacija niza kanti i brojača
        this->bucketSize = bucketSize;
        buckets = new MyList<pair<Key, Value>>[bucketSize];
        brojac = 0;
    }

    ~MyDictionary() {
        // OVDJE SAM RADIO: Oslobađanje alocirane memorije
        delete[] buckets;
        buckets = nullptr;
    }

    bool dodaj(const Key& key, const Value& value) {
        // OVDJE SAM RADIO: Provjera duplikata, ubacivanje u kantu i checkLoadFactor
        if (trazi(key)) {
            return false;
        }
        int i = hashFunction(key);
        buckets[i].dodaj(make_pair(key, value));
        brojac++;
        checkLoadFactor();
        return true;
    }

    bool obrisi(const Key& key) {
        // OVDJE SAM RADIO: Pronalaženje ključa u kanti i brisanje čvora
        int i = hashFunction(key);
        Cvor<pair<Key, Value>>* current = buckets[i].prvi;

        while (current != nullptr) {
            if (current->info.first == key) {
                buckets[i].obrisi_po_cvoru(current);
                brojac--;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    bool trazi(const Key& key) const {
        // OVDJE SAM RADIO: Pretraga ključa u odgovarajućoj kanti
        int i = hashFunction(key);
        Cvor<pair<Key, Value>>* current = buckets[i].prvi;

        while (current != nullptr) {
            if (current->info.first == key) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    Value& operator[](const Key& key) {
        // OVDJE SAM RADIO: Vraćanje reference ili auto-inserting sa default vrijednošću
        int i = hashFunction(key);
        Cvor<pair<Key, Value>>* current = buckets[i].prvi;

        while (current != nullptr) {
            if (current->info.first == key) {
                return current->info.second;
            }
            current = current->next;
        }

        dodaj(key, Value());
        i = hashFunction(key);
        current = buckets[i].prvi;
        while (current != nullptr) {
            if (current->info.first == key) {
                return current->info.second;
            }
            current = current->next;
        }
        throw runtime_error("Greska pri pristupu elementu.");
    }

    int getBrojac() const { return brojac; }
    bool jelPrazna() const { return brojac == 0; }
};

// ============================================================
//                    TEST FRAMEWORK
// ============================================================
int ukupnoTestova = 0;
int prosloTestova = 0;

void provjeri(bool uslov, const string& opis) {
    ukupnoTestova++;
    if (uslov) {
        prosloTestova++;
        cout << "  [PASS] " << opis << endl;
    }
    else {
        cout << "  [FAIL] " << opis << " <<<" << endl;
    }
}

template<typename Fn>
void safeTest(Fn fn) {
    try {
        fn();
    }
    catch (const exception& e) {
        ukupnoTestova++;
        cout << "  [FAIL] Izuzetak: " << e.what() << " <<<" << endl;
    }
    catch (...) {
        ukupnoTestova++;
        cout << "  [FAIL] Nepoznat izuzetak <<<" << endl;
    }
}

void separator(const string& naslov) {
    cout << "\n========================================" << endl;
    cout << "  " << naslov << endl;
    cout << "========================================" << endl;
}

// ============================================================
//                  TESTOVI ZA MyDictionary
// ============================================================

void testKonstruktorIPrazna() {
    separator("T1: Konstruktor i jelPrazna()");

    safeTest([] {
        MyDictionary<int, string> dict(5);
        provjeri(dict.jelPrazna() == true, "Novi dictionary je prazan");
        provjeri(dict.getBrojac() == 0, "getBrojac() = 0 za prazan dictionary");
        });
}

void testDodaj() {
    separator("T2: dodaj()");

    safeTest([] {
        MyDictionary<int, string> dict(5);

        bool r1 = dict.dodaj(5, "pet");
        provjeri(r1 == true, "dodaj(5, \"pet\") vraca true");

        bool r2 = dict.dodaj(10, "deset");
        provjeri(r2 == true, "dodaj(10, \"deset\") vraca true");

        bool r3 = dict.dodaj(15, "petnaest");
        provjeri(r3 == true, "dodaj(15, \"petnaest\") vraca true");

        provjeri(dict.getBrojac() == 3, "getBrojac() = 3 nakon 3 dodavanja");
        provjeri(dict.jelPrazna() == false, "Dictionary nije prazan");
        });
}

void testDodajDuplikat() {
    separator("T3: dodaj() - duplikat kljuca");

    safeTest([] {
        MyDictionary<int, string> dict(5);
        dict.dodaj(10, "deset");
        dict.dodaj(20, "dvadeset");

        bool dup = dict.dodaj(10, "NOVI");
        provjeri(dup == false, "dodaj(10, \"NOVI\") vraca false (duplikat)");
        provjeri(dict.getBrojac() == 2, "getBrojac() = 2 (duplikat nije ubrojan)");
        });
}

void testTrazi() {
    separator("T4: trazi()");

    safeTest([] {
        MyDictionary<int, string> dict(5);
        dict.dodaj(5, "pet");
        dict.dodaj(10, "deset");
        dict.dodaj(15, "petnaest");

        provjeri(dict.trazi(5) == true, "trazi(5) = true");
        provjeri(dict.trazi(10) == true, "trazi(10) = true");
        provjeri(dict.trazi(15) == true, "trazi(15) = true");
        provjeri(dict.trazi(99) == false, "trazi(99) = false (ne postoji)");
        provjeri(dict.trazi(0) == false, "trazi(0) = false (ne postoji)");
        });
}

void testOperatorIndex() {
    separator("T5: operator[]");

    safeTest([] {
        MyDictionary<int, string> dict(5);
        dict.dodaj(5, "pet");

        // Pristup postojecem
        provjeri(dict[5] == "pet", "dict[5] = \"pet\"");

        // Izmjena postojeceg
        dict[5] = "PET";
        provjeri(dict[5] == "PET", "dict[5] = \"PET\" nakon izmjene");

        // Pristup nepostojecem (auto-insert sa default Value)
        string val = dict[99];
        provjeri(val == "", "dict[99] vraca \"\" (default string)");
        provjeri(dict.trazi(99) == true, "trazi(99) = true nakon dict[99]");
        provjeri(dict.getBrojac() == 2, "getBrojac() = 2 (5 + auto-insert 99)");

        // Upis preko operatora
        dict[99] = "devedeset devet";
        provjeri(dict[99] == "devedeset devet", "dict[99] = \"devedeset devet\" nakon upisa");
        });
}

void testObrisi() {
    separator("T6: obrisi()");

    safeTest([] {
        MyDictionary<int, string> dict(5);
        dict.dodaj(5, "pet");
        dict.dodaj(10, "deset");
        dict.dodaj(15, "petnaest");

        bool r1 = dict.obrisi(10);
        provjeri(r1 == true, "obrisi(10) vraca true");
        provjeri(dict.trazi(10) == false, "trazi(10) = false nakon brisanja");
        provjeri(dict.getBrojac() == 2, "getBrojac() = 2 nakon brisanja");

        bool r2 = dict.obrisi(777);
        provjeri(r2 == false, "obrisi(777) vraca false (ne postoji)");
        provjeri(dict.getBrojac() == 2, "getBrojac() = 2 (neuspjesno brisanje ne mijenja brojac)");
        });
}

void testObrisiPaTrazi() {
    separator("T7: obrisi() pa ponovo dodaj isti kljuc");

    safeTest([] {
        MyDictionary<int, int> dict(5);
        dict.dodaj(1, 100);
        dict.dodaj(2, 200);

        dict.obrisi(1);
        provjeri(dict.trazi(1) == false, "trazi(1) = false nakon brisanja");

        bool r = dict.dodaj(1, 999);
        provjeri(r == true, "dodaj(1, 999) vraca true (ponovo dozvoljeno)");
        provjeri(dict[1] == 999, "dict[1] = 999 (nova vrijednost)");
        });
}

void testRazlicitiKljucevi() {
    separator("T8: Razliciti kljucevi i vrijednosti");

    safeTest([] {
        MyDictionary<int, string> dict(5);
        dict.dodaj(71, "Sarajevo");
        dict.dodaj(36, "Mostar");
        dict.dodaj(75, "Tuzla");

        provjeri(dict.trazi(71) == true, "trazi(71) = true (Sarajevo)");
        provjeri(dict.trazi(99) == false, "trazi(99) = false (ne postoji)");
        provjeri(dict[36] == "Mostar", "dict[36] = \"Mostar\"");

        dict.obrisi(75);
        provjeri(dict.trazi(75) == false, "trazi(75) = false nakon brisanja (Tuzla)");
        provjeri(dict.getBrojac() == 2, "getBrojac() = 2");
        });
}

void testViseElemenata() {
    separator("T9: Vise elemenata (test hash distribucije)");

    safeTest([] {
        MyDictionary<int, int> dict(3); // mali bucket size
        for (int i = 0; i < 20; i++) {
            dict.dodaj(i, i * 10);
        }
        provjeri(dict.getBrojac() == 20, "getBrojac() = 20 nakon 20 dodavanja");

        // Provjeri da su svi dostupni
        bool sviPronadjeni = true;
        for (int i = 0; i < 20; i++) {
            if (!dict.trazi(i)) { sviPronadjeni = false; break; }
        }
        provjeri(sviPronadjeni, "Svih 20 elemenata pronadjeno");

        // Provjeri vrijednosti
        bool vrijednostiOk = true;
        for (int i = 0; i < 20; i++) {
            if (dict[i] != i * 10) { vrijednostiOk = false; break; }
        }
        provjeri(vrijednostiOk, "Sve vrijednosti ispravne (dict[i] == i*10)");

        // Obrisi neke
        dict.obrisi(0);
        dict.obrisi(10);
        dict.obrisi(19);
        provjeri(dict.getBrojac() == 17, "getBrojac() = 17 nakon 3 brisanja");
        provjeri(dict.trazi(0) == false, "trazi(0) = false nakon brisanja");
        provjeri(dict.trazi(10) == false, "trazi(10) = false nakon brisanja");
        provjeri(dict.trazi(5) == true, "trazi(5) = true (nije obrisan)");
        });
}

void testEdgeCases() {
    separator("T10: Edge cases");

    // Obrisi iz praznog
    safeTest([] {
        MyDictionary<int, int> dict(5);
        bool r = dict.obrisi(1);
        provjeri(r == false, "obrisi(1) na praznom = false");
        });

    // Trazi u praznom
    safeTest([] {
        MyDictionary<int, int> dict(5);
        provjeri(dict.trazi(1) == false, "trazi(1) na praznom = false");
        });

    // Operator[] na praznom (auto-insert)
    safeTest([] {
        MyDictionary<int, int> dict(5);
        int val = dict[42];
        provjeri(val == 0, "dict[42] na praznom = 0 (default int)");
        provjeri(dict.getBrojac() == 1, "getBrojac() = 1 nakon auto-inserta");
        provjeri(dict.trazi(42) == true, "trazi(42) = true nakon auto-inserta");
        });

    // Dodaj pa obrisi sve
    safeTest([] {
        MyDictionary<int, string> dict(5);
        dict.dodaj(1, "a");
        dict.dodaj(2, "b");
        dict.dodaj(3, "c");
        dict.obrisi(1);
        dict.obrisi(2);
        dict.obrisi(3);
        provjeri(dict.jelPrazna() == true, "jelPrazna() = true nakon brisanja svega");
        provjeri(dict.getBrojac() == 0, "getBrojac() = 0 nakon brisanja svega");
        });
}

// ============================================================
//                        MAIN
// ============================================================
int main() {
    cout << "=========================================" << endl;
    cout << "   TESTER: MyDictionary (Hash Tabela)    " << endl;
    cout << "=========================================" << endl;

    testKonstruktorIPrazna();
    testDodaj();
    testDodajDuplikat();
    testTrazi();
    testOperatorIndex();
    testObrisi();
    testObrisiPaTrazi();
    testRazlicitiKljucevi();
    testViseElemenata();
    testEdgeCases();

    separator("REZULTAT");
    cout << "\n  Proslo: " << prosloTestova << " / " << ukupnoTestova << endl;
    if (prosloTestova == ukupnoTestova) {
        cout << "\n  *** SVE PROLAZI! Svaka cast! ***" << endl;
    }
    else {
        cout << "\n  >>> Ima gresaka - pogledaj [FAIL] testove gore. <<<" << endl;
    }
    cout << endl;

    return 0;
}