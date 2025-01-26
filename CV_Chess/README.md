## Computer Vision Chess GUI

**A Virtual Drag and Drop GUI for playing chess against Stockfish AI.**

This project provides a lightweight graphical interface for Stockfish, allowing players to enjoy a game of chess in the VR.
It features real-time board updates, colored Pieces, and seamless interaction between OpenCV and MediaPipe for enhanced gameplay.



## **Key Features**

1. **Dynamic LandMarks Interface**  
   - Continuous screen refresh mechanism for a smooth and interactive display.
   - Hand recognition and detection.

2. **Advanced Game Logic**  
   - All chess rules and special moves (e.g., castling, en passant, promotion) implemented in Python.  
   - Real-time move validation and feedback.

3. **Stockfish Integration**  
   - Powered by the Stockfish UCI engine for AI moves.  
   - "Thinking..." animation during the AI's turn for immersive gameplay.

4. **Player vs. AI Mode**  
   - Toggle turns seamlessly between the player and Stockfish AI.  
   - AI responds with powerful moves that challenge players of all skill levels.

---

## **Prerequisites**

To compile this project, ensure the following are installed on your system:

1. **Python Libraries**  
   Install via pip:  
   ```bash
   pip install opencv-python opencv-python-headless chess mediapipe
   ```

2. **StockFish Engine**  
   Download the Stockfish binary from [Stockfish's official site](https://stockfishchess.org/download/).  
   Once downloaded, place it in the same directory where you cloned the repository. If you'd prefer to keep it elsewhere, make sure to update the path to the Stockfish binary.

---

## **How to Play**

1. **Clone the Repository**
   ```bash
   git clone https://github.com/a7mddra/ChessGUI.git
   cd ChessGUI/CV_Chess
   ```

2. **Run Main Script**
   ```bash
   Python3 main.py
   ```

   - You can reset the board by pressing `R`.


---

## **Demo**

You can check out a demo of the chess game in action by visiting this [LinkedIn post](https://www.linkedin.com/feed/update/urn:li:activity:7289358361463017472?utm_source=share&utm_medium=member_desktop).

In the post, I showcase how the chess game runs, highlighting the AI's moves and the player's interaction with the game.

Feel free to take a look at the video to see the game in real-time!

---
