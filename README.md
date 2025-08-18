
# Comet - Simulatore di Sistema Solare con Qt6

## Descrizione
Comet è un simulatore interattivo di sistema solare scritto in C++/Qt6. Mostra tre pianeti che orbitano attorno a un sole secondo le leggi della gravità newtoniana, con orbite ellittiche realistiche. L'utente può interagire con la simulazione tramite mouse e tastiera.

## Funzionalità principali
- Finestra Qt6 1600x1000.
- Tre pianeti con orbite ellittiche e colori diversi.
- Sole posizionato in uno dei fuochi dell'ellisse.
- Simulazione fisica con costanti configurabili in `headers/solarsimconstants.h`.
- Possibilità di spostare il sole con il mouse (trascinamento).
- I pianeti ripartono sempre in orbita rispetto alla nuova posizione del sole.
- La simulazione parte/si ferma premendo la barra spaziatrice.
- Premendo il tasto R la simulazione viene resettata alle condizioni iniziali.
- Premendo ESC o Q si esce dal programma.

## Comandi da tastiera
- **Spazio**: avvia/metti in pausa la simulazione.
- **R**: resetta la simulazione (sole e pianeti tornano alle condizioni iniziali).
- **ESC** o **Q**: esci dal programma.

## Interazione con il mouse
- **Trascina il sole**: clicca sul sole e trascina per riposizionarlo. I pianeti si riadattano automaticamente.

## Struttura del progetto
- `sources/` - file sorgente C++
- `headers/` - header, incluse tutte le costanti di simulazione
- `.github/copilot-instructions.md` - istruzioni per Copilot
- `.gitignore` - ignora la directory build
- `CMakeLists.txt` - configurazione CMake

## Dipendenze
- Qt6 (Core, Widgets, Gui)
- CMake >= 3.14

## Build
```sh
mkdir build
cd build
cmake ..
make
./comet
```

## Note
- Tutte le costanti fisiche e di simulazione sono in `headers/solarsimconstants.h`.
- Puoi modificare facilmente masse, raggi, costante gravitazionale, dimensioni finestra, ecc.
- La simulazione è pensata per essere didattica e facilmente estendibile.
