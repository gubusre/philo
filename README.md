*Este proyecto ha sido creado como parte del currículo de 42 por gubusque.*

---

## Descripción

**Philosophers** es una implementación del clásico problema de sincronización conocido como **El problema de la cena de los filósofos**, propuesto originalmente por Edsger Dijkstra en 1965.

N filósofos se sientan alrededor de una mesa circular. Entre cada par de filósofos hay un tenedor. Para comer, un filósofo necesita coger los dos tenedores que tiene a su lado (izquierdo y derecho). El reto es coordinar el acceso a los tenedores de forma que:

- Ningún filósofo muera de hambre.
- No haya deadlock (bloqueo circular donde todos esperan indefinidamente).
- No haya data races (accesos concurrentes sin protección a memoria compartida).

La implementación usa **hilos POSIX** (`pthread`) y **mutexes** para gestionar la concurrencia. Cada filósofo es un hilo independiente, cada tenedor es un mutex, y un hilo monitor supervisa el estado de la simulación.

### Comportamiento

Cada filósofo cicla entre tres estados:

1. **Pensar** — espera a que los tenedores estén disponibles.
2. **Comer** — mantiene ambos tenedores durante `time_to_eat` ms.
3. **Dormir** — suelta los tenedores y duerme durante `time_to_sleep` ms.

La simulación termina cuando un filósofo muere (supera `time_to_die` ms sin comer) o cuando todos han comido el número mínimo de veces requerido.

---

## Instrucciones

### Compilación

```bash
make
```

Genera el binario `philo` en el directorio actual.

```bash
make clean    # elimina los archivos objeto (.o)
make fclean   # elimina los .o y el binario
make re       # recompila desde cero
```

### Ejecución

```bash
./philo <n_philosophers> <time_to_die> <time_to_eat> <time_to_sleep> [times_must_eat]
```

| Argumento | Descripción |
|---|---|
| `n_philosophers` | Número de filósofos (y de tenedores) |
| `time_to_die` | Tiempo en ms sin comer antes de morir |
| `time_to_eat` | Tiempo en ms que tarda en comer |
| `time_to_sleep` | Tiempo en ms que duerme tras comer |
| `times_must_eat` | (Opcional) La simulación para cuando todos han comido este número de veces |

### Formato de salida

```
<timestamp_ms> <philosopher_id> <action>
```

Acciones posibles: `has taken a fork`, `is eating`, `is sleeping`, `is thinking`, `died`.

---

## Decisiones técnicas

### Un solo mutex para `simulation_stop`

Todas las lecturas y escrituras de `simulation_stop` están protegidas exclusivamente por `write_mutex`. Esto garantiza que la impresión del mensaje `died` y la activación del flag ocurren en la misma sección crítica, haciendo imposible que un filósofo imprima un mensaje después de su propia muerte.

### Sincronización de `last_meal_time` con `start_time`

Ambos valores se fijan en el mismo instante, justo antes de lanzar los hilos en `init_simulation`. Esto evita que el monitor calcule un delta de hambre inflado por el tiempo de inicialización.

### Orden de adquisición de tenedores por puntero

Para evitar deadlock, cada filósofo adquiere siempre primero el tenedor con la dirección de memoria más baja. Esto rompe la condición de espera circular del deadlock sin necesitar `pthread_mutex_trylock`.

### Staggering par/impar

Los filósofos con ID impar esperan `time_to_eat` ms antes de comenzar su primer ciclo. Esto desincroniza filósofos adyacentes desde el inicio, reduciendo la contención sobre los tenedores.

---

## Recursos

### Documentación oficial

- [POSIX Threads Programming — Lawrence Livermore National Laboratory](https://hpc-tutorials.llnl.gov/posix/)
- [pthread_mutex_lock — Linux man page](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html)
- [gettimeofday — Linux man page](https://man7.org/linux/man-pages/man2/gettimeofday.2.html)

### Artículos y referencias

- Dijkstra, E. W. (1971). *Hierarchical ordering of sequential processes.* — Artículo original donde se describe el problema.
- [The Dining Philosophers Problem — Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Deadlock — Wikipedia](https://en.wikipedia.org/wiki/Deadlock)

### Uso de IA

Durante el desarrollo se utilizaron dos herramientas de IA como apoyo: **GitHub Copilot** y **Claude (Anthropic)**. La IA no ha escrito el código final, pero sí ha servido como herramienta de apoyo para depurar comportamientos no deterministas, mejorar la claridad del diseño, validar estrategias de sincronización y generar casos de prueba repetitivos y de estrés.

**GitHub Copilot** se utilizó para:

- Explicación de conceptos de concurrencia y sincronización.
- Análisis de condiciones de carrera y diseño de mutexes.
- Revisión de funciones críticas como `print_action`, `simulation_should_stop` y `smart_sleep`.
- Generación de testers automáticos para validar estabilidad y detectar muertes inconsistentes.
- Redacción de documentación y estructuración del README.

**Claude (Anthropic)** se utilizó para:

- Revisión y análisis de código: identificación de race conditions, data races y problemas de sincronización.
- Depuración de concurrencia: análisis del problema del `last_meal_time` inicializado antes que `start_time`, y del uso inconsistente de mutexes para proteger `simulation_stop`.
- Discusión de decisiones técnicas: evaluación de alternativas como `pthread_mutex_trylock` vs. `pthread_mutex_lock` bloqueante con orden de adquisición por puntero.
- Generación de este README.
