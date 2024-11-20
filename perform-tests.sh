#! /bin/bash

# Zaustavljanje izvršavanja skripte u slučaju greške
set -e

# Prevođenje programa
g++ -o dp_algorithm main.cpp

# Provera rezultata prevođenja
if [ $? -ne 0 ]; then
  echo "Kompajliranje nije uspelo!"
  exit 1
fi

# Folder sa test primerima
input_dir="test-cases-in"
output_dir="test-cases-out"

# Kreiraj direktorijum ukoliko vec ne postoji
if [ ! -d "$output_dir" ]; then
  mkdir -p "$output_dir"
fi

# Pokretanje DP algoritma za svaki test primer
for i in {1..10}; do
  input_file="${input_dir}/test${i}-in.txt"
  output_file="${output_dir}/test${i}-out.txt"

  # Provera postojanja ulazne datoteke
  if [ -f "$input_file" ]; then
    # Prosleđivanje sadržaja ulazne datoteke na ulaz programu
    # i upisivanje izlaza u izlaznu datoteku
    ./dp_algorithm < "$input_file" > "$output_file"
  fi
done

# Obrisi izvrsnu datoteku
rm dp_algorithm

echo "Testiranje je završeno. Rezultati su smešteni u folderu $output_dir."
