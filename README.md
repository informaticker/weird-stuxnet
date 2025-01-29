This is a weird project that I have found on an old Sun Netra T1. 

The files in this repo were last changed on 30 September 2011 08∶56∶51, which makes this a bit more interesting.

## From description.txt
```
Genetic evolution of malware


Vision:

Todays anti-malware solutions rely on signatures, binaries and network traffic to detect malware.
Our approach is to analyze the behavior of the suspicious programs. This enables us to detect previously unknown malware and is a step toward immune systems for computers.
Due to the growing demand for computer defense and security systems, immune systems are an active field of research.
This approach is a preemptive measure against future threats and will give us an advantage over attackers.


Work done so far:

Design and implementation of:
- a grammar-based bash command generator.
- the neural architecture that constitutes the brain of the evolving “individuals”. It drives the command generator and allows the development of strategies and the usage of both immediate and long-term memory.
- an interface that allows commands to be executed on the system.
- a distributed genetic algorithm. Every individual is an independent program and there is no global control whatsoever. Evolution looks for new behaviors by using a combination of novelty search and normalized compression distance. The selection processes is environment-driven.


Next steps:

- Enhance neural architecture and genetic interactions to be able to mate with members of other populations.
- Add a mechanism for humans to introduce artificial selection pressures to drive the evolution toward a specific goal.
- Allow individuals to move across the network and meet other populations.
- Introduce defense and fight mechanisms.
- ...


The project is carried out by Fabian Schläpfer and Adrien de Castelnau. A great help was also provided by John Doucette from Dalhousie university, Canada.
```

