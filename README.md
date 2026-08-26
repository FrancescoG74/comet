
# Comet - Simulatore di Sistema Solare con Qt6

## Descrizione
Comet è un visualizzatore interattivo del sistema solare scritto in C++/Qt6. Le posizioni del Sole, degli 8 pianeti (Mercurio-Nettuno), della Luna terrestre e delle 4 lune galileiane di Giove sono calcolate con dati di efemeridi reali tramite [Astronomy Engine](https://github.com/cosinekitty/astronomy) (libreria C, licenza MIT, di Don Cross), per la data/ora corrente o accelerata nel tempo. L'utente può interagire con la simulazione tramite mouse e tastiera.

## Motore astronomico
- Le posizioni di Sole/pianeti/Luna/lune di Giove **non** sono calcolate con un integratore N-body: derivano dai modelli analitici (VSOP87 troncato) di Astronomy Engine, vendorizzata in `third_party/astronomy/` (`astronomy.c`/`astronomy.h`, MIT license, vedi `third_party/astronomy/LICENSE`).
- Le 7 lune principali di Saturno **non** sono modellate da Astronomy Engine: la loro posizione è stilizzata con un'orbita circolare basata sul loro periodo orbitale reale.
- La simulazione parte dalla data/ora reale corrente all'avvio (o al reset) e avanza nel tempo in base allo slider di velocità (da 0.1 a 20 giorni simulati al secondo).

## Funzionalità principali
- Finestra Qt6 ridimensionabile (dimensione iniziale 1024x768, dimensione minima dimezzata).
- 8 pianeti (Mercurio, Venere, Terra, Marte, Giove, Saturno, Urano, Nettuno) con posizioni reali per la data/ora simulata e colori diversi.
- Satelliti: Luna (Terra) e 4 lune galileiane (Giove) con posizioni reali; 7 lune di Saturno con orbita circolare stilizzata.
- Sole al centro della vista.
- La simulazione parte/si ferma premendo la barra spaziatrice o il pulsante Start/Stop.
- Premendo il tasto R la simulazione viene resettata alla data/ora corrente.
- Premendo ESC o Q, oppure con il pulsante Quit, si esce dal programma.
- Slider per regolare la velocità della simulazione (da 0.1x a 20x, cioè da 0.1 a 20 giorni simulati per secondo reale).
- Etichetta con la data/ora simulata corrente (UTC).
- Rotazione della vista con il tasto destro del mouse, panning con il tasto sinistro e zoom con la rotella.

## Comandi da tastiera
- **Spazio**: avvia/metti in pausa la simulazione.
- **R**: resetta la simulazione alla data/ora corrente.
- **V**: reimposta la vista (zoom/rotazione/pan) ai valori iniziali.
- **ESC** o **Q**: esci dal programma.

## Interazione con il mouse
- **Click sinistro e trascinamento**: pan della camera nello spazio schermo.
- **Click destro e trascinamento**: ruota la vista 3D.
- **Rotella del mouse**: zoom avanti/indietro.

## Controlli UI
- **Start/Stop**: avvia/mette in pausa la simulazione.
- **Quit**: chiude l'applicazione.
- **Slider velocità**: regola il moltiplicatore di velocità della simulazione (0.1x - 20x).
- **Etichetta data/ora**: mostra la data/ora simulata corrente (UTC), calcolata da Astronomy Engine.

## Dipendenze
- Qt6 (Core, Widgets, Gui)
- Catch2 (per unit testing)
- CMake >= 3.16
- C++17 o superiore
- Astronomy Engine (vendorizzata in `third_party/astronomy/`, nessuna installazione richiesta)

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
- Le costanti di visualizzazione (dimensioni finestra, scala display, ecc.) sono in `headers/simulation/solarsimconstants.h`. Alcune costanti (`G`, `TIME_STEP`, `STEPS_PER_FRAME`) sono mantenute per compatibilità ma non guidano più la fisica, che ora deriva da Astronomy Engine.
- Le masse/raggi relativi a Terra sono usati solo a scopo di visualizzazione (scala dei pianeti), non per calcoli gravitazionali.
- Non è più possibile aggiungere pianeti arbitrari o spostare il Sole per alterare l'orbita: essendo posizioni reali, la simulazione mostra solo i corpi effettivamente presenti nel sistema solare.

## Test Suite
Il progetto include una suite di test Catch2 con **93 test unitari**, organizzati per dominio:
- `tests/core/test_astronomicalbody.cpp` - Costruttori, posizione, velocità, massa, raggio, colore, nome e parametri orbitali osculanti dei corpi celesti.
- `tests/simulation/test_physics.cpp` - Costanti di simulazione, calcoli fisici, dati del sistema solare, slider di velocità e tempo simulato.
- `tests/simulation/test_orbitalelements.cpp` - Verifica `computeOsculatingElements` (semiasse maggiore, eccentricità, direzione del periasse) contro casi Kepleriani noti (orbite circolari, al periasse/apoasse, ad anomalia vera arbitraria).

## Struttura del progetto
```
comet/
├── CMakeLists.txt                  # Configurazione build
├── README.md                       # Questo file
├── third_party/
│   └── astronomy/                  # Astronomy Engine vendorizzata (MIT license)
│       ├── astronomy.c
│       ├── astronomy.h
│       └── LICENSE
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
│   │   ├── solarsimconstants.h    # Costanti di visualizzazione
│   │   ├── solarsystem.h
│   │   ├── solarsystemcontroller.h
│   │   └── orbitalelements.h     # Calcolo elementi orbitali osculanti (testabile)
│   └── ui/
│       └── planetcontrolwidget.h
├── tests/                          # Test suite Catch2 (93 test)
│   ├── test_main.cpp
│   ├── core/
│   │   └── test_astronomicalbody.cpp
│   └── simulation/
│       ├── test_physics.cpp
│       └── test_orbitalelements.cpp
└── build/                          # Directory build (generata da CMake)
```
