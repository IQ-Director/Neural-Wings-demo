
import numpy as np
import cv2
import sys
import os

current_dir = os.path.dirname(os.path.abspath(__file__))
debug_path = os.path.join(current_dir, "Release")
sys.path.append(debug_path)

try:
    import nw_engine
except ImportError as e:
    print(f"Failed to load module: {e}")
except Exception as e:
    print(f"Error: {e}")


def test():
    print(f"Current Process ID (PID): {os.getpid()}")
    input("Press Enter after attaching C++ debugger...")

    nw_engine.AIEnv.initContext(128, 128)
    env = nw_engine.AIEnv(128, 128)
    # env.init()
    # obs = env.reset()
    while True:
        action = [0.1, 0.0, 0.5, 0.0, 0.0, 1.0]
        obs, reward, done = env.step(action)

        time = env.getTime()
        if (time > 10):
            env.reset()
            continue

        rgb = obs[:, :, :3]
        depth = obs[:, :, 3]

        rgb_view = cv2.cvtColor(rgb, cv2.COLOR_RGB2BGR)
        rgb_view = cv2.resize(rgb_view, (512, 512),
                              interpolation=cv2.INTER_NEAREST)
        depth_view = cv2.resize(
            depth, (512, 512), interpolation=cv2.INTER_NEAREST)

        cv2.imshow("AI RGB Eye", rgb_view)
        cv2.imshow("AI Depth Eye", depth_view/4000)
        print(f"Reward: {reward:.4f} | Done: {done} | Time: {time:.2f}")

        if cv2.waitKey(1) & 0xFF == ord('q') or done:
            break
    cv2.destroyAllWindows()


if __name__ == "__main__":
    test()
