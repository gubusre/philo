#!/usr/bin/env bash
# philo_tester.sh (ampliado)
# Uso: ./philo_tester.sh ./philo
# Requiere: timeout, awk, grep, sed, printf, optionally valgrind

PHILO_BIN="${1:-./philo}"
TIMEOUT_CMD="${TIMEOUT_CMD:-timeout}"
TMPDIR=$(mktemp -d /tmp/philo_test.XXXXXX)
LOG="$TMPDIR/out.log"
VERBOSE=${VERBOSE:-0}   # 1 para más salida de debug
VALGRIND=${VALGRIND:-0} # 1 para activar valgrind en tests de memcheck
REPEAT_DEFAULT=30

cleanup() {
    rm -rf "$TMPDIR"
}
trap cleanup EXIT

# Ejecuta un caso y guarda salida en $LOG
# args: <label> <timeout_seconds> <philo args...> <expectation>
# expectation: die | full | ok | invalid
run_case() {
    local label="$1"; shift
    local to="$1"; shift
    local expectation="${@: -1}"
    local args=("${@:1:$#-1}")
    echo "=== Test: $label ==="
    echo "Command: $PHILO_BIN ${args[*]}"
    echo "Timeout: ${to}s  Expect: $expectation"
    echo

    if [ "$VALGRIND" -eq 1 ] && [ "$expectation" != "invalid" ]; then
        valgrind --leak-check=full --error-exitcode=1 $PHILO_BIN "${args[@]}" > "$LOG" 2>&1
        rc=$?
    else
        $TIMEOUT_CMD "${to}"s "$PHILO_BIN" "${args[@]}" > "$LOG" 2>&1
        rc=$?
    fi

    if [ $rc -eq 124 ] || [ $rc -eq 137 ]; then
        echo "Result: TIMEOUT (exit $rc)"
    else
        echo "Result: exit $rc"
    fi

    if [ "$VERBOSE" -eq 1 ]; then
        echo "---- stdout (head 60) ----"
        head -n 60 "$LOG"
        echo "---- stdout (tail 30) ----"
        tail -n 30 "$LOG"
    else
        echo "---- stdout (head 20) ----"
        head -n 20 "$LOG"
        echo "---- stdout (tail 10) ----"
        tail -n 10 "$LOG"
    fi
    echo "--------------------------"

    # Detección de muerte y otras comprobaciones
    if grep -q "died" "$LOG"; then
        died=1
    else
        died=0
    fi

    case "$expectation" in
        die)
            if [ $died -eq 1 ]; then
                echo "OK: expected a death and one occurred."
            else
                echo "FAIL: expected a death but none detected."
            fi
            ;;
        full)
            if [ $died -eq 1 ]; then
                echo "FAIL: expected all philosophers to finish eating, but a death occurred."
            else
                local must_eat="${args[-1]}"
                if [ -z "$must_eat" ] || [ "$must_eat" -le 0 ]; then
                    echo "WARN: no must_eat provided; cannot assert fullness reliably."
                else
                    awk '/is eating/ { print $2 }' "$LOG" | sort -n | uniq -c > "$TMPDIR/eat_counts.txt"
                    echo "Eat counts (per id):"
                    cat "$TMPDIR/eat_counts.txt"
                    local ok=1
                    for ((i=1;i<=${args[0]};i++)); do
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
            ;;
        ok)
            if [ $died -eq 1 ]; then
                echo "FAIL: unexpected death."
            else
                echo "OK: no death detected (basic check)."
            fi
            ;;
        invalid)
            # Expect program to exit with error or print usage
            if grep -qiE "usage|error|invalid|wrong" "$LOG" || [ $rc -ne 0 ]; then
                echo "OK: invalid args handled (usage/error printed or non-zero exit)."
            else
                echo "WARN: invalid args may not be handled properly."
            fi
            ;;
        *)
            echo "Unknown expectation: $expectation"
            ;;
    esac

    echo
    sleep 1
}

# Ejecuta un caso N veces y muestra estadísticas de muertes
# args: <label> <repetitions> <timeout> <philo args...> <expectation>
run_repeat() {
    local label="$1"; shift
    local reps="$1"; shift
    local to="$1"; shift
    local expectation="${@: -1}"
    local args=("${@:1:$#-1}")
    echo "=== Repeat Test: $label (reps=$reps) ==="
    local count=0
    for i in $(seq 1 $reps); do
        $TIMEOUT_CMD "${to}"s "$PHILO_BIN" "${args[@]}" > "$TMPDIR/run_${i}.log" 2>&1
        if grep -q "died" "$TMPDIR/run_${i}.log"; then
            count=$((count+1))
        fi
    done
    echo "Deaths: $count / $reps"
    echo
}

