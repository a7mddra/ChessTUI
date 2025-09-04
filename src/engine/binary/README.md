# Stockfish Integration Guide

This project depends on the [Stockfish](https://stockfishchess.org) chess engine.  
The engine binary is **not distributed** with this repository and must be obtained and installed manually.

---

## 1. Installation (Ubuntu)

Run the following commands in a terminal:

```bash
# Navigate to the Downloads directory
cd ~/Downloads

# Download the latest Stockfish build for Ubuntu (AVX2)
wget https://github.com/official-stockfish/Stockfish/releases/latest/download/stockfish-ubuntu-x86-64-avx2.tar

# Extract the archive
tar -xf stockfish-ubuntu-x86-64-avx2.tar

# Create project engine folder (if not present) and copy the binary into the project
mkdir -p ~/ChessTUI/src/engine/binary
cp stockfish-ubuntu-x86-64-avx2/stockfish/stockfish-ubuntu-x86-64-avx2 \
   ~/ChessTUI/src/engine/binary/
````

The binary will be located at:

```
~/ChessTUI/src/engine/binary/stockfish-ubuntu-x86-64-avx2
```

---

## 2. Verification

Verify the engine is functional:

```bash
cd ~/ChessTUI/src/engine/binary
# Ensure the binary is executable
chmod +x stockfish-ubuntu-x86-64-avx2

# Run the engine
./stockfish-ubuntu-x86-64-avx2
```

At the Stockfish prompt, issue these UCI commands:

```text
uci
uciok
isready
readyok
position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
go
```

If the engine responds with `uciok`, `readyok`, and outputs a move after `go`, the installation is correct.

---

## 3. Notes & Licensing

* The Stockfish binary is **excluded from version control** via `.gitignore`. Do **not** commit engine binaries to the repository.
* Only the binary is required to run this project; the Stockfish source code is not necessary.
* For full license terms, see the Stockfish license: [GNU GPL v3](https://github.com/official-stockfish/Stockfish/blob/master/Copying.txt).
