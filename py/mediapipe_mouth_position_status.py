import cv2
import mediapipe as mp
import numpy as np

# Initialize MediaPipe
mp_face_mesh = mp.solutions.face_mesh

# Open video file
cap = cv2.VideoCapture("..//packages/input.mp4")

# from camera
#cap = cv2.VideoCapture(0) 

fps = cap.get(cv2.CAP_PROP_FPS)

# Mouth landmark indices
UPPER_LIP = 13
LOWER_LIP = 14
LEFT_MOUTH = 61
RIGHT_MOUTH = 291

# Threshold (tune this!)
MOUTH_OPEN_THRESHOLD = 0.03

frame_idx = 0

with mp_face_mesh.FaceMesh(
    static_image_mode=False,
    max_num_faces=1, # multiple faces
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

        if results.multi_face_landmarks:
            landmarks = results.multi_face_landmarks[0]

            def get_point(idx):
                lm = landmarks.landmark[idx]
                return np.array([lm.x * w, lm.y * h])

            # Get mouth points
            upper = get_point(UPPER_LIP)
            lower = get_point(LOWER_LIP)
            left = get_point(LEFT_MOUTH)
            right = get_point(RIGHT_MOUTH)

            # Compute mouth center
            mouth_center = (upper + lower) / 2

            # Compute mouth opening
            mouth_open_dist = np.linalg.norm(upper - lower)
            face_width = np.linalg.norm(left - right)

            # Normalize (important!)
            mouth_ratio = mouth_open_dist / face_width

            # Open / Close decision
            is_open = mouth_ratio > MOUTH_OPEN_THRESHOLD

            # ---- OUTPUT ----
            print(f"Time: {time_sec:.2f}s | Pos: {mouth_center} | Open: {is_open}")

            # ---- Visualization ----
            cv2.circle(frame, tuple(upper.astype(int)), 3, (0,255,0), -1)
            cv2.circle(frame, tuple(lower.astype(int)), 3, (0,255,0), -1)

            cv2.putText(frame,
                        f"Open: {is_open}",
                        (30, 40),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        1,
                        (0,255,0),
                        2)

        frame_idx += 1

        cv2.imshow("Mouth Tracking", frame)
        if cv2.waitKey(1) & 0xFF == 27:
            break

cap.release()
cv2.destroyAllWindows()