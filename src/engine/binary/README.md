## Quick Start with [Stockfish](https://stockfishchess.org) 🐟

> [!NOTE]  
> Please note that the engine binary is **not distributed** with this repository and must be obtained and installed manually.

> [!WARNING]
> You do not need to download the full source code for the engine just you need the binary.

> [!TIP]  
> If you wish to use another engine, we recommend to verfy that it supports FEN (Forsyth-Edwards Notation) not PGN.

---

### 1. Installation (Ubuntu) ⚙️

```bash
# Navigate to the Downloads directory
cd ~/Downloads

# Download the latest Stockfish build for Ubuntu (AVX2)
wget https://github.com/official-stockfish/Stockfish/releases/latest/download/stockfish-ubuntu-x86-64-avx2.tar

# Extract the archive
tar -xf stockfish-ubuntu-x86-64-avx2.tar

# Copy the binary into your cloned repository
cp stockfish-ubuntu-x86-64-avx2/stockfish/stockfish-ubuntu-x86-64-avx2 \
   path/to/your/cloned/repository/src/engine/binary/
```

The binary will be located at:

```
path/to/your/cloned/repository/src/engine/binary/stockfish-ubuntu-x86-64-avx2
```

---

### 2. Verification 🔍

Verify the engine is functional:

```bash
cd path/to/your/cloned/repository/src/engine/binary
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

### 3. Licensing 📜

* see the Stockfish license: [GNU GPL v3](https://github.com/official-stockfish/Stockfish/blob/master/Copying.txt).
