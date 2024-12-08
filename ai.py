import sys
import os
import chess
import chess.engine

'''''
Download the Stockfish binary from Stockfish's official site.
Place it in the CHESS directory or you can update path to it here.
"stockfish/src/stockfish" => "stockfish/stockfish-ubuntu-x86-64-avx2"
'''''

script_dir = os.path.dirname(os.path.realpath(__file__))
stockfish_path = os.path.join(script_dir, "stockfish/src/stockfish")

engine = chess.engine.SimpleEngine.popen_uci(stockfish_path)
board = chess.Board()

moves = sys.argv[1].split()

for move in moves:
    try:
        board.push_san(move)

    except ValueError:
        engine.quit()
        sys.exit(1)

result = engine.play(board, chess.engine.Limit(time=2.0))
print(result.move)
board.push(result.move)

engine.quit()
