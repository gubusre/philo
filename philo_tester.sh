#!/usr/bin/env bash
# philo_tester.sh
# Uso: ./philo_tester.sh ./philo
# Requiere: timeout, awk, grep, sed, printf

PHILO_BIN="${1:-./philo}"
TIMEOUT_CMD="${TIMEOUT_CMD:-timeout}"   # si tu sistema usa timeout distinto, ajusta
TMPDIR=$(mktemp -d /tmp/philo_test.XXXXXX)
LOG="$TMPDIR/out.log"

cleanup() {
    rm -rf "$TMPDIR"
}
trap cleanup EXIT

# Ejecuta un caso y guarda salida en $LOG
# args: <label> <timeout_seconds> <philo args...> <expectation>
# expectation: die | full
run_case() {
    local label="$1"; shift
    local to="$1"; shift
    local expectation="${@: -1}"   # último argumento
    local args=("${@:1:$#-1}")     # todos menos el último
    echo "=== Test: $label ==="
    echo "Command: $PHILO_BIN ${args[*]}"
    echo "Timeout: ${to}s  Expect: $expectation"
    echo

    # Ejecutar con timeout y capturar salida
    $TIMEOUT_CMD "${to}"s "$PHILO_BIN" "${args[@]}" > "$LOG" 2>&1
    local rc=$?
    if [ $rc -eq 124 ] || [ $rc -eq 137 ]; then
        echo "Result: TIMEOUT (exit $rc)"
    else
        echo "Result: exit $rc"
    fi

    # Mostrar salida breve (primeras 30 y últimas 10 líneas)
    echo "---- stdout (head 30) ----"
    head -n 30 "$LOG"
    echo "---- stdout (tail 10) ----"
    tail -n 10 "$LOG"
    echo "--------------------------"

    # Comprobaciones
    if grep -q "died" "$LOG"; then
        echo "Detected: a philosopher died (found 'died' in output)."
        died=1
    else
        died=0
    fi

    if [ "$expectation" = "die" ]; then
        if [ $died -eq 1 ]; then
            echo "OK: expected a death and one occurred."
        else
            echo "FAIL: expected a death but none detected."
        fi
    elif [ "$expectation" = "full" ]; then
        if [ $died -eq 1 ]; then
            echo "FAIL: expected all philosophers to finish eating, but a death occurred."
        else
            # extra check: count "is eating" per philosopher and compare with must_eat
            local must_eat="${args[-1]}"
            if [ -z "$must_eat" ] || [ "$must_eat" -le 0 ]; then
                echo "WARN: no must_eat provided; cannot assert fullness reliably."
            else
                # parse lines like: "<time> <id> is eating"
                awk '/is eating/ { print $2 }' "$LOG" | sort -n | uniq -c > "$TMPDIR/eat_counts.txt"
                echo "Eat counts (per id):"
                cat "$TMPDIR/eat_counts.txt"
                # check each philosopher has at least must_eat occurrences
                local ok=1
                for ((i=1;i<=${args[0]};i++)); do
                    # find count for id i
                    cnt=$(awk -v id="$i" '$2==id {print $1}' "$TMPDIR/eat_counts.txt" | tr -d '\n')
                    if [ -z "$cnt" ]; then cnt=0; fi
                    if [ "$cnt" -lt "$must_eat" ]; then
                        echo "FAIL: philosopher $i ate $cnt times (< $must_eat)."
                        ok=0
                    fi
                done
                if [ $ok -eq 1 ]; then
                    echo "OK: all philosophers ate at least $must_eat times and no death detected."
                fi
            fi
        fi
    else
        echo "Unknown expectation: $expectation"
    fi

    echo
    sleep 1
}

# Lista de tests recomendados (ajusta timeouts si tu máquina es lenta)
# 1) Caso 1 filósofo -> debe morir (toma un tenedor y muere)
run_case "one_philo_die" 5 1 800 200 200 die

# 2) Caso con tiempo de vida pequeño -> muerte esperada
run_case "small_tdie_die" 5 5 5 60 30 30 die

# 3) Caso con must_eat -> todos deben terminar sin muerte
# 5 filósofos, t_die amplio, cada uno debe comer 3 veces
run_case "five_philo_full" 20 5 800 200 200 3 full

# 4) Caso con must_eat y tiempos ajustados (prueba de concurrencia)
run_case "ten_philo_full" 40 10 1000 200 200 5 full

# 5) Stress corto: muchos filósofos, pocos ciclos (solo para ver que no deadlock)
run_case "many_philos_short" 30 50 1000 200 200 2 full

echo "All tests finished. Logs in $TMPDIR (kept until script exit)."
