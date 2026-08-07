
# Comet - Simulatore di Sistema Solare con Qt6

## Descrizione
Comet è un simulatore interattivo di sistema solare scritto in C++/Qt6. Mostra tre pianeti che orbitano attorno a un sole secondo le leggi della gravità newtoniana, con orbite ellittiche realistiche. L'utente può interagire con la simulazione tramite mouse e tastiera.

## Funzionalità principali
- Finestra Qt6 1024x768.
- Tre pianeti con orbite ellittiche e colori diversi.
- Sole posizionato in uno dei fuochi dell'ellisse.
- Simulazione fisica con costanti configurabili in `headers/solarsimconstants.h`.
- Possibilità di spostare il sole con il mouse (trascinamento).
- I pianeti ripartono sempre in orbita rispetto alla nuova posizione del sole.
- La simulazione parte/si ferma premendo la barra spaziatrice.
- Premendo il tasto R la simulazione viene resettata alle condizioni iniziali.
- Premendo ESC o Q si esce dal programma.
- Pulsanti per aggiungere/rimuovere pianeti durante la simulazione.
- Rotazione della vista con mouse destro.

## Comandi da tastiera
- **Spazio**: avvia/metti in pausa la simulazione.
- **R**: resetta la simulazione (sole e pianeti tornano alle condizioni iniziali).
- **ESC** o **Q**: esci dal programma.

## Interazione con il mouse
- **Click sinistro sul sole**: trascina il sole per riposizionarlo. I pianeti si riadattano automaticamente all'orbita.
- **Click destro e trascinamento**: ruota la vista 3D.
- **Rotella del mouse**: zoom avanti/indietro.

## Controlli UI
- **Add Planet**: aggiunge un nuovo pianeta in una posizione casuale con velocità iniziale.
- **Remove Planet**: rimuove l'ultimo pianeta aggiunto (sicuro anche se la lista è vuota).

## Dipendenze
- Qt6 (Core, Widgets, Gui)
- Catch2 (per unit testing)
- CMake >= 3.16
- C++17 o superiore

### Installazione dipendenze (Ubuntu/Debian)
```sh
sudo apt install qt6-base-dev catch2
```

### Installazione dipendenze (macOS)
```sh
brew install qt6 catch2
```

## Build
```sh
mkdir build
cd build
cmake ..
cmake --build .
```

### Esecuzione dell'applicazione
```sh
./comet
```

### Esecuzione dei test
```sh
./comet_tests
```

O tramite CTest:
```sh
ctest --output-on-failure
```

Per output dettagliato dei test:
```sh
./comet_tests --reporters compact
```

## Note
- Tutte le costanti fisiche e di simulazione sono in `headers/solarsimconstants.h`.
- Puoi modificare facilmente masse, raggi, costante gravitazionale, dimensioni finestra, ecc.
- La simulazione è pensata per essere didattica e facilmente estendibile.

## Test Suite
Il progetto include una suite di test Catch2 con **56 test unitari** che coprono:

### Categorie di test
1. **Constructor tests** (5 test) - Verifica corretta inizializzazione
2. **Position tests** (10 test) - Calcoli e gestione coordinate
3. **Velocity tests** (10 test) - Velocità e componenti vettoriali
4. **Mass tests** (7 test) - Massa di diversi ordini di grandezza
5. **Radius tests** (6 test) - Raggio e scale diverse
6. **Color tests** (7 test) - Gestione colori
7. **Constants tests** (7 test) - Validazione costanti di simulazione
8. **Physics tests** (4 test) - Calcoli fisici (distanza, velocità orbitale)

### Bug fixes e miglioramenti
- ✅ Risolto segmentation fault causato da propagazione circolare di eventi tastiera
- ✅ Aggiunto controllo sicurezza per rimozione pianeti da lista vuota
- ✅ Abilitato macro Q_OBJECT in tutte le classi Qt
- ✅ Aggiunto parent widget per corretta gestione memoria Qt
- ✅ Inizializzazione random seed per generazione pianeti casuali

## Struttura del progetto
```
comet/
├── CMakeLists.txt                 # Configurazione build
├── README.md                       # Questo file
├── main.cpp                        # Entry point applicazione
├── headers/                        # File header
│   ├── astronomicalbody.h
│   ├── solarsystem.h
│   ├── solarsystemcontroller.h
│   ├── planetcontrolwidget.h
│   └── solarsimconstants.h        # Costanti fisiche
├── sources/                        # Implementazione
│   ├── astronomicalbody.cpp
│   ├── solarsystem.cpp
│   ├── solarsystemcontroller.cpp
│   └── planetcontrolwidget.cpp
├── tests/                          # Test suite Catch2
│   └── test_main.cpp              # 56 unit test
└── build/                          # Directory build (generata da CMake)
```
