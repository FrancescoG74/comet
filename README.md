
# Comet - Simulatore di Sistema Solare con Qt6

## Descrizione
Comet è un simulatore interattivo di sistema solare scritto in C++/Qt6. Mostra gli 8 pianeti (Mercurio-Nettuno) in orbita attorno al sole secondo le leggi della gravità newtoniana, con orbite ellittiche realistiche, oltre alla Luna terrestre, alle 4 lune galileiane di Giove e alle 7 lune principali di Saturno. L'utente può interagire con la simulazione tramite mouse e tastiera.

## Funzionalità principali
- Finestra Qt6 ridimensionabile (dimensione iniziale 1024x768, dimensione minima dimezzata).
- 8 pianeti (Mercurio, Venere, Terra, Marte, Giove, Saturno, Urano, Nettuno) con orbite ellittiche realistiche e colori diversi.
- Satelliti: Luna (Terra), 4 lune galileiane (Giove), 7 lune principali (Saturno).
- Sole posizionato in uno dei fuochi dell'ellisse.
- Simulazione fisica con costanti configurabili in `headers/simulation/solarsimconstants.h`.
- Possibilità di spostare il sole con il mouse (trascinamento); i pianeti si riadattano automaticamente alla nuova posizione.
- La simulazione parte/si ferma premendo la barra spaziatrice o il pulsante Start/Stop.
- Premendo il tasto R la simulazione viene resettata alle condizioni iniziali.
- Premendo ESC o Q, oppure con il pulsante Quit, si esce dal programma.
- Pulsanti per aggiungere/rimuovere pianeti durante la simulazione.
- Slider per regolare la velocità della simulazione (da 0.1x a 20x).
- Etichetta con il tempo simulato corrente.
- Rotazione della vista con il tasto destro del mouse e zoom con la rotella.

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
- **Start/Stop**: avvia/mette in pausa la simulazione.
- **Quit**: chiude l'applicazione.
- **Slider velocità**: regola il moltiplicatore di velocità della simulazione (0.1x - 20x).
- **Etichetta tempo**: mostra il tempo simulato corrente.

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
- Tutte le costanti fisiche e di simulazione sono in `headers/simulation/solarsimconstants.h`.
- Puoi modificare facilmente masse, raggi, costante gravitazionale, dimensioni finestra, ecc.
- La simulazione è pensata per essere didattica e facilmente estendibile.

## Test Suite
Il progetto include una suite di test Catch2 con **82 test unitari**, organizzati per dominio:
- `tests/core/test_astronomicalbody.cpp` - Costruttori, posizione, velocità, massa, raggio, colore e nome dei corpi celesti.
- `tests/simulation/test_physics.cpp` - Costanti di simulazione, calcoli fisici, dati del sistema solare, slider di velocità e tempo simulato.

## Struttura del progetto
```
comet/
├── CMakeLists.txt                  # Configurazione build
├── README.md                       # Questo file
├── src/
│   ├── main.cpp                    # Entry point applicazione
│   ├── core/
│   │   ├── astronomicalbody.cpp
│   │   └── sprite.cpp
│   ├── simulation/
│   │   ├── solarsystem.cpp
│   │   └── solarsystemcontroller.cpp
│   └── ui/
│       └── planetcontrolwidget.cpp
├── headers/
│   ├── core/
│   │   ├── astronomicalbody.h
│   │   └── sprite.h
│   ├── simulation/
│   │   ├── solarsimconstants.h    # Costanti fisiche
│   │   ├── solarsystem.h
│   │   └── solarsystemcontroller.h
│   └── ui/
│       └── planetcontrolwidget.h
├── tests/                          # Test suite Catch2 (82 test)
│   ├── test_main.cpp
│   ├── core/
│   │   └── test_astronomicalbody.cpp
│   └── simulation/
│       └── test_physics.cpp
└── build/                          # Directory build (generata da CMake)
```
