## Terminal Chess GUI for Stockfish

**A Linux terminal-based GUI for playing chess against Stockfish AI.**

This project provides a lightweight graphical interface for Stockfish, allowing players to enjoy a game of chess in the terminal.
It features real-time board updates, color-coded text, and seamless interaction between C and Python for enhanced gameplay.


![App Screenshot](lib/test.png)

## **Key Features**

1. **Dynamic Terminal Interface**  
   - Continuous screen refresh mechanism for a smooth and interactive display.
   - Color-coded pieces for better readability.

2. **Advanced Game Logic**  
   - All chess rules and special moves (e.g., castling, en passant, promotion) implemented in C.  
   - Real-time move validation and feedback.

3. **Stockfish Integration**  
   - Powered by the Stockfish UCI engine for AI moves.  
   - "Thinking..." animation during the AI's turn for immersive gameplay.

4. **Player vs. AI Mode**  
   - Toggle turns seamlessly between the player and Stockfish AI.  
   - AI responds with powerful moves that challenge players of all skill levels.

---

## **Prerequisites**

To compile this project, ensure the following are installed on your Linux system:

1. **Chess Library for Python**  
   Install via pip:  
   ```bash
   pip install chess
   ```

2. **Stockfish Engine**  
   Download the Stockfish binary from [Stockfish's official site](https://stockfishchess.org/download/).  
   Once downloaded, place it in the same directory where you cloned the repository. If you'd prefer to keep it elsewhere, make sure to update the path to the Stockfish binary in the AI.py file."


3. **C Compiler**  
   GCC is required to compile the C code:  
   ```bash
   sudo apt install gcc
   ```

---

## **How to Play**

1. **Clone the Repository**
   ```bash
   git clone https://github.com/a7mddra/ChessGUI.git
   cd ChessGUI
   ```

2. **Run Makefile**
   ```bash
   make
   ```

3. **Run the Game**
   ```bash
   ./chess_game
   ```

   - Mark your possible moves in Standard Algebraic Notation (e.g., `e2`).
   - Enter your move in Standard Algebraic Notation (e.g., `e4`) from the blue dots.
   - The AI will calculate and display its move.

---

## **Behind the Scenes**

### **Terminal Utilities**
The project features a custom raw input mode for real-time move entry and updates. The display is continuously refreshed to simulate a graphical chessboard.

### **Seamless C-Python Interaction**
C handles the chessboard logic and rules, while Python, with Stockfish UCI, generates AI moves. The components communicate efficiently via input/output redirection.

---

## **Example Gameplay**

### **Start**
```
♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜ 
♟ ♟ ♟ ♟ ♟ ♟ ♟ ♟ 
· · · · · · · · 
· · · · · · · · 
· · · · · · · · 
· · · · · · · · 
♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙ 
♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖ 

a2

♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜ 
♟ ♟ ♟ ♟ ♟ ♟ ♟ ♟ 
· · · · · · · · 
· · · · · · · · 
• · · · · · · · 
• · · · · · · · 
♙ ♙ ♙ ♙ ♙ ♙ ♙ ♙ 
♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖ 

a4

♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜ 
♟ ♟ ♟ ♟ ♟ ♟ ♟ ♟ 
· · · · · · · · 
· · · · · · · · 
♙ · · · · · · · 
· · · · · · · · 
· ♙ ♙ ♙ ♙ ♙ ♙ ♙ 
♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖ 
```

### **AI Response**
```
Thinking...
♜ ♞ ♝ ♛ ♚ ♝ ♞ ♜ 
♟ ♟ ♟ ♟ ♟ ♟ · ♟ 
· · · · · · · · 
· · · · · · ♟ · 
♙ · · · · · · · 
· · · · · · · · 
· ♙ ♙ ♙ ♙ ♙ ♙ ♙ 
♖ ♘ ♗ ♕ ♔ ♗ ♘ ♖ 
```

---

## **Demo**

You can check out a demo of the chess game in action by visiting this [LinkedIn post](https://www.linkedin.com/posts/a7mddra_i-made-chess-in-terminal-using-c-hundreds-activity-7270785844255342592-MBoo?utm_source=share&utm_medium=member_desktop).

In the post, I showcase how the chess game runs in the terminal, highlighting the AI's moves and the player's interaction with the game using Standard Algebraic Notation.

Feel free to take a look at the video to see the game in real-time!

---
