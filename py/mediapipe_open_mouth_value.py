import cv2
import mediapipe as mp
import numpy as np

# Initialize
mp_face_mesh = mp.solutions.face_mesh
cap = cv2.VideoCapture(0)

fps = cap.get(cv2.CAP_PROP_FPS)

# Threshold for speaking detection
MOUTH_OPEN_THRESHOLD = 0.02

frame_idx = 0
speaking_segments = []
currently_speaking = False
start_time = 0

with mp_face_mesh.FaceMesh(
    static_image_mode=False,
    max_num_faces=1,
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

            # Get lip points
            upper = get_point(13)
            lower = get_point(14)
            left = get_point(61)
            right = get_point(291)

            # Compute mouth opening
            mouth_open = np.linalg.norm(upper - lower)

            # Normalize (optional)
            face_width = np.linalg.norm(left - right)
            mouth_ratio = mouth_open / face_width

            # ---- Speaking detection ----
            if mouth_ratio > MOUTH_OPEN_THRESHOLD:
                if not currently_speaking:
                    currently_speaking = True
                    start_time = time_sec
            else:
                if currently_speaking:
                    currently_speaking = False
                    speaking_segments.append((start_time, time_sec))

            # ---- Draw (optional) ----
            cv2.circle(frame, tuple(upper.astype(int)), 3, (0,255,0), -1)
            cv2.circle(frame, tuple(lower.astype(int)), 3, (0,255,0), -1)

            # Display info
            cv2.putText(frame, f"Open: {mouth_ratio:.3f}",
                        (30, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,255,0), 2)

        frame_idx += 1
        cv2.imshow("Mouth Tracking", frame)

        if cv2.waitKey(1) & 0xFF == 27:
            break

cap.release()
cv2.destroyAllWindows()

print("Speaking segments (seconds):")
for seg in speaking_segments:
    print(seg)