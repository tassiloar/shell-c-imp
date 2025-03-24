# shell-c-imp

A basic shell written in C that supports:

- Running commands
- Piping (`|`) between commands
- Exiting with `exit`

---

## Compile

```bash
gcc -o jsh shell.c
```

---

## Run

```bash
./jsh
```

---

## Example

```bash
jsh$ ls | grep txt | wc -l
```
