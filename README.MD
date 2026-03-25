# CT30A3370 - Operating Systems and System Programming
Han Nguyen | Han.Nguyen@student.lut.fi

---

# Project 1: Warmup to C and Unix Programming (reverse)

## Compile
gcc -o reverse reverse.c

## Test cases
# stdin mode
echo -e "hello\nthis\nis\na file" | ./reverse

# file to stdout
./reverse input.txt

# file to file
./reverse input.txt output.txt
cat output.txt

# Error cases
./reverse input.txt input.txt
./reverse a b c
./reverse none.txt

---

# Project 2: Unix Utilities

## my-cat
gcc -o my-cat my-cat.c
./my-cat test.txt
./my-cat test.txt test.txt
./my-cat
./my-cat notfound.txt

## my-grep
gcc -o my-grep my-grep.c
echo -e "LUT University\nC Programming\nOperating Systems" > test.txt
./my-grep University test.txt
./my-grep University test.txt test.txt
echo -e "LUT University\nC Programming" | ./my-grep University
./my-grep university test.txt
./my-grep
./my-grep University notfound.txt

## my-zip / my-unzip
gcc -o my-zip my-zip.c
gcc -o my-unzip my-unzip.c
echo -n "LLLLUUUTTT" > test.txt
./my-zip test.txt > file1.z
./my-unzip file1.z
./my-zip test.txt test.txt > file2.z
./my-unzip file2.z
./my-zip
./my-unzip

---

# Project 3: Unix Shell (wish)

## Compile
gcc -o wish wish.c

## Test cases
# Interactive mode
./wish
ls
ls -la
cd /tmp
pwd
path /bin /usr/bin
exit

# Batch mode
echo -e "ls\npwd\nexit" > batch.txt
./wish batch.txt

# Redirection
./wish
ls > output.txt
cat output.txt
exit

# Parallel
./wish
ls & pwd
exit

# Error cases
./wish a.txt b.txt
./wish
invalidcommand
exit