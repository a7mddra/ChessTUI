import os
import time
import cv2
import math
import chess
import chess.engine
import mediapipe as mp
from multiprocessing import Process, Queue

mp_hands = mp.solutions.hands
hands = mp_hands.Hands(min_detection_confidence=0.5, min_tracking_confidence=0.5)
mp_drawing = mp.solutions.drawing_utils

cap = cv2.VideoCapture(0)

dist = 10000
mid = 10000
curr = 'i9'
drag = 'i9'
drop = 'i9'
my_turn = True
ai_thinking = False  
result_queue = None
ai_process = None
ai_move = None

def genBoard():
    board = []
    for row_num in range(8, 0, -1):
        row = []
        for col_char in range(ord('a'), ord('h') + 1):
            pos = f"{chr(col_char)}{row_num}"
            row.append(pos)
        board.append(row)
    return board

def showBoard(frame, w, sqr, off):
    for i in range(9):
        cv2.line(frame, (off + i * sqr, off), (off + i * sqr, off + w), (0, 0, 0), 1)
        cv2.line(frame, (off, off + i * sqr), (off + w, off + i * sqr), (0, 0, 0), 1)

def getPiece(board, pos):
    if ord(pos[0]) > ord('h') or int(pos[1:]) > 8 or ord(pos[0]) < ord('a') or int(pos[1:]) < 1:
        return '_'
    square = chess.parse_square(pos)
    piece = board.piece_at(square)
    return piece.symbol() if piece else '_'

def myPiece(board, pos) ->bool:
    return ('a' <= getPiece(board, pos) <= 'z')

def showPiece(frame, piece_name, pos, w, sqr, off, size, drag_pos = (-1, -1)):
    
    global drag 
    if piece_name == '-':
        drag = pos
    if piece_name == '_' or piece_name == '-':
        return frame
    fac = 2.4 if piece_name == 'p' or piece_name == 'P' else 2.5
    piece_image = cv2.imread(f'assets/{piece_name}.png', cv2.IMREAD_UNCHANGED)

    img_h, img_w = piece_image.shape[:2]
    x, y = ((ord(pos[0]) - ord('a')) * sqr + sqr//2 + off,
            (8 - int(pos[1])) * sqr + sqr //2 + off) if drag_pos[0] ==-1 else (drag_pos[0], drag_pos[1])

    x1, y1 = max(0, x - int(img_w // (2*fac))), max(0, y - int(img_h // (2*fac)))
    x2, y2 = min(x1 + int(img_w//fac), w), min(y1 + int(img_h//fac), w)

    if size==2:
        x1 -= 2; y1 -= 2
        x2 += 2; y2 += 2

    frame_h, frame_w, _ = frame.shape
    if x2 > frame_w or y2 > frame_h:
        return frame

    resized_piece = cv2.resize(piece_image, (x2 - x1, y2 - y1), interpolation=cv2.INTER_AREA)

    global err
    if resized_piece.shape[2] == 4:
        b, g, r, a = cv2.split(resized_piece)
        alpha = a / 255.0

        for c in range(3):
            frame[y1:y2, x1:x2, c] = (1 - alpha) * frame[y1:y2, x1:x2, c] + alpha * resized_piece[:, :, c]
    else:
        err = True
        
    return frame

def cDist(point1, point2):
    x1, y1 = point1
    x2, y2 = point2
    return math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2)

def cMid(point1, point2):
    x1, y1 = point1
    x2, y2 = point2
    return (int((x1 + x2) / 2), int((y1 + y2) / 2))

script_dir = os.path.dirname(os.path.realpath(__file__))
stockfish_path = os.path.join(script_dir, "stockfish/src/stockfish")
init = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1"
board = chess.Board(fen=init)
pos_board = genBoard()

def stockfishWorker(board_fen, engine_path, result_queue):
    engine = chess.engine.SimpleEngine.popen_uci(engine_path)  
    board = chess.Board(fen=board_fen)  
    result = engine.play(board, chess.engine.Limit(time=2.0))  
    result_queue.put(result.move)  
    engine.quit()

while cap.isOpened():
    ret, frame = cap.read()
    if not ret: break

    frame = cv2.flip(frame, 1)
    h, w, _ = frame.shape

    off = 10
    sqr = (h - 2 * off) // 8
    dragging = False
    err = False

    results = hands.process(frame)
    
    if results.multi_hand_landmarks:
        for hand_lm in results.multi_hand_landmarks:
            mp_drawing.draw_landmarks(frame, hand_lm, mp_hands.HAND_CONNECTIONS)
            x4, y4 = hand_lm.landmark[4].x, hand_lm.landmark[4].y
            x8, y8 = hand_lm.landmark[8].x, hand_lm.landmark[8].y
            point4 = (int(x4 * w), int(y4 * h))
            point8 = (int(x8 * w), int(y8 * h))
            dist = cDist(point4, point8)
            
            showBoard(frame, 8*sqr, sqr, off)
            if dist < 50 and myPiece(board, curr) and my_turn:
                dragging = True
            if dragging:
                mid = cMid(point4, point8)
                row = 8-mid[1]//sqr
                col = mid[0]//sqr + ord('a')
                drop = f'{chr(col)}{row}'
            if not dragging:
                row = 8-point8[1]//sqr
                col = point8[0]//sqr + ord('a')
                curr = f'{chr(col)}{row}'


    if not dragging:
        showBoard(frame, 8*sqr, sqr, off)
    
        try:
            move = chess.Move.from_uci(f'{drag}{drop}')
            if move in board.legal_moves:
                board.push(move)
                dragging = False
                my_turn = not my_turn
                drop = 'i9'
            else:
                drop = 'i9'
        except ValueError:
            err = True

    for r, row in enumerate(pos_board):
        for c, pos in enumerate(row):
            showPiece(
                frame, '-' if dragging and pos==curr and myPiece(board, curr) else getPiece(board, pos),
                pos, h, sqr, off, 2 if pos==curr and myPiece(board, curr) else 1)

    if dragging:
        showPiece(frame, getPiece(board, curr), 'i9', h, sqr, off, 2, mid)

    cycle_speed = 0.3
    dots = int((time.time() / cycle_speed) % 4)
    thinking_text = "Thinking" + "." * dots
    if not my_turn and ai_thinking:
        cv2.putText(frame, thinking_text, (8 * sqr + 2 * off, 4 * off),
                cv2.FONT_HERSHEY_DUPLEX, 0.9, (0, 0, 0), 1, cv2.LINE_AA)
    
    if not my_turn and result_queue is not None and not result_queue.empty():
        ai_move = result_queue.get()
        board.push(ai_move)
        my_turn = True
        ai_thinking = False
        result_queue = None
        ai_process = None

    if not my_turn and not ai_thinking:
        ai_thinking = True
        result_queue = Queue()
        ai_process = Process(target=stockfishWorker, args=(board.fen(), stockfish_path, result_queue))
        ai_process.start()

    cv2.imshow("CV Chess", frame)

    key = cv2.waitKey(1) & 0xFF
    if key == ord('q'):
        break
    elif key == ord('r'):
        board = chess.Board(fen=init)
        curr = 'i9'
        drag = 'i9'
        drop = 'i9'
        my_turn = True
        ai_thinking = False
        result_queue = None
        ai_process = None
        ai_move = None

if ai_process is not None and ai_process.is_alive():
    ai_process.terminate()
    ai_process.join()

cap.release()
cv2.destroyAllWindows()
