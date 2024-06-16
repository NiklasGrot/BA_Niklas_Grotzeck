# Realisierung eines Audio-Plugins zur Simulation von Raumimpulsantworten

Dieses Repository enthält Code der im Rahmen der Bachelorarbeit von Niklas Grotzeck geschrieben wurde. Der Code wurde eigenständig ohne externe Hilfe verfasst. Alle Quellen wurden an den jeweiligen Stellen markiert.

## Plugin Code

Der Code für das Plugin befindet sich im [Plugin](Plugin) Ordner. Dieses wurde mit Hilfe des [JUCE-Frameworks](https://juce.com/) entwickelt. Ein Großteil der Dateien beinhalten den Code des Frameworks. Die für die Arbeit relevanten und selbst geschriebenen Dateien befinden sich im [Source](Plugin/IR_Convolution/Source/) Ordner. 

- [`IrSim.cpp`](Plugin/IR_Convolution/Source/IrSim.cpp) beinhaltet den Code zur Berechnung der Impulsantwort
- [`PluginEditor.cpp`](Plugin/IR_Convolution/Source/PluginEditor.cpp) beinhaltet das Userinterface
- [`PluginProcessor.cpp`](Plugin/IR_Convolution/Source/PluginProcessor.cpp) beinhaltet die Convolution Engine und die Verarbeitung des eingehenden Audio-Signals

Um den Code zu kompilieren wird die Projucer Anwendung auf MacOS und XCode benötigt. 

In [Windows-Anwendungen](Plugin/Windows_Anwendungen) befinden sich das Plugin und eine Standalone Variante für Windows. In [MacOS-Anwendungen](Plugin/MacOS_Anwendungen) befinden sich diese für MacOS. 

## Python Code

[`Impulse_Response_Sim.py`](Impulse_Response_Sim.py) enthält den gesamten Python Code. Um diesen Auszuführen müssen die benötigten Bibliotheken heruntergeladen werden. Mit Hilfe von pip kann der Befehl `pip install -r requirements.txt` ausgeführt werden um diese zu installieren.   
Um die Parameter eines Raumes selbst zu definieren kann der Code individuell angepasst werden.

## Sonstiges

In [Simulations_Ergebnisse](Simulations_Ergebnisse) finden sich außerdem Ergebnisse der Simulation aus EASE. 
In [Hörvergleich](Hörvergleich) befinden sich die Audio-Dateien des Hörvergleichs. Jeweils das trockene Signal und die mit den Impulsantworten gefalteten Signale.






