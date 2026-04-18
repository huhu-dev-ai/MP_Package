import cv2
import mediapipe as mp
import numpy as np
from collections import deque

mp_face_mesh = mp.solutions.face_mesh

cap = cv2.VideoCapture("input.mp4")
fps = cap.get(cv2.CAP_PROP_FPS)

# Output file
output_file = open("output.txt", "w")

# Mouth landmarks
UPPER_LIP = 13
LOWER_LIP = 14
LEFT_MOUTH = 61
RIGHT_MOUTH = 291

# Speaking detection params
MOTION_THRESHOLD = 0.005
WINDOW_SEC = 0.5
WINDOW_SIZE = int(fps * WINDOW_SEC)
SPEAK_RATIO = 0.6

# Face tracking
face_tracks = {}
next_id = 0
frame_idx = 0

def get_point(landmarks, idx, w, h):
    lm = landmarks.landmark[idx]
    return np.array([lm.x * w, lm.y * h])

def assign_id(center, tracks, threshold=80):
    for tid, data in tracks.items():
        if np.linalg.norm(center - data["center"]) < threshold:
            return tid
    return None

with mp_face_mesh.FaceMesh(
    max_num_faces=5,
    refine_landmarks=True
) as face_mesh:

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        h, w, _ = frame.shape
        rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        results = face_mesh.process(rgb)

        time_sec = frame_idx / fps
        current_ids = set()

        if results.multi_face_landmarks:
            for landmarks in results.multi_face_landmarks:

                upper = get_point(landmarks, UPPER_LIP, w, h)
                lower = get_point(landmarks, LOWER_LIP, w, h)
                left  = get_point(landmarks, LEFT_MOUTH, w, h)
                right = get_point(landmarks, RIGHT_MOUTH, w, h)

                center = (upper + lower) / 2

                mouth_open = np.linalg.norm(upper - lower)
                face_width = np.linalg.norm(left - right)
                ratio = mouth_open / face_width if face_width > 0 else 0

                # Assign ID
                tid = assign_id(center, face_tracks)
                if tid is None:
                    tid = next_id
                    next_id += 1
                    face_tracks[tid] = {
                        "center": center,
                        "history": deque(maxlen=WINDOW_SIZE),
                        "prev_ratio": ratio
                    }

                track = face_tracks[tid]
                current_ids.add(tid)

                # Motion detection
                motion = abs(ratio - track["prev_ratio"])
                track["prev_ratio"] = ratio

                motion_flag = 1 if motion > MOTION_THRESHOLD else 0
                track["history"].append(motion_flag)

                # Speaking detection
                if len(track["history"]) == WINDOW_SIZE:
                    score = sum(track["history"]) / WINDOW_SIZE
                    speaking = score > SPEAK_RATIO
                else:
                    speaking = False

                track["center"] = center

                # ---- WRITE TO FILE ----
                line = f"{time_sec:.2f}, ID:{tid}, X:{center[0]:.1f}, Y:{center[1]:.1f}, Speaking:{speaking}\n"
                output_file.write(line)

                # Optional display
                color = (0,255,0) if speaking else (0,0,255)
                cv2.circle(frame, tuple(center.astype(int)), 5, color, -1)
                cv2.putText(frame,
                            f"ID {tid} | {speaking}",
                            tuple(center.astype(int) + np.array([10, -10])),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.6,
                            color,
                            2)

        # Remove lost faces
        lost_ids = set(face_tracks.keys()) - current_ids
        for lid in lost_ids:
            del face_tracks[lid]

        frame_idx += 1

        cv2.imshow("Multi-face Speaking Detection", frame)
        if cv2.waitKey(1) & 0xFF == 27:
            break

cap.release()
output_file.close()
cv2.destroyAllWindows()