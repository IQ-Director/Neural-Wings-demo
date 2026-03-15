from stable_baselines3 import PPO
from stable_baselines3.common.callbacks import CheckpointCallback
import numpy as np
import cv2
import sys
import os
import gymnasium as gym
from gymnasium import spaces
import torch

current_dir = os.path.dirname(os.path.abspath(__file__))
debug_path = os.path.join(current_dir, "Release")
sys.path.append(debug_path)

try:
    import nw_engine
except ImportError as e:
    print(f"Failed to load module: {e}")
except Exception as e:
    print(f"Error: {e}")

width = 128
height = 128


class ppoEnv(gym.Env):
    def __init__(self):
        super(ppoEnv, self).__init__()

        nw_engine.AIEnv.initContext(width, height)
        self.inner_env = nw_engine.AIEnv(width, height)

        # 动作空间
        self.action_space = spaces.Box(
            low=-1.0, high=1.0, shape=(6,), dtype=np.float32)
        # 观测空间
        self.observation_space = spaces.Box(
            low=0, high=1.0, shape=(4, width, height), dtype=np.float32)

        self.steps_in_episode = 0

    def process_obs(self, obs):
        obs = np.array(obs, dtype=np.float32)
        if obs.max() > 1.0:
            obs /= 255.0
        obs = np.transpose(obs, (2, 0, 1))
        return obs

    def reset(self, seed=None, options=None):
        super().reset(seed=seed)
        raw_obs = self.inner_env.reset()
        obs = self.process_obs(raw_obs)

        self.steps_in_episode = 0
        print(f"Reset | Steps: {self.steps_in_episode}")

        return obs, {}

    def step(self, action):
        raw_obs, reward, done = self.inner_env.step(action)
        curTime = self.inner_env.getTime()
        truncated = curTime > 30.0
        obs = self.process_obs(raw_obs)

        self.steps_in_episode += 1
        if (self.steps_in_episode % 100 == 0):
            print(
                f"Reward: {reward:.4f} | Time: {curTime:.2f} | Action: {action}")
        return obs, reward, done, truncated, {}

    def render(self, obs, reward, done):

        render_obs = np.transpose(obs, (1, 2, 0))
        rgb = render_obs[:, :, :3]
        depth = render_obs[:, :, 3]

        if rgb.max() > rgb.min():
            rgb_view = cv2.normalize(
                rgb, None, 0, 255, cv2.NORM_MINMAX, cv2.CV_8U)
        else:
            rgb_view = (rgb * 255).astype(np.uint8)

        rgb_view = cv2.cvtColor(rgb_view, cv2.COLOR_RGB2BGR)
        rgb_view = cv2.resize(rgb_view, (512, 512),
                              interpolation=cv2.INTER_NEAREST)

        depth_normalized = (depth * 255.0) / 4000.0
        depth_view = cv2.resize(
            depth_normalized, (512, 512), interpolation=cv2.INTER_NEAREST)

        cv2.imshow("AI RGB Eye", rgb_view)
        cv2.imshow("AI Depth Eye", depth_view)
        if cv2.waitKey(1) & 0xFF == ord('q') or done:
            return True
        return False


def train():
    env = ppoEnv()
    policy_kwargs = dict(normalize_images=False)

    model = PPO("CnnPolicy",
                env,
                policy_kwargs=policy_kwargs,
                verbose=1,
                learning_rate=1e-4,
                n_steps=2048,
                batch_size=128,
                n_epochs=5,
                ent_coef=0.05,
                gamma=0.99,
                target_kl=0.015,
                device="cuda")

    print(f"Model device: {model.device}")

    checkpoint_callback = CheckpointCallback(
        save_freq=10000,
        save_path='./ai_train/checkpoints/',
        name_prefix='ppo_combat_model'
    )
    print("Starting Training...")

    model.learn(total_timesteps=3000000,
                callback=checkpoint_callback)
    model.save("ppo_combat_model")
    return model, env


def eval(model_path):
    env = ppoEnv()

    print(f"Loading model from: {model_path}")
    if not os.path.exists(model_path if model_path.endswith(".zip") else model_path + ".zip"):
        print(f"Error: Model file {model_path} not found!")
        return

    model = PPO.load(model_path, env=env)

    print("Evaluation started. Press 'q' on image window to stop.")
    action = np.array([0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    try:
        while True:

            obs, reward, done, truncated, info = env.step(action)
            action, _states = model.predict(obs, deterministic=True)

            user_quit = env.render(obs, reward, done)
            print(action)
            print(
                f"Reward: {reward:.4f} | Done: {done} | Truncated: {truncated} ")

            if done or truncated or user_quit:
                print(f"Episode finished. Reward: {reward:.4f}")
                obs, info = env.reset()
                if user_quit:
                    break

    except KeyboardInterrupt:
        print("Interrupted by user")
    finally:
        cv2.destroyAllWindows()


if (__name__ == "__main__"):
    if torch.cuda.is_available():
        print(f"GPU: {torch.cuda.get_device_name(0)} is ready.")
    else:
        print("CUDA is NOT available. Check your PyTorch installation.")

    model, env = train()
    # mode_path = "ai_train/checkpoints/ppo_combat_model_3000000_steps.zip"
    # eval(mode_path)