# Ejecuta un caso bajo valgrind (memcheck)
run_memcheck() {
    local label="$1"; shift
    local to="$1"; shift
    local args=("$@")
    echo "=== Memcheck: $label ==="
    valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 $PHILO_BIN "${args[@]}" > "$LOG" 2>&1
    local rc=$?
    echo "Valgrind exit: $rc"
    echo "---- valgrind output (tail 40) ----"
    tail -n 40 "$LOG"
    echo
}

# -------------------------
# Test suite ampliada
# -------------------------

# A. Validación de argumentos y casos inválidos
run_case "no_args" 2 "" invalid
run_case "zero_philos" 2 0 800 200 200 invalid
run_case "negative_values" 2 -1 800 200 200 invalid
run_case "non_numeric" 2 a b c d invalid

# B. Casos básicos y límites (1 filósofo)
run_case "one_philo_die_small" 5 1 399 200 200 die
run_case "one_philo_survive_large" 5 1 2000 200 200 die
run_case "one_philo_edge" 5 1 1010 200 200 die

# C. Casos con must_eat (full)
run_case "five_philo_musteat3" 20 5 800 200 200 3 full
run_case "five_philo_musteat1" 10 5 800 200 200 1 full

# D. Contención y deadlock potencial
run_case "two_philo_short_die" 10 2 300 200 200 die
run_case "two_philo_long" 10 2 1000 200 200 ok

# E. Escalado y estrés
run_case "ten_philo_short" 30 10 800 200 200 3 full
run_case "twenty_philo_stress" 60 20 1000 200 200 2 full
run_case "fifty_philo_stress" 120 50 2000 200 200 1 full

# F. Casos con tiempos extremos
run_case "tiny_times" 5 5 1 1 1 die
run_case "huge_times" 10 5 100000 100 100 ok

# G. Repeticiones estadísticas (mide fragilidad)
run_repeat "one_philo_399_repeat" 50 3 1 399 200 200 die
run_repeat "one_philo_800_repeat" 50 3 1 800 200 200 die

# H. Memcheck (valgrind) - solo si valgrind instalado y VALGRIND=1
if command -v valgrind >/dev/null 2>&1; then
    if [ "$VALGRIND" -eq 1 ]; then
        run_memcheck "five_philo_musteat3_mem" 30 5 800 200 200 3
    else
        echo "Valgrind available. To run memcheck set VALGRIND=1 and re-run the script."
    fi
else
    echo "Valgrind not found; skipping memcheck tests."
fi

# I. Tests de robustez de salida (asegura que no hay prints tras muerte)
# Ejecuta y comprueba si hay líneas después de la primera "died"
run_case "post_death_output_check" 10 5 800 200 200 ok
if grep -q "died" "$LOG"; then
    first_died_line=$(grep -n "died" "$LOG" | head -n1 | cut -d: -f1)
    total_lines=$(wc -l < "$LOG")
    lines_after=$((total_lines - first_died_line))
    echo "Lines after first died: $lines_after"
    if [ $lines_after -gt 0 ]; then
        echo "WARN: there are $lines_after lines after the first 'died' (may indicate threads printing after death)."
    else
        echo "OK: no output after death."
    fi
    echo
fi

# J. Test combinado: runs paralelas (simple, secuencial aquí)
echo "=== Parallel-ish stress (sequential runs) ==="
for i in 1 2 3; do
    run_case "stress_run_$i" 60 30 1000 200 200 2 full &
    sleep 1
done
wait

# -------------------------
# Repeticiones temporizadas: cuenta muertes en N ejecuciones
# -------------------------
# run_repeat_time <label> <reps> <timeout_seconds> <philo args...>
run_repeat_time() {
    local label="$1"; shift
    local reps="$1"; shift
    local to="$1"; shift
    local args=("$@")
    echo "=== Timed Repeat Test: $label (reps=$reps timeout=${to}s) ==="
    local deaths=0
    local runs=0
    for i in $(seq 1 $reps); do
        runs=$((runs+1))
        $TIMEOUT_CMD "${to}"s "$PHILO_BIN" "${args[@]}" > "$TMPDIR/run_${label}_${i}.log" 2>&1
        rc=$?
        if [ $rc -eq 124 ] || [ $rc -eq 137 ]; then
            echo "run $i: TIMEOUT (exit $rc)"
        fi
        if grep -q "died" "$TMPDIR/run_${label}_${i}.log"; then
            deaths=$((deaths+1))
            echo "run $i: died"
        else
            echo "run $i: no death"
        fi
    done
    echo "Summary: $label -> deaths: $deaths / $runs"
    echo
}

# Añadir los dos tests solicitados: 30 ejecuciones de 10s cada una
run_repeat_time "3philo_700_repeat" 10 10 3 800 200 200
run_repeat_time "5philo_700_repeat" 10 10 5 800 200 200
run_repeat_time "4philo_700_repeat" 10 10 4 800 200 200
run_repeat_time "100philo_700_repeat" 10 10 100 800 200 200

echo "All tests finished. Logs in $TMPDIR (kept until script exit)."
